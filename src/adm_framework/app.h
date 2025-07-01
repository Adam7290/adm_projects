#pragma once

#include <adm_utils/arena.h>
#include <stdbool.h>

typedef struct app_window_size_t {
    u32 width, height;
} app_window_size_t;

typedef struct app_def_t {
    const char* title;
    app_window_size_t window_size;
} app_def_t;

typedef struct input_t input_t;
typedef struct gpu_t gpu_t;
typedef struct app_t {
    arena_t* _arena;
    // User pointer points to this app
    void* _window;
    input_t* _input;
    gpu_t* _gpu;
} app_t;

app_t* app_new(arena_t* arena, const app_def_t* def);
void app_show(app_t* app);
void app_hide(app_t* app);
bool app_frame(app_t* app);
app_window_size_t app_window_size(app_t* app);
void app_set_window_size(app_t* app, const app_window_size_t* size);
const char* app_title(app_t* app);
void app_set_title(app_t* app, const char* title);
arena_t* app_arena(app_t* app);
void* app_window(app_t* app);