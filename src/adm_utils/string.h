#pragma once

#include "util.h"
#include "hash.h"

#define VEC_TEMPLATE char
#define VEC_TEMPLATE_PREFIX _admstrvec_
#include "vec_impl.h"

typedef struct arena_t arena_t;
typedef struct stream_t stream_t;

// A linear array of characters with string related functions
typedef struct string_t {
    _admstrvec_t _array;
} string_t;

// Creates a new string with no content
string_t string_new_empty(arena_t* arena);
// Creates a new string with a cstr (mem copies the bytes of cstr)
string_t string_new(arena_t* arena, const char* cstr);
// Copies the contents of a string into a new string
string_t string_clone(string_t* string);
// Get a char in string or '\0'
NULLABLE char string_char(string_t* string, usize index);
// Get a char in a string but will panic if out of bounds
char string_char_unwrap(string_t* string, usize index);
// Get a char in a string without checking bounds
char string_char_unchecked(string_t* string, usize index);
// Set a char in a string but will panic if out of bounds
void string_set_char(string_t* string, usize index, char ch);
// Set a char in a string without checking bounds
void string_set_char_unchecked(string_t* string, usize index, char ch);
// Contents of string_t is equal to another string_t
bool string_equals(string_t* string1, string_t* string2);
// Contents of string_t is equal to a null terminated cstr
bool string_equals_cstr(string_t* string, const char* cstr);
// Contents of string_t is equal to a cstr with length
bool string_equals_ncstr(string_t* string, const char* cstr, usize length);
// Concatenate string1 with string2 (concatenated in string1)
void string_concat(string_t* string1, string_t* string2);
// Concatenate string1 with null terminated cstr (concatenated in string1)
void string_concat_cstr(string_t* string, const char* cstr);
// Concatenate string1 with cstr with length (concatenated in string1)
void string_concat_ncstr(string_t* string, const char* cstr, usize length);
// Same as string_concat but store result in new string_t
string_t string_concat_new(arena_t* arena, string_t* string1, string_t* string2);
// Concatenate a singular character
void string_concat_char(string_t* string, char c);
// Makes content uppercase
void string_upper(string_t* string);
// Makes content lowercase
void string_lower(string_t* string);
// Reserve bytes in memory for string (does not change length)
void string_reserve(string_t* string, usize capacity);
// Get length (without null terminator)
usize string_length(string_t* string);
// Get the string's capacity
usize string_capacity(string_t* string);
// Returns true if string has no content
bool string_empty(string_t* string);
// Clear string content
void string_clear(string_t* string);
// Return null terminated cstr of string
char* string_ptr(string_t* string);

iter_t string_begin(string_t* string);
iter_t string_end(string_t* string);

stream_t string_stream_new(string_t* string);

// Hash function for a string_t
hash_t hash_string(string_t* string);