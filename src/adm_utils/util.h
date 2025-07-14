#pragma once
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef int8_t i8;
typedef uint8_t u8;
typedef int16_t i16;
typedef uint16_t u16;
typedef int32_t i32;
typedef uint32_t u32;
typedef int64_t i64;
typedef uint64_t u64;
typedef size_t usize;
typedef ptrdiff_t isize;
typedef float f32;
typedef double f64;
typedef u8 byte;
typedef i8 sbyte;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;
typedef char* cstr;

#define NULLABLE
#define NORETURN
#define NODISCARD
#define PUBLIC_READONLY
#define PRIVATE static

// The preprocessor is kinda stupid
#define EXPAND(a) a
#define CONCAT(a, b) a##b
#define CONCAT_EXPAND(a, b) CONCAT(a, b)
#define CONCAT3(a, b, c) a##b##c
#define CONCAT3_EXPAND(a, b, c) CONCAT3(a, b, c)
#define CONCAT4(a, b, c, d) a##b##c##d
#define CONCAT4_EXPAND(a, b, c, d) CONCAT4(a, b, c, d)
