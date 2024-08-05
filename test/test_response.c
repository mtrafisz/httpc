#include "acutest.h"
#include "httpc.h"
#include <stdbool.h>
#include <errno.h>

const char* sample_response_string = "HTTP/1.1 200 OK\r\n"
    "Host: example.com\r\n"
    "Content-Type: application/json\r\n"
    "Content-Length: 22\r\n"
    "\r\n"
    "{\r\n\t\"key\": \"value\"\n}\r\n";

const char* sample_response_string_bin = "HTTP/1.1 200 OK\r\n"  // 17
    "Host: example.com\r\n"                                     // 19
    "Content-Type: application/octet-stream\r\n"                // 40
    "Content-Length: 8\r\n"                                     // 19
    "\r\n"                                                      // 2
    "\0\1\2\3\4\5\6\7";                                         // 8
#define sample_response_string_bin_size 105

void test_httpc_response_headers(void) {
    HttpcResponse* res = httpc_response_new("OK", 200);
    TEST_ASSERT(res != NULL);

    httpc_add_header_h(&res->headers, httpc_header_new("Host", "example.com"));
    httpc_add_header_v(&res->headers, "Content-Type", "application/json");
    httpc_add_header_f(&res->headers, "Content-Length", "%d", 22);

    TEST_CHECK(res->status_code == 200);
    TEST_CHECK(strcmp(res->status_text, "OK") == 0);

    TEST_CHECK(strcmp(httpc_get_header_value(res->headers, "Host"), "example.com") == 0);
    TEST_MSG("Expected: example.com");
    TEST_MSG("Actual: %s", httpc_get_header_value(res->headers, "Host"));

    TEST_CHECK(strcmp(httpc_get_header_value(res->headers, "Content-Type"), "application/json") == 0);
    TEST_MSG("Expected: application/json");
    TEST_MSG("Actual: %s", httpc_get_header_value(res->headers, "Content-Type"));

    TEST_CHECK(strcmp(httpc_get_header_value(res->headers, "Content-Length"), "22") == 0);
    TEST_MSG("Expected: 22");
    TEST_MSG("Actual: %s", httpc_get_header_value(res->headers, "Content-Length"));

    httpc_response_free(res);
}

void test_httpc_response_parsing(void) {
    HttpcResponse* response = httpc_response_from_string((uint8_t*)sample_response_string, strlen(sample_response_string));
    TEST_ASSERT(response != NULL);

    TEST_CHECK(response->status_code == 200);
    TEST_CHECK(strcmp(response->status_text, "OK") == 0);

    HttpcHeader* h = response->headers;
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
    HttpcResponse* response = httpc_response_new("OK", 200);
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

// Valgrind will complain about invalid reads of size 1, but I think it's because input is not
// null-terminated, nor is it terminated by \r\n. At least it's not segfaulting...
void test_httpc_response_invalid_input(void) {
    HttpcResponse* response = httpc_response_from_string((uint8_t*)sample_response_string, 5);
    TEST_CHECK(response == NULL);
    TEST_CHECK(errno == EINVAL);
    TEST_MSG("Error: %s", strerror(errno));
}

void test_httpc_response_parse_binary(void) {
    HttpcResponse* response = httpc_response_from_string((uint8_t*)sample_response_string_bin, sample_response_string_bin_size);
    TEST_ASSERT(response != NULL);

    TEST_CHECK(response->status_code == 200);
    TEST_CHECK(strcmp(response->status_text, "OK") == 0);

    HttpcHeader* h = response->headers;
    TEST_ASSERT(h != NULL);
    TEST_CHECK(strcmp(h->key, "Host") == 0);
    TEST_CHECK(strcmp(h->value, "example.com") == 0);

    h = h->next;
    TEST_ASSERT(h != NULL);
    TEST_CHECK(strcmp(h->key, "Content-Type") == 0);
    TEST_CHECK(strcmp(h->value, "application/octet-stream") == 0);

    h = h->next;
    TEST_ASSERT(h != NULL);
    TEST_CHECK(strcmp(h->key, "Content-Length") == 0);
    TEST_CHECK(strcmp(h->value, "8") == 0);

    TEST_CHECK(response->body_size == 8);
    TEST_MSG("Expected: 8 bytes of binary data");
    TEST_MSG("Actual: %ld bytes of binary data", response->body_size);
    TEST_CHECK(memcmp(response->body, "\0\1\2\3\4\5\6\7", 8) == 0);
    TEST_DUMP("Actual:", response->body, response->body_size);
    TEST_DUMP("Expected:", "\0\1\2\3\4\5\6\7", 8);

    httpc_response_free(response);
}

TEST_LIST = {
    { "httpc_response_headers", test_httpc_response_headers },
    { "httpc_response_parsing", test_httpc_response_parsing },
    { "httpc_response_serialization", test_httpc_response_serialization },
    { "httpc_response_invalid_input", test_httpc_response_invalid_input },
    { "httpc_response_parse_binary", test_httpc_response_parse_binary },
    { NULL, NULL }
};
