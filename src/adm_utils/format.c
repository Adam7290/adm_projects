#include "format.h"
#include "arena.h"
#include "util.h"
#include "stream.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

// TODO: Make stream library for mediums to format like a file/console stream.
// This is mainly needed for PANIC formatting as we don't want to allocate heap memory in a panicked state.
// Example: stream_write_cstr(stream_t* stream, const char* cstr)

FORMAT_IMPL(testing_t)(stream_t* stream, testing_t* data) {
    format(stream, "{ .hiiiii = %i, .byeeee = %i }", data->hiiiii, data->byeeee);
}

PRIVATE char _format_digit_to_ascii(unsigned int digit, bool upper) {
    if (digit < 10) {
        return '0' + digit;
    }

    if (upper == false) {
        return '0' + 39 + digit;
    }
    
    return '0' + 7 + digit;
}

#define _IMPL_ASCII_CONV_FUNC(type, funcname) \
PRIVATE void funcname(stream_t* stream, type i, int base, bool upper) { \
    if (i == 0) { \
        stream_write_char(stream, '0'); \
        return; \
    } \
     \
    if (i < 0) { \
        stream_write_char(stream, '-'); \
        i = -i; \
    } \
     \
    int digit_count = (int)(log(i) / log(base)) + 1; \
    for (int j = 0; j < digit_count; j++) { \
        int digit = (int)(fmod(floor(i / pow(base, digit_count - j - 1)), base)); \
        stream_write_char(stream, _format_digit_to_ascii(digit, upper)); \
    } \
}

_IMPL_ASCII_CONV_FUNC(int, _format_itoa);
_IMPL_ASCII_CONV_FUNC(uint, _format_utoa);
_IMPL_ASCII_CONV_FUNC(long, _format_ltoa);
_IMPL_ASCII_CONV_FUNC(ulong, _format_lutoa);
_IMPL_ASCII_CONV_FUNC(usize, _format_zutoa);
// TODO: long long types (who even uses those tbh)
// TODO: Floating point to ascii (it is a LOT of work)

#undef _IMPL_ASCII_CONV_FUNC

// Returns: length of spec
// TODO: Specifier flags
PRIVATE usize _format_process_spec(stream_t* stream, const char* spec, va_list* args) {
    if (strncmp(spec, "%%", 2) == 0) {
        stream_write_char(stream, '%');
        return 2;
    } else if (strncmp(spec, "%s", 2) == 0) {
        const char* str = va_arg(*args, char*);
        stream_write_cstr(stream, str);
        return 2;
    } else if (strncmp(spec, "%c", 2) == 0) {
        stream_write_char(stream, (char)va_arg(*args, int));
        return 2;
    } else if (strncmp(spec, "%i", 2) == 0 || strncmp(spec, "%d", 2) == 0) {
        _format_itoa(stream, va_arg(*args, int), 10, false);
        return 2;
    } else if (strncmp(spec, "%zu", 3) == 0) {
        _format_zutoa(stream, va_arg(*args, int), 10, false);
        return 3;
    } else if (strncmp(spec, "%u", 2) == 0) {
        _format_utoa(stream, va_arg(*args, uint), 10, false);
        return 2;
    } else if (strncmp(spec, "%x", 2) == 0) {
        _format_utoa(stream, va_arg(*args, uint), 16, false);
        return 2;
    } else if (strncmp(spec, "%X", 2) == 0) {
        _format_utoa(stream, va_arg(*args, uint), 16, true);
        return 2;
    } else if (strncmp(spec, "%o", 2) == 0) {
        _format_utoa(stream, va_arg(*args, uint), 8, false);
        return 2;
    } else if (strncmp(spec, "%p", 2) == 0) {
        stream_write_cstr(stream, "0x");
        _format_zutoa(stream, (usize)va_arg(*args, void*), 16, false);
        return 2;
    }  else if (strncmp(spec, "%$", 2) == 0) {
        formatter_t formatter = va_arg(*args, formatter_t);
        formatter.func(stream, formatter.data);
        return 2;
    }

    stream_write_cstr(stream, spec);
}

// TODO: Optimization: Reserve ~1.5x format_string length (when stream reserve functionality implemented)
void format_va_args(stream_t* stream, const char* format_string, va_list* args) {
    usize format_len = strlen(format_string);

    int i = 0;
    while (i < format_len) {
        char c = format_string[i];
        
        // Collect specifier code into buffer
        if (c == '%') {
            char buf[4] = { 0 };
            int j = 0;
            for (; j < sizeof(buf) / sizeof(char) - 1; j++) {
                buf[j] = format_string[i + j];
            }
            i += _format_process_spec(stream, buf, args);
        } else {
            stream_write_char(stream, c);
            i++;
        }
    }
}

void format(stream_t* stream, const char* format_string, ...) {
    va_list args;
    va_start(args, format_string);
    format_va_args(stream, format_string, &args);
    va_end(args);
}

#include "string.h"
// TODO: Make console utilities (con_print, con_println)
void format_print(const char* format_string, ...) {
    va_list args;
    va_start(args, format_string);
    arena_t arena = arena_new();
    string_t string = string_new_empty(&arena);
    stream_t stream = string_stream_new(&string);
    format_va_args(&stream, format_string, &args);
    fputs(string_ptr(&string), stdout);
    arena_destroy(&arena);
    va_end(args);
}