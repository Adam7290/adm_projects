#pragma once

#include <adm_utils/util.h>

typedef struct gpu_t gpu_t;
typedef struct sprite_batch_t sprite_batch_t;
typedef struct gpu_texture_t gpu_texture_t;
typedef struct gpu_shader_t gpu_shader_t;
typedef struct arena_t arena_t;
typedef struct vec4f_t vec4f_t;
typedef struct vec2f_t vec2f_t;

sprite_batch_t* sprite_batch_new(gpu_t* gpu, arena_t* arena);
void sprite_batch_start(sprite_batch_t* sprite_batch);
void sprite_batch_end(sprite_batch_t* sprite_batch);
void sprite_batch_flush(sprite_batch_t* sprite_batch);
// If src is NULL then draw full texture
void sprite_batch_draw(sprite_batch_t* sprite_batch);
void sprite_batch_rect(sprite_batch_t* sprite_batch, NULLABLE const vec4f_t* rect);
void sprite_batch_src_rect(sprite_batch_t* sprite_batch, NULLABLE const vec4f_t* src_rect);
void sprite_batch_color(sprite_batch_t* sprite_batch, NULLABLE const vec4f_t* color);
void sprite_batch_rot(sprite_batch_t* sprite_batch, float rot);
void sprite_batch_origin(sprite_batch_t* sprite_batch, const vec2f_t* origin);
void sprite_batch_texture(sprite_batch_t* sprite_batch, NULLABLE gpu_texture_t* texture);
void sprite_batch_effect(sprite_batch_t* sprite_batch, NULLABLE gpu_shader_t* effect);
void sprite_batch_reset_params(sprite_batch_t* sprite_batch);
gpu_shader_t* sprite_batch_effect_create(sprite_batch_t* sprite_batch, arena_t* arena, const char* frag_source);
void sprite_batch_destroy(sprite_batch_t* sprite_batch);
gpu_t* sprite_batch_gpu(const sprite_batch_t* sprite_batch);
