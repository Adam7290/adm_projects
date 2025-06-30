#pragma once
#include <stdlib.h>
#include "util.h"

// Macros made to make types managed by an arena easier to understand
#define ARENA_MANAGED(type) type*
#define ARENA_MANAGED_OPAQUE ARENA_MANAGED(void)

// Allocator function typedefs
typedef void*(*arena_malloc_t)(usize size);
typedef void(*arena_free_t)(void* ptr);

// Returns old arena_malloc_t
arena_malloc_t arena_allocator_set_malloc(arena_malloc_t new_func);
// Returns old arena_free_t
arena_free_t arena_allocator_set_free(arena_free_t new_func);

typedef void(*arena_defer_t)(void* user_data);

// Linked list node for defers
typedef struct _arena_defer_node {
    usize user_data_size;
    u64 tag;
    NULLABLE arena_defer_t func;
    NULLABLE struct _arena_defer_node* next;
    NULLABLE struct _arena_defer_node* prev;
} _arena_defer_node;

// Object lifetime manager
typedef struct arena_t {
    NULLABLE _arena_defer_node* _defer_head;
    NULLABLE _arena_defer_node* _defer_tail;
} arena_t;

// Create a new arena
arena_t arena_new();
// Defer a function with raw user_data size, use arena_defer for a cleaner way.
ARENA_MANAGED_OPAQUE arena_defer_raw(arena_t* arena, NULLABLE arena_defer_t func, NULLABLE usize user_data_size);
// Allocate raw bytes that are managed by the arena
ARENA_MANAGED_OPAQUE arena_alloc_raw(arena_t* arena, usize size);
// Copy an allocated node in arena
ARENA_MANAGED_OPAQUE arena_alloc_copy_raw(arena_t* arena, ARENA_MANAGED_OPAQUE mem);
// Free memory allocated in arena before the arena is cleaned/destroyed
void arena_free(arena_t* arena, ARENA_MANAGED_OPAQUE mem);
// Calls all defered functions and frees all memory in arena
void arena_cleanup(arena_t* arena);
// Right now does the exact same thing as arena_cleanup, use this function when the arena isn't gonna be used again
void arena_destroy(arena_t* arena);
// Add an arena that will be destroyed up by a parent arena
ARENA_MANAGED(arena_t) arena_new_child_arena(arena_t* parent);

// Allocates memory with the size of the type passed in, for raw allocation use arena_alloc_raw
#define arena_alloc(arena, type) ((ARENA_MANAGED(type))arena_alloc_raw(arena, sizeof(type)))
// Defer a function with a passed in user_data type, pass void for no user_data
#define arena_defer(arena, func, type) ((ARENA_MANAGED(type))arena_defer_raw(arena, (arena_defer_t)func, sizeof(type)))
// Copy an allocated node in arena
#define arena_alloc_copy(arena, mem, type) ((ARENA_MANAGED(type))arena_alloc_copy_raw(arena, mem))