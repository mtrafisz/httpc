#include <assert.h>
#include <httpc.h>
#include <string.h>

#include "macros.h"

httpc_request_t httpc_request_new(httpc_method_t method, const char* url) {
    assert(url != NULL);

    httpc_request_t request = {
        .method = method,
        .url = httpc_allocator.alloc(strlen(url) + 1),
        .headers = list_new(httpc_header_t),
        .body = array_new(byte),
    };

    assert(request.url != NULL);
    strcpy(request.url, url);

    return request;
}

void httpc_request_free(httpc_request_t* request) {
    assert(request != NULL);

    if (request->url != NULL) httpc_allocator.free(request->url);
    list_clear(&request->headers, httpc_header_free);
    array_clear(&request->body);
}

array_t(byte) httpc_request_serialize(httpc_request_t* request){NOT_IMPLEMENTED}

httpc_request_t httpc_request_deserialize(const array_t(byte) * data) {
    NOT_IMPLEMENTED
}
