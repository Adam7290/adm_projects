#include "time.h"
#include "app.h"

#include <adm_utils/arena.h>

#include <GLFW/glfw3.h>

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

    // Make delta not have a huge number for first frame
    app->_time->last = time_stamp_new(glfwGetTime());
}

void _time_frame(app_t* app) {
    app->_time->last = app->_time->current;
    app->_time->current = time_stamp_new(glfwGetTime());
    app->_time->delta = time_stamp_sub(&app->_time->current, &app->_time->last);
}

time_stamp_t time_app_elapsed(app_t* app) {
    return app->_time->current;
}

time_stamp_t time_app_delta(app_t* app) {
    return app->_time->delta;
}