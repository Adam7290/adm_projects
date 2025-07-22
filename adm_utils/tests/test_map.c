#include <adm_utils/arena.h>
#include <adm_utils/iter.h>

#define MAP_IMPLEMENTATION
#define MAP_TEMPLATE i32
#include <adm_utils/map_impl.h>

void test_map() {
    arena_t arena = arena_new();
    
    map_i32_t map = map_i32_new(&arena);

    PANIC_ASSERT(map_i32_get(&map, hash_cstr("Mystery")) == NULL, "");
    map_i32_insert(&map, hash_cstr("Mystery"), 123);
    PANIC_ASSERT(*map_i32_get(&map, hash_cstr("Mystery")) == 123, "");

    PANIC_ASSERT(map_i32_has(&map, hash_cstr("Devil's Number")) == false, "");
    map_i32_insert(&map, hash_cstr("Devil's Number"), 666);
    PANIC_ASSERT(map_i32_has(&map, hash_cstr("Devil's Number")) == true, "");

    PANIC_ASSERT(map_i32_get(&map, hash_cstr("Funny Number")) == NULL, "");
    map_i32_insert(&map, hash_cstr("Funny Number"), 420);
    PANIC_ASSERT(*map_i32_get(&map, hash_cstr("Funny Number")) == 420, "");
    map_i32_insert(&map, hash_cstr("Funny Number"), 69);
    PANIC_ASSERT(*map_i32_get(&map, hash_cstr("Funny Number")) == 69, "");
    map_i32_insert(&map, hash_cstr("Funny Number"), 69420);
    PANIC_ASSERT(*map_i32_get(&map, hash_cstr("Funny Number")) == 69420, "");

    PANIC_ASSERT(map_i32_has(&map, hash_cstr("Devil's Number")) == true, "");
    map_i32_delete(&map, hash_cstr("Devil's Number"));
    PANIC_ASSERT(map_i32_has(&map, hash_cstr("Devil's Number")) == false, "");
    map_i32_insert(&map, hash_cstr("Devil's Number"), 333);
    PANIC_ASSERT(*map_i32_get(&map, hash_cstr("Devil's Number")) == 333, "");

    arena_destroy(&arena);
}