#include <httpc.h>
#include <ba.h>

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

void httpc_response_set_body(httpc_response_t* res, const uint8_t* body, size_t body_size) {
    if (res->body != NULL) {
        free(res->body);
    }

    res->body = malloc(body_size);
    memcpy(res->body, body, body_size);
    res->body_size = body_size;
}

httpc_string_t httpc_response_to_string(httpc_response_t* res, size_t* out_size) {
    byte_array_t* ba = byte_array_new(1024);

    byte_array_append_str(ba, "HTTP/1.1 ");
    char status_code_str[4];
    sprintf(status_code_str, "%d", res->status_code);
    byte_array_append_str(ba, status_code_str);

    byte_array_append_str(ba, " ");
    byte_array_append_str(ba, res->status_text);

    byte_array_append_str(ba, "\r\n");

    httpc_header_t* h = res->headers;
    while (h != NULL) {
        char* header_string = httpc_header_to_string(h);
        byte_array_append_str(ba, header_string);
        free(header_string);
        byte_array_append_str(ba, "\r\n");
        h = h->next;
    }

    // add temporary headers: Content-Length
    // What even is this piece of shit
    char content_length_str[16];
    sprintf(content_length_str, "%ld", res->body_size);
    httpc_header_t* temp = httpc_header_new("Content-Length", content_length_str);
    char* header_string = httpc_header_to_string(temp);
    byte_array_append_str(ba, header_string);
    byte_array_append_str(ba, "\r\n");
    free(header_string);

    // cleanup temp
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
