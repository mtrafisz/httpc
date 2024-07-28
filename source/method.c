#include <httpc.h>

httpc_method_t httpc_method_from_string(const char* method) {
    if (strcmp(method, "GET") == 0) {
        return HTTPC_GET;
    } else if (strcmp(method, "HEAD") == 0) {
        return HTTPC_HEAD;
    } else if (strcmp(method, "POST") == 0) {
        return HTTPC_POST;
    } else if (strcmp(method, "PUT") == 0) {
        return HTTPC_PUT;
    } else if (strcmp(method, "DELETE") == 0) {
        return HTTPC_DELETE;
    } else if (strcmp(method, "OPTIONS") == 0) {
        return HTTPC_OPTIONS;
    } else if (strcmp(method, "PATCH") == 0) {
        return HTTPC_PATCH;
    } else {
        return -1;
    }
}

const char* httpc_method_to_string(httpc_method_t method) {
    if (method < HTTPC_GET || method > HTTPC_PATCH) {
        return NULL;
    }

    const char* methods[] = {
        "GET",
        "HEAD",
        "POST",
        "PUT",
        "DELETE",
        "OPTIONS",
        "PATCH",
    };

    return methods[method];
}


