#pragma once

#include "linmath.h"
#include <adm_utils/util.h>

typedef struct color_t {
    u8 red, green, blue, alpha;
} color_t;

#define COLOR_BLACK ((color_t){ 0, 0, 0, 255 })
#define COLOR_WHITE ((color_t){ 255, 255, 255, 255 })
