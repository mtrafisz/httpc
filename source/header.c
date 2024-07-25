#include <httpc.h>

httpc_header_t* httpc_header_new(httpc_static_string_t key, httpc_static_string_t value) {
    httpc_header_t* h = calloc(1, sizeof(httpc_header_t));
    h->key = calloc(strlen(key) + 1, sizeof(char));
    h->value = calloc(strlen(value) + 1, sizeof(char));
    strcpy(h->key, key);
    strcpy(h->value, value);
    h->next = NULL;
    return h;

}

httpc_string_t httpc_header_to_string(httpc_header_t* header) {
    httpc_string_t str = calloc(strlen(header->key) + strlen(header->value) + 4, sizeof(char));
    sprintf(str, "%s: %s", header->key, header->value);
    return str;
}

httpc_header_t* httpc_header_from_string(httpc_static_string_t header) {
    httpc_header_t* h = calloc(1, sizeof(httpc_header_t));
    h->key = calloc(strlen(header) + 1, sizeof(char));
    h->value = calloc(strlen(header) + 1, sizeof(char));
    h->next = NULL;
    sscanf(header, "%[^:]: %s", h->key, h->value);
    return h;
}

void httpc_header_free(httpc_header_t* header) {
    if (header->next != NULL) {
        httpc_header_free(header->next);
    }
    free(header->key);
    free(header->value);
    free(header);
}

void httpc_add_header_h(httpc_header_t* header_list, httpc_header_t* header) {
    if (header_list->next == NULL) {
        header_list->next = header;
    } else {
        httpc_add_header_h(header_list->next, header);
    }
}

void httpc_add_header_v(httpc_header_t** headers, httpc_static_string_t key, httpc_static_string_t value) {
    httpc_header_t* h = httpc_header_new(key, value);
    if (*headers == NULL) {
        *headers = h;
    } else {
        if (httpc_get_header_value(*headers, key) != NULL) {
            return;
        }
        httpc_add_header_h(*headers, h);
    }
}

char* httpc_get_header_value(httpc_header_t* headers, httpc_static_string_t key) {
    httpc_header_t* h = headers;
    while (h != NULL) {
        if (strcmp(h->key, key) == 0) {
            return h->value;
        }
        h = h->next;
    }

    return NULL;
}

