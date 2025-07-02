#define VEC_IMPLEMENTATION
#include "gpu.h"
#include "app.h"

#include <adm_utils/panic.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdarg.h>

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

gpu_vert_decl_t gpu_vert_decl_new(app_t* app, usize count, ...) {
    va_list args;
    va_start(args, count);

    gpu_vert_decl_t decl = _gpu_vert_decl_new(app->_arena);
    _gpu_vert_decl_reserve(&decl, count);
    for (int i = 0; i < count; i++) {
        gpu_vert_attr_t attr = va_arg(args, gpu_vert_attr_t);
        _gpu_vert_decl_push(&decl, attr);
    }

    va_end(args);

    return decl;
}

PRIVATE void _gpu_verts_destroy_internal(gpu_verts_t* verts) {
    glDeleteVertexArrays(1, &verts->_vao);
    glDeleteBuffers(1, &verts->_vbo);
}

gpu_verts_t* gpu_verts_create(app_t* app, arena_t* arena, gpu_vert_decl_t* vert_decl) {
    gpu_verts_t* verts = arena_defer(arena, _gpu_verts_destroy_internal, gpu_verts_t);
    verts->_arena = arena;
    verts->_app = app;

    glGenVertexArrays(1, &verts->_vao);
    glGenBuffers(1, &verts->_vbo);

    glBindVertexArray(verts->_vao);


    // Calculate stride
    usize stride = 0;
    for (int i = 0; i < _gpu_vert_decl_length(vert_decl); i++) {
        gpu_vert_attr_t* vert = _gpu_vert_decl_get_unchecked(vert_decl, i);
        stride += vert->size * sizeof(float); // MAGIC TYPE SIZEOF
    }
    verts->_vertex_size = stride;

    // Actually setup opengl vertex attribs
    usize offset = 0;
    for (int i = 0; i < _gpu_vert_decl_length(vert_decl); i++) {
        gpu_vert_attr_t* vert = _gpu_vert_decl_get_unchecked(vert_decl, i);
        glVertexAttribPointer(i, vert->size, GL_FLOAT, GL_FALSE, stride, (void*)offset); // MAGIC GL_FLOAT
        glEnableVertexAttribArray(i);
        offset += vert->size * sizeof(float); // MAGIC TYPE SIZEOF
    }

    glBindVertexArray(0);

    return verts;
}

void gpu_verts_upload(gpu_verts_t* verts, void* ptr, usize length) {
    glBindBuffer(GL_ARRAY_BUFFER, verts->_vbo);
    glBufferData(GL_ARRAY_BUFFER, length, ptr, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void gpu_verts_draw(gpu_verts_t* verts) {
    glBindVertexArray(verts->_vao);
    glDrawArrays(GL_TRIANGLES, 0, verts->_vertex_size);
    glBindVertexArray(0);
}

void gpu_verts_destroy(gpu_verts_t* verts) {
    arena_free(verts->_arena, verts);
}