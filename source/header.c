#include <httpc.h>

httpc_header_t* httpc_header_new(const char* key, const char* value) {
    if (key == NULL || value == NULL) {
        return NULL;
    }
    
    httpc_header_t* h = calloc(1, sizeof(httpc_header_t));
    h->key = strdup(key);
    h->value = strdup(value);
    h->next = NULL;
    return h;

}

char* httpc_header_to_string(httpc_header_t* header) {
    char* str = malloc(strlen(header->key) + strlen(header->value) + 4);
    sprintf(str, "%s: %s", header->key, header->value);
    return str;
}

httpc_header_t* httpc_header_from_string(const char* header) {
    httpc_header_t* h = calloc(1, sizeof(httpc_header_t));
    h->key = calloc(strlen(header) + 1, sizeof(char));
    h->value = calloc(strlen(header) + 1, sizeof(char));
    h->next = NULL;
    sscanf(header, "%[^:]: %s", h->key, h->value);
    
    if (strlen(h->key) == 0 || strlen(h->value) == 0) {
        free(h->key);
        free(h->value);
        free(h);
        return NULL;
    }

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

void httpc_add_header_v(httpc_header_t** headers, const char* key, const char* value) {
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

char* httpc_get_header_value(httpc_header_t* headers, const char* key) {
    httpc_header_t* h = headers;
    while (h != NULL) {
        if (strcmp(h->key, key) == 0) {
            return h->value;
        }
        h = h->next;
    }

    return NULL;
}

