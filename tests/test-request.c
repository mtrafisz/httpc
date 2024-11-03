#include <acutest.h>
#include <httpc.h>
#include <stdlib.h>

const allocator_t httpc_allocator = {
    .alloc = malloc,
    .free = free,
    .realloc = realloc,
};

void test_request_new_free() {
    httpc_request_t request = httpc_request_new(HTTP_GET, "http://example.com");
    httpc_header_list_add_v(&request.headers, "User-Agent", "httpc/0.1");

    TEST_CHECK(request.method == HTTP_GET);
    TEST_MSG("request.method: %d (%s)", request.method, httpc_method_to_string(request.method));
    TEST_CHECK(strcmp(request.url, "http://example.com") == 0);
    TEST_MSG("request.url: %s", request.url);
    TEST_CHECK(request.headers.size == 1);
    TEST_MSG("request.headers.size: %lu", request.headers.size);

    TEST_ASSERT(httpc_header_list_get(&request.headers, "User-Agent") != NULL);
    TEST_CHECK(strcmp(httpc_header_list_get(&request.headers, "User-Agent"), "httpc/0.1") == 0);
    TEST_DUMP("User-Agent", httpc_header_list_get(&request.headers, "User-Agent"),
              strlen(httpc_header_list_get(&request.headers, "User-Agent")));

    httpc_request_free(&request);
}

const char* sample_request =
    "GET http://example.com HTTP/1.1\r\n"
    "User-Agent: httpc/0.1\r\n"
    "Content-Length: 0\r\n"
    "\r\n";

void test_serialization() {
    httpc_request_t request = httpc_request_new(HTTP_GET, "http://example.com");
    httpc_header_list_add_v(&request.headers, "User-Agent", "httpc/0.1");

    array_t(byte) serialized = httpc_request_serialize(&request);

    TEST_CHECK(serialized.size == strlen(sample_request));
    TEST_MSG("serialized.size: %lu", serialized.size);
    TEST_CHECK(memcmp(serialized.data, sample_request, serialized.size) == 0);
    TEST_DUMP("serialized", serialized.data, serialized.size);

    httpc_request_free(&request);
    array_clear(&serialized);
}

void test_deserialization() {
    httpc_request_t request;

    array_t(byte) serialized = {
        .data = (byte*)sample_request,
        .size = strlen(sample_request),
    };

    httpc_error_t error = httpc_request_deserialize(&serialized, &request);

    TEST_ASSERT(error == HTTPC_NO_ERROR);
    TEST_MSG("error: %d", error);

    TEST_CHECK(request.method == HTTP_GET);
    TEST_MSG("request.method: %d (%s)", request.method, httpc_method_to_string(request.method));
    TEST_CHECK(strcmp(request.url, "http://example.com") == 0);
    TEST_MSG("request.url: %s", request.url);
    TEST_CHECK(request.headers.size == 1);
    TEST_MSG("request.headers.size: %ld", request.headers.size);

    TEST_ASSERT(httpc_header_list_get(&request.headers, "User-Agent") != NULL);
    TEST_CHECK(strcmp(httpc_header_list_get(&request.headers, "User-Agent"), "httpc/0.1") == 0);
    TEST_DUMP("User-Agent", httpc_header_list_get(&request.headers, "User-Agent"),
              strlen(httpc_header_list_get(&request.headers, "User-Agent")));

    httpc_request_free(&request);
}

TEST_LIST = {
    {"new_free", test_request_new_free},
    {"serialization", test_serialization},
    {"deserialization", test_deserialization},
    {NULL, NULL},
};
