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
    app->_window = glfwCreateWindow(def->width, def->height, def->title, NULL, NULL);
    glfwMakeContextCurrent(app->_window);

    // Defer glfw terminate
    arena_defer(arena, glfwTerminate, void);

    return app;
}

bool app_frame(app_t* app) {
    if (glfwWindowShouldClose(app->_window) == GLFW_TRUE) {
        return false;
    }

    glfwSwapBuffers(app->_window);
    glfwPollEvents();
    
    return true;
}

arena_t* app_arena(app_t* app) {
    return app->_arena;
}

void* app_window(app_t* app) {
    return app->_window;
}