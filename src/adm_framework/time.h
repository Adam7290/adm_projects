#pragma once

#include <adm_utils/util.h>

typedef struct time_stamp_t {
    f64 time;
} time_stamp_t;

typedef struct _time_t {
    time_stamp_t current;
    time_stamp_t last;
    time_stamp_t delta;
} _time_t;

time_stamp_t time_stamp_new(f64 time_stamp);
time_stamp_t time_stamp_add(time_stamp_t* time_stamp_1, time_stamp_t* time_stamp_2);
time_stamp_t time_stamp_sub(time_stamp_t* time_stamp_1, time_stamp_t* time_stamp_2);

typedef struct app_t app_t;
void _time_init(app_t* app);
void _time_frame(app_t* app);
time_stamp_t time_app_elapsed(app_t* app);
time_stamp_t time_app_delta(app_t* app);