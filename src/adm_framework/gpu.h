#pragma once

#include "color.h"

typedef struct app_t app_t;
typedef struct gpu_t {

} gpu_t;

void _gpu_init(app_t* app);
void gpu_clear(app_t* app, color_t color);