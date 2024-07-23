#include "httpc.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    const char* request_string = "POST /api/example HTTP/1.1\r\n"
                                 "Content-Type: application/json\r\n"
                                 "Content-Length: 18\r\n"
                                 "\r\n"
                                 "{\r\n"
                                 "\t\"key\": \"value\"\n"
                                 "}\r\n";

    httpc_request_t* request = httpc_request_from_string(request_string);
    assert(request->method == HTTPC_POST);
    assert(strcmp(request->url, "/api/example") == 0);
    assert(strcmp(request->headers->key, "Content-Type") == 0);
    assert(strcmp(request->headers->value, "application/json") == 0);
    assert(strcmp(request->headers->next->key, "Content-Length") == 0);
    assert(strcmp(request->headers->next->value, "18") == 0);
    assert(strcmp(request->body, "{\r\n\t\"key\": \"value\"\n}\r\n") == 0);

    httpc_request_free(request);

    puts("All tests passed!");
    
    return 0;
}
