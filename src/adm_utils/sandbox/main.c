#include <adm_utils/arena.h>
#include <adm_utils/format.h>
#include <adm_utils/util.h>
#include <adm_utils/console.h>
#include <adm_utils/stream.h>
#include <adm_utils/string.h>

#include <stdio.h>

int main() {
    arena_t arena = arena_new();
    string_t string = string_new_empty(&arena);
    stream_t stream = console_stdout_stream();

    format(
        &stream,
        "%%%%: %%\n"
        "%%s: %s\n"
        "%%c: %c\n"
        "%%i: %i\n"
        "%%u: %u\n"
        "%%x: %x\n" 
        "%%X: %X\n"
        "%%o: %o\n"
        "%%zu: %zu\n"
        "%%p: %p\n"
        "%%$: %$\n"
        ,
        "Hello, World!", 
        'F',
        (int)(123),
        (uint)(UINT32_MAX),
        0xffaa11,
        0xAABBCC,
        020,
        SIZE_MAX,
        (void*)0xFF666AA,
        FORMAT(testing_t, &(testing_t){
            .hiiiii = 12345,
            .byeeee = 54321,
        })
    );

    arena_destroy(&arena);
    return 0;
}