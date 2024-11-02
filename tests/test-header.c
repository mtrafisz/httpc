#include <acutest.h>
#include <httpc.h>
#include <stdlib.h>

const allocator_t httpc_allocator = {
    .alloc = malloc,
    .free = free,
    .realloc = realloc,
};

void test_header() {
    httpc_header_t* header = httpc_header_new("Content-Type", "application/json");
    TEST_CHECK(header != NULL);
    TEST_CHECK(header->key != NULL);
    TEST_CHECK(header->value != NULL);
    TEST_CHECK(strcmp(header->key, "Content-Type") == 0);
    TEST_CHECK(strcmp(header->value, "application/json") == 0);
    httpc_header_free(header);
}

void test_header_list() {
    httpc_header_list_t list = list_new(httpc_header_t);
    TEST_CHECK(list.size == 0);
    TEST_CHECK(list.head == NULL);

    TEST_CHECK(httpc_header_list_add_v(&list, "Content-Type", "application/json"));
    TEST_CHECK(list.size == 1);
    TEST_CHECK(list.head != NULL);

    httpc_header_t* header = list_get(&list, LIST_FRONT);
    TEST_CHECK(header != NULL);
    TEST_CHECK(strcmp(header->key, "Content-Type") == 0);
    TEST_CHECK(strcmp(header->value, "application/json") == 0);

    httpc_header_t* header2 = httpc_header_new("Language", "en");

    TEST_CHECK(httpc_header_list_add_h(&list, header2));
    TEST_CHECK(list.size == 2);

    TEST_CHECK(httpc_header_list_add_f(&list, "Content-Length", "%d", 42));
    TEST_CHECK(list.size == 3);

    TEST_CHECK(strcmp(httpc_header_list_get(&list, "Content-Type"), "application/json") == 0);
    TEST_CHECK(strcmp(httpc_header_list_get(&list, "Content-Length"), "42") == 0);
    TEST_CHECK(strcmp(httpc_header_list_get(&list, "Language"), "en") == 0);

    list_clear(&list, (free_fn)httpc_header_free);
    TEST_CHECK(list.size == 0);
}

TEST_LIST = {
    {"header", test_header},
    {"header_list", test_header_list},
    {NULL, NULL},
};
