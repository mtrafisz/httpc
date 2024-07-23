#include "httpc.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    const char* response_test_string = "HTTP/1.1 200 OK\r\n"
                                       "Content-Type: application/json\r\n"
                                       "Content-Length: 16\r\n"
                                       "\r\n"
                                       "{\"key\": \"value\"}";

    httpc_response_t* response = httpc_response_new("OK", 200);
    httpc_add_header(&response->headers, "Content-Type", "application/json");
    const char* body = "{\"key\": \"value\"}";

    httpc_response_set_body(response, (uint8_t*)body, strlen(body));

    char* response_str = httpc_response_to_string(response, NULL);
    assert(strcmp(response_str, response_test_string) == 0);

    free(response_str);

    httpc_response_free(response);

    puts("All tests passed!");
    
    return 0;
}
