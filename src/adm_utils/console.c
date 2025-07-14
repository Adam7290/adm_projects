#include "console.h"
#include "format.h"
#include "stream.h"

#include <stdio.h>
#include <stdarg.h>

void console_print(const char* cstr_or_format, ...) {
    va_list args;
    va_start(args, cstr_or_format);
    stream_t stdout_stream = console_stdout_stream();
    format_va_args(&stdout_stream, cstr_or_format, &args);
    va_end(args);
}

void console_println(const char* cstr_or_format, ...) {
    va_list args;
    va_start(args, cstr_or_format);
    stream_t stdout_stream = console_stdout_stream();
    format_va_args(&stdout_stream, cstr_or_format, &args);
    stream_write_char(&stdout_stream, '\n');
    va_end(args);
}

// Console stream_t begin

// TODO: This should be moved to the future file util

PRIVATE void _console_stream_write_bytes(stream_t* stream, const void* bytes, usize length) {
    fwrite(bytes, 1, length, stream_user_data(stream));
}

PRIVATE const stream_vtable_t _console_stream_vtable = (stream_vtable_t){
    .write_bytes = _console_stream_write_bytes,
};

stream_t console_stdout_stream() {
    return (stream_t){
        ._user_data = stdout,
        ._vtable = &_console_stream_vtable,
    };
}

stream_t console_stderr_stream() {
    return (stream_t){
        ._user_data = stderr,
        ._vtable = &_console_stream_vtable,
    };
}

// Console stream_t end
