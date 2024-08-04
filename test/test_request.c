#include "acutest.h"
#include "httpc.h"
#include <stdbool.h>

const char* sample_request_string = "POST /api/example HTTP/1.1\r\n"
    "Host: example.com\r\n"
    "Content-Type: application/json\r\n"
    "Content-Length: 22\r\n"
    "\r\n"
    "{\r\n\t\"key\": \"value\"\n}\r\n";

const char* binary_request_string = "POST /api/example HTTP/1.1\r\n"
    "Host: example.com\r\n"
    "Content-Type: application/octet-stream\r\n"
    "Content-Length: 15\r\n"
    "\r\n"
    "\x00\x69\x00\x6d\x00\x20\x00\x6c\x00\x6f\x00\x73\x00\x69\x00";
#define binary_request_string_size 126

void test_httpc_request_parsing(void) {
    HttpcRequest* req = httpc_request_from_string((uint8_t*)sample_request_string, strlen(sample_request_string));
    TEST_ASSERT(req != NULL);

    TEST_CHECK(req->method == HTTPC_POST);
    TEST_CHECK(strcmp(req->url, "/api/example") == 0);
    TEST_DUMP("Expected:", "/api/example", 12);
    TEST_DUMP("Actual:", req->url, strlen(req->url));

    TEST_ASSERT(req->headers != NULL);

    TEST_CHECK(strcmp(httpc_get_header_value(req->headers, "Host"), "example.com") == 0);
    TEST_DUMP("Expected:", "example.com", 11);
    TEST_DUMP("Actual:", httpc_get_header_value(req->headers, "Host"), strlen(httpc_get_header_value(req->headers, "Host")));

    TEST_CHECK(strcmp(httpc_get_header_value(req->headers, "Content-Type"), "application/json") == 0);
    TEST_DUMP("Expected:", "application/json", 17);
    TEST_DUMP("Actual:", httpc_get_header_value(req->headers, "Content-Type"), strlen(httpc_get_header_value(req->headers, "Content-Type")));

    TEST_CHECK(strcmp(httpc_get_header_value(req->headers, "Content-Length"), "22") == 0);
    TEST_DUMP("Expected:", "22", 2);
    TEST_DUMP("Actual:", httpc_get_header_value(req->headers, "Content-Length"), strlen(httpc_get_header_value(req->headers, "Content-Length")));

    TEST_CHECK(strcmp(req->body, "{\r\n\t\"key\": \"value\"\n}\r\n") == 0);
    TEST_DUMP("Expected:", "{\r\n\t\"key\": \"value\"\n}\r\n", 22);
    TEST_DUMP("Actual:", req->body, req->body_size);

    TEST_CHECK(req->body_size == 23);
    TEST_MSG("Body size: %zu", req->body_size);

    httpc_request_free(req);
}

void test_httpc_request_parsing_binary(void) {
    HttpcRequest* req = httpc_request_from_string((uint8_t*)binary_request_string, binary_request_string_size);
    TEST_ASSERT(req != NULL);

    TEST_CHECK(req->method == HTTPC_POST);
    TEST_CHECK(strcmp(req->url, "/api/example") == 0);
    TEST_ASSERT(req->headers != NULL);

    TEST_CHECK(strcmp(httpc_get_header_value(req->headers, "Host"), "example.com") == 0);
    TEST_CHECK(strcmp(httpc_get_header_value(req->headers, "Content-Type"), "application/octet-stream") == 0);
    TEST_CHECK(strcmp(httpc_get_header_value(req->headers, "Content-Length"), "15") == 0);

    TEST_CHECK(strcmp(req->body, "\x00\x69\x00\x6d\x00\x20\x00\x6c\x00\x6f\x00\x73\x00\x69\x00") == 0);
    TEST_DUMP("Expected:", "\x00\x69\x00\x6d\x00\x20\x00\x6c\x00\x6f\x00\x73\x00\x69\x00", 15);
    TEST_DUMP("Actual:", req->body, req->body_size);
    TEST_CHECK(req->body_size == 16);
    TEST_MSG("Body size: %zu", req->body_size);

    httpc_request_free(req);
}

void test_httpc_request_serialization(void) {
    HttpcRequest* req = httpc_request_new("/api/example", HTTPC_POST);
    httpc_add_header_v(&req->headers, "Host", "example.com");
    httpc_add_header_v(&req->headers, "Content-Type", "application/json");
    httpc_request_set_body(req, (uint8_t*)"{\r\n\t\"key\": \"value\"\n}\r\n", 22);

    size_t size;
    char* serialized = httpc_request_to_string(req, &size);
    TEST_ASSERT(serialized != NULL);

    TEST_CHECK(strcmp(serialized, sample_request_string) == 0);
    TEST_DUMP("Expected:", sample_request_string, strlen(sample_request_string));
    TEST_DUMP("Actual:", serialized, size);
    TEST_CHECK(size == strlen(sample_request_string));
    TEST_MSG("Size: %zu", size);
    TEST_MSG("Expected size: %zu", strlen(sample_request_string));

    free(serialized);
    httpc_request_free(req);
}

TEST_LIST = {
    { "httpc_request_parsing", test_httpc_request_parsing },
    { "httpc_request_parsing_binary", test_httpc_request_parsing_binary },
    { "httpc_request_serialization", test_httpc_request_serialization },
    { NULL, NULL }
};
