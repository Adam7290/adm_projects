#include <adm_utils/arena.h>
#include <adm_utils/format.h>
#include <adm_utils/util.h>
#include <adm_utils/console.h>
#include <adm_utils/stream.h>
#include <adm_utils/string.h>

#include <stdio.h>

int main() {
    arena_t arena = arena_new();
    string_t string = string_new(&arena, "Testing string_t");

	console_println(
		"{{}}\n"
		"char: {}\n"
		"bool: {}\n"
		"int: {}\n"
		"uint: {}\n"
		"float: {}\n"
		"float (precision: 3): {}\n"
		"cstr: {}\n",

		FORMAT(char, 't'),
		FORMAT(bool, true),
		FORMAT(int, -123),
		FORMAT(uint, -1),
		FORMAT(float, 123.45678),
		FORMAT(float, 123.45678, 3),
		FORMAT(cstr, "Hello!!")
	);
	
	arena_destroy(&arena);
    return 0;
}
