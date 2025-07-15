#define VEC_IMPLEMENTATION
#include "gpu.h"
#include "app.h"
#include "image.h"

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
	glDeleteBuffers(1, &verts->_ebo);
}

gpu_verts_t* gpu_verts_create(app_t* app, arena_t* arena, gpu_vert_decl_t* vert_decl, bool has_indices) {
    gpu_verts_t* verts = arena_defer(arena, _gpu_verts_destroy_internal, gpu_verts_t);
    verts->_arena = arena;
    verts->_app = app;
    verts->_buffer_length = 0;

    glGenVertexArrays(1, &verts->_vao);
    glGenBuffers(1, &verts->_vbo);

	if (has_indices == true) {
		glGenBuffers(1, &verts->_ebo);
	}

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
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    return verts;
}

void gpu_verts_upload(gpu_verts_t* verts, const void* ptr, usize length) {
	glBindVertexArray(verts->_vao);
    glBindBuffer(GL_ARRAY_BUFFER, verts->_vbo);
    glBufferData(GL_ARRAY_BUFFER, length, ptr, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

    verts->_buffer_length = length;
}

void gpu_verts_upload_indices(gpu_verts_t* verts, const gpu_index_t* ptr, usize length) {
	glBindVertexArray(verts->_vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, verts->_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, length, ptr, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	verts->_index_buffer_length = length;
}

void gpu_verts_draw(gpu_verts_t* verts) {
	glBindVertexArray(verts->_vao);
	if (verts->_ebo == 0) {
		glDrawArrays(GL_TRIANGLES, 0, verts->_buffer_length / verts->_vertex_size);
	} else {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, verts->_ebo);
		glDrawElements(GL_TRIANGLES, verts->_index_buffer_length / sizeof(gpu_index_t), GL_UNSIGNED_SHORT, NULL);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
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

        PANIC("Vertex shader compilation failed:\n{}", FORMAT(cstr, error));
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

        PANIC("Fragment shader compilation failed:\n{}", FORMAT(cstr, error));
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

        PANIC("Shader program linking failed:\n{}", FORMAT(cstr, error));
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

void gpu_shader_set_float(gpu_shader_t* shader, const char* name, float value) {
    glProgramUniform1f(shader->_handle, glGetUniformLocation(shader->_handle, name), value);
}


void gpu_shader_set_int(gpu_shader_t* shader, const char* name, int value) {
    glProgramUniform1i(shader->_handle, glGetUniformLocation(shader->_handle, name), value);
}

void gpu_shader_destroy(gpu_shader_t* shader) {
    arena_free(shader->_arena, shader);
}

PRIVATE void _gpu_texture_destroy_internal(gpu_texture_t* texture) {
	glDeleteTextures(1, &texture->_handle);
}

gpu_texture_t* gpu_texture_create(app_t* app, arena_t* arena) {
	gpu_texture_t* texture = arena_defer(arena, _gpu_texture_destroy_internal, gpu_texture_t);

	texture->_app = app;
	texture->_arena = arena;
	glGenTextures(1, &texture->_handle);
	return texture;
}

void gpu_texture_upload(gpu_texture_t* texture, const image_t* image) {
	glBindTexture(GL_TEXTURE_2D, texture->_handle);

	// TODO: User ability to set these parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width(image), image_height(image), 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data(image));

	glBindTexture(GL_TEXTURE_2D, 0);
}

void gpu_texture_use(NULLABLE gpu_texture_t* texture, uint channel) {
	// TODO: Texture channel bounds checking
	glActiveTexture(GL_TEXTURE0 + channel);
	if (texture != NULL) {
		glBindTexture(GL_TEXTURE_2D, texture->_handle);
	} else {
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void gpu_texture_destroy(gpu_texture_t* texture) {
	arena_free(texture->_arena, texture);
}
