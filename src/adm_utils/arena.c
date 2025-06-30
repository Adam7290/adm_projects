#include "arena.h"
#include "panic.h"
#include <stdio.h>
#include <string.h>

PRIVATE arena_malloc_t _arena_malloc_func = malloc;
PRIVATE arena_free_t _arena_free_func = free;

arena_malloc_t arena_allocator_set_malloc(arena_malloc_t new_func) {
    arena_malloc_t old_func = _arena_malloc_func;
    _arena_malloc_func = new_func;
    return old_func;
}

arena_free_t arena_allocator_set_free(arena_free_t new_func) {
    arena_free_t old_func = _arena_free_func;
    _arena_free_func = new_func;
    return old_func;
}

arena_t arena_new() {
    return (arena_t){
        ._defer_head = NULL,
        ._defer_tail = NULL,
    };
}

PRIVATE u64 s_tag_counter = 0;
ARENA_MANAGED_OPAQUE arena_defer_raw(arena_t* arena, NULLABLE arena_defer_t func, NULLABLE usize user_data_size) {
    PANIC_ASSERT_DEBUG(user_data_size >= 0, "");

    // We allocate our node alongside our user_data
    usize size = sizeof(_arena_defer_node) + user_data_size;
    _arena_defer_node* node = _arena_malloc_func(size);
    memset(node, 0, size);

    node->func = func;
    node->user_data_size = user_data_size;
    node->tag = s_tag_counter++;
    
    if (arena->_defer_head == NULL) {
        // Node is the head
        arena->_defer_head = node;
    } else {
        // Node is not the head
        PANIC_ASSERT_DEBUG(arena->_defer_tail != NULL, "");
        node->prev = arena->_defer_tail;
        arena->_defer_tail->next = node;
    }
    
    // Set tail as the latest node
    arena->_defer_tail = node;
    
    // Return pointer to user_data
    return (byte*)node + sizeof(_arena_defer_node);
}

ARENA_MANAGED_OPAQUE arena_alloc_raw(arena_t* arena, usize size) {
    return arena_defer_raw(arena, NULL, size);
}

ARENA_MANAGED_OPAQUE arena_alloc_copy_raw(arena_t* arena, ARENA_MANAGED_OPAQUE mem) {
    _arena_defer_node* node = (_arena_defer_node*)((byte*)mem - sizeof(_arena_defer_node));
    
    ARENA_MANAGED_OPAQUE new = arena_defer_raw(arena, node->func, node->user_data_size);
    memcpy(new, mem, node->user_data_size);
    return new;
}

PRIVATE void _arena_free_node(arena_t* arena, _arena_defer_node* node) {
    PANIC_ASSERT_DEBUG(node != NULL, "");

    // Run defer function if provided
    if (node->func != NULL) {
        node->func((byte*)node + sizeof(_arena_defer_node));
    }
    
    // Update pointers
    if (node == arena->_defer_head) {
        if (node == arena->_defer_tail) {
            // Node is last in list, we now null out the node pointers
            arena->_defer_head = NULL;
            arena->_defer_tail = NULL;
        } else {
            // Node is the head
            PANIC_ASSERT_DEBUG(node->next != NULL, "");
            arena->_defer_head = node->next;
            node->next->prev = NULL;
        }
    } else if (node == arena->_defer_tail) {
        // Node is the tail (but not the head)
        PANIC_ASSERT_DEBUG(node->prev != NULL, "");
        arena->_defer_tail = node->prev;
        arena->_defer_tail->next = NULL;
    } else {
        // Node is a body
        PANIC_ASSERT_DEBUG(node->prev != NULL, "");
        PANIC_ASSERT_DEBUG(node->prev->next != NULL, "");
        PANIC_ASSERT_DEBUG(node->next != NULL, "");
        PANIC_ASSERT_DEBUG(node->next->prev != NULL, "");
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }

    _arena_free_func(node);
}

void arena_free(arena_t* arena, ARENA_MANAGED_OPAQUE mem) {
    PANIC_ASSERT_DEBUG(mem != NULL, "");
    _arena_defer_node* node = (_arena_defer_node*)((byte*)mem - sizeof(_arena_defer_node));
    _arena_free_node(arena, node);
}

void arena_cleanup(arena_t* arena) {
    // Walk through nodes and clean them up starting with the most recent allocation

    _arena_defer_node* node = arena->_defer_tail;
    _arena_defer_node* tmp;
    while (node != NULL) {
        tmp = node->prev;
        _arena_free_node(arena, node);
        node = tmp;
    }

    arena->_defer_head = NULL;
    arena->_defer_tail = NULL;
}

void arena_destroy(arena_t* arena) {
    arena_cleanup(arena);
}

ARENA_MANAGED(arena_t) arena_new_child_arena(arena_t* parent) {
    ARENA_MANAGED(arena_t) child = arena_defer(parent, arena_destroy, arena_t);
    *child = arena_new();
    return child;
}