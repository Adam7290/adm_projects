#pragma once
#include <stdbool.h>
#include "util.h"

struct iter_vtable_t;
typedef struct iter_t {
    void* _container;
    void* _element;
    struct iter_vtable_t* _vtable;
} iter_t;

typedef struct iter_vtable_t {
    iter_t(*next_func)(const iter_t self);
    iter_t(*prev_func)(const iter_t self);
    iter_t(*access_func)(const iter_t self, usize offset);
    isize(*distance_func)(const iter_t first, const iter_t last);
} iter_vtable_t;

iter_t iter_next(const iter_t self);
iter_t iter_prev(const iter_t self);
iter_t iter_access(const iter_t self, usize offset);
isize iter_distance(const iter_t first, const iter_t last);
bool iter_equals(const iter_t iter1, const iter_t iter2);

void* iter_container(const iter_t self);
void* iter_element(const iter_t self);