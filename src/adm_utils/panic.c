#include "panic.h"
#include "stream.h"
#include "console.h"
#include "format.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#define PANIC_DEFAULT_FUNC abort

PRIVATE panic_handler_t _panic_func = PANIC_DEFAULT_FUNC;
panic_handler_t panic_set_handler(NULLABLE panic_handler_t func) {
    // If null passed then reset to default
    if (func == NULL) {
        func = PANIC_DEFAULT_FUNC;
    }

    panic_handler_t old = _panic_func;
    _panic_func = func;
    return old;
}

NORETURN int _panic(const char* file_name, i32 line, NULLABLE const char* message, ...) {
    if (message == NULL) {
        message = "No message provided.";
    }

    va_list args;
    va_start(args, message);
    stream_t stream = console_stderr_stream();
    format(&stream, "Panicked at %s:%i: ", file_name, line);
    format_va_args(&stream, message, &args);
    stream_write_char(&stream, '\n');
    va_end(args);
    _panic_func();
    return 1;
}