#include "hash.h"

#ifndef MAP_TEMPLATE
#error You need to define MAP_TEMPLATE before including this header
#endif // MAP_TEMPLATE

#ifdef MAP_TEMPLATE_DISPLAY_NAME
#define MAP_NAME MAP_TEMPLATE_DISPLAY_NAME
#else
#define MAP_NAME MAP_TEMPLATE
#endif // MAP_TEMPLATE_DISPLAY_NAME

#ifndef MAP_TEMPLATE_PREFIX
#define MAP_TEMPLATE_PREFIX CONCAT3_EXPAND(map_, MAP_NAME, _)
#endif // MAP_TEMPLATE_PREFIX

#define MAP_SYMBOL(after) CONCAT_EXPAND(MAP_TEMPLATE_PREFIX, after)
#define MAP_PRIVATE_SYMBOL(after) CONCAT_EXPAND(_, MAP_SYMBOL(after))
#define MAP_TYPE MAP_SYMBOL(t)


typedef struct MAP_SYMBOL(entry_t) {
    hash_t _key;
    MAP_TEMPLATE _value;
} MAP_SYMBOL(entry_t);

#ifdef MAP_IMPLEMENTATION
#define LIST_IMPLEMENTATION
#define VEC_IMPLEMENTATION
#endif
#define LIST_TEMPLATE MAP_SYMBOL(entry_t)
#define LIST_TEMPLATE_PREFIX MAP_PRIVATE_SYMBOL(entry_list_)
#include "list_impl.h"
#define VEC_TEMPLATE MAP_PRIVATE_SYMBOL(entry_list_t)
#define VEC_TEMPLATE_PREFIX MAP_PRIVATE_SYMBOL(bucket_vec_)
#include "vec_impl.h"

// Hash map structure
typedef struct MAP_TYPE {
    usize _entry_count;
    MAP_PRIVATE_SYMBOL(bucket_vec_t) _buckets;
} MAP_TYPE;

// TODO: Clone function
// TODO: Merge map with another


// Create a new map
MAP_TYPE MAP_SYMBOL(new)(arena_t* arena);
// Insert new item into map with key
void MAP_SYMBOL(insert)(MAP_TYPE* map, hash_t key, MAP_TEMPLATE value);
// Delete item with key
MAP_TEMPLATE MAP_SYMBOL(delete)(MAP_TYPE* map, hash_t key);
// Get item with key
NULLABLE MAP_TEMPLATE* MAP_SYMBOL(get)(MAP_TYPE* map, hash_t key);
// TODO: getc method
bool MAP_SYMBOL(has)(const MAP_TYPE* map, hash_t key);
// TODO: Free function
// Returns a pointer to the map entry or NULL if doesn't exist
// TODO: Return pointer instead
// TODO: Rename to get_entry
// TODO: Make getc and get_entryc
// TODO: Const iters
NULLABLE MAP_SYMBOL(entry_t*) MAP_SYMBOL(get_entry)(const MAP_TYPE* map, hash_t key);
const NULLABLE MAP_SYMBOL(entry_t*) MAP_SYMBOL(getc_entry)(const MAP_TYPE* map, hash_t key);
iter_t MAP_SYMBOL(begin)(const MAP_TYPE* map);
iter_t MAP_SYMBOL(end)(const MAP_TYPE* map);

MAP_TEMPLATE MAP_SYMBOL(entry_value)(const MAP_SYMBOL(entry_t*) entry);
hash_t MAP_SYMBOL(entry_key)(const MAP_SYMBOL(entry_t*) entry);
void MAP_SYMBOL(entry_set_value)(MAP_SYMBOL(entry_t*) entry, MAP_TEMPLATE value);

#ifdef MAP_IMPLEMENTATION

PRIVATE const usize MAP_PRIVATE_SYMBOL(bucket_count_max) = 8;
PRIVATE const f32 MAP_PRIVATE_SYMBOL(load_factor_max) = 2.0f;
PRIVATE const f32 MAP_PRIVATE_SYMBOL(load_factor_min) = MAP_PRIVATE_SYMBOL(load_factor_max) / 4.0f;

PRIVATE usize MAP_PRIVATE_SYMBOL(hash_to_bucket_index)(const MAP_TYPE* map, hash_t hash) {
    return hash % MAP_PRIVATE_SYMBOL(bucket_vec_length)(&map->_buckets);
}

PRIVATE MAP_PRIVATE_SYMBOL(entry_list_t*) MAP_PRIVATE_SYMBOL(get_entry_list)(const MAP_TYPE* map, hash_t key) {
    const MAP_PRIVATE_SYMBOL(bucket_vec_t*) buckets = &map->_buckets;

    usize index = MAP_PRIVATE_SYMBOL(hash_to_bucket_index)(map, key);
    MAP_PRIVATE_SYMBOL(entry_list_t*) entry_list = MAP_PRIVATE_SYMBOL(bucket_vec_getc_unchecked)(buckets, index);

    return entry_list;
}

PRIVATE NULLABLE MAP_PRIVATE_SYMBOL(entry_list_t*) MAP_PRIVATE_SYMBOL(get_next_filled_entry_list)(const MAP_TYPE* map, usize index) {
    const MAP_PRIVATE_SYMBOL(bucket_vec_t*) buckets = &map->_buckets;
    for (int i = index; i < MAP_PRIVATE_SYMBOL(bucket_vec_length)(buckets); i++) {
        MAP_PRIVATE_SYMBOL(entry_list_t*) entry_list = MAP_PRIVATE_SYMBOL(bucket_vec_getc_unchecked)(buckets, i);
        if (MAP_PRIVATE_SYMBOL(entry_list_empty)(entry_list) == false) {
            return entry_list;
        }
    }

    return NULL;
}

PRIVATE f32 MAP_PRIVATE_SYMBOL(get_load_factor)(MAP_TYPE* map) {
    // load factor = n / m
    return map->_entry_count / (float)(MAP_PRIVATE_SYMBOL(bucket_vec_length)(&map->_buckets));
}

PRIVATE void MAP_PRIVATE_SYMBOL(rehash)(MAP_TYPE* map, usize bucket_count) {
    MAP_PRIVATE_SYMBOL(bucket_vec_t) new_buckets = MAP_PRIVATE_SYMBOL(bucket_vec_new)(map->_buckets._arena);

    MAP_PRIVATE_SYMBOL(bucket_vec_reserve)(&new_buckets, bucket_count);
    MAP_PRIVATE_SYMBOL(bucket_vec_set_length)(&new_buckets, MAP_PRIVATE_SYMBOL(bucket_vec_capacity)(&new_buckets));

    // Initialize each bucket entry list with our arena
    for (usize i = 0; i < MAP_PRIVATE_SYMBOL(bucket_vec_length)(&new_buckets); i++) {
        MAP_PRIVATE_SYMBOL(bucket_vec_set_unchecked)(&new_buckets, i, MAP_PRIVATE_SYMBOL(entry_list_new)(new_buckets._arena));
    }

    // Copy entries from old bucket vector to new one
    for (iter_t it = MAP_SYMBOL(begin)(map); !iter_equals(it, MAP_SYMBOL(end)(map)); it = iter_next(it)) {
        MAP_SYMBOL(entry_t*) entry = iter_element(it);
        hash_t index = MAP_PRIVATE_SYMBOL(hash_to_bucket_index)(map, entry->_key);
        MAP_PRIVATE_SYMBOL(entry_list_t*) entry_list = MAP_PRIVATE_SYMBOL(bucket_vec_get_unchecked)(&new_buckets, index);

        // Copy entry over
        MAP_PRIVATE_SYMBOL(entry_list_push_back)(entry_list, *entry);
    }

    // Free old bucket vector and set new one in struct
    MAP_PRIVATE_SYMBOL(bucket_vec_free)(&map->_buckets);
    map->_buckets = new_buckets;
}

MAP_TYPE MAP_SYMBOL(new)(arena_t* arena) {
    MAP_TYPE map = (MAP_TYPE){
        ._entry_count = 0,
        ._buckets = MAP_PRIVATE_SYMBOL(bucket_vec_new)(arena),
    };

    MAP_PRIVATE_SYMBOL(rehash)(&map, MAP_PRIVATE_SYMBOL(bucket_count_max));

    return map;
}

void MAP_SYMBOL(insert)(MAP_TYPE* map, hash_t key, MAP_TEMPLATE value) {
    NULLABLE MAP_SYMBOL(entry_t*) entry = MAP_SYMBOL(get_entry)(map, key);

    if (entry != NULL) {
        // Override value in existing entry
        entry->_value = value;
    } else {
        // If no previously existing entry then just push entry onto list
        MAP_PRIVATE_SYMBOL(entry_list_push_back)(MAP_PRIVATE_SYMBOL(get_entry_list)(map, key), (MAP_SYMBOL(entry_t)){
            ._key = key,
            ._value = value,
        });
        map->_entry_count++;

        // Check if over max load factor
        if (MAP_PRIVATE_SYMBOL(get_load_factor)(map) > MAP_PRIVATE_SYMBOL(load_factor_max)) {
            MAP_PRIVATE_SYMBOL(rehash)(map, MAP_PRIVATE_SYMBOL(bucket_vec_length)(&map->_buckets) * 2);
        }
    }
}

MAP_TEMPLATE MAP_SYMBOL(delete)(MAP_TYPE* map, hash_t key) {
    MAP_PRIVATE_SYMBOL(entry_list_t*) entry_list = MAP_PRIVATE_SYMBOL(get_entry_list)(map, key);
    MAP_SYMBOL(entry_t*) entry = MAP_SYMBOL(get_entry)(map, key);
    if (entry != NULL) {
        MAP_PRIVATE_SYMBOL(entry_list_remove)(entry_list, MAP_PRIVATE_SYMBOL(entry_list_iter)(entry_list, entry));
        map->_entry_count--;
        PANIC_ASSERT_DEBUG(map->_entry_count != SIZE_MAX, "_entry_count underflowed.");

        // Check if under min load factor
        if (MAP_PRIVATE_SYMBOL(bucket_vec_length)(&map->_buckets) > MAP_PRIVATE_SYMBOL(bucket_count_max) && 
          MAP_PRIVATE_SYMBOL(get_load_factor)(map) < MAP_PRIVATE_SYMBOL(load_factor_min)) {
            MAP_PRIVATE_SYMBOL(rehash)(map, MAP_PRIVATE_SYMBOL(bucket_vec_length)(&map->_buckets) / 2);
        }
    }
}

MAP_SYMBOL(entry_t*) MAP_SYMBOL(get_entry)(const MAP_TYPE* map, hash_t key) {
    MAP_PRIVATE_SYMBOL(entry_list_t*) entry_list = MAP_PRIVATE_SYMBOL(get_entry_list)(map, key);

    for (iter_t it = MAP_PRIVATE_SYMBOL(entry_list_begin)(entry_list); !iter_equals(it, MAP_PRIVATE_SYMBOL(entry_list_end)(entry_list)); it = iter_next(it)) {
        MAP_SYMBOL(entry_t*) entry = iter_element(it);
        if (entry->_key == key) {
            return entry;
        }
    }

    return NULL;
}

const MAP_SYMBOL(entry_t*) MAP_SYMBOL(getc_entry)(const MAP_TYPE* map, hash_t key) {
    return MAP_SYMBOL(get_entry)(map, key);
}

NULLABLE MAP_TEMPLATE* MAP_SYMBOL(get)(MAP_TYPE* map, hash_t key) {
    MAP_SYMBOL(entry_t*) entry = MAP_SYMBOL(get_entry)(map, key);

    if (entry != NULL) {
        return &entry->_value;
    }

    return NULL;
}

bool MAP_SYMBOL(has)(const MAP_TYPE* map, hash_t key) {
    return MAP_SYMBOL(getc_entry)(map, key) != NULL;
}

// ITERATOR STUFF //

PRIVATE iter_t MAP_PRIVATE_SYMBOL(iter_next_func)(iter_t self) {
    // Basically iterate through current entry list then move to next bucket at end

    // If we have no element (aka end iterator) we just return self
    if (iter_element(self) == NULL) {
        return self;
    }

    const MAP_TYPE* map = iterc_container(self);
    const MAP_SYMBOL(entry_t*) entry = iterc_element(self);

    MAP_PRIVATE_SYMBOL(entry_list_t*) entry_list = MAP_PRIVATE_SYMBOL(get_entry_list)(map, MAP_SYMBOL(entry_key)(entry));
    iter_t entry_iter = MAP_PRIVATE_SYMBOL(entry_list_iter)(entry_list, entry);
    iter_t entry_iter_next = iter_next(entry_iter);

    iter_t ret = MAP_SYMBOL(end)(map);
    if (iter_equals(entry_iter_next, MAP_PRIVATE_SYMBOL(entry_list_end)(entry_list))) {
        // If we reach the end of this entry list we find the next filled entry list and return the first element
        usize next_index = MAP_PRIVATE_SYMBOL(hash_to_bucket_index)(map, entry->_key) + 1;
        MAP_PRIVATE_SYMBOL(entry_list_t*) next_entry_list = MAP_PRIVATE_SYMBOL(get_next_filled_entry_list)(map, next_index);
        if (next_entry_list != NULL) {
            ret._element = MAP_PRIVATE_SYMBOL(entry_list_begin)(next_entry_list)._element;
        }
    } else {
        // Else we simply return the next element in our current entry_list
        ret._element = entry_iter_next._element;
    }

    return ret;
}

// TODO: Implement function
PRIVATE iter_t MAP_PRIVATE_SYMBOL(iter_prev_func)(iter_t self) {
    UNIMPLEMENTED_FUNCTION();
}

PRIVATE iter_vtable_t MAP_PRIVATE_SYMBOL(iter_vtable) = (iter_vtable_t){
    .next_func = MAP_PRIVATE_SYMBOL(iter_next_func),
    .prev_func = MAP_PRIVATE_SYMBOL(iter_prev_func),
};

iter_t MAP_SYMBOL(begin)(const MAP_TYPE* map) {
    MAP_PRIVATE_SYMBOL(entry_list_t*) entry_list = MAP_PRIVATE_SYMBOL(get_next_filled_entry_list)(map, 0);

    return (iter_t){
        ._container = map,
        ._element = entry_list != NULL ? iter_element(MAP_PRIVATE_SYMBOL(entry_list_begin)(entry_list)) : NULL,
        ._vtable = &MAP_PRIVATE_SYMBOL(iter_vtable),
    };
}

iter_t MAP_SYMBOL(end)(const MAP_TYPE* map) {
    return (iter_t){
        ._container = map,
        ._element = NULL,
        ._vtable = &MAP_PRIVATE_SYMBOL(iter_vtable),
    };
}

// ENTRY STUFF

MAP_TEMPLATE MAP_SYMBOL(entry_value)(const MAP_SYMBOL(entry_t*) entry) {
    return entry->_value;
}

hash_t MAP_SYMBOL(entry_key)(const MAP_SYMBOL(entry_t*) entry) {
    return entry->_key;
}

void MAP_SYMBOL(entry_set_value)(MAP_SYMBOL(entry_t*) entry, MAP_TEMPLATE value) {
    entry->_value = value;
}

#endif // MAP_IMPLEMENTATION

#undef MAP_TEMPLATE
#undef MAP_TEMPLATE_DISPLAY_NAME
#undef MAP_SYMBOL
#undef MAP_PRIVATE_SYMBOL
#undef MAP_TYPE
#undef MAP_NAME
#undef MAP_IMPLEMENTION