#include <httpc.h>
#include <ba.h>

httpc_request_t* httpc_request_new(const char* url, httpc_method_t method) {
    httpc_request_t* r = malloc(sizeof(httpc_request_t));
    r->url = strdup(url);
    r->method = method;
    r->headers = NULL;
    r->body = NULL;
    r->body_size = 0;
    return r;
}

void httpc_request_set_body(httpc_request_t* req, const uint8_t* body, size_t size) {
    if (req->body != NULL) {
        free(req->body);
    }

    req->body = malloc(size + 1);
    memcpy(req->body, body, size);
    req->body[size] = '\0';
    req->body_size = size + 1;
}

httpc_request_t* httpc_request_from_string(const uint8_t* req_static, size_t size) {
    httpc_request_t* r = malloc(sizeof(httpc_request_t));
    r->url = NULL;
    r->method = -1;
    r->headers = NULL;
    r->body = NULL;
    r->body_size = 0;

    char* req = strndup(req_static, size);

    char** lines = malloc(1 * sizeof(char*));
    size_t lines_count = 0;
    size_t lines_len = 0;

    char* c = req;
    
    // parse headers
    while (c) {
        char* line = strsep_x(&c, "\r\n");
        if (line == NULL) {
            break;
        }

        lines[lines_count] = strdup(line);
        lines_count++;

        lines = realloc(lines, (lines_count + 1) * sizeof(char*));

        size_t line_strlen = strlen(line);
        lines_len += line_strlen + 2; // +2 for \r\n

        if (line_strlen == 0) {
            break;
        }
    }

    char* method = strtok(lines[0], " ");
    r->method = httpc_method_from_string(method);
    r->url = strdup(strtok(NULL, " "));
    for (int i = 1; i < lines_count; i++) {
        httpc_header_t* h = httpc_header_from_string(lines[i]);
        if (r->headers == NULL) {
            r->headers = h;
        } else {
            httpc_add_header_h(r->headers, h);
        }
    }

    for (int i = 0; i < lines_count; i++) {
        free(lines[i]);
    }
    free(lines);

    // c should now point to the body
    size_t body_size = size - lines_len;
    if (httpc_get_header_value(r->headers, "Content-Length") != NULL) { // is it even possible?
        // todo: use something safer than atoi
        size_t body_size_h = atoi(httpc_get_header_value(r->headers, "Content-Length"));
        if (body_size > body_size_h) {
            body_size = body_size_h;
        }
    }

    r->body = malloc(body_size + 1);
    memcpy(r->body, c, body_size);
    r->body[body_size] = '\0';
    r->body_size = body_size + 1;

    free(req);

    return r;
}

char* httpc_request_to_string(httpc_request_t* req, size_t* out_size) {
    byte_array_t* ba = byte_array_new(1024);

    byte_array_append_str(ba, httpc_method_to_string(req->method));
    byte_array_append_byte(ba, ' ');
    byte_array_append_str(ba, req->url);
    byte_array_append_str(ba, " HTTP/1.1\r\n");

    httpc_header_t* h = req->headers;
    while (h != NULL) {
        char* header_string = httpc_header_to_string(h);
        byte_array_append_str(ba, header_string);
        free(header_string);
        byte_array_append_str(ba, "\r\n");
        h = h->next;
    }

    if (req->body != NULL) {
        char content_length_str[16];
        sprintf(content_length_str, "%ld", req->body_size - 1);
        httpc_header_t* temp = httpc_header_new("Content-Length", content_length_str);
        char* header_string = httpc_header_to_string(temp);
        httpc_header_free(temp);
        byte_array_append_str(ba, header_string);
        free(header_string);
        byte_array_append_str(ba, "\r\n");
    }

    byte_array_append_str(ba, "\r\n");

    if (req->body != NULL) {
        byte_array_append_str(ba, req->body);
    }

    char* out = byte_array_collect(ba);
    if (out_size != NULL) {
        *out_size = ba->size;
    }

    byte_array_free(ba);

    return out;
}

void httpc_request_free(httpc_request_t* req) {
    if (req->headers != NULL) {
        httpc_header_free(req->headers);
    }
    req->headers = NULL;

    free(req->url);
    req->url = NULL;
    
    if (req->body != NULL) {
        free(req->body);
    }
    req->body = NULL;

    free(req);
    req = NULL;
}
