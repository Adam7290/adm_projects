#include <adm_utils/arena.h>
#include <adm_framework/app.h>
#include <adm_framework/input.h>

#include <stdio.h>

int main() {
    arena_t arena = arena_new();
    app_t* app = app_new(&arena, &(app_def_t){
        .title = "Test Window",
        .window_size = (app_window_size_t){ 1280, 720 },
    });

    app_show(app);
    while (app_frame(app) == true) {
        input_button_state_t state = input_key_state(app, INPUT_KEY_W);
        if (state != INPUT_BUTTON_STATE_RELEASED) {
            printf("W key state: %i\n", state);
        }
    }

    arena_destroy(&arena);
}