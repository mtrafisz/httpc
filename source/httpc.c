#include "httpc.h"

#include <stdio.h>
#include <string.h>

httpc_method_t httpc_method_from_string(httpc_static_string_t method) {
    if (strcmp(method, "GET") == 0) {
        return HTTPC_GET;
    } else if (strcmp(method, "POST") == 0) {
        return HTTPC_POST;
    } else if (strcmp(method, "PUT") == 0) {
        return HTTPC_PUT;
    } else if (strcmp(method, "DELETE") == 0) {
        return HTTPC_DELETE;
    } else if (strcmp(method, "OPTIONS") == 0) {
        return HTTPC_OPTIONS;
    } else {
        return -1;
    }
}

httpc_static_string_t httpc_method_to_string(httpc_method_t method) {
    const char* methods[] = {
        "GET",
        "POST",
        "PUT",
        "DELETE",
        "OPTIONS",
    };

    return methods[method];
}

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

void _httpc_add_header(httpc_header_t* header_list, httpc_header_t* header) {
    if (header_list->next == NULL) {
        header_list->next = header;
    } else {
        _httpc_add_header(header_list->next, header);
    }
}

httpc_request_t* httpc_request_from_string(httpc_static_string_t req_static) {
    httpc_request_t* r = calloc(1, sizeof(httpc_request_t));
    r->url = NULL;
    r->method = -1;
    r->headers = NULL;
    r->body = NULL;

    char* req = strdup(req_static);

    char** lines = malloc(1 * sizeof(char*));
    size_t lines_count = 0;
    size_t body_line_idx = 0;

    char* line = strtok(req, "\r\n");
    while (line != NULL) {
        lines[lines_count] = calloc(strlen(line) + 1, sizeof(char));
        strcpy(lines[lines_count], line);
        lines_count++;
        lines = realloc(lines, (lines_count + 1) * sizeof(char*));
        
        char* next_line = strtok(NULL, "\r\n");
        if (next_line == NULL || strlen(next_line) == 0) {
            body_line_idx = lines_count - 1;
            break; 
        }

        line = next_line;
    }
    char* method = strtok(lines[0], " ");
    r->method = httpc_method_from_string(method);
    r->url = strdup(strtok(NULL, " "));

    for (int i = 1; i < lines_count - 1; i++) {
        httpc_header_t* h = httpc_header_from_string(lines[i]);
        if (r->headers == NULL) {
            r->headers = h;
        } else {
            _httpc_add_header(r->headers, h);
        }
    }

    if (body_line_idx != 0) {
        r->body = calloc(strlen(lines[body_line_idx]) + 1, sizeof(char));
        strcpy(r->body, lines[body_line_idx]);
    }

    free(req);
    for (int i = 0; i < lines_count; i++) {
        free(lines[i]);
    }
    free(lines);

    return r;
}

void httpc_request_free(httpc_request_t* req) {
    if (req->headers != NULL) {
        httpc_header_free(req->headers);
    }

    free(req->url);
    
    if (req->body != NULL) {
        free(req->body);
    }

    free(req);
}

httpc_response_t* httpc_response_new(httpc_static_string_t status_text, uint16_t status_code) {
    httpc_response_t* r = calloc(1, sizeof(httpc_response_t));
    r->status_code = status_code;
    r->status_text = status_text;
    r->headers = NULL;
    r->body = NULL;
    r->body_size = 0;
    return r;
}

void httpc_response_free(httpc_response_t* res) {
    if (res->headers != NULL) {
        httpc_header_free(res->headers);
    }
    if (res->body != NULL) {
        free(res->body);
    }

    free(res);
}

void httpc_response_add_header(httpc_response_t* res, httpc_static_string_t key, httpc_static_string_t value) {
    if (res->headers != NULL && httpc_get_header_value(res->headers, key) != NULL) {
        return;
    }

    httpc_header_t* h = httpc_header_new(key, value);
    if (res->headers == NULL) {
        res->headers = h;
    } else {
        _httpc_add_header(res->headers, h);
    }
}

void httpc_response_set_body(httpc_response_t* res, uint8_t* body, size_t body_size) {
    if (res->body != NULL) {
        free(res->body);
    }

    res->body = malloc(body_size);
    memcpy(res->body, body, body_size);
    res->body_size = body_size;
}

struct growing_array {
    size_t size;
    size_t capacity;
    char* data;
};

struct growing_array* ga_new(size_t capacity) {
    struct growing_array* ga = calloc(1, sizeof(struct growing_array));
    ga->size = 0;
    ga->capacity = capacity;
    ga->data = malloc(capacity);
    return ga;
}

void ga_grow(struct growing_array* ga) {
    ga->capacity *= 2;
    ga->data = realloc(ga->data, ga->capacity);
}

void ga_append_char(struct growing_array* ga, char c) {
    if (ga->size == ga->capacity) {
        ga_grow(ga);
    }

    ga->data[ga->size] = c;
    ga->size++;    
}

void ga_append_str(struct growing_array* ga, httpc_static_string_t str) {
    size_t len = strlen(str);
    if (ga->size + len >= ga->capacity) {
        ga->capacity = ga->size + len + 1;
        ga->data = realloc(ga->data, ga->capacity);
    }

    strcpy(ga->data + ga->size, str);
    ga->size += len;
}

void ga_append_bytes(struct growing_array* ga, uint8_t* bytes, size_t size) {
    if (ga->size + size >= ga->capacity) {
        ga->capacity = ga->size + size + 1;
        ga->data = realloc(ga->data, ga->capacity);
    }

    memcpy(ga->data + ga->size, bytes, size);
    ga->size += size;
}

char* ga_collect(struct growing_array* ga) {
    ga_append_char(ga, '\0');
    return strdup(ga->data);
}

void ga_free(struct growing_array* ga) {
    free(ga->data);
    free(ga);
}

httpc_string_t httpc_response_to_string(httpc_response_t* res, size_t* out_size) {
    struct growing_array* ga = ga_new(1024);

    ga_append_str(ga, "HTTP/1.1 ");
    char status_code_str[4];
    sprintf(status_code_str, "%d", res->status_code);
    ga_append_str(ga, status_code_str);

    ga_append_str(ga, " ");
    ga_append_str(ga, res->status_text);

    ga_append_str(ga, "\r\n");

    httpc_header_t* h = res->headers;
    while (h != NULL) {
        char* header_string = httpc_header_to_string(h);
        ga_append_str(ga, header_string);
        free(header_string);
        ga_append_str(ga, "\r\n");
        h = h->next;
    }

    // add temporary headers: Content-Length
    char content_length_str[16];
    sprintf(content_length_str, "%ld", res->body_size);
    httpc_header_t* temp = httpc_header_new("Content-Length", content_length_str);
    char* header_string = httpc_header_to_string(temp);
    ga_append_str(ga, header_string);
    ga_append_str(ga, "\r\n");
    free(header_string);

    // cleanup temp
    httpc_header_free(temp);

    ga_append_str(ga, "\r\n");

    if (res->body != NULL) {
        ga_append_bytes(ga, res->body, res->body_size);
    }

    char* str = ga_collect(ga);
    if (out_size != NULL) {
        *out_size = ga->size;
    }

    ga_free(ga);

    return str;
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
