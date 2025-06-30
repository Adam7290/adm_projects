#include "framework/test.h"

#include <adm_utils/util.h>
#include <adm_utils/panic.h>
#include <adm_utils/arena.h>

#include <string.h>
#include <stdbool.h>

#define VEC_IMPLEMENTATION
#define VEC_TEMPLATE char
#include <adm_utils/vec_impl.h>

void test_vec_char_content(vec_char_t* vec, const char* cmp) {
    PANIC_ASSERT(memcmp(vec_char_ptr(vec), cmp, vec_char_length(vec)) == 0, "VEC doesn't equal cmp.");
}

void test_vector() {
    arena_t arena = arena_new();
    vec_char_t vec = vec_char_new(&arena);
    test_vec_char_content(&vec, "");
    
    { // Pushing
        vec_char_push(&vec, 'A');
        vec_char_push(&vec, 'b');
        test_vec_char_content(&vec, "Ab");
    }
    
    { // Popping
        char popped;
        vec_char_pop(&vec, &popped); PANIC_ASSERT(popped == 'b', "");
        test_vec_char_content(&vec, "A");

        PANIC_ASSERT(vec_char_pop_unwrap(&vec) == 'A', "");
        test_vec_char_content(&vec, "");

        PANIC_ASSERT(vec_char_pop(&vec, &popped) == false, "");
        TEST_EXPECT_PANIC(vec_char_pop_unwrap(&vec), "");
    }

    { // Pushing arrays
        vec_char_push_array(&vec, "Hello, ", 7);
        vec_char_push_array(&vec, "World!", 6);
        test_vec_char_content(&vec, "Hello, World!");
    }

    { // Getting and setting
        PANIC_ASSERT(*vec_char_get(&vec, 7) == 'W', "");

        vec_char_set(&vec, 7, 'F');
        PANIC_ASSERT(*vec_char_get_unwrap(&vec, 7) == 'F', "");

        PANIC_ASSERT(vec_char_get(&vec, 999) == NULL, "");
        TEST_EXPECT_PANIC(vec_char_get_unwrap(&vec, 2222), "");

        // TODO: More setting tests
    }
    
    { // Clearing
        vec_char_clear(&vec);
        PANIC_ASSERT(vec_char_length(&vec) == 0, "");

        // Double clear
        vec_char_clear(&vec);
    }

    { // Reserving
        vec_char_reserve(&vec, 1024);
        PANIC_ASSERT(vec_char_capacity(&vec) == 1024, "");

        // Check that smaller reserve still holds 1024
        vec_char_reserve(&vec, 100);
        PANIC_ASSERT(vec_char_capacity(&vec) == 1024, "");
    }
    
    { // Pushing on empty vec with big reserve
        vec_char_push(&vec, 'T');
        vec_char_push(&vec, 'e');
        vec_char_push(&vec, 's');
        vec_char_push(&vec, 't');
        test_vec_char_content(&vec, "Test");
        PANIC_ASSERT(vec_char_length(&vec) == 4, "");
        PANIC_ASSERT(vec_char_capacity(&vec) == 1024, "");
    }

    { // Reallocating
        vec_char_reallocate(&vec, 10);
        PANIC_ASSERT(vec_char_capacity(&vec) == 10, "");
        // TODO: Test reallocate
    }

    { // Setting array
        vec_char_set_array(&vec, 1, "its", 3);
        test_vec_char_content(&vec, "Tits");
    }

    { // Shrinking
        vec_char_shrink(&vec);
        PANIC_ASSERT(vec_char_capacity(&vec) == 4, "");

        // Shrink empty vec
        vec_char_shrink(&vec);
    }

    { // Freeing
        vec_char_free(&vec);
        PANIC_ASSERT(vec_char_ptr(&vec) == NULL, "");
        PANIC_ASSERT(vec_char_capacity(&vec) == 0, "");
        PANIC_ASSERT(vec_char_length(&vec) == 0, "");
    }

    { // Push after freeing
        vec_char_push(&vec, 'C');
        vec_char_push(&vec, 'o');
        vec_char_push(&vec, 'p');
        vec_char_push(&vec, 'y');
    }

    { // Clone VEC
        vec_char_t copied_vec = vec_char_clone(&vec);
        PANIC_ASSERT(vec_char_ptr(&vec) != vec_char_ptr(&copied_vec), "");
        test_vec_char_content(&vec, "Copy");
        test_vec_char_content(&copied_vec, "Copy");

        // Copy empty array
        vec_char_clear(&copied_vec);
        vec_char_t empty_copy = vec_char_clone(&copied_vec);
        PANIC_ASSERT(vec_char_ptr(&copied_vec) != vec_char_ptr(&empty_copy), "");
        PANIC_ASSERT(vec_char_capacity(&empty_copy) == 0, "");
        PANIC_ASSERT(vec_char_length(&empty_copy) == 0, "");
    }

    arena_destroy(&arena);
}