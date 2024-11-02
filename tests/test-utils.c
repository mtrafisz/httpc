#include <acutest.h>
#include <httpc.h>
#include <stdlib.h>

const allocator_t std_allocator = {
    .alloc = malloc,
    .free = free,
    .realloc = realloc,
};

void test_array() {
    array_t(int) array = array_new(int);
    TEST_CHECK(array.size == 0);
    TEST_CHECK(array.capacity == 0);
    TEST_CHECK(array.data == NULL);

    int data[] = {1, 2, 3, 4, 5};
    TEST_CHECK(array_append_v(&array, data, 5));
    TEST_CHECK(array.size == 5);
    TEST_CHECK(array.capacity >= 5);
    TEST_CHECK(array.data != NULL);

    int* elem;
    array_foreach(elem, &array) {
        TEST_ASSERT(elem != NULL);
        TEST_CHECK(*elem == data[elem - (int*)array.data]);
        TEST_MSG("*elem: %d, data[idxof(elem)]: %d", *elem, data[elem - (int*)array.data]);
    }

    array_t(int) slice = array_slice(&array, 0, 2);
    TEST_CHECK(slice.size == 3);
    TEST_CHECK(slice.capacity >= 3);
    TEST_CHECK(memcmp(slice.data, data, 3 * sizeof(int)) == 0);
    TEST_DUMP("slice.data", slice.data, 3 * sizeof(int));
    TEST_DUMP("data", data, 5 * sizeof(int));

    array_clear(&slice);
    TEST_CHECK(slice.size == 0);
    TEST_CHECK(slice.capacity == 0);
    TEST_CHECK(slice.data == NULL);

    array_clear(&array);
    TEST_CHECK(array.size == 0);
    TEST_CHECK(array.capacity == 0);
    TEST_CHECK(array.data == NULL);

    array_t byte_array = array_new(byte);

    byte data2[] = "Jebać PiS";
    TEST_CHECK(array_append_v(&byte_array, data2, strlen(data2)));
    TEST_CHECK(byte_array.size == sizeof(data2) - 1);
    TEST_MSG("byte_array.size: %zu, sizeof(data2) - 1: %zu", byte_array.size, sizeof(data2) - 1);
    TEST_DUMP("byte_array.data", byte_array.data, byte_array.size);
    TEST_DUMP("data2", data2, sizeof(data2) - 1);
    TEST_CHECK(byte_array.capacity >= sizeof(data2) - 1);
    TEST_CHECK(byte_array.data != NULL);

    TEST_CHECK(memcmp(byte_array.data, data2, sizeof(data2) - 1) == 0);

    TEST_CHECK(array_append_v(&byte_array, " i Konfederacje", 15));
    TEST_CHECK(byte_array.size == sizeof(data2) - 1 + 15);
    TEST_CHECK(byte_array.capacity >= sizeof(data2) - 1 + 15);

    TEST_CHECK(memcmp(byte_array.data, "Jebać PiS i Konfederacje", sizeof("Jebać PiS i Konfederacje") - 1) == 0);
    TEST_DUMP("byte_array.data", byte_array.data, byte_array.size);

    array_clear(&byte_array);
}

TEST_LIST = {{"test_array", test_array}, {NULL, NULL}};
