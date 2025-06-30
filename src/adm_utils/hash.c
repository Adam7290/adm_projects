#include "hash.h"
#include "panic.h"

#include <string.h>

// FNV-1a algorithm
PRIVATE const hash_t _fnv_offset = 14695981039346656037ul;
PRIVATE const hash_t _fnv_prime = 1099511628211ul;
hash_t hash_bytes(void* ptr, usize size) {
    hash_t hash = _fnv_offset;

    // Loop through each byte...
    for (int i = 0; i < size; i++) {
        byte b = (byte)((byte*)ptr)[i];
        hash ^= b;
        hash *= _fnv_prime;
    }

    return hash;
}

hash_t hash_cstr(const char* str) {
    return hash_bytes((void*)str, strlen(str));
}