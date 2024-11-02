#ifndef _HTTPC_ARRAY_H_
#define _HTTPC_ARRAY_H_

#include <limits.h>
#include <stdbool.h>
#include <stddef.h>

#include "allocator.h"

typedef struct _array_s {
    void* data;
    size_t size;
    size_t capacity;
    size_t sizeof_element;
    allocator_t allocator;
} array_t;
#define array_t(T) array_t

typedef struct _array_iterator_s {
    array_t* array;
    size_t index;
} array_iter_t;

array_t array_new_ex(size_t sizeof_element, allocator_t allocator);
#define array_new(type) array_new_ex(sizeof(type), std_allocator)

array_iter_t array_iter(array_t* array);
void* array_next(array_iter_t* iter);

#define array_foreach(elem, array) \
    for (array_iter_t _iter_##elem = array_iter(array); (elem = array_next(&_iter_##elem)) != NULL;)

enum {
    ARRAY_FRONT = 0,
    // ...
    ARRAY_BACK = SSIZE_MAX
};

bool array_reserve(array_t* array, size_t capacity);

bool array_append_v(array_t* array, const void* data, size_t count);
bool array_append_a(array_t* array_to, array_t* array_from);
void* array_collect(array_t* array, size_t from, size_t to);
array_t array_slice(array_t* array, size_t from, size_t to);
void array_clear(array_t* array);

#endif
