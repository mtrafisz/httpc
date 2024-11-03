#include <acutest.h>
#include <httpc.h>
#include <stdlib.h>

const allocator_t httpc_allocator = {
    .alloc = malloc,
    .free = free,
    .realloc = realloc,
};

void test_response_new_free() {
    httpc_response_t response = httpc_response_new("OK", 200);
    httpc_header_list_add_v(&response.headers, "Server", "httpd/2.4.41");

    TEST_CHECK(response.status_code == 200);
    TEST_MSG("status_code: %d", response.status_code);
    TEST_CHECK(strcmp(response.status_message, "OK") == 0);
    TEST_MSG("status_message: %s", response.status_message);
    TEST_CHECK(response.headers.size == 1);
    TEST_MSG("headers.size: %lu", response.headers.size);

    TEST_ASSERT(httpc_header_list_get(&response.headers, "Server") != NULL);
    TEST_CHECK(strcmp(httpc_header_list_get(&response.headers, "Server"), "httpd/2.4.41") == 0);
    TEST_DUMP("Server", httpc_header_list_get(&response.headers, "Server"),
              strlen(httpc_header_list_get(&response.headers, "Server")));

    httpc_response_free(&response);
}

const char* sample_response =
    "HTTP/1.1 200 OK\r\n"
    "Server: httpd/2.4.41\r\n"
    "\r\n";

void test_serialization() {
    httpc_response_t response = httpc_response_new("OK", 200);
    httpc_header_list_add_v(&response.headers, "Server", "httpd/2.4.41");

    array_t(byte) serialized = httpc_response_serialize(&response);

    TEST_CHECK(serialized.size == strlen(sample_response));
    TEST_MSG("serialized.size: %lu, expected: %lu", serialized.size, strlen(sample_response));
    TEST_CHECK(memcmp(serialized.data, sample_response, serialized.size) == 0);
    TEST_DUMP("serialized", serialized.data, serialized.size);
    TEST_DUMP("expected", sample_response, strlen(sample_response));

    httpc_response_free(&response);
    array_clear(&serialized);
}

void test_deserialization() {
    httpc_response_t response;

    array_t(byte) serialized = array_new(byte, httpc_allocator);
    array_append_v(&serialized, (byte*)sample_response, strlen(sample_response));

    httpc_error_t error = httpc_response_deserialize(&serialized, &response);

    TEST_ASSERT(error == HTTPC_NO_ERROR);
    TEST_MSG("error: %d", error);

    TEST_CHECK(response.status_code == 200);
    TEST_MSG("status_code: %d", response.status_code);
    TEST_CHECK(strcmp(response.status_message, "OK") == 0);
    TEST_MSG("status_message: %s", response.status_message);
    TEST_CHECK(response.headers.size == 1);
    TEST_MSG("headers.size: %lu", response.headers.size);

    TEST_ASSERT(httpc_header_list_get(&response.headers, "Server") != NULL);
    TEST_CHECK(strcmp(httpc_header_list_get(&response.headers, "Server"), "httpd/2.4.41") == 0);
    TEST_DUMP("Server", httpc_header_list_get(&response.headers, "Server"),
              strlen(httpc_header_list_get(&response.headers, "Server")));
}

TEST_LIST = {
    {"test_response_new_free", test_response_new_free},
    {"test_serialization", test_serialization},
    {"test_deserialization", test_deserialization},
    {NULL, NULL},
};
