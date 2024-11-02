#include "array.h"

#include <assert.h>
#include <stdint.h>
#include <string.h>

array_t array_new_ex(size_t sizeof_element, allocator_t allocator) {
    return (array_t){
        .data = NULL,
        .size = 0,
        .capacity = 0,
        .sizeof_element = sizeof_element,
        .allocator = allocator,
    };
}

bool array_reserve(array_t* array, size_t capacity) {
    assert(array != NULL);

    if (capacity <= array->capacity) {
        return true;
    }

    void* new_data = array->allocator.alloc(capacity * array->sizeof_element);
    if (new_data == NULL) {
        return false;
    }

    if (array->data) {
        memcpy(new_data, array->data, array->size * array->sizeof_element);
        array->allocator.free(array->data);
    }

    array->data = new_data;
    array->capacity = capacity;

    return true;
}

void array_clear(array_t* array) {
    assert(array != NULL);

    if (array->data) {
        array->allocator.free(array->data);
        array->data = NULL;
    }

    array->size = 0;
    array->capacity = 0;
}

bool array_append_v(array_t* array, const void* data, size_t count) {
    assert(array != NULL);
    assert(data != NULL);

    if (!array_reserve(array, array->size + count)) {
        return false;
    }

    memcpy((uint8_t*)array->data + array->size * array->sizeof_element, data, count * array->sizeof_element);
    array->size += count;

    return true;
}

bool array_append_a(array_t* array_to, array_t* array_from) {
    assert(array_to != NULL);
    assert(array_from != NULL);

    return array_append_v(array_to, array_from->data, array_from->size);
}

void* array_collect(array_t* array, size_t from, size_t to) {
    assert(array != NULL);

    if (to == ARRAY_BACK) {
        to = array->size - 1;
    }

    if (from >= array->size || to >= array->size || from > to) {
        return NULL;
    }

    size_t count = to - from + 1;
    void* data = array->allocator.alloc(count * array->sizeof_element);
    if (data == NULL) {
        return NULL;
    }

    memcpy(data, (uint8_t*)array->data + from * array->sizeof_element, count * array->sizeof_element);

    return data;
}

array_t array_slice(array_t* array, size_t from, size_t to) {
    assert(array != NULL);

    if (to == ARRAY_BACK) {
        to = array->size - 1;
    }

    if (from >= array->size || to >= array->size || from > to) {
        return array_new_ex(array->sizeof_element, array->allocator);
    }

    size_t count = to - from + 1;
    array_t slice = array_new_ex(array->sizeof_element, array->allocator);
    if (!array_reserve(&slice, count)) {
        return array_new_ex(array->sizeof_element, array->allocator);
    }

    memcpy(slice.data, (uint8_t*)array->data + from * array->sizeof_element, count * array->sizeof_element);
    slice.size = count;

    return slice;
}

array_iter_t array_iter(array_t* array) {
    assert(array != NULL);

    return (array_iter_t){
        .array = array,
        .index = 0,
    };
}

void* array_next(array_iter_t* iter) {
    assert(iter != NULL);

    if (iter->index >= iter->array->size) {
        return NULL;
    }

    return (uint8_t*)iter->array->data + iter->index++ * iter->array->sizeof_element;
}
