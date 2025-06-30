#include "framework/test.h"

#include <stdio.h>

void test_test();
void test_arena();
void test_vector();
void test_list();
void test_string();
void test_iter();
void test_map();

int main() {
    arena_t arena = arena_new();
    test_list_t tests = test_list_new(&arena);

    test_list_add(&tests, test_test);
    test_list_add(&tests, test_arena);
    test_list_add(&tests, test_vector);
    test_list_add(&tests, test_list);
    test_list_add(&tests, test_string);
    test_list_add(&tests, test_iter);
    test_list_add(&tests, test_map);
    
    int exit_code = test_list_run(&tests);
    arena_destroy(&arena);
    return exit_code;
}