// TODO: Don't allocate the first node on the heap

#include "util.h"
#include "arena.h"
#include "panic.h"
#include "iter.h"

#ifndef LIST_TEMPLATE
#error You need to define LIST_TEMPLATE before including this header
#endif // LIST_TEMPLATE

#ifdef LIST_TEMPLATE_DISPLAY_NAME
#define LIST_NAME LIST_TEMPLATE_DISPLAY_NAME
#else
#define LIST_NAME LIST_TEMPLATE
#endif // LIST_TEMPLATE_DISPLAY_NAME

#ifndef LIST_TEMPLATE_PREFIX
#define LIST_TEMPLATE_PREFIX CONCAT3_EXPAND(list_, LIST_NAME, _)
#endif // LIST_TEMPLATE_PREFIX

#define LIST_SYMBOL(after) CONCAT_EXPAND(LIST_TEMPLATE_PREFIX, after)
#define LIST_PRIVATE_SYMBOL(after) CONCAT_EXPAND(_, LIST_SYMBOL(after))
#define LIST_TYPE LIST_SYMBOL(t)

#define LIST_NODE_TYPE LIST_PRIVATE_SYMBOL(node_t)

typedef struct LIST_NODE_TYPE {
    LIST_TEMPLATE _data;
    NULLABLE struct ARENA_MANAGED(LIST_NODE_TYPE) _next;
    NULLABLE struct ARENA_MANAGED(LIST_NODE_TYPE) _previous;
} LIST_NODE_TYPE;

typedef struct LIST_TYPE {
    arena_t* _arena;
    NULLABLE ARENA_MANAGED(LIST_NODE_TYPE) _head;
    NULLABLE ARENA_MANAGED(LIST_NODE_TYPE) _tail;
} LIST_TYPE;

// TODO: Clone function

// Create new linked list
LIST_TYPE LIST_SYMBOL(new)(arena_t* arena);
void LIST_SYMBOL(push_back)(LIST_TYPE* list, LIST_TEMPLATE element);
void LIST_SYMBOL(push_front)(LIST_TYPE* list, LIST_TEMPLATE element);
bool LIST_SYMBOL(pop_back)(LIST_TYPE* list, LIST_TEMPLATE* out);
bool LIST_SYMBOL(pop_front)(LIST_TYPE* list, LIST_TEMPLATE* out);
iter_t LIST_SYMBOL(remove)(LIST_TYPE* list, const iter_t iter);
bool LIST_SYMBOL(empty)(LIST_TYPE* list);
void LIST_SYMBOL(clear)(LIST_TYPE* list);
// TODO: Free function
// O(n) time
usize LIST_SYMBOL(length)(LIST_TYPE* list);
iter_t LIST_SYMBOL(iter)(LIST_TYPE* list, LIST_TEMPLATE* element);
iter_t LIST_SYMBOL(begin)(LIST_TYPE* list);
iter_t LIST_SYMBOL(end)(LIST_TYPE* list);


#ifdef LIST_IMPLEMENTATION

PRIVATE void LIST_PRIVATE_SYMBOL(remove_node)(LIST_TYPE* list, LIST_NODE_TYPE* node);

LIST_TYPE LIST_SYMBOL(new)(arena_t* arena) {
    return (LIST_TYPE){
        ._arena = arena,
        ._head = NULL,
        ._tail = NULL,
    };
}

void LIST_SYMBOL(push_back)(LIST_TYPE* list, LIST_TEMPLATE element) {
    ARENA_MANAGED(LIST_NODE_TYPE) node = arena_alloc(list->_arena, LIST_NODE_TYPE);
    node->_data = element;
    node->_next = NULL;
    node->_previous = NULL;

    if (list->_tail == NULL) {
        // This node is the only element in the list
        list->_head = node;
        list->_tail = node;
    } else {
        // Set prev tail next to new node, then make new node the tail
        list->_tail->_next = node;
        node->_previous = list->_tail;
        list->_tail = node;
    }
}

void LIST_SYMBOL(push_front)(LIST_TYPE* list, LIST_TEMPLATE element) {
    ARENA_MANAGED(LIST_NODE_TYPE) node = arena_alloc(list->_arena, LIST_NODE_TYPE);
    node->_data = element;
    node->_next = NULL;
    node->_previous = NULL;

    if (list->_head == NULL) {
        // This node is the only element in the list
        list->_tail = node;
        list->_head = node;
    } else {
        // Set prev tail next to new node, then make new node the tail
        list->_head->_previous = node;
        node->_next = list->_head;
        list->_head = node;
    }
}

bool LIST_SYMBOL(pop_back)(LIST_TYPE* list, LIST_TEMPLATE* out) {
    if (list->_tail == NULL) {
        return false;
    }

    *out = list->_tail->_data;
    LIST_PRIVATE_SYMBOL(remove_node)(list, list->_tail);
    return true;
}

bool LIST_SYMBOL(pop_front)(LIST_TYPE* list, LIST_TEMPLATE* out) {
    if (list->_head == NULL) {
        return false;
    }

    *out = list->_head->_data;
    LIST_PRIVATE_SYMBOL(remove_node)(list, list->_head);
    return true;
}

iter_t LIST_SYMBOL(remove)(LIST_TYPE* list, const iter_t iter) {
    PANIC_ASSERT_DEBUG(list == iter_container(iter), "Iterator isn't for this specific list.");
    PANIC_ASSERT_DEBUG(LIST_SYMBOL(empty)(list) == false, "Invalid iterator.");
    PANIC_ASSERT_DEBUG(!iter_equals(iter, LIST_SYMBOL(end)(list)), "Invalid iterator.");


    iter_t ret = iter_next(iter);
    LIST_PRIVATE_SYMBOL(remove_node)(list, iter_element(iter));
    return ret;
}

bool LIST_SYMBOL(empty)(LIST_TYPE* list) {
    return list->_head == NULL;
}

void LIST_SYMBOL(clear)(LIST_TYPE* list) {
    for (iter_t it = LIST_SYMBOL(begin)(list); !iter_equals(it, LIST_SYMBOL(end)(list)); it = iter_next(it)) {
        LIST_SYMBOL(remove)(list, it);
    }
}

// TODO: Make O(1) time by incrementing/decrementing counter
usize LIST_SYMBOL(length)(LIST_TYPE* list) {
    usize length = 0;

    for (iter_t it = LIST_SYMBOL(begin)(list); !iter_equals(it, LIST_SYMBOL(end)(list)); it = iter_next(it)) {
        length++;
    }

    return length;
}

// ITERATOR STUFF //

PRIVATE iter_t LIST_PRIVATE_SYMBOL(iter_next_func)(const iter_t self) {
    iter_t ret = self;
    LIST_NODE_TYPE* current_node = ret._element;

    // If this current iter is null then just return null
    if (current_node == NULL) {
        return ret;
    }

    ret._element = current_node->_next;

    return ret;
}

PRIVATE iter_t LIST_PRIVATE_SYMBOL(iter_prev_func)(const iter_t self) {
    LIST_TYPE* list = iter_container(self);
    
    iter_t ret = self;
    LIST_NODE_TYPE* current_node = ret._element;

    // If current_node is end then previous would be tail
    if (current_node == NULL) {
        ret._element = list->_tail;
        return ret;
    }

    ret._element = current_node->_previous;

    return ret;
}

PRIVATE iter_vtable_t LIST_PRIVATE_SYMBOL(iter_vtable) = (iter_vtable_t){
    .next_func = LIST_PRIVATE_SYMBOL(iter_next_func),
    .prev_func = LIST_PRIVATE_SYMBOL(iter_prev_func),
    .access_func = NULL,
    .distance_func = NULL,
};

iter_t LIST_SYMBOL(iter)(LIST_TYPE* list, LIST_TEMPLATE* element) {
    return (iter_t){
        ._container = list,
        ._element = element,
        ._vtable = &LIST_PRIVATE_SYMBOL(iter_vtable),
    };
}

iter_t LIST_SYMBOL(begin)(LIST_TYPE* list) {
    return (iter_t){
        ._container = list,
        ._element = list->_head,
        ._vtable = &LIST_PRIVATE_SYMBOL(iter_vtable),
    };
}

iter_t LIST_SYMBOL(end)(LIST_TYPE* list) {
    return (iter_t){
        ._container = list,
        ._element = NULL,
        ._vtable = &LIST_PRIVATE_SYMBOL(iter_vtable),
    };
}

// PRIVATE METHODS

PRIVATE void LIST_PRIVATE_SYMBOL(remove_node)(LIST_TYPE* list, LIST_NODE_TYPE* node) {
    // We are removing the last node in the list
    if (list->_head == list->_tail) {
        PANIC_ASSERT_DEBUG(list->_head == node, "Removing node that isn't in list.");

        arena_free(list->_arena, node);
        list->_head = NULL;
        list->_tail = NULL;
        return;
    }

    // Remove head node
    if (node == list->_head) {
        PANIC_ASSERT_DEBUG(node->_next != NULL, "Node doesn't have a next node even though there is multiple nodes in list.");
        node->_next->_previous = NULL;
        list->_head = node->_next;
        arena_free(list->_arena, node);
        return;
    }

    // Remove tail node
    if (node == list->_tail) {
        PANIC_ASSERT_DEBUG(node->_previous != NULL, "Node doesn't have a previous node even though there is multiple node in list.");
        node->_previous->_next = NULL;
        list->_tail = node->_previous;
        arena_free(list->_arena, node);
        return;
    }

    // Remove body node
    PANIC_ASSERT_DEBUG(node->_previous != NULL && node->_next != NULL, "Node is a body node but doesn't have a next or previous node.");
    node->_previous->_next = node->_next;
    node->_next->_previous = node->_previous;
    arena_free(list->_arena, node);
}

#endif // LIST_IMPLEMENTATION

#undef LIST_TEMPLATE
#undef LIST_TEMPLATE_DISPLAY_NAME
#undef LIST_TEMPLATE_PREFIX
#undef LIST_SYMBOL
#undef LIST_PRIVATE_SYMBOL
#undef LIST_TYPE
#undef LIST_NODE_TYPE
#undef LIST_NAME
#undef LIST_IMPLEMENTION