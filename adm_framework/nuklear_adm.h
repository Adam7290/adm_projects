#pragma once

#include "adm_framework/input.h"
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#include "external/Nuklear/nuklear.h"

typedef struct nk_adm_t nk_adm_t;
typedef struct gpu_t gpu_t;
typedef struct arena_t arena_t;

nk_adm_t* nk_adm_new(gpu_t* gpu, arena_t* arena);
void nk_adm_font_stash_begin(nk_adm_t* nk, struct nk_font_atlas** atlas);
void nk_adm_font_stash_end(nk_adm_t* nk);
void nk_adm_upload_atlas(nk_adm_t* nk, const void* image, int width, int height);
void nk_adm_new_frame(nk_adm_t* nk);
void nk_adm_render(nk_adm_t* nk, enum nk_anti_aliasing AA, int max_vertex_buffer, int max_element_buffer);
struct nk_context* nk_adm_ctx(nk_adm_t* nk);
void nk_adm_destroy(nk_adm_t* nk);

#ifdef NK_IMPLEMENTATION

#include "gmath.h"
#include "gpu.h"
#include "app.h"
#include "input.h"
#include "time.h"
#include <adm_utils/console.h>

typedef struct nk_adm_t {
	gpu_t* gpu;
	app_t* app;	
	arena_t* arena;

	struct nk_buffer cmds; struct nk_draw_null_texture tex_null;
	gpu_verts_t* verts;
	gpu_shader_t* shader;
	gpu_uniform_t uniform_tex;
	gpu_uniform_t uniform_proj;
	gpu_texture_t* font_tex;

	struct nk_context ctx;
	struct nk_font_atlas atlas;
	struct nk_vec2 fb_scale;
	double last_button_click;
	bool is_double_click_down;
	vec2i_t double_click_pos;
} nk_adm_t;

typedef struct nk_adm_vertex_t {
	float pos[2];
	float uv[2];
	nk_byte color[4];
} nk_adm_vertex_t;

PRIVATE void _nk_adm_destroy_internal(nk_adm_t* nk) {
	nk_buffer_free(&nk->cmds);
	nk_font_atlas_clear(&nk->atlas);
	nk_free(&nk->ctx);
}

PRIVATE void _nk_adm_clipboard_paste(nk_handle user_data, struct nk_text_edit* edit) {	
	nk_adm_t* nk = (nk_adm_t*)user_data.ptr;
	const char* text = input_clipboard_string(nk->app);
	if (text != NULL) {
		nk_textedit_paste(edit, text, strlen(text));
	}
}

PRIVATE void _nk_adm_clipboard_copy(nk_handle user_data, const char* text, int length) {
	nk_adm_t* nk = (nk_adm_t*)user_data.ptr;
	char* str = 0;
	if (length == 0) { return; }
	str = arena_alloc_raw(nk->arena, length+1);
	memcpy(str, text, length);
	str[length] = '\0';
	input_set_clipboard_string(nk->app, str);
	arena_free(nk->arena, str);
}

nk_adm_t* nk_adm_new(gpu_t* gpu, arena_t* arena) {
	nk_adm_t* nk = arena_defer(arena, _nk_adm_destroy_internal, nk_adm_t);
	nk->gpu = gpu;
	nk->app = gpu_app(gpu);
	nk->arena = arena;

	nk_init_default(&nk->ctx, 0);
	// TODO: Clipboard stuff
	
	nk->last_button_click = 0.0; 
	nk->is_double_click_down = false;

	nk->ctx.clip.copy = _nk_adm_clipboard_copy;
	nk->ctx.clip.paste = _nk_adm_clipboard_paste;
	nk->ctx.clip.userdata = nk_handle_ptr(nk);
	
	static const char* vertex_shader = 
	"#version 330 core\n"
	"uniform mat4 u_proj;\n"
	"layout (location = 0) in vec2 in_pos;\n"
	"layout (location = 1) in vec2 in_uv;\n"
	"layout (location = 2) in vec4 in_color;\n"
	"out vec2 vert_uv;\n"
	"out vec4 vert_color;\n"
	"void main() {\n"
	"	vert_uv = in_uv;\n"
	"	vert_color = in_color / vec4(255, 255, 255, 255);\n"
	"	gl_Position = u_proj * vec4(in_pos.xy, 0, 1);\n"
	"}\n"
	;
	static const char* fragment_shader = 
	"#version 330 core\n"
	"precision mediump float;\n"
	"uniform sampler2D u_texture;\n"
	"in vec2 vert_uv;\n"
	"in vec4 vert_color;\n"
	"out vec4 out_color;\n"
	"void main() {\n"
	"	out_color = vert_color * texture(u_texture, vert_uv.st);\n"
	"}\n"
	;

	nk_buffer_init_default(&nk->cmds);
	nk->shader = gpu_shader_create(gpu, arena);
	gpu_shader_upload_source(nk->shader, vertex_shader, fragment_shader);
	nk->uniform_tex = gpu_shader_get_uniform(nk->shader, "u_texture"); 
	nk->uniform_proj = gpu_shader_get_uniform(nk->shader, "u_proj");

	nk->verts = gpu_verts_create(gpu, arena, (gpu_attribute_t[]){
		{ 2, GPU_ATTRIBUTE_TYPE_FLOAT },
		{ 2, GPU_ATTRIBUTE_TYPE_FLOAT },
		{ 4, GPU_ATTRIBUTE_TYPE_BYTE },
		{ 0 }
	});

	return nk;
}

void nk_adm_font_stash_begin(nk_adm_t* nk, struct nk_font_atlas** atlas) {
	nk_font_atlas_init_default(&nk->atlas);
	nk_font_atlas_begin(&nk->atlas);
	*atlas = &nk->atlas;
}

void nk_adm_font_stash_end(nk_adm_t* nk) {
	const void* image; int w, h;
	image = nk_font_atlas_bake(&nk->atlas, &w, &h, NK_FONT_ATLAS_RGBA32);
	nk_adm_upload_atlas(nk, image, w, h);
	nk_font_atlas_end(&nk->atlas, nk_handle_id((int)nk->font_tex->_handle), &nk->tex_null);
	if (nk->atlas.default_font) {
		nk_style_set_font(&nk->ctx, &nk->atlas.default_font->handle);
	}
}

void nk_adm_upload_atlas(nk_adm_t* nk, const void* image, int width, int height) {
	if (nk->font_tex != NULL) {
		gpu_texture_destroy(nk->font_tex);
	}
	nk->font_tex = gpu_texture_create(nk->gpu, nk->arena);
	gpu_texture_upload_raw(nk->font_tex, image, width, height, IMAGE_FORMAT_RGBA);
	gpu_texture_set_filter(nk->font_tex, GPU_TEXTURE_FILTER_LINEAR);
}

void nk_adm_new_frame(nk_adm_t* nk) {
	app_t* app = nk->app;
	struct nk_context* ctx = nk_adm_ctx(nk);

	ctx->delta_time_seconds = time_app_delta(app).time;

	nk_input_begin(ctx);
	uint codepoint = input_codepoint(app);
	if (codepoint != 0) {
		nk_input_unicode(ctx, codepoint);
	}

	vec2i_t mouse_pos = input_mouse_pos(app);
	nk_input_motion(ctx, mouse_pos.x, mouse_pos.y); 
	if (input_mouse_button_pressed(app, INPUT_MOUSE_BUTTON_LEFT) == true) {
		double delta = time_app_elapsed(app).time - nk->last_button_click;
		if (delta < 0.2) {
			nk->is_double_click_down = true;
			nk->double_click_pos = mouse_pos;
		}
	} else if (input_mouse_button_released(app, INPUT_MOUSE_BUTTON_LEFT) == true) {
		nk->is_double_click_down = false;
	}

	nk_input_button(ctx, NK_BUTTON_LEFT, mouse_pos.x, mouse_pos.y, input_mouse_button_down(app, INPUT_MOUSE_BUTTON_LEFT));
	nk_input_button(ctx, NK_BUTTON_RIGHT, mouse_pos.x, mouse_pos.y, input_mouse_button_down(app, INPUT_MOUSE_BUTTON_RIGHT));
	nk_input_button(ctx, NK_BUTTON_MIDDLE, mouse_pos.x, mouse_pos.y, input_mouse_button_down(app, INPUT_MOUSE_BUTTON_MIDDLE));
	nk_input_button(ctx, NK_BUTTON_DOUBLE, nk->double_click_pos.x, nk->double_click_pos.y, nk->is_double_click_down);

	vec2f_t scroll = input_mouse_scroll(app);
	nk_input_scroll(ctx, (struct nk_vec2){ scroll.x, scroll.y });

	if (ctx->input.mouse.grabbed) {
		input_set_mouse_pos(app, &(vec2i_t){ ctx->input.mouse.prev.x, ctx->input.mouse.prev.y });
		ctx->input.mouse.pos.x = ctx->input.mouse.prev.x;
		ctx->input.mouse.pos.y = ctx->input.mouse.prev.y;
	}

	input_button_state_t state;
	#define _adm_key_transfer(nkkey, admkey) \
		state = input_key_state(app, admkey); \
		nk_input_key(ctx, nkkey, state == INPUT_BUTTON_STATE_JUST_PRESSED || state == INPUT_BUTTON_STATE_REPEAT); 

	_adm_key_transfer(NK_KEY_DEL, INPUT_KEY_DELETE);
	_adm_key_transfer(NK_KEY_ENTER, INPUT_KEY_ENTER);

	_adm_key_transfer(NK_KEY_TAB, INPUT_KEY_TAB);
	_adm_key_transfer(NK_KEY_BACKSPACE, INPUT_KEY_BACKSPACE);
	_adm_key_transfer(NK_KEY_UP, INPUT_KEY_UP);
	_adm_key_transfer(NK_KEY_DOWN, INPUT_KEY_DOWN);
	_adm_key_transfer(NK_KEY_LEFT, INPUT_KEY_LEFT);
	_adm_key_transfer(NK_KEY_RIGHT, INPUT_KEY_RIGHT);
	_adm_key_transfer(NK_KEY_SCROLL_UP, INPUT_KEY_PAGE_UP);
	_adm_key_transfer(NK_KEY_SCROLL_DOWN, INPUT_KEY_PAGE_DOWN);

	_adm_key_transfer(NK_KEY_TEXT_START, INPUT_KEY_HOME);
	_adm_key_transfer(NK_KEY_TEXT_END, INPUT_KEY_END);
	_adm_key_transfer(NK_KEY_SCROLL_START, INPUT_KEY_HOME);
	_adm_key_transfer(NK_KEY_SCROLL_END, INPUT_KEY_END);
	_adm_key_transfer(NK_KEY_SHIFT, input_key_down(app, INPUT_KEY_LEFT_SHIFT) || input_key_down(app, INPUT_KEY_RIGHT_SHIFT));

	if (input_key_down(app, INPUT_KEY_LEFT_CONTROL) == true || input_key_down(app, INPUT_KEY_RIGHT_CONTROL) == true) { 
		_adm_key_transfer(NK_KEY_COPY, INPUT_KEY_C);
		_adm_key_transfer(NK_KEY_PASTE, INPUT_KEY_V);
		_adm_key_transfer(NK_KEY_CUT, INPUT_KEY_X);
		_adm_key_transfer(NK_KEY_TEXT_UNDO, INPUT_KEY_Z);
		_adm_key_transfer(NK_KEY_TEXT_REDO, INPUT_KEY_Y);
		_adm_key_transfer(NK_KEY_TEXT_LINE_START, INPUT_KEY_B);
		_adm_key_transfer(NK_KEY_TEXT_LINE_END, INPUT_KEY_E);
		_adm_key_transfer(NK_KEY_TEXT_WORD_LEFT, INPUT_KEY_LEFT);
		_adm_key_transfer(NK_KEY_TEXT_WORD_RIGHT, INPUT_KEY_RIGHT);
		nk_input_key(ctx, NK_KEY_TEXT_SELECT_ALL, input_key_just_pressed(app, INPUT_KEY_A));
	} else {
		nk_input_key(ctx, NK_KEY_COPY, false);
		nk_input_key(ctx, NK_KEY_PASTE, false);
		nk_input_key(ctx, NK_KEY_CUT, false);
		_adm_key_transfer(NK_KEY_LEFT, INPUT_KEY_LEFT);
		_adm_key_transfer(NK_KEY_RIGHT, INPUT_KEY_RIGHT);
	}

	#undef _adm_key_transfer

	nk_input_end(ctx);
}

void nk_adm_render(nk_adm_t* nk, enum nk_anti_aliasing AA, int max_vertex_buffer, int max_element_buffer) {
	struct nk_buffer vbuf, ebuf;
	app_window_size_t wind_size = app_window_size(nk->app);
	app_window_size_t fb_size = app_framebuffer_size(nk->app);
	vec2f_t fb_scale = (vec2f_t){ wind_size.width / (float)fb_size.width, wind_size.height / (float)fb_size.height };

	mat4x4_t ortho = mat4x4_orthographic(0.0f, wind_size.width, wind_size.height, 0.0f, 0.0f, 1.0f); 
	gpu_shader_use(nk->shader);
	gpu_shader_set_mat4x4(nk->shader, nk->uniform_proj, &ortho);
	gpu_shader_set_int(nk->shader, nk->uniform_tex, 0);
	{
		gpu_verts_alloc_vertices(nk->verts, GPU_BUFFER_USAGE_STREAM, max_vertex_buffer, NULL);
		gpu_verts_alloc_indices(nk->verts, GPU_BUFFER_USAGE_STREAM, max_element_buffer, NULL);

		void* vertices = gpu_verts_map_vertices(nk->verts, GPU_BUFFER_ACCESS_WRITE);
		void* elements = gpu_verts_map_indices(nk->verts, GPU_BUFFER_ACCESS_WRITE);
		{
			static const struct nk_draw_vertex_layout_element vertex_layout[] = {
				{NK_VERTEX_POSITION, NK_FORMAT_FLOAT, NK_OFFSETOF(nk_adm_vertex_t, pos)},
				{NK_VERTEX_TEXCOORD, NK_FORMAT_FLOAT, NK_OFFSETOF(nk_adm_vertex_t, uv)},
				{NK_VERTEX_COLOR, NK_FORMAT_R8G8B8A8, NK_OFFSETOF(nk_adm_vertex_t, color)},
				{NK_VERTEX_LAYOUT_END}
			};

			struct nk_convert_config config; 
			memset(&config, 0, sizeof(config));
			config.vertex_layout = vertex_layout,
			config.vertex_size = sizeof(nk_adm_vertex_t),
			config.vertex_alignment = NK_ALIGNOF(nk_adm_vertex_t),
			config.tex_null = nk->tex_null,
			config.circle_segment_count = 22,
			config.curve_segment_count = 22,
			config.arc_segment_count = 22,
			config.global_alpha = 1.0f,
			config.shape_AA = AA,
			config.line_AA = AA,

			nk_buffer_init_fixed(&vbuf, vertices, max_vertex_buffer);
			nk_buffer_init_fixed(&ebuf, elements, max_element_buffer);
			nk_convert(&nk->ctx, &nk->cmds, &vbuf, &ebuf, &config);
		}
		gpu_verts_unmap_vertices(nk->gpu);
		gpu_verts_unmap_indices(nk->gpu);

		const struct nk_draw_command* cmd;
		nk_size offset = 0;
		nk_draw_foreach(cmd, &nk->ctx, &nk->cmds) {
			if (!cmd->elem_count) {
				continue;
			}
		
			// TODO: Fix this hack
			gpu_texture_t tmp = (gpu_texture_t){
				._handle = cmd->texture.id,
				._gpu = nk->gpu,
				._arena = nk->arena,
			};
			gpu_texture_use(&tmp, 0);

			gpu_scissor_begin(nk->gpu, &(vec4f_t){
				(cmd->clip_rect.x * fb_scale.x),
				((wind_size.height - (cmd->clip_rect.y + cmd->clip_rect.h)) * fb_scale.y),
				(cmd->clip_rect.w * fb_scale.x),
				(cmd->clip_rect.h * fb_scale.y),
			});

			gpu_verts_draw_sub(nk->verts, offset, cmd->elem_count);
			offset += cmd->elem_count * sizeof(nk_draw_index);	
		}
		nk_clear(&nk->ctx);
		nk_buffer_clear(&nk->cmds);
	}
	gpu_scissor_end(nk->gpu);
}

struct nk_context* nk_adm_ctx(nk_adm_t* nk) {
	return &nk->ctx;
}

void nk_adm_destroy(nk_adm_t* nk) {
	arena_free(nk->arena, nk);
}

#endif // NK_IMPLEMENTATION
