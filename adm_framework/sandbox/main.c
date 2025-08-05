#include <adm_framework/app.h>
#include <adm_framework/input.h>
#include <adm_framework/gpu.h>
#include <adm_framework/time.h>
#include <adm_framework/image.h>
#include <adm_framework/input.h>
#include <adm_framework/dbgtext.h>
#include <adm_framework/sprite_batch.h>
#include <adm_framework/gmath.h>

#define NK_IMPLEMENTATION
#include <adm_framework/nuklear_adm.h>
#include <adm_framework/external/Nuklear/demo/common/overview.c>

#include <adm_utils/arena.h>
#include <adm_utils/console.h>

int main() {
	arena_t arena = arena_new();
	app_t* app = app_new(&arena, &(app_def_t){
		.title = "Test Window",
		.window_size = (app_window_size_t){ 1280, 720 },
		.debug = true,
	});

	gpu_t* gpu = app_gpu(app);
	sprite_batch_t* sprite_batch = sprite_batch_new(gpu, &arena);
	dbgtext_t* dbgtext = dbgtext_new(sprite_batch, &arena);


	nk_adm_t* nk = nk_adm_new(gpu, &arena);
	{ struct nk_font_atlas* atlas;
		nk_adm_font_stash_begin(nk, &atlas);
		nk_adm_font_stash_end(nk);
	}

	gpu_texture_t* happy = gpu_texture_create(gpu, &arena);
	{ image_t img = image_create_from_file(&arena, "assets/smile.png"); gpu_texture_upload(happy, &img); }

	vec4f_t platforms[1] = { (vec4f_t){-200, -200, 500, 100} };
	vec2f_t player = (vec2f_t){0, 0};

	struct nk_colorf bg;
    bg.r = 0.10f, bg.g = 0.18f, bg.b = 0.24f, bg.a = 1.0f;

	app_show(app);
	while (app_frame(app) == true) {
		dbgtext_println(dbgtext, "Elapsed: {}s", FORMAT(int, time_app_elapsed(app).time));
		dbgtext_println(dbgtext, "Delta: {}", FORMAT(float, time_app_delta(app).time, 3));
		dbgtext_println(dbgtext, "FPS: {}", FORMAT(int, app_frames_per_second(app))); 

		uint codepoint = input_codepoint(app);
		if (codepoint == 0) { codepoint = ' '; }
		dbgtext_println(dbgtext, "Codepoint: {}", FORMAT(char, codepoint)); 

		// UPDATE
		const float speed = 200.f;
		const float delta = time_app_delta(app).time;
		if (input_key_down(app, INPUT_KEY_W) == true) {
			player.y += speed * delta;
		} else if (input_key_down(app, INPUT_KEY_S) == true) {
			player.y -= speed * delta;
		}
		
		if (input_key_down(app, INPUT_KEY_A) == true) {
			player.x -= speed * delta;
		} else if (input_key_down(app, INPUT_KEY_D) == true) {
			player.x += speed * delta;
		}

		dbgtext_println(dbgtext, "Player Pos: ({}, {})", FORMAT(int, player.x), FORMAT(int, player.y));
		dbgtext_println(dbgtext, "Mouse Pos: ({}, {})", FORMAT(int, input_mouse_pos(app).x), FORMAT(int, input_mouse_pos(app).y));

		// DRAW
		gpu_clear(gpu, &(color4b_t){ .r = bg.r * 255, .g = bg.g * 255, .b = bg.b * 255, .a = bg.a * 255 });

		gpu_wireframe(gpu, input_key_down(app, INPUT_KEY_F));
		sprite_batch_start(sprite_batch);

		float zoom = sin(time_app_elapsed(app).time);
		app_window_size_t wind_size = app_window_size(app);
		mat4x4_t view = mat4x4_identity();
		mat4x4_translate(&view, &(vec3f_t){ wind_size.width / 2.f, wind_size.height / 2.f });
		sprite_batch_view(sprite_batch, &view);
		
		sprite_batch_texture(sprite_batch, NULL);
		for (int i = 0; i < sizeof(platforms) / sizeof(platforms[0]); i++) {
			sprite_batch_rect(sprite_batch, &platforms[i]);
			sprite_batch_draw(sprite_batch);
		}

		sprite_batch_texture(sprite_batch, happy);
		sprite_batch_origin(sprite_batch, &(vec2f_t){ 0.5, 0.0 });
		sprite_batch_rect(sprite_batch, &(vec4f_t){ player.x, player.y, 250, 250 });	
		sprite_batch_draw(sprite_batch);

		sprite_batch_end(sprite_batch);

		dbgtext_flush(dbgtext);

		// GUI
		nk_adm_new_frame(nk);

		struct nk_context* ctx = nk_adm_ctx(nk);

		overview(ctx);	

		if (nk_begin(ctx, "Demo", nk_rect(50, 50, 230, 250),
            NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
            NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE))
        {
            enum {EASY, HARD};
            static int op = EASY;
            static int property = 20;
            nk_layout_row_static(ctx, 30, 80, 1);
            if (nk_button_label(ctx, "button"))
                fprintf(stdout, "button pressed\n");

			nk_layout_row_static(ctx, 30, 250, 1);
			static char buffer[256] = "This is some content";
			nk_edit_string_zero_terminated(
				ctx,
				NK_EDIT_BOX | NK_EDIT_AUTO_SELECT,
				buffer, sizeof(buffer), nk_filter_ascii
			);

            nk_layout_row_dynamic(ctx, 30, 2);
            if (nk_option_label(ctx, "easy", op == EASY)) op = EASY;
            if (nk_option_label(ctx, "hard", op == HARD)) op = HARD;

            nk_layout_row_dynamic(ctx, 25, 1);
            nk_property_int(ctx, "Compression:", 0, &property, 100, 10, 1);

            nk_layout_row_dynamic(ctx, 20, 1);
            nk_label(ctx, "background:", NK_TEXT_LEFT);
            nk_layout_row_dynamic(ctx, 25, 1);
            if (nk_combo_begin_color(ctx, nk_rgb_cf(bg), nk_vec2(nk_widget_width(ctx),400))) {
                nk_layout_row_dynamic(ctx, 120, 1);
                bg = nk_color_picker(ctx, bg, NK_RGBA);
                nk_layout_row_dynamic(ctx, 25, 1);
                bg.r = nk_propertyf(ctx, "#R:", 0, bg.r, 1.0f, 0.01f,0.005f);
                bg.g = nk_propertyf(ctx, "#G:", 0, bg.g, 1.0f, 0.01f,0.005f);
                bg.b = nk_propertyf(ctx, "#B:", 0, bg.b, 1.0f, 0.01f,0.005f);
                bg.a = nk_propertyf(ctx, "#A:", 0, bg.a, 1.0f, 0.01f,0.005f);
                nk_combo_end(ctx);
            }
        }
        nk_end(ctx);

		nk_adm_render(nk, NK_ANTI_ALIASING_ON, 512 * 1024, 128 * 1024);

		if (input_key_down(app, INPUT_KEY_ESCAPE) == true) {
			app_exit(app);	
		}
	}

	arena_destroy(&arena);
}
