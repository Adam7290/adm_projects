#pragma once

#include <adm_utils/util.h>

typedef enum image_format_t {
	IMAGE_FORMAT_INVALID = -1,
	IMAGE_FORMAT_RED,
	IMAGE_FORMAT_GREEN,
	IMAGE_FORMAT_BLUE,
	IMAGE_FORMAT_RG,
	IMAGE_FORMAT_RGB,
	IMAGE_FORMAT_RGBA,
} image_format_t;

typedef struct arena_t arena_t;
typedef struct image_t {
	byte* _data;
	uint _width, _height;
	image_format_t _format;
	// If _arena is NULL the image struct isn't managing memory
	NULLABLE arena_t* _arena;
} image_t;

image_t image_create_from_file(arena_t* arena, const char* filename);
const byte* image_data(const image_t* image);
uint image_width(const image_t* image);
uint image_height(const image_t* image);
image_format_t image_format(const image_t* image);
void image_free(image_t* image);
