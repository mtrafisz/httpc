#include <assert.h>
#include <httpc.h>
#include <list.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "macros.h"

httpc_header_t* httpc_header_new(const char* key, const char* value) {
    assert(key != NULL);
    assert(value != NULL);

    httpc_header_t* header = httpc_allocator.alloc(sizeof(httpc_header_t));
    assert(header != NULL);

    header->key = httpc_allocator.alloc(strlen(key) + 1);
    header->value = httpc_allocator.alloc(strlen(value) + 1);

    assert(header->key != NULL);
    strcpy(header->key, key);

    assert(header->value != NULL);
    strcpy(header->value, value);

    return header;
}

void httpc_header_free(httpc_header_t* header) {
    assert(header != NULL);

    if (header->key != NULL) httpc_allocator.free(header->key);
    if (header->value != NULL) httpc_allocator.free(header->value);

    httpc_allocator.free(header);
}

bool httpc_header_list_add_v(httpc_header_list_t* list, const char* key, const char* value) {
    assert(list != NULL);

    httpc_header_t* header = httpc_header_new(key, value);
    return list_insert(list, header, LIST_BACK);
}

bool httpc_header_list_add_h(httpc_header_list_t* list, httpc_header_t* header) {
    assert(list != NULL);
    assert(header != NULL);

    return list_insert(list, header, LIST_BACK);
}

static int _httpc_vasprint_(char** strp, const char* fmt, va_list args) {
    va_list args_copy;
    va_copy(args_copy, args);

    int size = vsnprintf(NULL, 0, fmt, args_copy);
    va_end(args_copy);

    *strp = httpc_allocator.alloc(size + 1);
    return vsprintf(*strp, fmt, args);
}

bool httpc_header_list_add_f(httpc_header_list_t* list, const char* key, const char* fmt, ...) {
    assert(list != NULL);
    assert(key != NULL);
    assert(fmt != NULL);

    va_list args;
    va_start(args, fmt);

    char* value = NULL;
    _httpc_vasprint_(&value, fmt, args);

    va_end(args);

    bool result = httpc_header_list_add_v(list, key, value);
    httpc_allocator.free(value);

    return result;
}

char* httpc_header_list_get(httpc_header_list_t* list, const char* key) {
    assert(list != NULL);
    assert(key != NULL);

    httpc_header_t* header;

    list_foreach(header, list) {
        if (strcmp(header->key, key) == 0) {
            return header->value;
        }
    }

    return NULL;
}
