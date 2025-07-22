// TODO: REAL const iters

#pragma once
#include <stdbool.h>
#include "util.h"

struct iter_vtable_t;
// A generic interface for iterating through various containers
// Example of looping through a container with iter_t:
// vec_int_t vec = vec_int_new blah blah;
// for (iter_t it = vec_int_begin(&vec); !iter_equals(it, vec_int_end(&vec)); it = iter_next(it)) {
//     console_println("{}", FORMAT(int, *iter_element(it));
// }
typedef struct iter_t {
    const void* _container;
    const void* _element;
    const struct iter_vtable_t* _vtable;
} iter_t;

// An iterator with that cannot modify the container or its elements
typedef struct iter_t iterc_t;

// Table of methods for the iter_t interface
// Any vtable should specify which methods it implements (TODO).
typedef struct iter_vtable_t {
	iter_t(*next_func)(const iter_t self);
    iter_t(*prev_func)(const iter_t self);
    iter_t(*access_func)(const iter_t self, usize offset);
    isize(*distance_func)(const iter_t first, const iter_t last);
} iter_vtable_t;

// Return the next element
iter_t iter_next(const iter_t self);
// Return the previous element
iter_t iter_prev(const iter_t self);
// Return the iter with a specified index offset from self iter 
iter_t iter_access(const iter_t self, usize offset);
// Return the number of elements between the specified iterators
isize iter_distance(const iter_t first, const iter_t last);
// If the iterators point to the same element
bool iter_equals(const iter_t iter1, const iter_t iter2);

// Returns a const pointer to the container
const void* iterc_container(const iterc_t self);
// Returns a const pointer to the element
const void* iterc_element(const iterc_t self);

// Returns a pointer to the container
void* iter_container(const iter_t self);
// Returns a pointer to the element
void* iter_element(const iter_t self);
