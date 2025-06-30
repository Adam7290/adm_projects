#pragma once
#include <adm_utils/util.h>
#include <adm_utils/panic.h>

typedef void(*test_func_t)();
typedef struct {
    const char* name;
    test_func_t func;
} test_t;

#define VEC_TEMPLATE test_t
#define VEC_TEMPLATE_DISPLAY_NAME test
#define VEC_TEMPLATE_PREFIX test_list_
#include <adm_utils/vec_impl.h>

void _test_start_expect_panic();
void _test_end_expect_panic(const char* file_name, i32 line, NULLABLE const char* message);
#define TEST_EXPECT_PANIC(expr, NULLABLE_message) _test_start_expect_panic(); expr; _test_end_expect_panic(__FILE__, __LINE__, NULLABLE_message);

#define test_list_add(list, _func) test_list_push(list, (test_t){ .name = #_func, .func = _func })

int test_list_run(test_list_t* test_list);

