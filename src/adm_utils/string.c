#define VEC_IMPLEMENTATION
#include "string.h"
#include "stream.h"

#include <ctype.h>

string_t string_new_empty(arena_t* arena) {
    _admstrvec_t array = _admstrvec_new(arena);
    return (string_t){
        ._array = array,
    };
}

string_t string_new(arena_t* arena, const char* cstr) {
    string_t string = string_new_empty(arena);
    string_concat_cstr(&string, cstr);
    return string;
}

string_t string_clone(const string_t* string) {
    return (string_t){
        ._array = _admstrvec_clone(&string->_array),
    };
}

NULLABLE char string_char(const string_t* string, usize index) {
    if (index < 0 || index >= string_length(string)) {
        return '\0';
    }

    return string->_array._ptr[index];
}

char string_char_unwrap(const string_t* string, usize index) {
    if (index < 0 || index >= string_length(string)) {
        PANIC("Index outside of bounds.");
    }

    return string->_array._ptr[index];
}

char string_char_unchecked(const string_t* string, usize index) {
    return string->_array._ptr[index];
}

void string_set_char(string_t* string, usize index, char ch) {
    if (index < 0 || index >= string_length(string)) {
        PANIC("Index outside of bounds.");
    }

    string->_array._ptr[index] = ch;
}

void string_set_char_unchecked(string_t* string, usize index, char ch) {
    string->_array._ptr[index] = ch;
}

bool string_equals(const string_t* string1, const string_t* string2) {
    return string_equals_ncstr(string1, string_ptr(string2), string_length(string2));
}

bool string_equals_cstr(const string_t* string, const char* cstr) {
    return string_equals_ncstr(string, cstr, strlen(cstr));
}

bool string_equals_ncstr(const string_t* string, const char* cstr, usize length) {
    if (string_length(string) != length) {
        return false;
    }

    return memcmp(string_ptr(string), cstr, length) == 0;
}

void string_concat(string_t* string1, const string_t* string2) {
    string_concat_ncstr(string1, string_ptr(string2), string_length(string2));
}

void string_concat_cstr(string_t* string, const char* cstr) {
    string_concat_ncstr(string, cstr, strlen(cstr));
}

void string_concat_ncstr(string_t* string, const char* cstr, usize length) {
    if (length == 0) {
        return;
    }

    usize prev_length = string_length(string);
    _admstrvec_set_length(&string->_array, prev_length + length + 1);
    if (prev_length == 0) {
        _admstrvec_set_array(&string->_array, prev_length, cstr, length);
    } else {
        _admstrvec_set_array(&string->_array, prev_length, cstr, length);
    }
    _admstrvec_set(&string->_array, string->_array._length - 1, '\0');
}

string_t string_concat_new(arena_t* arena, const string_t* string1, const string_t* string2) {
    string_t new = string_new_empty(arena);
    string_reserve(&new, string_length(string1) + string_length(string2) + 1);

    // Both strings are empty
    if (string_capacity(&new) == 1) {
        return new;
    }

    if (!string_empty(string1)) {
        string_concat(&new, string1);
    }

    if (!string_empty(string2)) {
        string_concat(&new, string2);
    }

    return new;
}

void string_concat_char(string_t* string, char c) {
    string_concat_ncstr(string, &c, 1);
}

void string_upper(string_t* string) {
    for (int i = 0; i < string_length(string); i++) {
        string_set_char_unchecked(string, i, toupper(string_char_unchecked(string, i)));
    }
}

void string_lower(string_t* string) {
    for (int i = 0; i < string_length(string); i++) {
        string_set_char_unchecked(string, i, tolower(string_char_unchecked(string, i)));
    }
}

void string_reserve(string_t* string, usize capacity) {
    _admstrvec_reserve(&string->_array, capacity);
}

usize string_length(const string_t* string) {
    usize length = string->_array._length;

    // Return length without the null term
    if (length == 0) {
        return 0;
    } else {
        return length - 1;
    }
}

usize string_capacity(const string_t* string) {
    return _admstrvec_capacity(&string->_array);
}

bool string_empty(const string_t* string) {
    return string_ptr(string) == NULL || string->_array._length == 0;
}

void string_clear(string_t* string) {
    _admstrvec_clear(&string->_array);
}

char* string_ptr(const string_t* string) {
    return string->_array._ptr;
}

iter_t string_begin(string_t* string) {
    return _admstrvec_begin(&string->_array);
}

iter_t string_end(string_t* string) {
    return _admstrvec_end(&string->_array);
}

hash_t hash_string(const string_t* string) {
    return hash_bytes(string_ptr(string), string_length(string) * sizeof(char));
}

// STREAM STUFF BEGIN

PRIVATE void _string_stream_write_bytes(stream_t* stream, const void* bytes, usize length) {
    string_t* string = stream_user_data(stream);
    string_concat_ncstr(string, bytes, length);
}

PRIVATE const stream_vtable_t _string_stream_vtable = (stream_vtable_t){
    .write_bytes = _string_stream_write_bytes,
};

stream_t string_stream_new(string_t* string) {
    return (stream_t){
        ._user_data = string,
        ._vtable = &_string_stream_vtable,
    };
}

// STREAM STUFF END
