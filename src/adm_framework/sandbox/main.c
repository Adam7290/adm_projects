#include <adm_utils/arena.h>
#include <adm_framework/app.h>

int main() {
    arena_t arena = arena_new();
    app_t* app = app_new(&arena, &(app_def_t){
        .title = "Test Window",
        .width = 1280, .height = 720,
    });

    while (app_frame(app) == true) {
        
    }

    arena_destroy(&arena);
}