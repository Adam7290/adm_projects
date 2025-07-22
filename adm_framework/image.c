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

PRIVATE image_format_t _image_channels_to_format(uint channels) {
	switch (channels) {
		case 1:
			return IMAGE_FORMAT_RED;
		case 2:
			return IMAGE_FORMAT_RG;
		case 3: 
			return IMAGE_FORMAT_RGB;
		case 4: 
			return IMAGE_FORMAT_RGBA;
		default:
			return IMAGE_FORMAT_INVALID;
	}
}

image_t image_create_from_file(arena_t* arena, const char* filename) {
	// TODO: When we add multiple graphics systems this needs to be sorted out
	stbi_set_flip_vertically_on_load(true);

	int width, height, channels;
	byte* data = stbi_load(filename, &width, &height, &channels, 4);

	if (data == NULL) {
		PANIC("Failed to load image: {}", FORMAT(cstr, filename));
	}

	image_format_t format = _image_channels_to_format(channels);

	image_t* image = arena_defer(arena, _image_free_internal, image_t);
	*image = (image_t){
		._data = data,
		._width = width,
		._height = height,
		._format = format,
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

image_format_t image_format(const image_t* image) {
	return image->_format;
}

void image_free(image_t* image) {
	if (image->_arena != NULL) {
		arena_free(image->_arena, image);
	}

	memset(image, 0, sizeof(image_t));
}
