#include <assert.h>
#include <ctype.h>
#include <httpc.h>
#include <stdio.h>
#include <string.h>

#include "macros.h"

httpc_request_t httpc_request_new(httpc_method_t method, const char* url) {
    assert(url != NULL);

    httpc_request_t request = {
        .method = method,
        .url = httpc_allocator.alloc(strlen(url) + 1),
        .headers = list_new(httpc_header_t, httpc_allocator),
        .body = array_new(byte, httpc_allocator),
    };

    assert(request.url != NULL);
    strcpy(request.url, url);

    return request;
}

void httpc_request_free(httpc_request_t* request) {
    assert(request != NULL);

    if (request->url != NULL) httpc_allocator.free(request->url);
    list_clear(&request->headers, (free_fn)httpc_header_free);
    array_clear(&request->body);
}

array_t(byte) httpc_request_serialize(httpc_request_t* request) {
    assert(request != NULL);

    array_t bytes = array_new(byte, httpc_allocator);

    {
        const char* method_str = httpc_method_to_string(request->method);
        array_append_v(&bytes, (byte*)method_str, strlen(method_str));
    }

    array_append_v(&bytes, (byte*)" ", 1);
    array_append_v(&bytes, (byte*)request->url, strlen(request->url));
    array_append_v(&bytes, (byte*)" HTTP/", 6);

    {
        char version[16];
        sprintf(version, "%d.%d", HTTP_VERSION_MAJOR, HTTP_VERSION_MINOR);
        array_append_v(&bytes, (byte*)version, strlen(version));
    }

    array_append_v(&bytes, (byte*)"\r\n", 2);

    {
        httpc_header_t* header = NULL;
        list_foreach(header, &request->headers) {
            array_append_v(&bytes, (byte*)header->key, strlen(header->key));
            array_append_v(&bytes, (byte*)": ", 2);
            array_append_v(&bytes, (byte*)header->value, strlen(header->value));
            array_append_v(&bytes, (byte*)"\r\n", 2);
        }

        // Add Content-Length header if not present. You can lie about it, if you want.
        if (httpc_header_list_get(&request->headers, "Content-Length") == NULL) {
            char content_length[16];
            sprintf(content_length, "%lu", request->body.size);
            array_append_v(&bytes, (byte*)"Content-Length: ", 16);
            array_append_v(&bytes, (byte*)content_length, strlen(content_length));
            array_append_v(&bytes, (byte*)"\r\n", 2);
        }
    }

    array_append_v(&bytes, (byte*)"\r\n", 2);

    if (request->body.size > 0) {
        array_append_a(&bytes, &request->body);
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

httpc_error_t httpc_request_deserialize(const array_t(byte) * buffer, httpc_request_t* out_request) {
    assert(buffer != NULL);
    assert(out_request != NULL);

    // And here I was, wondering just HOW can call to malloc case a segfault...
    out_request->headers = list_new(httpc_header_t, httpc_allocator);
    out_request->body = array_new(byte, httpc_allocator);

    if (buffer->size < 18) {
        return HTTPC_NOT_ENOUGH_DATA;
    }

    char* data = httpc_allocator.alloc(buffer->size + 1);
    memcpy((void*)data, buffer->data, buffer->size);

    {
        char* method = strtok(data, " ");
        if (method == NULL) {
            httpc_allocator.free((void*)data);
            return HTTPC_MALFORMED_DATA;
        }

        httpc_method_t http_method = httpc_method_from_string(method);
        if (http_method == -1) {
            httpc_allocator.free((void*)data);
            return HTTPC_INVALID_METHOD;
        }

        out_request->method = http_method;
    }

    {
        char* url = strtok(NULL, " ");
        if (url == NULL) {
            httpc_allocator.free((void*)data);
            return HTTPC_MALFORMED_DATA;
        }

        out_request->url = _httpc_strclone_(url);
    }

    {
        char* version = strtok(NULL, "\r\n");
        if (version == NULL) {
            httpc_allocator.free((void*)data);
            return HTTPC_MALFORMED_DATA;
        }

        // for now, we don't care about the version
        // TODO: version should be stored in the request struct
    }

    {
        char* line = strtok(NULL, "\r\n");
        while (line != NULL && strlen(line) > 0) {
            char* key = strtok(line, ":");
            key = _httpc_strip_(key);
            char* value = strtok(NULL, "");
            value = _httpc_strip_(value);

            httpc_header_t* header = httpc_header_new(key, value);
            list_insert(&out_request->headers, header, LIST_BACK);

            line = strtok(NULL, "\r\n");
        }
    }

    {
        char* body = strtok(NULL, "");
        if (body != NULL) {
            array_append_v(&out_request->body, (byte*)body, strlen(body));
        }
    }

    httpc_allocator.free((void*)data);

    return HTTPC_NO_ERROR;
}
