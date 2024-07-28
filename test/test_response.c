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
    httpc_response_t* response = httpc_response_from_string(sample_response_string, strlen(sample_response_string));
    TEST_ASSERT(response != NULL);

    TEST_CHECK(response->status_code == 200);
    TEST_CHECK(strcmp(response->status_text, "OK") == 0);

    httpc_header_t* h = response->headers;
    TEST_ASSERT(h != NULL);
    TEST_CHECK(strcmp(h->key, "Host") == 0);
    TEST_CHECK(strcmp(h->value, "example.com") == 0);

    h = h->next;
    TEST_ASSERT(h != NULL);
    TEST_CHECK(strcmp(h->key, "Content-Type") == 0);
    TEST_CHECK(strcmp(h->value, "application/json") == 0);

    h = h->next;
    TEST_ASSERT(h != NULL);
    TEST_CHECK(strcmp(h->key, "Content-Length") == 0);
    TEST_CHECK(strcmp(h->value, "22") == 0);

    TEST_CHECK(response->body_size == 22);
    TEST_CHECK(memcmp(response->body, "{\r\n\t\"key\": \"value\"\n}\r\n", 22) == 0);

    httpc_response_free(response);
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

    TEST_CHECK(strncmp(response_str, sample_response_string, out_size) == 0);
    TEST_DUMP("Expected:", sample_response_string, strlen(sample_response_string));
    TEST_DUMP("Actual:", response_str, out_size);

    free(response_str);

    httpc_response_free(response);
}

TEST_LIST = {
    { "httpc_response_parsing", test_httpc_response_parsing },
    { "httpc_response_serialization", test_httpc_response_serialization },
    { NULL, NULL }
};
