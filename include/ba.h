#ifndef _GA_H
#define _GA_H

#include <stddef.h>

typedef struct _byte_array_s {
    char* data;
    size_t size;
    size_t capacity;
} byte_array_t;

byte_array_t* byte_array_new(size_t capacity);
void byte_array_free(byte_array_t* ba);

void byte_array_grow(byte_array_t* ba);
void byte_array_append_byte(byte_array_t* ba, char c);
void byte_array_append_bytes(byte_array_t* ba, const char* data, size_t size);
void byte_array_append_str(byte_array_t* ba, const char* str);

char* byte_array_collect(byte_array_t* ba);

char* strsep_x(char** stringp, const char* delim);

#endif