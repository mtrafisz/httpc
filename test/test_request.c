#include "acutest.h"
#include "httpc.h"
#include <stdbool.h>

const char* sample_request_string = "POST /api/example HTTP/1.1\r\n"
    "Host: example.com\r\n"
    "Content-Type: application/json\r\n"
    "Content-Length: 22\r\n"
    "\r\n"
    "{\r\n\t\"key\": \"value\"\n}\r\n";

void test_httpc_request_parsing(void) {
    httpc_request_t* req = httpc_request_from_string(sample_request_string);
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

    TEST_CHECK(req->body_size == 23);
    TEST_MSG("Body size: %zu", req->body_size);

    httpc_request_free(req);
}

void test_httpc_request_serialization(void) {
    // TODO
    TEST_CHECK(false);
    TEST_MSG("Not implemented");
}

TEST_LIST = {
    { "httpc_request_parsing", test_httpc_request_parsing },
    { "httpc_request_serialization", test_httpc_request_serialization },
    { NULL, NULL }
};
