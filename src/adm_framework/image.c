#include "image.h"

#define STB_IMAGE_IMPLEMENTATION
#include "external/stb_image.h" 

#include <adm_utils/arena.h>
#include <adm_utils/panic.h>
#include <adm_utils/format.h>

PRIVATE void _image_free_internal(image_t* image) {
	if (image->_data != NULL) {
		stbi_image_free(image->_data);
	}
}

image_t image_create_from_file(arena_t* arena, const char* filename) {
	int width, height, channels;
	byte* data = stbi_load(filename, &width, &height, &channels, 4);

	if (data == NULL) {
		PANIC("Failed to load image: {}", FORMAT(cstr, filename));
	}

	image_t* image = arena_defer(arena, _image_free_internal, image_t);
	*image = (image_t){
		._data = data,
		._width = width,
		._height = height,
		._arena = arena,
	};
	return *image;
}

const byte* image_data(const image_t* image) {
	return image->_data;
}

uint image_width(const image_t* image) {
	return image->_width;
}

uint image_height(const image_t* image) {
	return image->_height;
}

void image_free(image_t* image) {
	if (image->_arena != NULL) {
		arena_free(image->_arena, image);
	}

	memset(image, 0, sizeof(image_t));
}
