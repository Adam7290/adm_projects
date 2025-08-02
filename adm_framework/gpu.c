#include "gpu.h"
#include "app.h"
#include "image.h"
#include "gmath.h"

#include <adm_utils/panic.h>
#include <adm_utils/string.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdarg.h>

typedef struct gpu_t {
	app_t* app;
} gpu_t;

void _gpu_init(app_t* app) {
	gpu_t* gpu = arena_alloc(app->_arena, gpu_t);
	app->_gpu = gpu;
	gpu->app = app;

    PANIC_ASSERT(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == 1, "Failed to load GLAD.");

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void _gpu_frame(gpu_t* gpu) {
	app_t* app = gpu_app(gpu);
    app_window_size_t framebuffer_size = app_framebuffer_size(app);
    glViewport(0, 0, framebuffer_size.width, framebuffer_size.height);
    glUseProgram(0);
}

app_t* gpu_app(gpu_t* gpu) {
	return gpu->app;
}

void gpu_clear(gpu_t* gpu, const color4b_t* color) {
    glClearColor(
        color->r / 255.0f, 
        color->g / 255.0f, 
        color->b / 255.0f, 
        color->a / 255.0f
    );
    glClear(GL_COLOR_BUFFER_BIT);
}

void gpu_wireframe(gpu_t* gpu, bool wireframe) {
	glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
}

PRIVATE void _gpu_verts_destroy_internal(gpu_verts_t* verts) {
    glDeleteVertexArrays(1, &verts->_vao);
    glDeleteBuffers(1, &verts->_vbo);
	glDeleteBuffers(1, &verts->_ebo);
}

gpu_verts_t* gpu_verts_create(gpu_t* gpu, arena_t* arena, bool has_indices, gpu_attribute_t attributes[]) {
    gpu_verts_t* verts = arena_defer(arena, _gpu_verts_destroy_internal, gpu_verts_t);
    verts->_arena = arena;
	verts->_gpu = gpu;
    verts->_buffer_length = 0;

    glGenVertexArrays(1, &verts->_vao);
    glGenBuffers(1, &verts->_vbo);

	if (has_indices == true) {
		glGenBuffers(1, &verts->_ebo);
	}

    glBindVertexArray(verts->_vao);
    glBindBuffer(GL_ARRAY_BUFFER, verts->_vbo);

	// Calculate total_attributes and stride
	usize total_attributes = 0;
	usize stride = 0;
	{
		gpu_attribute_t* attr = attributes;
		while (attr->size != 0) {
			total_attributes++;	
			stride += attr->size * sizeof(float); // MAGIC TYPE SIZEOF
			attr++;
		}
	}
    verts->_vertex_size = stride;

    // Actually setup opengl vertex attribs
    usize offset = 0;
    for (int i = 0; i < total_attributes; i++) {
		gpu_attribute_t attr = attributes[i];
        glEnableVertexAttribArray(i);
        glVertexAttribPointer(i, attr.size, GL_FLOAT, GL_FALSE, stride, (void*)offset); // MAGIC GL_FLOAT
        offset += attr.size * sizeof(float); // MAGIC TYPE SIZEOF
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

void gpu_verts_draw_instanced(gpu_verts_t* verts, usize count) {
	glBindVertexArray(verts->_vao);
	if (verts->_ebo == 0) {
		glDrawArraysInstanced(GL_TRIANGLES, 0, verts->_buffer_length / verts->_vertex_size, count);
	} else {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, verts->_ebo);
		glDrawElementsInstanced(GL_TRIANGLES, verts->_index_buffer_length / sizeof(gpu_index_t), GL_UNSIGNED_SHORT, NULL, count);
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

gpu_shader_t* gpu_shader_create(gpu_t* gpu, arena_t* arena) {
    gpu_shader_t* shader = arena_defer(arena, _gpu_shader_destroy_internal, gpu_shader_t);
    
	shader->_gpu = gpu;
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

gpu_uniform_t gpu_shader_get_uniform(gpu_shader_t* shader, const char* name) {
	return (gpu_uniform_t)glGetUniformLocation(shader->_handle, name);
}

void gpu_shader_set_float(gpu_shader_t* shader, gpu_uniform_t uniform, float value) {
	gpu_shader_use(shader);
    glUniform1f(uniform, value);
}

void gpu_shader_set_int(gpu_shader_t* shader, gpu_uniform_t uniform, int value) {
	gpu_shader_use(shader);
    glUniform1i(uniform, value);
}

void gpu_shader_set_vec2i(gpu_shader_t* shader, gpu_uniform_t uniform, int x, int y) {
	gpu_shader_use(shader);
	glUniform2i(uniform, x, y);
}

void gpu_shader_set_vec2f(gpu_shader_t* shader, gpu_uniform_t uniform, const vec2f_t* vec2) {
	gpu_shader_use(shader);
	glUniform2fv(uniform, 1, vec2->array);
}

void gpu_shader_set_vec3f(gpu_shader_t* shader, gpu_uniform_t uniform, const vec3f_t* vec3) {
	gpu_shader_use(shader);
	glUniform3fv(uniform, 1, vec3->array);
}

void gpu_shader_set_mat4x4(gpu_shader_t* shader, gpu_uniform_t uniform, const mat4x4_t* mat4x4) {
	gpu_shader_use(shader);
	glUniformMatrix4fv(uniform, 1, GL_FALSE, (float*)mat4x4);
}

void gpu_shader_bind_uniform_buffer(gpu_shader_t* shader, const char* name, gpu_uniform_buffer_t* buffer) {
	glUniformBlockBinding(shader->_handle, glGetUniformBlockIndex(shader->_handle, name), buffer->_binding_point); 
}

void gpu_shader_destroy(gpu_shader_t* shader) {
    arena_free(shader->_arena, shader);
}

PRIVATE void _gpu_texture_destroy_internal(gpu_texture_t* texture) {
	glDeleteTextures(1, &texture->_handle);
}

gpu_texture_t* gpu_texture_create(gpu_t* gpu, arena_t* arena) {
	gpu_texture_t* texture = arena_defer(arena, _gpu_texture_destroy_internal, gpu_texture_t);

	texture->_gpu = gpu;
	texture->_arena = arena;
	glGenTextures(1, &texture->_handle);
	return texture;
}

PRIVATE GLenum _gpu_image_format_to_ogl_format(image_format_t format) {
	switch (format) {
		case IMAGE_FORMAT_RED:
			return GL_RED;
		case IMAGE_FORMAT_GREEN:
			return GL_GREEN;
		case IMAGE_FORMAT_BLUE:
			return GL_BLUE;
		case IMAGE_FORMAT_RG:
			return GL_RG;
		case IMAGE_FORMAT_RGB:
			return GL_RGB;
		case IMAGE_FORMAT_RGBA:
			return GL_RGBA;
		default:
			PANIC("Invalid image_format_t: {}", FORMAT(int, format));
	}
	return 0;
}

void gpu_texture_upload_raw(gpu_texture_t* texture, const byte* ptr, uint width, uint height, image_format_t format) {
	glBindTexture(GL_TEXTURE_2D, texture->_handle);

	// TODO: User ability to set these parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	GLenum gl_format = _gpu_image_format_to_ogl_format(format);
	glTexImage2D(GL_TEXTURE_2D, 0, gl_format, width, height, 0, gl_format, GL_UNSIGNED_BYTE, ptr);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void gpu_texture_upload(gpu_texture_t* texture, const image_t* image) {
	gpu_texture_upload_raw(texture, image_data(image), image_width(image), image_height(image), image_format(image));
}

void gpu_texture_set_filter(gpu_texture_t* texture, gpu_texture_filter_t filter) {
	GLenum gl_filter;
	switch (filter) {
		case GPU_TEXTURE_FILTER_NEAREST: gl_filter = GL_NEAREST; break;
		case GPU_TEXTURE_FILTER_LINEAR:  gl_filter = GL_LINEAR; break;
		default: gl_filter = GL_LINEAR; break;
	}

	glBindTexture(GL_TEXTURE_2D, texture->_handle);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, gl_filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, gl_filter);
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

PRIVATE void _gpu_uniform_buffer_destroy_internal(gpu_uniform_buffer_t* buffer) {
	glDeleteBuffers(1, &buffer->_handle);
}

gpu_uniform_buffer_t* gpu_uniform_buffer_create(gpu_t* gpu, arena_t* arena) {
	gpu_uniform_buffer_t* buffer = arena_defer(arena, _gpu_uniform_buffer_destroy_internal, gpu_uniform_buffer_t);

	buffer->_arena = arena;
	buffer->_gpu = gpu;
	
	glGenBuffers(1, &buffer->_handle);
	
	return buffer;
}

void gpu_uniform_buffer_upload(gpu_uniform_buffer_t* buffer, usize length, NULLABLE const void* data) {
	glBindBuffer(GL_UNIFORM_BUFFER, buffer->_handle);
	glBufferData(GL_UNIFORM_BUFFER, length, data, GL_STREAM_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	static usize s_binding = 0;
	glBindBufferRange(GL_UNIFORM_BUFFER, s_binding, buffer->_handle, 0, length);
	buffer->_binding_point = s_binding;
	s_binding++;
}

void gpu_uniform_buffer_upload_sub(gpu_uniform_buffer_t* buffer, usize offset, usize length, const void* data) {
	glBindBuffer(GL_UNIFORM_BUFFER, buffer->_handle);
	glBufferSubData(GL_UNIFORM_BUFFER, offset, length, data);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void gpu_uniform_buffer_destroy(gpu_uniform_buffer_t* buffer) {
	arena_free(buffer->_arena, buffer);
}
