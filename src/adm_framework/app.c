#include "app.h"

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
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    app->_window = glfwCreateWindow(def->window_size.width, def->window_size.height, def->title, NULL, NULL);
    glfwMakeContextCurrent(app->_window);

    // Defer glfw terminate
    arena_defer(arena, glfwTerminate, void);

    return app;
}

void app_show(app_t* app) {
    glfwShowWindow(app->_window);
}

void app_hide(app_t* app) {
    glfwHideWindow(app->_window);
}

bool app_frame(app_t* app) {
    if (glfwWindowShouldClose(app->_window) == GLFW_TRUE) {
        return false;
    }

    glfwSwapBuffers(app->_window);
    glfwPollEvents();
    
    return true;
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