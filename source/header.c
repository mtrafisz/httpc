#include <httpc.h>

HttpcHeader* httpc_header_new(const char* key, const char* value) {
    if (key == NULL || value == NULL) {
        return NULL;
    }
    
    HttpcHeader* h = calloc(1, sizeof(HttpcHeader));
    h->key = strdup(key);
    h->value = strdup(value);
    h->next = NULL;
    return h;

}

char* httpc_header_to_string(HttpcHeader* header) {
    char* str = malloc(strlen(header->key) + strlen(header->value) + 4);
    sprintf(str, "%s: %s", header->key, header->value);
    return str;
}

HttpcHeader* httpc_header_from_string(const char* header) {
    HttpcHeader* h = calloc(1, sizeof(HttpcHeader));
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

void httpc_header_free(HttpcHeader* header) {
    if (header->next != NULL) {
        httpc_header_free(header->next);
    }
    free(header->key);
    free(header->value);
    free(header);
}

void httpc_add_header_h(HttpcHeader* header_list, HttpcHeader* header) {
    if (header_list->next == NULL) {
        header_list->next = header;
    } else {
        httpc_add_header_h(header_list->next, header);
    }
}

void httpc_add_header_v(HttpcHeader** headers, const char* key, const char* value) {
    HttpcHeader* h = httpc_header_new(key, value);
    if (*headers == NULL) {
        *headers = h;
    } else {
        if (httpc_get_header_value(*headers, key) != NULL) {
            return;
        }
        httpc_add_header_h(*headers, h);
    }
}

char* httpc_get_header_value(HttpcHeader* headers, const char* key) {
    HttpcHeader* h = headers;
    while (h != NULL) {
        if (strcmp(h->key, key) == 0) {
            return h->value;
        }
        h = h->next;
    }

    return NULL;
}

