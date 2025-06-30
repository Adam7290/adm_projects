#include "framework/test.h"

#include <stdio.h>

void test_test() {
    TEST_EXPECT_PANIC(PANIC("TEST!!"), "Testing expect panic failed");
}