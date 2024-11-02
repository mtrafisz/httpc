#include <assert.h>
#include <httpc.h>
#include <string.h>

const static char* httpc_method_strings[] = {
    "OPTIONS", "GET", "HEAD", "POST", "PUT", "DELETE", "TRACE", "CONNECT",
};

httpc_method_t httpc_method_from_string(const char* method) {
    for (int i = 0; i < 8; i++) {
        if (strcmp(method, httpc_method_strings[i]) == 0) {
            return (httpc_method_t)i;
        }
    }
    return -1;
}

const char const* httpc_method_to_string(httpc_method_t method) {
    assert(method >= 0 && method < 8);
    return httpc_method_strings[method];
}
