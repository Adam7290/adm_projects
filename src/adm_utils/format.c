#include "format.h"
#include "arena.h"
#include "util.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

// TODO: Make stream library for mediums to format like a file/console stream.
// This is mainly needed for PANIC formatting as we don't want to allocate heap memory in a panicked state.
// Example: stream_write_cstr(stream_t* stream, const char* cstr)

FORMAT_IMPL(testing_t)(string_t* string, testing_t* data) {
    arena_t arena = arena_new();
    string_t result = format(&arena, "{ .hiiiii = %i, .byeeee = %i }", data->hiiiii, data->byeeee);
    string_concat(string, &result);
    arena_destroy(&arena);
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
PRIVATE void funcname(string_t* string, type i, int base, bool upper) { \
    if (i == 0) { \
        string_concat_char(string, '0'); \
        return; \
    } \
     \
    if (i < 0) { \
        string_concat_char(string, '-'); \
        i = -i; \
    } \
     \
    int digit_count = (int)(log(i) / log(base)) + 1; \
    for (int j = 0; j < digit_count; j++) { \
        int digit = (int)(fmod(floor(i / pow(base, digit_count - j - 1)), base)); \
        string_concat_char(string, _format_digit_to_ascii(digit, upper)); \
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
PRIVATE usize _format_process_spec(string_t* string, const char* spec, va_list* args) {
    if (strncmp(spec, "%%", 2) == 0) {
        string_concat_char(string, '%');
        return 2;
    } else if (strncmp(spec, "%s", 2) == 0) {
        const char* str = va_arg(*args, char*);
        string_concat_cstr(string, str);
        return 2;
    } else if (strncmp(spec, "%c", 2) == 0) {
        string_concat_char(string, (char)va_arg(*args, int));
        return 2;
    } else if (strncmp(spec, "%i", 2) == 0 || strncmp(spec, "%d", 2) == 0) {
        _format_itoa(string, va_arg(*args, int), 10, false);
        return 2;
    } else if (strncmp(spec, "%zu", 3) == 0) {
        _format_zutoa(string, va_arg(*args, int), 10, false);
        return 3;
    } else if (strncmp(spec, "%u", 2) == 0) {
        _format_utoa(string, va_arg(*args, uint), 10, false);
        return 2;
    } else if (strncmp(spec, "%x", 2) == 0) {
        _format_utoa(string, va_arg(*args, uint), 16, false);
        return 2;
    } else if (strncmp(spec, "%X", 2) == 0) {
        _format_utoa(string, va_arg(*args, uint), 16, true);
        return 2;
    } else if (strncmp(spec, "%o", 2) == 0) {
        _format_utoa(string, va_arg(*args, uint), 8, false);
        return 2;
    } else if (strncmp(spec, "%p", 2) == 0) {
        string_concat_cstr(string, "0x");
        _format_zutoa(string, (usize)va_arg(*args, void*), 16, false);
        return 2;
    }  else if (strncmp(spec, "%$", 2) == 0) {
        formatter_t formatter = va_arg(*args, formatter_t);
        formatter.func(string, formatter.data);
        return 2;
    }

    string_concat_cstr(string, spec);
}

PRIVATE string_t _format_internal(arena_t* arena, const char* format_string, va_list* args) {
    usize format_len = strlen(format_string);

    string_t ret_string = string_new_empty(arena);
    // Let's reserve about 1.5x the length of the format string
    string_reserve(&ret_string, format_len + (format_len / 2));

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
            i += _format_process_spec(&ret_string, buf, args);
        } else {
            string_concat_char(&ret_string, c);
            i++;
        }
    }

    return ret_string;
}

string_t format(arena_t* arena, const char* format_string, ...) {
    va_list args;
    va_start(args, format_string);
    string_t ret = _format_internal(arena, format_string, &args);
    va_end(args);
    return ret;
}

// TODO: Make console utilities (con_print, con_println)
void format_print(const char* format_string, ...) {
    va_list args;
    va_start(args, format_string);
    arena_t arena = arena_new();
    string_t string = _format_internal(&arena, format_string, &args);
    printf("%s", string_ptr(&string));
    arena_destroy(&arena);
    va_end(args);
}