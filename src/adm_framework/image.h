#pragma once

#include <adm_utils/util.h>

typedef struct arena_t arena_t;
typedef struct image_t {
	byte* _data;
	uint _width, _height;
	arena_t* _arena;
} image_t;

image_t image_create_from_file(arena_t* arena, const char* filename);
const byte* image_data(const image_t* image);
uint image_width(const image_t* image);
uint image_height(const image_t* image);
void image_free(image_t* image);
