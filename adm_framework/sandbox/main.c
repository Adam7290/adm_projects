#include <adm_framework/app.h>
#include <adm_framework/input.h>
#include <adm_framework/gpu.h>
#include <adm_framework/time.h>
#include <adm_framework/image.h>
#include <adm_framework/dbgtext.h>
#include <adm_framework/sprite_batch.h>

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
	gpu_texture_t* sad = gpu_texture_create(gpu, &arena);
	{ image_t img = image_create_from_file(&arena, "assets/sad.png"); gpu_texture_upload(sad, &img); }

	app_show(app);
	while (app_frame(app) == true) {
		gpu_clear(gpu, input_key_down(app, INPUT_KEY_SPACE) ? COLOR_WHITE : COLOR_BLACK);

		sprite_batch_start(sprite_batch);

		sprite_batch_texture(sprite_batch, sad);
		sprite_batch_rect(sprite_batch, &(vec4_t){ 100.0, 100.0, 100.0, 100.0 });
		sprite_batch_src_rect(sprite_batch, &(vec4_t){ 0.5, 0.5, 1.0, 1.0 });
		sprite_batch_color(sprite_batch, &(vec4_t){ 1.0, 0.3, 0.5, 1.0 });
		sprite_batch_draw(sprite_batch);

		sprite_batch_texture(sprite_batch, NULL);
		sprite_batch_rect(sprite_batch, &(vec4_t){ 300.0, 200.0, 250.0, 50.0 });
		sprite_batch_src_rect(sprite_batch, NULL);
		sprite_batch_color(sprite_batch, &(vec4_t){ 0.5, 0.5, 1.0 });
		sprite_batch_draw(sprite_batch);

		sprite_batch_texture(sprite_batch, happy);
		sprite_batch_src_rect(sprite_batch, NULL);
		sprite_batch_color(sprite_batch, &(vec4_t){ 1.0, 1.0, 1.0, 1.0 });
		for (int i = 0; i < 1024; i++) {
			sprite_batch_rect(sprite_batch, &(vec4_t){ i * 10, 0, 10, 10 });
			sprite_batch_draw(sprite_batch);
		}

		sprite_batch_end(sprite_batch);

		dbgtext_println(dbgtext,
		 	"Elapsed: {}s\nDelta: {}\nFPS: {}", 
		 	FORMAT(int, time_app_elapsed(app).time), 
		 	FORMAT(float, time_app_delta(app).time, 3), 
		 	FORMAT(int, app_frames_per_second(app))
		 );

		dbgtext_flush(dbgtext);

		if (input_key_down(app, INPUT_KEY_ESCAPE) == true) {
			app_exit(app);	
		}
	}

	arena_destroy(&arena);
}
