#include "acutest.h"
#include "httpc.h"
#include <stdbool.h>

const char* sample_response_string = "HTTP/1.1 200 OK\r\n"
    "Host: example.com\r\n"
    "Content-Type: application/json\r\n"
    "Content-Length: 22\r\n"
    "\r\n"
    "{\r\n\t\"key\": \"value\"\n}\r\n";

void test_httpc_response_parsing(void) {
    TEST_ASSERT(false);
    TEST_MSG("Not implemented");
}

void test_httpc_response_serialization(void) {
    httpc_response_t* response = httpc_response_new("OK", 200);
    TEST_ASSERT(response != NULL);

    httpc_add_header_v(&response->headers, "Host", "example.com");
    httpc_add_header_v(&response->headers, "Content-Type", "application/json");
    // Content-Length is not added, but it should be present in the serialized response
    
    const uint8_t body[] = "{\r\n\t\"key\": \"value\"\n}\r\n";
    httpc_response_set_body(response, body, strlen((const char*)body));

    size_t out_size;
    char* response_str = httpc_response_to_string(response, &out_size);
    TEST_ASSERT(response_str != NULL);

    TEST_CHECK(strcmp(response_str, sample_response_string) == 0);
    TEST_DUMP("Expected:", sample_response_string, strlen(sample_response_string));
    TEST_DUMP("Actual:", response_str, strlen(response_str));

    free(response_str);
}

TEST_LIST = {
    { "httpc_response_parsing", test_httpc_response_parsing },
    { "httpc_response_serialization", test_httpc_response_serialization },
    { NULL, NULL }
};
