#include "httpc.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    httpc_method_t method = httpc_method_from_string("GET");
    assert(method == HTTPC_GET);

    httpc_static_string_t method_str = httpc_method_to_string(method);
    assert(strcmp(method_str, "GET") == 0);

    puts("All tests passed!");
    
    return 0;
}
