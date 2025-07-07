#pragma once

#include "util.h"

// TODO: Make reader functionality
// TODO: Make writer reserve func

struct stream_vtable_t;

// An interface for reading(not implemented)/writing(implemented) bytes.
// Does not need deallocating as it is basically a table of pointers on the stack.
typedef struct stream_t {
    void* _user_data;
    const struct stream_vtable_t* _vtable;
} stream_t;

typedef struct stream_vtable_t {
    void(*write_bytes)(stream_t* stream, void* bytes, usize length);
} stream_vtable_t;

void stream_write(stream_t* stream, void* bytes, usize length);
void stream_write_cstr(stream_t* stream, const char* cstr);
void stream_write_char(stream_t* stream, char c);

void* stream_user_data(stream_t* stream);

// TODO: Some convenient stream_write_type functions