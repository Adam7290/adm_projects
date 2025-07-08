// TODO: Const iters

#pragma once
#include <stdbool.h>
#include "util.h"

struct iter_vtable_t;
typedef struct iter_t {
    const void* _container;
    const void* _element;
    const struct iter_vtable_t* _vtable;
} iter_t;

typedef struct iter_t iterc_t;

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

const void* iterc_container(const iterc_t self);
const void* iterc_element(const iterc_t self);

void* iter_container(const iter_t self);
void* iter_element(const iter_t self);