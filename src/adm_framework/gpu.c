#define VEC_IMPLEMENTATION
#include "gpu.h"
#include "app.h"

#include <adm_utils/panic.h>
#include <adm_utils/string.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdarg.h>

void _gpu_init(app_t* app) {
    PANIC_ASSERT(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == 1, "Failed to load GLAD.");
}

void _gpu_frame(app_t* app) {
    app_window_size_t framebuffer_size = app_framebuffer_size(app);
    glViewport(0, 0, framebuffer_size.width, framebuffer_size.height);
    glUseProgram(0);
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
    verts->_buffer_length = 0;

    glGenVertexArrays(1, &verts->_vao);
    glGenBuffers(1, &verts->_vbo);

    glBindVertexArray(verts->_vao);
    glBindBuffer(GL_ARRAY_BUFFER, verts->_vbo);

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
        glEnableVertexAttribArray(i);
        glVertexAttribPointer(i, vert->size, GL_FLOAT, GL_FALSE, stride, (void*)offset); // MAGIC GL_FLOAT
        offset += vert->size * sizeof(float); // MAGIC TYPE SIZEOF
    }

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return verts;
}

void gpu_verts_upload(gpu_verts_t* verts, void* ptr, usize length) {
    glBindBuffer(GL_ARRAY_BUFFER, verts->_vbo);
    glBufferData(GL_ARRAY_BUFFER, length, ptr, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    verts->_buffer_length = length;
}

void gpu_verts_draw(gpu_verts_t* verts) {
    glBindVertexArray(verts->_vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
}

void gpu_verts_destroy(gpu_verts_t* verts) {
    arena_free(verts->_arena, verts);
}

PRIVATE void _gpu_shader_destroy_internal(gpu_shader_t* shader) {
    glDeleteProgram(shader->_handle);
}

gpu_shader_t* gpu_shader_create(app_t* app, arena_t* arena) {
    gpu_shader_t* shader = arena_defer(arena, _gpu_shader_destroy_internal, gpu_shader_t);
    
    shader->_app = app;
    shader->_arena = arena;
    shader->_handle = glCreateProgram();

    return shader;
}

void gpu_shader_upload_source(gpu_shader_t* shader, const char* vertex_source, const char* fragment_source) {
    GLint success;

    u32 vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertex_source, NULL);
    glCompileShader(vertex);
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
        GLint length;
        glGetShaderiv(vertex, GL_INFO_LOG_LENGTH, &length);
        
        arena_t arena = arena_new();
        char* error = arena_alloc_raw(&arena, length * sizeof(char));

        glGetShaderInfoLog(vertex, length, &length, error);
        glDeleteShader(vertex);

        PANIC("Vertex shader compilation failed:\n%s", error);
    }

    u32 fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragment_source, NULL);
    glCompileShader(fragment);
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
        GLint length;
        glGetShaderiv(fragment, GL_INFO_LOG_LENGTH, &length);

        arena_t arena = arena_new();
        char* error = arena_alloc_raw(&arena, length * sizeof(char));

        glGetShaderInfoLog(fragment, length, &length, error);
        glDeleteShader(vertex);
        glDeleteShader(fragment);

        PANIC("Fragment shader compilation failed:\n%s", error);
    }

    glAttachShader(shader->_handle, vertex);
    glAttachShader(shader->_handle, fragment);
    glLinkProgram(shader->_handle);

    glGetProgramiv(shader->_handle, GL_LINK_STATUS, &success);
    if (success == GL_FALSE) {
        GLint length;
        glGetProgramiv(shader->_handle, GL_INFO_LOG_LENGTH, &length);

        arena_t arena = arena_new();
        char* error = arena_alloc_raw(&arena, length * sizeof(char));

        glGetProgramInfoLog(shader->_handle, length, &length, error);
        glDeleteProgram(shader->_handle);

        PANIC("Shader program linking failed:\n%s", error);
    }

    glDetachShader(shader->_handle, vertex);
    glDetachShader(shader->_handle, fragment);
}

void gpu_shader_use(NULLABLE gpu_shader_t* shader) {
    if (shader != NULL) {
        glUseProgram(shader->_handle);
    } else {
        glUseProgram(0);
    }
}

void gpu_shader_set_f32(gpu_shader_t* shader, const char* name, f32 value) {
    glProgramUniform1f(shader->_handle, glGetUniformLocation(shader->_handle, name), value);
}

void gpu_shader_destroy(gpu_shader_t* shader) {
    arena_free(shader->_arena, shader);
}