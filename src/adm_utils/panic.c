#include "panic.h"
#include <stdio.h>
#include <stdlib.h>

PRIVATE NORETURN int _panic_default(const char* file_name, i32 line, NULLABLE const char* message) {
    if (message == NULL) {
        message = "No message provided.";
    }

    fprintf(stderr, "Panicked at %s:%i: %s\n", file_name, line, message);
    abort();
    return 1;
}

PRIVATE panic_handler_t _panic_func = _panic_default;
panic_handler_t panic_set_handler(NULLABLE panic_handler_t func) {
    // If null passed then reset to default
    if (func == NULL) {
        func = _panic_default;
    }

    panic_handler_t old = _panic_func;
    _panic_func = func;
    return old;
}

NORETURN int _panic(const char* file_name, i32 line, NULLABLE const char* message) {
    return _panic_func(file_name, line, message);
}