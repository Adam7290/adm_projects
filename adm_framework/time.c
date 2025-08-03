#include "time.h"
#include "app.h"

#include <adm_utils/arena.h>

#include <GLFW/glfw3.h>

typedef struct _time_t {
	app_t* app;
    time_stamp_t current;
    time_stamp_t last;
    time_stamp_t delta;
} _time_t;

time_stamp_t time_stamp_new(f64 time_stamp) {
    return (time_stamp_t){ time_stamp };
}

time_stamp_t time_stamp_add(time_stamp_t* time_stamp_1, time_stamp_t* time_stamp_2) {
    return time_stamp_new(time_stamp_1->time + time_stamp_2->time);
}

time_stamp_t time_stamp_sub(time_stamp_t* time_stamp_1, time_stamp_t* time_stamp_2) {
    return time_stamp_new(time_stamp_1->time - time_stamp_2->time);
}

void _time_init(app_t* app) {
    app->_time = arena_alloc(app->_arena, _time_t);
	app->_time->app = app;

    // Make delta not have a huge number for first frame
    app->_time->last = time_stamp_new(glfwGetTime());
}

void _time_frame(_time_t* time) {
    time->last = time->current;
    time->current = time_stamp_new(glfwGetTime());
    time->delta = time_stamp_sub(&time->current, &time->last);
}

time_stamp_t time_app_elapsed(app_t* app) {
    return app->_time->current;
}

time_stamp_t time_app_last(app_t* app) {
	return app->_time->last;
}

time_stamp_t time_app_delta(app_t* app) {
    return app->_time->delta;
}
