#include "app.h"
#include "input.h"
#include "gpu.h"
#include "time.h"

#include <adm_utils/util.h>
#include <adm_utils/panic.h>
#include <GLFW/glfw3.h>

PRIVATE bool _app_already_created = false;

app_t* app_new(arena_t* arena, const app_def_t* def) {
    PANIC_ASSERT(_app_already_created == false, "You can only have one app created at this time.");
    _app_already_created = true;

    app_t* app = arena_alloc(arena, app_t);

    app->_arena = arena;

    // Setup window
    PANIC_ASSERT(glfwInit() == GLFW_TRUE, "GLFW failed to initialize.");
    
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, def->debug);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    app->_window = glfwCreateWindow(def->window_size.width, def->window_size.height, def->title, NULL, NULL);
    PANIC_ASSERT(app->_window != NULL, "Failed to create GLFW window.");

    glfwMakeContextCurrent(app->_window);
    glfwSetWindowUserPointer(app->_window, app);

    // Defer glfw terminate
    arena_defer(arena, glfwTerminate, void);

    // Init components
    _time_init(app);
    _input_init(app);
    _gpu_init(app);

    return app;
}

void app_show(app_t* app) {
    glfwShowWindow(app->_window);
}

void app_hide(app_t* app) {
    glfwHideWindow(app->_window);
}

void app_exit(app_t* app) {
	glfwSetWindowShouldClose(app->_window, true);
}

bool app_frame(app_t* app) {
    if (glfwWindowShouldClose(app->_window) == GLFW_TRUE) {
        return false;
    }

    _time_frame(app);
    _input_frame(app);
    _gpu_frame(app);

    glfwSwapBuffers(app->_window);
    glfwPollEvents();
    
    return true;
}

u64 app_frame_count(app_t* app) {
    return app->_frame_count;
}

u64 app_frames_per_second(app_t* app) {
    return 1.0f / time_app_delta(app).time;
}

app_window_size_t app_framebuffer_size(app_t* app) {
    int width, height;
    glfwGetFramebufferSize(app->_window, &width, &height);
    return (app_window_size_t){ (u32)width, (u32)height };
}

app_window_size_t app_window_size(app_t* app) {
    int width, height;
    glfwGetFramebufferSize(app->_window, &width, &height);
    return (app_window_size_t){ (u32)width, (u32)height };
}

void app_set_window_size(app_t* app, const app_window_size_t* size) {
    glfwSetWindowSize(app->_window, size->width, size->height);
}

// TODO: get framebuffer size func

const char* app_title(app_t* app) {
    return glfwGetWindowTitle(app->_window);
}

void app_set_title(app_t* app, const char* title) {
    glfwSetWindowTitle(app->_window, title);
}

arena_t* app_arena(app_t* app) {
    return app->_arena;
}

void* app_window(app_t* app) {
    return app->_window;
}
