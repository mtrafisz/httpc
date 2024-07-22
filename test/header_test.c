#include "httpc.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    httpc_header_t* header = httpc_header_new("Content-Type", "application/json");
    assert(strcmp(header->key, "Content-Type") == 0);
    assert(strcmp(header->value, "application/json") == 0);
    httpc_string_t header_str = httpc_header_to_string(header);
    assert(strcmp(header_str, "Content-Type: application/json") == 0);

    httpc_header_free(header);

    httpc_header_t* header2 = httpc_header_from_string("Content-Type: application/json");

    assert(strcmp(header2->key, "Content-Type") == 0);
    assert(strcmp(header2->value, "application/json") == 0);

    httpc_header_free(header2);

    free(header_str);

    puts("All tests passed!");
    
    return 0;
}
