#include "framework/test.h"
#include <adm_utils/string.h>
#include <adm_utils/arena.h>
#include <adm_utils/stream.h>

void test_format() {
	arena_t arena = arena_new();
	const char* expected = 
		"c true false 123 -123 ff FF 123.456 123.46 testing"
	;

	string_t result = string_new_empty(&arena);
	stream_t stream = string_stream_new(&result);

	format(&stream,
		"{} {} {} {} {} {} {} {} {} {}",
		FORMAT(char, 'c'),
		FORMAT(bool, true),
		FORMAT(bool, false),
		FORMAT(int, 123),
		FORMAT(int, -123),
		FORMAT(int, 0xff, 16),
		FORMAT(int, 0xff, 16, true),
		FORMAT(float, 123.456f, 3),
		FORMAT(float, 123.456f, 2),
		FORMAT(cstr, "testing")
	);

	PANIC_ASSERT(string_equals_cstr(&result, expected) == true, "");

	arena_destroy(&arena);
}
