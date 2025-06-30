#include "framework/test.h"

#include <adm_utils/arena.h>
#include <adm_utils/panic.h>
#include <stdio.h>

PRIVATE i32 s_alloc_counter = 0;

PRIVATE void* _debug_malloc_func(usize size) {
    // printf("Malloc\n");
    s_alloc_counter++;
    return malloc(size);
}

PRIVATE void _debug_free_func(void* ptr) {
    // printf("Free!\n");
    s_alloc_counter--;
    return free(ptr);
}

PRIVATE i32 test_defer_num_to_change = 0;
PRIVATE void test_defer_func(i32* num) {
    test_defer_num_to_change = *num;
}

PRIVATE void actual_test() {
    arena_t root_arena = arena_new();

    ARENA_MANAGED(arena_t) child_arena = arena_new_child_arena(&root_arena);
    ARENA_MANAGED(arena_t) grand_child_arena = arena_new_child_arena(child_arena);

    ARENA_MANAGED(i32) test = arena_alloc(grand_child_arena, i32);
    *test = 123;
    arena_free(grand_child_arena, test);

    *arena_defer(grand_child_arena, test_defer_func, i32) = 666;

    ARENA_MANAGED(arena_t) great_grand_child_arena = arena_new_child_arena(grand_child_arena);
    ARENA_MANAGED(char) other_test = arena_alloc(grand_child_arena, char);
    arena_destroy(grand_child_arena);

    arena_destroy(&root_arena);
    
    PANIC_ASSERT(test_defer_num_to_change == 666, "test_defer_num_to_change wasn't changed.");
    PANIC_ASSERT(s_alloc_counter == 0, "Memory leaks detected, check s_alloc_counter.");
}

void test_arena() {
    arena_malloc_t prev_malloc = arena_allocator_set_malloc(_debug_malloc_func);
    arena_free_t prev_free = arena_allocator_set_free(_debug_free_func);

    actual_test();

    arena_allocator_set_malloc(prev_malloc);
    arena_allocator_set_free(prev_free);
}