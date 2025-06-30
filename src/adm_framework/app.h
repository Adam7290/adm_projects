#include <adm_utils/arena.h>
#include <stdbool.h>

typedef struct app_def_t {
    const char* title;
    u32 width, height;
} app_def_t;

typedef struct app_t {
    arena_t* _arena;
    void* _window;
} app_t;

app_t* app_new(arena_t* arena, const app_def_t* def);
bool app_frame(app_t* app);
arena_t* app_arena(app_t* app);
void* app_window(app_t* app);