#include <httpc.h>
#include <internal_ba.h>
#include <errno.h>

HttpcResponse* httpc_response_new(const char* status_text, uint16_t status_code) {
    HttpcResponse* r = malloc(sizeof(HttpcResponse));
    r->status_code = status_code;
    r->status_text = strdup(status_text);
    r->headers = NULL;
    r->body = NULL;
    r->body_size = 0;
    return r;
}

void httpc_response_free(HttpcResponse* res) {
    if (res->headers != NULL) {
        httpc_header_free(res->headers);
        res->headers = NULL;
    }
    if (res->body != NULL) {
        free(res->body);
        res->body = NULL;
    }
    if (res->status_text != NULL) {
        free(res->status_text);
        res->status_text = NULL;
    }

    free(res);
    res = NULL;
}

void httpc_response_set_body(HttpcResponse* res, const uint8_t* body, size_t body_size) {
    if (body == NULL || body_size == 0) {
        errno = EINVAL;
        return;
    }
    
    if (res->body != NULL) {
        free(res->body);
    }

    res->body = malloc(body_size);
    memcpy(res->body, body, body_size);
    res->body_size = body_size;
}

char* httpc_response_to_string(HttpcResponse* res, size_t* out_size) {
    byte_array_t* ba = byte_array_new(1024);

    byte_array_append_str(ba, "HTTP/1.1 ");
    char status_code_str[8];
    sprintf(status_code_str, "%d", res->status_code);
    byte_array_append_str(ba, status_code_str);

    byte_array_append_str(ba, " ");
    byte_array_append_str(ba, res->status_text);

    byte_array_append_str(ba, "\r\n");

    HttpcHeader* h = res->headers;
    while (h != NULL) {
        char* header_string = httpc_header_to_string(h);
        byte_array_append_str(ba, header_string);
        free(header_string);
        byte_array_append_str(ba, "\r\n");
        h = h->next;
    }

    char content_length_str[16];
    sprintf(content_length_str, "%ld", res->body_size);

    HttpcHeader* temp = httpc_header_new("Content-Length", content_length_str);
    char* header_string = httpc_header_to_string(temp);

    byte_array_append_str(ba, header_string);
    byte_array_append_str(ba, "\r\n");

    free(header_string);
    httpc_header_free(temp);

    byte_array_append_str(ba, "\r\n");

    if (res->body != NULL) {
        byte_array_append_bytes(ba, res->body, res->body_size);
    }

    char* str = byte_array_collect(ba);
    if (out_size != NULL) {
        *out_size = ba->size;
    }

    byte_array_free(ba);

    return str;
}

HttpcResponse* httpc_response_from_string(const uint8_t* res_static, size_t size) {
    if (res_static == NULL || size == 0) {
        errno = EINVAL;
        return NULL;
    }

    HttpcResponse* r = malloc(sizeof(HttpcResponse));
    r->headers = NULL;
    r->body = NULL;
    r->body_size = 0;

    char* res = malloc(size);
    memcpy(res, res_static, size);

    char** lines = malloc(sizeof(char*));
    size_t lines_count = 0;
    size_t lines_len = 0;

    char* c = res;

    while (c) {
        char* line = strsep_x(&c, "\r\n");
        if (line == NULL || strlen(line) == 0) {
            lines_len += 2;
            break;
        }

        lines[lines_count] = strdup(line);
        lines_count++;

        lines = realloc(lines, sizeof(char*) * (lines_count + 1));

        size_t line_len = strlen(line);
        lines_len += line_len + 2;
    }

    if (lines_count == 0) {
        errno = EINVAL;
        free(res);
        free(lines);
        return NULL;
    }

    strtok(lines[0], " ");  // HTTP version

    char* status_code_str = strtok(NULL, " ");
    if (status_code_str == NULL) {
        errno = EINVAL;
        goto return_err;
    }
    r->status_code = (uint16_t)strtol(status_code_str, NULL, 10);

    char* status_text = strtok(NULL, "");
    if (status_text == NULL) {
        errno = EINVAL;
        goto return_err;
    }
    r->status_text = strdup(status_text);

    for (size_t i = 1; i < lines_count; i++) {
        HttpcHeader* h = httpc_header_from_string(lines[i]);
        if (r->headers == NULL) {
            r->headers = h;
        } else {
            httpc_add_header_h(r->headers, h);
        }
    }

    for (size_t i = 0; i < lines_count; i++) {
        free(lines[i]);
    }
    free(lines);

    size_t body_size = size - lines_len;
    if (r->headers != NULL && httpc_get_header_value(r->headers, "Content-Length") != NULL) {
        size_t body_size_h = atoi(httpc_get_header_value(r->headers, "Content-Length"));
        if (body_size > body_size_h) {
            body_size = body_size_h;
        }
    }

    if (body_size > 0) {
        r->body = malloc(body_size);
        memcpy(r->body, c, body_size);
        r->body_size = body_size;
    }

    free(res);

    return r;

return_err:
    free(r);
    for (size_t i = 0; i < lines_count; i++) {
        free(lines[i]);
    }
    free(lines);
    free(res);
    return NULL;
}
