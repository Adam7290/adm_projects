#define VEC_IMPLEMENTATION
#include "test.h"
#include <stdbool.h>
#include <stdio.h>

// Panic status
PRIVATE bool _panicked;
PRIVATE bool _expect_panic_success;
PRIVATE const char* _panic_file_name;
PRIVATE i32 _panic_file_line;
PRIVATE NULLABLE const char* _panic_message;

PRIVATE int _test_panic_handler(const char* file_name, i32 line, NULLABLE const char* message) {
    _panicked = true;
    _panic_file_name = file_name;
    _panic_file_line = line;
    _panic_message = message;
    return 1;
}

PRIVATE int _test_expect_panic_handler(const char*, i32, NULLABLE const char*) {
    _expect_panic_success = true;
    return 1;
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

    bool current_test_success;
    test_t current_test;
    current_test_success = test_list_pop(test_list, &current_test);
    while (current_test_success == true) {
        // Reset panic status
        _panicked = false;
        _panic_file_name = NULL;
        _panic_file_line = -1;
        _panic_message = NULL;

        current_test.func();
        
        if (_panicked == true) {
            goto error;
        } 

        current_test_success = test_list_pop(test_list, &current_test);
    }

success:
    printf("\033[32m%zu out of %zu tests passed!\n\033[0m\n", total_tests, total_tests);

    return EXIT_SUCCESS;

error:
    if (_panic_message == NULL) {
        _panic_message = "No message provided.";
    }

    printf("\033[31mTest \"%s\" panicked at: %s:%i: %s\033[0m\n", current_test.name, _panic_file_name, _panic_file_line, _panic_message);

    return EXIT_FAILURE;
}