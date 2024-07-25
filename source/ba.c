#include <ba.h>

#include <string.h>
#include <stdlib.h>

byte_array_t* byte_array_new(size_t capacity) {
    byte_array_t* ba = malloc(sizeof(byte_array_t));
    ba->data = malloc(capacity);
    ba->size = 0;
    ba->capacity = capacity;
    return ba;
}

void byte_array_free(byte_array_t* ba) {
    if (ba->data != NULL) {
        free(ba->data);
    }
    if (ba != NULL) {
        free(ba);
    }
}

void byte_array_grow(byte_array_t* ba) {
    ba->capacity *= 2;
    ba->data = realloc(ba->data, ba->capacity);
}

void byte_array_append_byte(byte_array_t* ba, char c) {
    if (ba->size == ba->capacity) {
        byte_array_grow(ba);
    }

    ba->data[ba->size] = c;
    ba->size++;
}

void byte_array_append_bytes(byte_array_t* ba, const char* data, size_t size) {
    size_t new_size = ba->size + size;
    if (new_size > ba->capacity) {
        while (new_size > ba->capacity) {
            byte_array_grow(ba);
        }
    }

    strncpy(ba->data + ba->size, data, size);
    ba->size = new_size;
}

char* byte_array_collect(byte_array_t* ba) {
    char* data_cpy = malloc(ba->size);
    memcpy(data_cpy, ba->data, ba->size);
    return data_cpy;
}

void byte_array_append_str(byte_array_t* ba, const char* str) {
    byte_array_append_bytes(ba, str, strlen(str));
}
