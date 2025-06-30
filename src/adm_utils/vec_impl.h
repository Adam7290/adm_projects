#include "util.h"
#include "arena.h"
#include "panic.h"
#include "iter.h"

#include <string.h>
#include <stdbool.h>

#ifndef VEC_TEMPLATE
#error You need to define VEC_TEMPLATE before including this header
#endif

#ifdef VEC_TEMPLATE_DISPLAY_NAME
#define VEC_NAME VEC_TEMPLATE_DISPLAY_NAME
#else
#define VEC_NAME VEC_TEMPLATE
#endif

#ifndef VEC_TEMPLATE_PREFIX
#define VEC_TEMPLATE_PREFIX CONCAT3_EXPAND(vec_, VEC_NAME, _)
#endif

#define VEC_SYMBOL(after) CONCAT_EXPAND(VEC_TEMPLATE_PREFIX, after)
#define VEC_PRIVATE_SYMBOL(after) CONCAT_EXPAND(_, VEC_SYMBOL(after))
#define VEC_TYPE VEC_SYMBOL(t)

// Dynamically growable linear array
typedef struct VEC_TYPE {
    arena_t* _arena;
    usize _length;
    usize _capacity;
    NULLABLE ARENA_MANAGED(VEC_TEMPLATE) _ptr;
} VEC_TYPE;

// TODO: Descriptions of methods

VEC_TYPE VEC_SYMBOL(new)(arena_t* arena);
VEC_TYPE VEC_SYMBOL(new_with_data)(arena_t* arena, VEC_TEMPLATE src[], usize size);
VEC_TYPE VEC_SYMBOL(clone)(VEC_TYPE* vec);
void VEC_SYMBOL(reallocate)(VEC_TYPE* vec, usize new_size);
void VEC_SYMBOL(reserve)(VEC_TYPE* vec, usize size);
void VEC_SYMBOL(shrink)(VEC_TYPE* vec);
void VEC_SYMBOL(set_length)(VEC_TYPE* vec, usize length);
usize VEC_SYMBOL(length)(VEC_TYPE* vec);
usize VEC_SYMBOL(capacity)(VEC_TYPE* vec);
ARENA_MANAGED(VEC_TEMPLATE) VEC_SYMBOL(ptr)(VEC_TYPE* vec);
bool VEC_SYMBOL(empty)(VEC_TYPE* vec);
void VEC_SYMBOL(push)(VEC_TYPE* vec, VEC_TEMPLATE element);
void VEC_SYMBOL(push_array)(VEC_TYPE* vec, VEC_TEMPLATE src[], usize size);
bool VEC_SYMBOL(pop)(VEC_TYPE* vec, VEC_TEMPLATE* out);
VEC_TEMPLATE VEC_SYMBOL(pop_unwrap)(VEC_TYPE* vec);
VEC_TEMPLATE* VEC_SYMBOL(get_unchecked)(VEC_TYPE* vec, usize index);
NULLABLE VEC_TEMPLATE* VEC_SYMBOL(get)(VEC_TYPE* vec, usize index);
VEC_TEMPLATE* VEC_SYMBOL(get_unwrap)(VEC_TYPE* vec, usize index);
void VEC_SYMBOL(set_unchecked)(VEC_TYPE* vec, usize index, VEC_TEMPLATE element);
void VEC_SYMBOL(set)(VEC_TYPE* vec, usize index, VEC_TEMPLATE element);
void VEC_SYMBOL(set_array)(VEC_TYPE* vec, usize index, VEC_TEMPLATE src[], usize size);
void VEC_SYMBOL(insert)(VEC_TYPE* vec, usize index, VEC_TEMPLATE element);
// TODO: Replace return value with next iter
VEC_TEMPLATE VEC_SYMBOL(remove)(VEC_TYPE* vec, usize index);
void VEC_SYMBOL(clear)(VEC_TYPE* vec);
void VEC_SYMBOL(free)(VEC_TYPE* vec);

#ifdef VEC_IMPLEMENTATION

const usize VEC_PRIVATE_SYMBOL(minimum_capacity) = 8;

#define VEC_PRIVATE_SYMBOL(after) CONCAT_EXPAND(_, VEC_SYMBOL(after))

VEC_TYPE VEC_SYMBOL(new)(arena_t* arena) {
    return (VEC_TYPE){
        ._arena = arena,
        ._length = 0,
        ._capacity = 0,
        ._ptr = NULL,
    };
}

VEC_TYPE VEC_SYMBOL(new_with_data)(arena_t* arena, VEC_TEMPLATE src[], usize size) {
    VEC_TYPE vec = VEC_SYMBOL(new)(arena);

    VEC_SYMBOL(reserve)(&vec, size);
    memcpy(vec._ptr, src, size * sizeof(VEC_TEMPLATE));

    return vec;
}

// TODO: Pass in arena to allocate new VEC in
VEC_TYPE VEC_SYMBOL(clone)(VEC_TYPE* vec) {
    // If VEC is empty just return a new empty VEC
    if (vec->_ptr == NULL || vec->_capacity == 0) {
        return VEC_SYMBOL(new)(vec->_arena);
    }

    ARENA_MANAGED(VEC_TEMPLATE) copied_ptr = arena_alloc_copy_raw(vec->_arena, vec->_ptr);
    return (VEC_TYPE){
        ._arena = vec->_arena,
        ._length = vec->_length,
        ._capacity = vec->_length,
        ._ptr = copied_ptr,
    };
}

void VEC_SYMBOL(reallocate)(VEC_TYPE* vec, usize new_size) {
    ARENA_MANAGED(VEC_TEMPLATE) new_ptr = arena_alloc_raw(vec->_arena, sizeof(VEC_TEMPLATE) * new_size);
    vec->_capacity = new_size;

    if (vec->_ptr != NULL) {
        // If our newly allocated block is smaller we need to change the length
        if (new_size < vec->_length) {
            vec->_length = new_size;
        }

        memcpy(new_ptr, vec->_ptr, sizeof(VEC_TEMPLATE) * vec->_length);
        arena_free(vec->_arena, vec->_ptr);
    }

    vec->_ptr = new_ptr;
}

void VEC_SYMBOL(reserve)(VEC_TYPE* vec, usize size) {
    if (vec->_capacity < size) {
        VEC_SYMBOL(reallocate)(vec, size);
    }
}

void VEC_SYMBOL(shrink)(VEC_TYPE* vec) {
    // If vec has no allocated data
    if (vec->_ptr == NULL) {
        return;
    }

    // We can't shrink at all without losing data
    if (vec->_capacity == vec->_length) {
        return;
    }

    // If length is zero, BUT we have something allocated
    if (vec->_length == 0 && vec->_ptr != NULL) {
        VEC_SYMBOL(free)(vec);
        return;
    }

    // Shrink
    VEC_SYMBOL(reallocate)(vec, vec->_length);
}

void VEC_SYMBOL(set_length)(VEC_TYPE* vec, usize length) {
    PANIC_ASSERT(length >= 0, "Length must be greater than zero.");
    if (length > vec->_capacity) {
        VEC_SYMBOL(reserve)(vec, length);
    }
    
    vec->_length = length;
}

usize VEC_SYMBOL(length)(VEC_TYPE* vec) {
    return vec->_length;
}

usize VEC_SYMBOL(capacity)(VEC_TYPE* vec) {
    return vec->_capacity;
}

ARENA_MANAGED(VEC_TEMPLATE) VEC_SYMBOL(ptr)(VEC_TYPE* vec) {
    return vec->_ptr;
}

bool VEC_SYMBOL(empty)(VEC_TYPE* vec) {
    return VEC_SYMBOL(length)(vec) == 0;
}

void VEC_SYMBOL(push)(VEC_TYPE* vec, VEC_TEMPLATE element) {
    // VEC growth
    if (vec->_capacity == 0) {
        VEC_SYMBOL(reserve)(vec, VEC_PRIVATE_SYMBOL(minimum_capacity));
    } else if (vec->_length + 1 > vec->_capacity) {
        VEC_SYMBOL(reallocate)(vec, vec->_capacity * 2);
    }

    vec->_ptr[vec->_length] = element;
    vec->_length++;
}

void VEC_SYMBOL(push_array)(VEC_TYPE* vec, VEC_TEMPLATE src[], usize size) {
    VEC_SYMBOL(reserve)(vec, vec->_length + size);

    // TODO: Replace with memset for optimization
    for (usize i = 0; i < size; i++) {
        VEC_SYMBOL(push)(vec, src[i]);
    }
}

bool VEC_SYMBOL(pop)(VEC_TYPE* vec, VEC_TEMPLATE* out) {
    if (vec->_length <= 0) {
        return false;
    }

    vec->_length--;
    VEC_TEMPLATE element = vec->_ptr[vec->_length];
    *out = element;

    // If we are using less than half of our reserved capacity
    if (vec->_length >= VEC_PRIVATE_SYMBOL(minimum_capacity) && vec->_length <= vec->_capacity / 2) {
        VEC_SYMBOL(reallocate)(vec, vec->_capacity / 2);
    }

    return true;
}

VEC_TEMPLATE VEC_SYMBOL(pop_unwrap)(VEC_TYPE* vec) {
    VEC_TEMPLATE value;
    bool success = VEC_SYMBOL(pop)(vec, &value);

    if (!success) {
        PANIC("Nothing left in vec to pop.");
    }

    return value;
}

VEC_TEMPLATE* VEC_SYMBOL(get_unchecked)(VEC_TYPE* vec, usize index) {
    return vec->_ptr + index;
}

NULLABLE VEC_TEMPLATE* VEC_SYMBOL(get)(VEC_TYPE* vec, usize index) {
    if (index < 0 || index >= vec->_length) {
        return NULL;
    }

    return vec->_ptr + index;
}

VEC_TEMPLATE* VEC_SYMBOL(get_unwrap)(VEC_TYPE* vec, usize index) {
    NULLABLE VEC_TEMPLATE* value = VEC_SYMBOL(get)(vec, index);

    if (value == NULL) {
        PANIC("Index outside of bounds.");
    }

    return value;
}

void VEC_SYMBOL(set_unchecked)(VEC_TYPE* vec, usize index, VEC_TEMPLATE element) {
    vec->_ptr[index] = element;
}

void VEC_SYMBOL(set)(VEC_TYPE* vec, usize index, VEC_TEMPLATE element) {
    PANIC_ASSERT(index < vec->_length, "Index outside of bounds.");
    PANIC_ASSERT(index >= 0, "Index less than zero.");
    vec->_ptr[index] = element;
}

void VEC_SYMBOL(set_array)(VEC_TYPE* vec, usize index, VEC_TEMPLATE src[], usize size) {
    PANIC_ASSERT(index + size <= vec->_length, "Index range outside of bounds");
    PANIC_ASSERT(index >= 0, "Index less than zero.");

    memcpy(vec->_ptr + index, src, size * sizeof(VEC_TEMPLATE));
}

void VEC_SYMBOL(insert)(VEC_TYPE* vec, usize index, VEC_TEMPLATE element) {
    PANIC_ASSERT(index < vec->_length, "Index outside of bounds.");
    PANIC_ASSERT(index >= 0, "Index less than zero.");
    
    VEC_SYMBOL(reserve)(vec, VEC_SYMBOL(length)(vec) + 1);
    VEC_SYMBOL(set_length)(vec, VEC_SYMBOL(length)(vec) + 1);
    for (int i = VEC_SYMBOL(length)(vec) - 1; i > index && i > 0; i--) {
        VEC_SYMBOL(set_unchecked)(vec, i, *VEC_SYMBOL(get_unchecked)(vec, i - 1));
    }
    VEC_SYMBOL(set_unchecked)(vec, index, element);
}

VEC_TEMPLATE VEC_SYMBOL(remove)(VEC_TYPE* vec, usize index) {
    VEC_TEMPLATE element = *VEC_SYMBOL(get_unwrap)(vec, index);

    for (int i = index; i < VEC_SYMBOL(length)(vec) - 1; i++) {
        VEC_SYMBOL(set_unchecked)(vec, i, *VEC_SYMBOL(get_unchecked)(vec, i + 1));
    }

    vec->_length--;

    return element;
}

void VEC_SYMBOL(clear)(VEC_TYPE* vec) {
    vec->_length = 0;
}

void VEC_SYMBOL(free)(VEC_TYPE* vec) {
    if (vec->_ptr != NULL) {
        arena_free(vec->_arena, vec->_ptr);
        vec->_ptr = NULL;
    }
    vec->_capacity = 0;
    vec->_length = 0;
}

// ITERATOR STUFF //

PRIVATE iter_t VEC_PRIVATE_SYMBOL(iter_next_func)(const iter_t self) {
    iter_t ret = self;
    ret._element += sizeof(VEC_TEMPLATE);
    return ret;
}

PRIVATE iter_t VEC_PRIVATE_SYMBOL(iter_prev_func)(const iter_t self) {
    iter_t ret = self;
    ret._element -= sizeof(VEC_TEMPLATE);
    return ret;
}

PRIVATE iter_t VEC_PRIVATE_SYMBOL(iter_access_func)(const iter_t self, usize offset) {
    iter_t ret = self;
    ret._element += offset * sizeof(VEC_TEMPLATE);
}

PRIVATE isize VEC_PRIVATE_SYMBOL(iter_distance_func)(const iter_t first, iter_t last) {
    return (last._element - first._element) / sizeof(VEC_TEMPLATE);
}

PRIVATE iter_vtable_t VEC_PRIVATE_SYMBOL(iter_vtable) = (iter_vtable_t){
    .next_func = VEC_PRIVATE_SYMBOL(iter_next_func),
    .prev_func = VEC_PRIVATE_SYMBOL(iter_prev_func),
    .access_func = VEC_PRIVATE_SYMBOL(iter_access_func),
    .distance_func = VEC_PRIVATE_SYMBOL(iter_distance_func),
};

iter_t VEC_SYMBOL(begin)(VEC_TYPE* vec) {
    return (iter_t){
        ._container = vec,
        ._element = VEC_SYMBOL(get)(vec, 0),
        ._vtable = &VEC_PRIVATE_SYMBOL(iter_vtable),
    };
}

iter_t VEC_SYMBOL(end)(VEC_TYPE* vec) {
    return (iter_t){
        ._container = vec,
        ._element = vec->_ptr + VEC_SYMBOL(length)(vec),
        ._vtable = &VEC_PRIVATE_SYMBOL(iter_vtable),
    };
}

#undef VEC_PRIVATE_SYMBOL
#endif // VEC_IMPLEMENTATION

#undef VEC_TEMPLATE
#undef VEC_TEMPLATE_OPTIONAL
#undef VEC_TEMPLATE_PREFIX
#undef VEC_TEMPLATE_DISPLAY_NAME
#undef VEC_NAME
#undef VEC_SYMBOL
#undef VEC_PRIVATE_SYMBOL
#undef VEC_TYPE
#undef VEC_IMPLEMENTATION