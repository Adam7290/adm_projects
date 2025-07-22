#include "framework/test.h"

#define LIST_IMPLEMENTATION
#define LIST_TEMPLATE char
#include <adm_utils/list_impl.h>

#include <string.h>

void test_list_char_content(list_char_t* list, const char* cmp) {
    PANIC_ASSERT(strlen(cmp) == list_char_length(list), "Cmp does not equal list length.");
    usize index = 0;
    for (iter_t it = list_char_begin(list); !iter_equals(it, list_char_end(list)); it = iter_next(it)) {
        if ((*(char*)iter_element(it)) != cmp[index++]) {
            PANIC("List doesn't equal cmp.");
        }
    }
}

void test_list() {
    arena_t arena = arena_new();

    list_char_t list = list_char_new(&arena);
    { // Pushing and popping
        test_list_char_content(&list, "");

        list_char_push_back(&list, 'B');
        test_list_char_content(&list, "B");

        list_char_push_back(&list, 'C');
        test_list_char_content(&list, "BC");

        list_char_push_front(&list, 'A');
        test_list_char_content(&list, "ABC");

        char out;
        PANIC_ASSERT(list_char_pop_back(&list, &out) == true, "");
        PANIC_ASSERT(out == 'C', "");
        test_list_char_content(&list, "AB");

        PANIC_ASSERT(list_char_pop_front(&list, &out) == true, "");
        PANIC_ASSERT(out == 'A', "");
        test_list_char_content(&list, "B");

        PANIC_ASSERT(list_char_pop_front(&list, &out) == true, "");
        PANIC_ASSERT(out == 'B', "");
        PANIC_ASSERT(list_char_empty(&list) == true, "");
        
        PANIC_ASSERT(list_char_pop_back(&list, &out) == false, "");
        PANIC_ASSERT(list_char_pop_front(&list, &out) == false, "");
    }

    { // Removing, clearing, and empty
        list_char_push_back(&list, 'Y');
        list_char_push_back(&list, 'e');
        list_char_push_back(&list, 'a');

        list_char_remove(&list, list_char_begin(&list));
        test_list_char_content(&list, "ea");

        list_char_push_front(&list, 'S');
        list_char_clear(&list);
        PANIC_ASSERT(list_char_empty(&list) == true, "");
    }

    { // Length
        PANIC_ASSERT(list_char_empty(&list) == true && list_char_length(&list) == 0, "");

        list_char_push_back(&list, 'A');
        PANIC_ASSERT(list_char_length(&list) == 1, "");

        list_char_push_back(&list, 'B');
        PANIC_ASSERT(list_char_length(&list) == 2, "");

        list_char_push_back(&list, 'C');
        PANIC_ASSERT(list_char_length(&list) == 3, "");
    }

    arena_destroy(&arena);
}