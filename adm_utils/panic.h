#pragma once
#include "util.h"
#include "format.h"

typedef void(*panic_handler_t)();

// Set panic handler and return previous handler
panic_handler_t panic_set_handler(NULLABLE panic_handler_t func);

NORETURN int _panic(const char* file_name, i32 line, NULLABLE const char* message, ...);
NORETURN int _assert(const char* file_name, i32 line, NULLABLE const char* message, ...);
// Abort the program immediately with a specified message 
// Message can be formatted with ...
#define PANIC(NULLABLE_msg, ...) _panic(__FILE__, __LINE__, NULLABLE_msg, ## __VA_ARGS__)
// Assert that a statement results in true
#define PANIC_ASSERT(expr, msg) (!(expr) && _panic(__FILE__, __LINE__, "Assertion failed: {}: {}", FORMAT(cstr, #expr), FORMAT(cstr, msg)))

#define UNIMPLEMENTED_FUNCTION() PANIC("Unimplemented function.")

#ifdef NDEBUG
#define PANIC_DEBUG (void)
#define PANIC_ASSERT_DEBUG(expr, msg) expr
#else
#define PANIC_DEBUG PANIC
#define PANIC_ASSERT_DEBUG PANIC_ASSERT
#endif
