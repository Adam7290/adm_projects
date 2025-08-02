#pragma once

#include <adm_utils/util.h>

typedef struct gpu_t gpu_t;
typedef struct app_t app_t;
typedef struct arena_t arena_t;
typedef struct image_t image_t;
typedef enum image_format_t image_format_t;

typedef struct color4b_t color4b_t;
typedef struct vec2f_t vec2f_t;
typedef struct vec3f_t vec3f_t;
typedef struct vec4f_t vec4f_t;
typedef struct mat4x4_t mat4x4_t;

typedef enum gpu_attribute_type_t {
    GPU_ATTRIBUTE_TYPE_FLOAT,
    GPU_ATTRIBUTE_TYPE_BYTE,
    _gpu_attribute_type_count,
} gpu_attribute_type_t;

typedef struct gpu_attribute_t {
    usize size;
    gpu_attribute_type_t type;
} gpu_attribute_t;

typedef ushort gpu_index_t;

typedef struct gpu_verts_t {
    uint _vbo, _vao, _ebo;
    usize _vertex_size, _buffer_length;
	usize _index_buffer_length;
	gpu_t* _gpu;
    arena_t* _arena;
} gpu_verts_t;

typedef struct gpu_shader_t {
    uint _handle;
	gpu_t* _gpu;
    arena_t* _arena;
} gpu_shader_t;

typedef int gpu_uniform_t;

typedef struct gpu_texture_t {
	uint _handle;
	gpu_t* _gpu;
	arena_t* _arena;
} gpu_texture_t;

typedef enum gpu_texture_filter_t {
	GPU_TEXTURE_FILTER_NEAREST,
	GPU_TEXTURE_FILTER_LINEAR,
} gpu_texture_filter_t;

// TODO: Make into generic buffer api
typedef struct gpu_uniform_buffer_t {
	uint _handle;
	gpu_t* _gpu;
	arena_t* _arena;
	usize _binding_point;
} gpu_uniform_buffer_t;

void _gpu_init(app_t* app);
void _gpu_frame(gpu_t* gpu);
app_t* gpu_app(gpu_t* gpu);

void gpu_clear(gpu_t* gpu, const color4b_t* color);
void gpu_wireframe(gpu_t* gpu, bool wireframe);

gpu_verts_t* gpu_verts_create(gpu_t* gpu, arena_t* arena, bool has_indices, gpu_attribute_t attributes[]);
void gpu_verts_upload(gpu_verts_t* verts, const void* ptr, usize length);
void gpu_verts_upload_indices(gpu_verts_t* verts, const gpu_index_t* ptr, usize length); 

void gpu_verts_draw(gpu_verts_t* verts);
void gpu_verts_draw_instanced(gpu_verts_t* verts, usize count);
void gpu_verts_destroy(gpu_verts_t* verts);

gpu_shader_t* gpu_shader_create(gpu_t* gpu, arena_t* arena);
void gpu_shader_upload_source(gpu_shader_t* shader, const char* vertex_source, const char* fragment_source);
void gpu_shader_use(NULLABLE gpu_shader_t* shader);
void gpu_shader_set_float(gpu_shader_t* shader, gpu_uniform_t uniform, float value);
void gpu_shader_set_int(gpu_shader_t* shader, gpu_uniform_t uniform, int value); 
// TODO: Add all the uniform functions
gpu_uniform_t gpu_shader_get_uniform(gpu_shader_t* shader, const char* name);
void gpu_shader_set_vec2i(gpu_shader_t* shader, gpu_uniform_t uniform, int x, int y);
void gpu_shader_set_vec2f(gpu_shader_t* shader, gpu_uniform_t uniform, const vec2f_t* vec2);
void gpu_shader_set_vec3f(gpu_shader_t* shader, gpu_uniform_t uniform, const vec3f_t* vec3);
void gpu_shader_set_mat4x4(gpu_shader_t* shader, gpu_uniform_t uniform, const mat4x4_t* mat4x4);
void gpu_shader_bind_uniform_buffer(gpu_shader_t* shader, const char* name, gpu_uniform_buffer_t* buffer);
void gpu_shader_destroy(gpu_shader_t* shader);

gpu_texture_t* gpu_texture_create(gpu_t* gpu, arena_t* arena);
void gpu_texture_upload_raw(gpu_texture_t* texture, const byte* ptr, uint width, uint height, image_format_t format);
void gpu_texture_upload(gpu_texture_t* texture, const image_t* image);
void gpu_texture_set_filter(gpu_texture_t* texture, gpu_texture_filter_t filter);
void gpu_texture_use(NULLABLE gpu_texture_t* texture, uint channel);
void gpu_texture_destroy(gpu_texture_t* texture);

gpu_uniform_buffer_t* gpu_uniform_buffer_create(gpu_t* gpu, arena_t* arena);
// Pass NULL to data to just allocate buffer
void gpu_uniform_buffer_upload(gpu_uniform_buffer_t* buffer, usize length, NULLABLE const void* data);
void gpu_uniform_buffer_upload_sub(gpu_uniform_buffer_t* buffer, usize offset, usize length, const void* data); 
void gpu_uniform_buffer_destroy(gpu_uniform_buffer_t* buffer);

void gpu_scissor_begin(gpu_t* gpu, const vec4f_t* rect);
void gpu_scissor_end(gpu_t* gpu);
