#include "framework/test.h"

#include <stdio.h>

#include <adm_utils/arena.h>
#include <adm_utils/string.h>

#define VEC_IMPLEMENTATION
#define VEC_TEMPLATE string_t
#define VEC_TEMPLATE_DISPLAY_NAME string
#include <adm_utils/vec_impl.h>

void test_iter() {
    arena_t arena = arena_new();

    string_t test = string_new_empty(&arena);
    vec_string_t vec = vec_string_new(&arena);
    vec_string_push(&vec, string_new(&arena, "Adam"));
    vec_string_push(&vec, string_new(&arena, "Michael"));
    vec_string_push(&vec, string_new(&arena, "Bauder"));
    vec_string_push(&vec, string_new(&arena, "Lucina"));
    vec_string_push(&vec, string_new(&arena, "Rose"));
    vec_string_push(&vec, string_new(&arena, "Seymour"));

    for (iter_t it = vec_string_begin(&vec); !iter_equals(it, vec_string_end(&vec)); it = iter_next(it)) {
        string_concat(&test, iter_element(it));
    }
    PANIC_ASSERT(string_equals_cstr(&test, "AdamMichaelBauderLucinaRoseSeymour") == true, "");

    arena_destroy(&arena);
}