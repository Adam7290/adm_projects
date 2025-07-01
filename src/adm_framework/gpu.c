#include "gpu.h"

#include <adm_utils/panic.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

void _gpu_init(app_t* app) {
    PANIC_ASSERT(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == 1, "Failed to load GLAD.");
}

void gpu_clear(app_t* app, color_t color) {
    glClearColor(
        color.red / 255.0f, 
        color.green / 255.0f, 
        color.blue / 255.0f, 
        color.alpha / 255.0f
    );
    glClear(GL_COLOR_BUFFER_BIT);
}