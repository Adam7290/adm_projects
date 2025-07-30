#include <adm_framework/app.h>
#include <adm_framework/input.h>
#include <adm_framework/gpu.h>
#include <adm_framework/time.h>
#include <adm_framework/image.h>
#include <adm_framework/input.h>
#include <adm_framework/dbgtext.h>
#include <adm_framework/sprite_batch.h>
#include <adm_framework/gmath.h>

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

	gpu_texture_t* happy = gpu_texture_create(gpu, &arena);
	{ image_t img = image_create_from_file(&arena, "assets/smile.png"); gpu_texture_upload(happy, &img); }

	vec4f_t platforms[1] = { (vec4f_t){0, 0, 500, 100} };
	vec2f_t player = (vec2f_t){0, 0};

	app_show(app);
	while (app_frame(app) == true) {
		dbgtext_println(dbgtext, "Elapsed: {}s", FORMAT(int, time_app_elapsed(app).time));
		dbgtext_println(dbgtext, "Delta: {}", FORMAT(float, time_app_delta(app).time, 3));
		dbgtext_println(dbgtext, "FPS: {}", FORMAT(int, app_frames_per_second(app))); 

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

		// DRAW
		gpu_clear(gpu, &(color4b_t){ .hex = 0x00000000 });

		gpu_wireframe(gpu, input_key_down(app, INPUT_KEY_F));
		sprite_batch_start(sprite_batch);

		float zoom = sin(time_app_elapsed(app).time);
		app_window_size_t wind_size = app_window_size(app);
		mat4x4_t view = mat4x4_identity();
		mat4x4_scale(&view, &(vec3f_t){ zoom, zoom, 1.0f });
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
		gpu_wireframe(gpu, false);

		dbgtext_flush(dbgtext);

		if (input_key_down(app, INPUT_KEY_ESCAPE) == true) {
			app_exit(app);	
		}
	}

	arena_destroy(&arena);
}
