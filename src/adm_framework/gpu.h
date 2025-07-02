#pragma once

#include "color.h"

typedef struct app_t app_t;
typedef struct arena_t arena_t;

typedef struct gpu_t {

} gpu_t;

typedef enum gpu_vert_attr_type_t {
    GPU_VERT_ATTR_TYPE_FLOAT,
    _gpu_vert_attr_type_count,
} gpu_vert_attr_type_t;

typedef struct gpu_vert_attr_t {
    usize size;
    gpu_vert_attr_type_t type;
} gpu_vert_attr_t;

#define VEC_TEMPLATE gpu_vert_attr_t
#define VEC_TEMPLATE_PREFIX _gpu_vert_decl_
#include <adm_utils/vec_impl.h>

typedef struct _gpu_vert_decl_t gpu_vert_decl_t;
typedef struct gpu_verts_t {
    u32 _vbo, _vao;
    usize _vertex_size, _buffer_length;
    app_t* _app;
    arena_t* _arena;
} gpu_verts_t;

typedef struct gpu_shader_t {
    u32 _handle;
    app_t* _app;
    arena_t* _arena;
} gpu_shader_t;

void _gpu_init(app_t* app);
void _gpu_frame(app_t* app);
void gpu_clear(app_t* app, color_t color);
gpu_vert_decl_t gpu_vert_decl_new(app_t* app, usize count, ...);
gpu_verts_t* gpu_verts_create(app_t* app, arena_t* arena, gpu_vert_decl_t* vert_decl);
void gpu_verts_upload(gpu_verts_t* verts, void* ptr, usize length);
void gpu_verts_draw(gpu_verts_t* verts);
void gpu_verts_destroy(gpu_verts_t* verts);
gpu_shader_t* gpu_shader_create(app_t* app, arena_t* arena);
void gpu_shader_upload_source(gpu_shader_t* shader, const char* vertex_source, const char* fragment_source);
void gpu_shader_use(NULLABLE gpu_shader_t* shader);
void gpu_shader_destroy(gpu_shader_t* shader);