#include <assert.h>
#include <ctype.h>
#include <httpc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"

httpc_response_t httpc_response_new(const char* status_message, int status_code) {
    assert(status_message != NULL);

    httpc_response_t response = {
        .status_code = status_code,
        .status_message = httpc_allocator.alloc(strlen(status_message) + 1),
        .headers = list_new(httpc_header_t, httpc_allocator),
        .body = array_new(byte, httpc_allocator),
    };

    assert(response.status_message != NULL);
    strcpy(response.status_message, status_message);

    return response;
}

void httpc_response_free(httpc_response_t* response) {
    assert(response != NULL);

    if (response->status_message != NULL) httpc_allocator.free(response->status_message);
    list_clear(&response->headers, (free_fn)httpc_header_free);
    array_clear(&response->body);
}

array_t(byte) httpc_response_serialize(httpc_response_t* response) {
    assert(response != NULL);

    array_t bytes = array_new(byte, httpc_allocator);

    {
        char status_code[16];
        sprintf(status_code, "%d", response->status_code);
        array_append_v(&bytes, (byte*)"HTTP/1.1 ", 9);
        array_append_v(&bytes, (byte*)status_code, strlen(status_code));
        array_append_v(&bytes, (byte*)" ", 1);
        array_append_v(&bytes, (byte*)response->status_message, strlen(response->status_message));
        array_append_v(&bytes, (byte*)"\r\n", 2);
    }

    {
        httpc_header_t* header = NULL;
        list_foreach(header, &response->headers) {
            array_append_v(&bytes, (byte*)header->key, strlen(header->key));
            array_append_v(&bytes, (byte*)": ", 2);
            array_append_v(&bytes, (byte*)header->value, strlen(header->value));
            array_append_v(&bytes, (byte*)"\r\n", 2);
        }
    }

    array_append_v(&bytes, (byte*)"\r\n", 2);

    if (response->body.size > 0) {
        array_append_v(&bytes, response->body.data, response->body.size);
    }

    return bytes;
}

// TODO: move to strutils?

static char* _httpc_strclone_(const char* str) {
    char* clone = httpc_allocator.alloc(strlen(str) + 1);
    strcpy(clone, str);
    return clone;
}

static char* _httpc_strip_(char* str) {
    char* end;

    while (isspace((unsigned char)*str)) {
        str++;
    }

    if (*str == 0) {
        return str;
    }

    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) {
        end--;
    }

    *(end + 1) = 0;

    return str;
}

#include <logger.h>
// isprint:
#include <ctype.h>

#define isprint(c) ((c) >= 32 && (c) < 127)

void _httpc_buffer_dump_(const void* buffer, size_t size_bytes) {
    log_info("Buffer dump:");

    byte* data_hex = (byte*)buffer;
    char* data_ascii = (char*)buffer;

    for (size_t i = 0; i < size_bytes; i += 16) {
        char line[80];
        char* ptr = line;
        ptr += sprintf(ptr, "%08x: ", (unsigned int)i);
        for (size_t j = 0; j < 16; j++) {
            if (i + j < size_bytes) {
                ptr += sprintf(ptr, "%02x ", data_hex[i + j]);
            } else {
                ptr += sprintf(ptr, "   ");
            }
        }
        ptr += sprintf(ptr, " ");
        for (size_t j = 0; j < 16; j++) {
            if (i + j < size_bytes) {
                ptr += sprintf(ptr, "%c", isprint(data_ascii[i + j]) ? data_ascii[i + j] : '.');
            }
        }
        log_info("%s", line);
    }
}

httpc_error_t httpc_response_deserialize(const array_t(byte) * buffer, httpc_response_t* out_response) {
    assert(buffer != NULL);
    assert(out_response != NULL);

    out_response->headers = list_new(httpc_header_t, httpc_allocator);
    out_response->body = array_new(byte, httpc_allocator);

    if (buffer->size < 18) {
        return HTTPC_NOT_ENOUGH_DATA;
    }

    char* data = httpc_allocator.alloc(buffer->size + 1);
    memcpy(data, buffer->data, buffer->size);

    {
        char* proto = strtok(data, " ");
        char* status_code = strtok(NULL, " ");
        char* status_message = strtok(NULL, "\r\n");

        if (proto == NULL || status_code == NULL || status_message == NULL) {
            httpc_allocator.free((void*)data);
            return HTTPC_MALFORMED_DATA;
        }

        if (strcmp(proto, "HTTP/1.1") != 0) {
            log_warn("Unsupported protocol: %s", proto);
            // TODO: http version should be stored in the response struct
        }

        char* err = NULL;
        out_response->status_code = strtol(status_code, &err, 10);
        if (*err != '\0') {
            httpc_allocator.free((void*)data);
            return HTTPC_MALFORMED_DATA;
        }

        out_response->status_message = _httpc_strclone_(status_message);
    }

    {
        char* line = strtok(NULL, "\r\n");
        while (line != NULL && *line != '\0') {
            char* key = strtok(line, ":");
            char* value = strtok(NULL, "\r\n");

            if (key == NULL || value == NULL) {
                httpc_allocator.free((void*)data);
                return HTTPC_MALFORMED_DATA;
            }

            key = _httpc_strip_(key);
            value = _httpc_strip_(value);

            httpc_header_t* header = httpc_header_new(key, value);
            list_insert(&out_response->headers, header, LIST_BACK);

            line = strtok(NULL, "\r\n");
        }
    }

    {
        char* body = strtok(NULL, "\0");
        if (body != NULL) {
            array_append_v(&out_response->body, (byte*)body, strlen(body));
        }
    }

    httpc_allocator.free((void*)data);
    return HTTPC_NO_ERROR;
}
