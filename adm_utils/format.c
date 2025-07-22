#include "format.h"
#include "util.h"
#include "stream.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

PRIVATE char _format_digit_to_ascii(uint digit, bool upper) {
    if (digit < 10) {
        return '0' + digit;
    }

    if (upper == false) {
        return '0' + 39 + digit;
    }
    
    return '0' + 7 + digit;
}

#define _FORMAT_IMPL_INTEGER(type) \
FORMAT_IMPL(type)(stream_t* stream, const FORMAT_ARGS_TYPE(type)* args) { \
	type i = args->value; \
	uint base = args->base != 0 ? args->base : 10; \
	bool upper = args->upper; \
	 \
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

_FORMAT_IMPL_INTEGER(int);
_FORMAT_IMPL_INTEGER(uint);
_FORMAT_IMPL_INTEGER(long);
_FORMAT_IMPL_INTEGER(ulong);
_FORMAT_IMPL_INTEGER(usize);

#undef _FORMAT_IMPL_INTEGER 

FORMAT_IMPL(float)(stream_t* stream, const FORMAT_ARGS_TYPE(float)* args) {
	// I tried to make an ftoa implementation myself but
	// sometimes you gotta just swallow your pride and take the easy way out :(
	float val = args->value;
	uint precision = args->precision;

	char buffer[64];
	if (precision == 0) {
		snprintf(buffer, sizeof(buffer), "%f", val);
	} else {
		// Hacky way to format uint into chars without stream because
		// I haven't implemented a raw memory stream.
		if (precision > 99) { precision = 99; }
		char cfmt[6] = "%.00f";
		cfmt[2] = '0' + ((char)(fmod(floor(precision / 100.0), 10)));
		cfmt[3] = '0' + ((char)(fmod(precision, 10)));
		snprintf(buffer, sizeof(buffer), cfmt, val);
	}
	stream_write_cstr(stream, buffer);
}

FORMAT_IMPL(char)(stream_t* stream, const char* c) {
	stream_write_char(stream, *c);
}

FORMAT_IMPL(bool)(stream_t* stream, const bool* b) {
	stream_write_cstr(stream, *b == true ? "true" : "false");
}

FORMAT_IMPL(cstr)(stream_t* stream, const FORMAT_ARGS_TYPE(cstr)* args) {
	stream_write(
		stream,
		args->value,
		args->length == 0 ? strlen(args->value) : args->length
	);
}

// TODO: Optimization: Reserve ~1.5x format_string length (when stream reserve functionality implemented)
void format_va_args(stream_t* stream, const char* format_string, va_list* args) {
    usize format_len = strlen(format_string);

    int i = 0;
    while (i < format_len) {
		// Shouldn't need bounds checking due to NUL term
		char c0 = format_string[i];
        char c1 = format_string[i+1];

		if (c0 == '{' && c1 == '{') {
			stream_write_char(stream, '{');
			i += 2;
			continue;
		} else if (c0 == '}' && c1 == '}') {
			stream_write_char(stream, '}');
			i += 2;
			continue;
		}

		if (c0 == '{' && c1 == '}') {
			formatter_t formatter = va_arg(*args, formatter_t);
			formatter.func(stream, formatter.data);
			i += 2;	
			continue;
		}

		stream_write_char(stream, c0);
		i++;
	}
}

void format(stream_t* stream, const char* format_string, ...) {
    va_list args;
    va_start(args, format_string);
    format_va_args(stream, format_string, &args);
    va_end(args);
}
