#include <adm_utils/arena.h>
#include <adm_utils/iter.h>

#define LIST_IMPLEMENTATION
#define LIST_TEMPLATE i32
#include <adm_utils/list_impl.h>

#define VEC_IMPLEMENTATION
#define VEC_TEMPLATE i32
#include <adm_utils/vec_impl.h>

#include <stdio.h>

int main() {
    arena_t arena = arena_new();
    
    list_i32_t list = list_i32_new(&arena);
    list_i32_push_back(&list, 1);
    list_i32_push_back(&list, 2);
    list_i32_push_back(&list, 3);
    list_i32_push_back(&list, 4);

    for (iter_t it = list_i32_begin(&list); !iter_equals(it, list_i32_end(&list));) {
        it = list_i32_remove(&list, it);
    }

    arena_destroy(&arena);
    return 0;
}