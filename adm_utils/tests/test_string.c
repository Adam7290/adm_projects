#include "framework/test.h"

#include <adm_utils/util.h>
#include <adm_utils/string.h>
#include <adm_utils/arena.h>

PRIVATE void test_constructors(arena_t* arena) {
    string_t empty = string_new_empty(arena);
    PANIC_ASSERT(string_equals_cstr(&empty, ""), "Empty string isn't empty.");

    string_t hello = string_new(arena, "Hello");
    PANIC_ASSERT(string_equals_cstr(&hello, "Hello"), "Hello string doesn't match \"Hello\"");

    string_t hello_duplicate = string_clone(&hello);
    PANIC_ASSERT(string_ptr(&hello) != string_ptr(&hello_duplicate), "hello_duplicate isn't a duplicate");
    PANIC_ASSERT(string_equals(&hello, &hello_duplicate), "hello and hello_duplicate don't match");
}

void test_string() {
    arena_t arena = arena_new();
    test_constructors(&arena);
    arena_destroy(&arena);
}