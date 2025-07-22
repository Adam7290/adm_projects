#define VEC_IMPLEMENTATION
#include "test.h"
#include <stdbool.h>
#include <stdio.h>

// State
PRIVATE test_t _current_test;
PRIVATE bool _expect_panic_success;

PRIVATE void _test_panic_handler() {
    printf("\033[31mTest \"%s\" failed.\033[0m\n", _current_test.name);
    abort();
}

PRIVATE void _test_expect_panic_handler() {
    _expect_panic_success = true;
}

void _test_start_expect_panic() {
    _expect_panic_success = false;
    panic_set_handler(_test_expect_panic_handler);
}

void _test_end_expect_panic(const char* file_name, i32 line, NULLABLE const char* message) {
    panic_set_handler(_test_panic_handler);

    if (_expect_panic_success == false) {
        _panic(file_name, line, message);
    }
}

int test_list_run(test_list_t* test_list) {
    panic_set_handler(_test_panic_handler);

    usize total_tests = test_list_length(test_list);

    bool has_next_test;
    has_next_test = test_list_pop(test_list, &_current_test);
    while (has_next_test == true) {
        _current_test.func();
        has_next_test = test_list_pop(test_list, &_current_test);
    }

    printf("\033[32m%zu out of %zu tests passed!\n\033[0m\n", total_tests, total_tests);

    panic_set_handler(NULL);
    return EXIT_SUCCESS;
}