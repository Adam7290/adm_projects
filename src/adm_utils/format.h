#pragma once

#include "util.h"
#include "string.h"

// TODO: I need to add more forward decls like this in headers
typedef struct arena_t arena_t;

typedef void(*format_func_t)(string_t* string, void* data);
typedef struct formatter_t {
    void* data;
    format_func_t func;
} formatter_t;

#define FORMAT_SYMBOL(type) _FORMAT__##type##__FUNC
#define FORMAT_DECL(type) void FORMAT_SYMBOL(type)(string_t* string, type* data)
#define FORMAT_IMPL(type) void FORMAT_SYMBOL(type)
#define FORMAT(type, ...) (formatter_t){ .data = __VA_ARGS__, .func = (format_func_t)FORMAT_SYMBOL(type) }

typedef struct testing_t {
    i32 hiiiii;
    i32 byeeee;
} testing_t;

FORMAT_DECL(testing_t);

string_t format(arena_t* arena, const char* format_string, ...);
void format_print(const char* format_string, ...);