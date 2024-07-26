#include "acutest.h"
#include "httpc.h"
#include <stdbool.h>

void test_httpc_header_new(void) {
    httpc_header_t *header = httpc_header_new("Content-Type", "application/json");
    TEST_ASSERT(header != NULL);

    bool key_eq = strcmp(header->key, "Content-Type") == 0;
    bool value_eq = strcmp(header->value, "application/json") == 0;

    if (!key_eq) {
        TEST_DUMP("Expected:", "Content-Type", 13);
        TEST_DUMP("Actual:", header->key, strlen(header->key));
    }
    TEST_CHECK(key_eq);

    if (!value_eq) {
        TEST_DUMP("Expected:", "application/json", 17);
        TEST_DUMP("Actual:", header->value, strlen(header->value));
    }
    TEST_CHECK(value_eq);

    TEST_ASSERT(header->next == NULL);

    httpc_header_free(header);
}

void test_httpc_header_from_string(void) {
    httpc_header_t *header = httpc_header_from_string("Content-Type: application/json");
    TEST_ASSERT(header != NULL);

    TEST_CHECK(strcmp(header->key, "Content-Type") == 0);
    TEST_DUMP("Expected:", "Content-Type", 13);
    TEST_DUMP("Actual:", header->key, strlen(header->key));

    TEST_CHECK(strcmp(header->value, "application/json") == 0);
    TEST_DUMP("Expected:", "application/json", 17);
    TEST_DUMP("Actual:", header->value, strlen(header->value));

    TEST_ASSERT(header->next == NULL);

    httpc_header_free(header);
}

void test_httpc_header_set_get(void) {
    httpc_header_t *header = httpc_header_new("Content-Type", "application/json");
    TEST_ASSERT(header != NULL);

    httpc_add_header_v(&header, "Host", "example.com");
    httpc_add_header_h(header, httpc_header_from_string("Accept: application/json"));

    TEST_CHECK(strcmp(httpc_get_header_value(header, "Content-Type"), "application/json") == 0);
    TEST_DUMP("Expected:", "application/json", 17);
    TEST_DUMP("Actual:", httpc_get_header_value(header, "Content-Type"), strlen(httpc_get_header_value(header, "Content-Type")));
    
    TEST_CHECK(strcmp(httpc_get_header_value(header, "Host"), "example.com") == 0);
    TEST_DUMP("Expected:", "example.com", 11);
    TEST_DUMP("Actual:", httpc_get_header_value(header, "Host"), strlen(httpc_get_header_value(header, "Host")));
    
    TEST_CHECK(strcmp(httpc_get_header_value(header, "Accept"), "application/json") == 0);
    TEST_DUMP("Expected:", "application/json", 17);
    TEST_DUMP("Actual:", httpc_get_header_value(header, "Accept"), strlen(httpc_get_header_value(header, "Accept")));

    httpc_header_free(header);
}

void test_httpc_method(void) {
    TEST_ASSERT(httpc_method_from_string("GET") == HTTPC_GET);
    TEST_ASSERT(httpc_method_from_string("HEAD") == HTTPC_HEAD);
    TEST_ASSERT(httpc_method_from_string("POST") == HTTPC_POST);
    TEST_ASSERT(httpc_method_from_string("PUT") == HTTPC_PUT);
    TEST_ASSERT(httpc_method_from_string("DELETE") == HTTPC_DELETE);
    TEST_ASSERT(httpc_method_from_string("OPTIONS") == HTTPC_OPTIONS);
    TEST_ASSERT(httpc_method_from_string("PATCH") == HTTPC_PATCH);

    TEST_ASSERT(strcmp(httpc_method_to_string(HTTPC_GET), "GET") == 0);
    TEST_ASSERT(strcmp(httpc_method_to_string(HTTPC_HEAD), "HEAD") == 0);
    TEST_ASSERT(strcmp(httpc_method_to_string(HTTPC_POST), "POST") == 0);
    TEST_ASSERT(strcmp(httpc_method_to_string(HTTPC_PUT), "PUT") == 0);
    TEST_ASSERT(strcmp(httpc_method_to_string(HTTPC_DELETE), "DELETE") == 0);
    TEST_ASSERT(strcmp(httpc_method_to_string(HTTPC_OPTIONS), "OPTIONS") == 0);
    TEST_ASSERT(strcmp(httpc_method_to_string(HTTPC_PATCH), "PATCH") == 0);
}

TEST_LIST = {
    { "httpc_header_new", test_httpc_header_new },
    { "httpc_header_from_string", test_httpc_header_from_string },
    { "httpc_header_set_get", test_httpc_header_set_get },
    { "httpc_method", test_httpc_method },
    { NULL, NULL }
};
