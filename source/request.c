#include <httpc.h>

// i'm loosing my mind
char* strsep_x(char** stringp, const char* delim) {
    char* start = *stringp;
    if (start == NULL) {
        return NULL;
    }

    char* end = strstr(start, delim);
    if (end == NULL) {
        *stringp = NULL;
        return start;
    }

    *end = '\0';
    *stringp = end + strlen(delim);
    return start;
}

char* concat_all(char** lines, size_t lines_count) {
    size_t size = 0;
    for (int i = 0; i < lines_count; i++) {
        size += strlen(lines[i]) + 1;
    }

    char* str = calloc(size + 1, sizeof(char));
    for (int i = 0; i < lines_count; i++) {
        strcat(str, lines[i]);
        if (i != lines_count - 1) {
            strcat(str, "\r\n");
        }
    }

    return str;
}

httpc_request_t* httpc_request_from_string(httpc_static_string_t req_static) {
    httpc_request_t* r = calloc(1, sizeof(httpc_request_t));
    r->url = NULL;
    r->method = -1;
    r->headers = NULL;
    r->body = NULL;
    r->body_size = 0;

    char* req = strdup(req_static);

    char** lines = malloc(1 * sizeof(char*));
    size_t lines_count = 0;
    int body_line_idx = -1;

    char* c = req;
    while (c) {
        char* line = strsep_x(&c, "\r\n");
        if (line == NULL) {
            break;
        }

        lines[lines_count] = strdup(line);
        lines_count++;

        lines = realloc(lines, (lines_count + 1) * sizeof(char*));

        if (strlen(line) == 0 && body_line_idx == -1) {
            body_line_idx = lines_count;
        }
    }

    char* method = strtok(lines[0], " ");
    r->method = httpc_method_from_string(method);
    r->url = strdup(strtok(NULL, " "));

    for (int i = 1; i < body_line_idx; i++) {
        httpc_header_t* h = httpc_header_from_string(lines[i]);
        if (r->headers == NULL) {
            r->headers = h;
        } else {
            httpc_add_header_h(r->headers, h);
        }
    }

    if (body_line_idx != 0) {
        if (body_line_idx == lines_count - 1) {
            r->body = strdup(lines[body_line_idx]);
            r->body_size = strlen(r->body) + 1;
        } else {
            r->body = concat_all(lines + body_line_idx, lines_count - body_line_idx);
            r->body_size = strlen(r->body) + 1;
        }
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
