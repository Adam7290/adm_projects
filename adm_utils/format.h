//
// EXAMPLE:
// format(&stream, "Formatted int: {}", FORMAT(int, 123));
// stream == "Formatted int: 123"
//

#pragma once

#include "util.h"

#include <stdarg.h>

// TODO: I need to add more forward decls like this in headers
typedef struct stream_t stream_t;

typedef void(*format_func_t)(stream_t* stream, void* data);
// Use FORMAT macro
typedef struct formatter_t {
    void* data;
    format_func_t func;
} formatter_t;

#define FORMAT_SYMBOL(type, after) __FORMAT__##type##__##after
#define FORMAT_ARGS_TYPE(type) FORMAT_SYMBOL(type, args_t)
#define FORMAT_DECL(type) void FORMAT_SYMBOL(type, func)(stream_t* stream, const FORMAT_ARGS_TYPE(type)* data)
#define FORMAT_IMPL(type) void FORMAT_SYMBOL(type, func)
// Macro for putting your data into a formatter that format functions can read.
// ... is passed straight into a constructor for the format args struct for the specified type.
// See <adm_utils/format.h> on how args are passed for each type.
#define FORMAT(type, ...) (formatter_t){ .data = &(FORMAT_ARGS_TYPE(type)){__VA_ARGS__}, .func = (format_func_t)(FORMAT_SYMBOL(type, func)) }

// Formatters for primitives
typedef char FORMAT_ARGS_TYPE(char);	FORMAT_DECL(char);
typedef bool FORMAT_ARGS_TYPE(bool);	FORMAT_DECL(bool);

#define _FORMAT_DECL_INTEGER(type) \
typedef struct FORMAT_ARGS_TYPE(type) { \
	type value; \
	uint base; \
	bool upper; \
} FORMAT_ARGS_TYPE(type); \
FORMAT_DECL(type);

_FORMAT_DECL_INTEGER(int);
_FORMAT_DECL_INTEGER(uint);
_FORMAT_DECL_INTEGER(long);
_FORMAT_DECL_INTEGER(ulong);
_FORMAT_DECL_INTEGER(usize);

#undef _FORMAT_DECL_INTEGER

// TODO: Add double formatter
typedef struct FORMAT_ARGS_TYPE(float) {
	float value;
	uint precision;
} FORMAT_ARGS_TYPE(float);
FORMAT_DECL(float);

typedef struct FORMAT_ARGS_TYPE(cstr) {
	const char* value;
	usize length;
} FORMAT_ARGS_TYPE(cstr);
FORMAT_DECL(cstr);

// See format for description
void format_va_args(stream_t* stream, const char* format_string, va_list* args);
// Writes to the specified stream with the formatted format_string.
// See <adm_utils/format.h> for an example on how to use it.
void format(stream_t* stream, const char* format_string, ...);
