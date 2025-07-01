#include <adm_utils/arena.h>
#include <adm_framework/app.h>
#include <adm_framework/input.h>
#include <adm_framework/gpu.h>

#include <stdio.h>

int main() {
    arena_t arena = arena_new();
    app_t* app = app_new(&arena, &(app_def_t){
        .title = "Test Window",
        .window_size = (app_window_size_t){ 1280, 720 },
    });

    app_show(app);
    while (app_frame(app) == true) {
        gpu_clear(app, input_key_down(app, INPUT_KEY_SPACE) ? COLOR_WHITE : COLOR_BLACK);
    }

    arena_destroy(&arena);
}