#include "iter.h"
#include "panic.h"

iter_t iter_next(const iter_t self) {
    PANIC_ASSERT_DEBUG(self._vtable->next_func != NULL, "Iterator has no next_func in vtable.");
    return self._vtable->next_func(self);
}

iter_t iter_prev(const iter_t self) {
    PANIC_ASSERT_DEBUG(self._vtable->next_func != NULL, "Iterator has no prev_func in vtable.");
    return self._vtable->prev_func(self);
}

iter_t iter_access(const iter_t self, usize offset) {
    PANIC_ASSERT_DEBUG(self._vtable->access_func != NULL, "Iterator has no access_func in vtable.");
    return self._vtable->access_func(self, offset);
}

isize iter_distance(const iter_t first, const iter_t last) {
    PANIC_ASSERT_DEBUG(first._vtable->distance_func != NULL, "First iterator has no distance_func in vtable.");
    PANIC_ASSERT_DEBUG(last._vtable->distance_func != NULL, "Last iterator has no distance_func in vtable.");
    PANIC_ASSERT_DEBUG(first._vtable->distance_func == last._vtable->distance_func, "Incompatible iterators.");
    return first._vtable->distance_func(first, last);
}

bool iter_equals(const iter_t iter1, const iter_t iter2) {
    return iter1._container == iter2._container && iter1._element == iter2._element;
}

const void* iterc_container(const iterc_t self) {
    return self._container;
}

const void* iterc_element(const iterc_t self) {
    return self._element;
}

void* iter_container(const iter_t self) {
    return self._container;
}

void* iter_element(const iter_t self) {
    return self._element;
}