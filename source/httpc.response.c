#include <assert.h>
#include <httpc.h>
#include <string.h>

#include "macros.h"

httpc_response_t httpc_response_new(const char* status_message, int status_code) {
    assert(status_message != NULL);

    httpc_response_t response = {
        .status_code = status_code,
        .status_message = httpc_allocator.alloc(strlen(status_message) + 1),
        .headers = list_new(httpc_header_t),
        .body = array_new(byte),
    };

    assert(response.status_message != NULL);
    strcpy(response.status_message, status_message);

    return response;
}

void httpc_response_free(httpc_response_t* response) {
    assert(response != NULL);

    if (response->status_message != NULL) httpc_allocator.free(response->status_message);
    list_clear(&response->headers, httpc_header_free);
    array_clear(&response->body);
}

array_t(byte) httpc_response_serialize(httpc_response_t* response){NOT_IMPLEMENTED}

httpc_response_t httpc_response_deserialize(const array_t(byte) * data) {
    NOT_IMPLEMENTED
}
