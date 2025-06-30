#pragma once

#include "util.h"
typedef u64 hash_t;

hash_t hash_bytes(void* ptr, usize size);
hash_t hash_cstr(const char* str);