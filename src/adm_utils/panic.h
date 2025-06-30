#pragma once
#include "util.h"

typedef int(*panic_handler_t)(const char* file_name, i32 line, NULLABLE const char* message);

// Set panic handler and return previous handler
panic_handler_t panic_set_handler(NULLABLE panic_handler_t func);

NORETURN int _panic(const char* file_name, i32 line, NULLABLE const char* message);
#define PANIC(NULLABLE_msg) _panic(__FILE__, __LINE__, NULLABLE_msg)
#define PANIC_ASSERT(expr, msg) (!(expr) && _panic(__FILE__, __LINE__, "Assertion failed: " #expr ": " msg))

#define UNIMPLEMENTED_FUNCTION() PANIC("Unimplemented function.")

#ifdef NDEBUG
#define PANIC_DEBUG (void)
#define PANIC_ASSERT_DEBUG(expr, msg) expr
#else
#define PANIC_DEBUG PANIC
#define PANIC_ASSERT_DEBUG PANIC_ASSERT
#endif