#include "sprite_batch.h"

#include "adm_framework/time.h"
#include "gpu.h"
#include "image.h"
#include "app.h"
#include "gmath.h"

#include <adm_utils/arena.h>
#include <adm_utils/util.h>
#include <adm_utils/console.h>
#include <adm_utils/panic.h>

// Macro to share between C and GLSL
#define _sprite_batch_max_size 256

PRIVATE const char* _sprite_batch_vert_shader =
"#version 460 core\n"

"layout (location = 0) in vec2 in_pos;\n"
"layout (location = 1) in vec2 in_uv;\n"

"out vec2 vert_uv;\n"
"out vec4 vert_color;\n"

"struct sprite_batch_item_t {\n"
"	vec4 rect;\n"
"	vec4 src_rect;\n"
"	vec4 color;\n"
"	vec2 origin;\n"
"	float rot;\n"
"};\n"

"layout (std140) uniform sprite_batch_b {\n"
"	sprite_batch_item_t u_sprite_batch_items[" STRMACRO(_sprite_batch_max_size) "];\n"
"};\n"

"uniform mat4 u_view;\n"
"uniform mat4 u_proj;\n"

"void main() {\n"
"	sprite_batch_item_t item = u_sprite_batch_items[gl_InstanceID];\n"
"	vec2 pos = (in_pos - item.origin) * item.rect.zw;\n"
"	if (item.rot != 0.0f) {\n"
"		float s = sin(item.rot);\n"
"		float c = cos(item.rot);\n"
"		pos = vec2(\n"
"			pos.x * c - pos.y * s,\n"
"			pos.x * s + pos.y * c\n"
"		);\n"
"	}\n"

"	gl_Position = u_proj * u_view * vec4(item.rect.xy + pos, 0.0, 1.0);\n"

"	vert_uv = item.src_rect.xy + (item.src_rect.zw * in_uv);\n"
"	vert_color = item.color;\n"
"}\n"
;

PRIVATE const char* _sprite_batch_frag_shader = 
"#version 460 core\n"

"in vec2 vert_uv;\n"
"in vec4 vert_color;\n"
"out vec4 out_color;\n"
"uniform sampler2D u_texture;\n"

"void main() {\n"
"	out_color = texture(u_texture, vert_uv) * vert_color;\n" 
"}\n"
;

// MUST CHANGE SHADER STRUCT TOO
// Warning: Must align bytes
typedef struct _sprite_batch_item_t {
	vec4f_t rect;
	vec4f_t src_rect;
	vec4f_t color;
	vec2f_t origin;
	float rot;
	float __PADDING[1];
} _sprite_batch_item_t;

#define VEC_IMPLEMENTATION
#define VEC_TEMPLATE _sprite_batch_item_t
#define VEC_TEMPLATE_PREFIX _vec_sprite_batch_item_
#include <adm_utils/vec_impl.h>

typedef struct sprite_batch_t {
	gpu_t* _gpu;
	arena_t* _arena;

	bool _started;
	_vec_sprite_batch_item_t _items;
	gpu_shader_t* _default_effect;
	gpu_uniform_buffer_t* _buffer;
	gpu_verts_t* _verts;

	gpu_texture_t* _texture_blank;

	vec4f_t _param_rect;
	vec4f_t _param_src_rect;
	vec4f_t _param_color;
	vec2f_t _param_origin;
	float _param_rot;
	gpu_texture_t* _param_texture;
	gpu_shader_t* _param_effect;
} sprite_batch_t;

PRIVATE void _sprite_batch_destroy_internal(sprite_batch_t* sprite_batch) {
}

sprite_batch_t* sprite_batch_new(gpu_t* gpu, arena_t* arena) {
	sprite_batch_t* sprite_batch = arena_defer(arena, _sprite_batch_destroy_internal, sprite_batch_t); 

	sprite_batch->_gpu = gpu;
	sprite_batch->_arena = arena;

	sprite_batch->_started = false;

	// Items vector
	sprite_batch->_items = _vec_sprite_batch_item_new(arena);	
	_vec_sprite_batch_item_reserve(&sprite_batch->_items, _sprite_batch_max_size);

	// Buffer
	sprite_batch->_buffer = gpu_uniform_buffer_create(gpu, arena);
	gpu_uniform_buffer_upload(sprite_batch->_buffer, _sprite_batch_max_size * sizeof(_sprite_batch_item_t), NULL);

	// Shader
	sprite_batch->_default_effect = sprite_batch_effect_create(sprite_batch, arena, _sprite_batch_frag_shader);
	sprite_batch->_param_effect = sprite_batch->_default_effect;

	// Verts
	typedef struct vertex_t {
		float x, y; 
		float u, v;
	} vertex_t;
	
	const vertex_t verts[] = {
		(vertex_t){  1.0f,	1.0f,  1.0f, 1.0f, }, // Top-right
		(vertex_t){  1.0f,  0.0f,  1.0f, 0.0f, }, // Bottom-right
		(vertex_t){  0.0f,  0.0f,  0.0f, 0.0f, }, // Bottom-left
	    (vertex_t){  0.0f,  1.0f,  0.0f, 1.0f, }, // Top-left
	};

	const gpu_index_t indices[] = {
		0, 1, 3,
		1, 2, 3,
	};

	gpu_vert_decl_t vert_decl = gpu_vert_decl_new(arena, 2,
		(gpu_vert_attr_t){ 2, GPU_VERT_ATTR_TYPE_FLOAT, },
		(gpu_vert_attr_t){ 2, GPU_VERT_ATTR_TYPE_FLOAT, }
	);

	sprite_batch->_verts = gpu_verts_create(gpu, arena, &vert_decl, true); 
	gpu_verts_upload(sprite_batch->_verts, verts, sizeof(verts));
	gpu_verts_upload_indices(sprite_batch->_verts, indices, sizeof(indices));
	gpu_vert_decl_free(&vert_decl);

	// Blank texture
	sprite_batch->_texture_blank = gpu_texture_create(gpu, arena);
	const u64 pix = 0xFFFFFFFF;
	gpu_texture_upload_raw(sprite_batch->_texture_blank, (byte*)(&pix), 1, 1, IMAGE_FORMAT_RGBA);

	return sprite_batch;
}

PRIVATE void _sprite_batch_add(sprite_batch_t* sprite_batch, const _sprite_batch_item_t* item) {
	// We flush out the buffer if we go over the max size
	if (_vec_sprite_batch_item_length(&sprite_batch->_items) >= _sprite_batch_max_size) {
		sprite_batch_flush(sprite_batch);
	}

	_vec_sprite_batch_item_push(&sprite_batch->_items, *item);
}

void sprite_batch_start(sprite_batch_t* sprite_batch) {
	if (sprite_batch->_started == true) {
		PANIC("sprite_batch already started.");
	}
	sprite_batch->_started = true;

	sprite_batch_reset_params(sprite_batch);
}


void sprite_batch_end(sprite_batch_t* sprite_batch) {
	if (sprite_batch->_started == false) {
		PANIC("sprite_batch not started");
	}
	sprite_batch->_started = false;
	sprite_batch_flush(sprite_batch);
}

void sprite_batch_flush(sprite_batch_t* sprite_batch) {
	usize item_count = _vec_sprite_batch_item_length(&sprite_batch->_items);
	if (item_count == 0) {
		return;
	}

//	console_println("Flushing {} sprites in batch...", FORMAT(int, item_count));

	gpu_shader_use(sprite_batch->_param_effect);
	gpu_shader_set_int(sprite_batch->_param_effect, "u_texture", 0);

	gpu_uniform_buffer_upload_sub(
		sprite_batch->_buffer,
		0,
		item_count * sizeof(_sprite_batch_item_t),
		_vec_sprite_batch_item_ptr(&sprite_batch->_items)
	);

	app_t* app = gpu_app(sprite_batch->_gpu);
	app_window_size_t wind_size = app_window_size(app);
	mat4x4_t proj = mat4x4_orthographic(0, wind_size.width, 0, wind_size.height, -1.f, 1.f); 
	gpu_shader_set_mat4x4(sprite_batch->_param_effect, "u_proj", &proj);

	mat4x4_t view = mat4x4_identity();
	// mat4x4_rotate_z(&view, 45.f);
	// mat4x4_scale(&view, &(vec3f_t){ 0.5f, 0.5f, 1.0f });
	// mat4x4_translate(&view, &(vec3f_t){ 400, 100, 0 });

	gpu_shader_set_mat4x4(sprite_batch->_param_effect, "u_view", &view);
	
	gpu_texture_use(sprite_batch->_param_texture, 0);
	gpu_verts_draw_instanced(sprite_batch->_verts, item_count);

	_vec_sprite_batch_item_clear(&sprite_batch->_items);
}

void sprite_batch_draw(sprite_batch_t* sprite_batch) {
	_sprite_batch_item_t item;
	item.rect = sprite_batch->_param_rect;
	item.src_rect = sprite_batch->_param_src_rect;
	item.color = sprite_batch->_param_color;
	item.rot = sprite_batch->_param_rot;
	item.origin = sprite_batch->_param_origin;

	_sprite_batch_add(sprite_batch, &item);
}

void sprite_batch_rect(sprite_batch_t* sprite_batch, NULLABLE const vec4f_t* rect) {
	sprite_batch->_param_rect = rect != NULL ? *rect : (vec4f_t){ 0.0f, 0.0f, 0.0f, 0.0f };
}

void sprite_batch_src_rect(sprite_batch_t* sprite_batch, NULLABLE const vec4f_t* src_rect) {
	sprite_batch->_param_src_rect = src_rect != NULL ? *src_rect: (vec4f_t){ 0.0f, 0.0f, 1.0f, 1.0f };
}

void sprite_batch_color(sprite_batch_t* sprite_batch, NULLABLE const vec4f_t* color) {
	sprite_batch->_param_color = color != NULL ? *color: (vec4f_t){ 1.0f, 1.0f, 1.0f, 1.0f };
}

void sprite_batch_rot(sprite_batch_t* sprite_batch, float rot) {
	sprite_batch->_param_rot = deg_to_rad(rot);
}

void sprite_batch_origin(sprite_batch_t* sprite_batch, const vec2f_t* origin) {
	sprite_batch->_param_origin = origin != NULL ? *origin : (vec2f_t){ 0.f, 0.f };
}

void sprite_batch_texture(sprite_batch_t* sprite_batch, NULLABLE gpu_texture_t* texture) {
	// Don't flush if we don't actually need to
	if (texture == sprite_batch->_param_texture) {
		return;
	}

	sprite_batch_flush(sprite_batch);
	if (texture != NULL) {
		sprite_batch->_param_texture = texture;
	} else {
		sprite_batch->_param_texture = sprite_batch->_texture_blank;
	}
}

void sprite_batch_effect(sprite_batch_t* sprite_batch, NULLABLE gpu_shader_t* effect) {
	// Don't flush if we don't actually need to
	if (effect == sprite_batch->_param_effect) {
		return;
	}

	sprite_batch_flush(sprite_batch);
	if (effect != NULL) {
		sprite_batch->_param_effect = effect;
	} else {
		sprite_batch->_param_effect = sprite_batch->_default_effect;
	}
}

void sprite_batch_reset_params(sprite_batch_t* sprite_batch) {
	sprite_batch_rect(sprite_batch, NULL);
	sprite_batch_src_rect(sprite_batch, NULL);
	sprite_batch_color(sprite_batch, NULL);	
	sprite_batch_origin(sprite_batch, NULL);
	sprite_batch_rot(sprite_batch, 0.0f);
	sprite_batch_texture(sprite_batch, NULL);
	sprite_batch_effect(sprite_batch, NULL);
}

gpu_shader_t* sprite_batch_effect_create(sprite_batch_t* sprite_batch, arena_t* arena, const char* frag_source) {
	gpu_shader_t* effect = gpu_shader_create(sprite_batch->_gpu, arena);
	gpu_shader_upload_source(effect, _sprite_batch_vert_shader, frag_source);
	gpu_shader_bind_uniform_buffer(effect, "sprite_batch_b", sprite_batch->_buffer);
	return effect;
}

void sprite_batch_destroy(sprite_batch_t* sprite_batch) {
	// TODO: Implement
	UNIMPLEMENTED_FUNCTION();
}

gpu_t* sprite_batch_gpu(const sprite_batch_t* sprite_batch) {
	return sprite_batch->_gpu;
}
