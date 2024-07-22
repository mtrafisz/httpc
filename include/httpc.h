#ifndef _HTTPC_H_
#define _HTTPC_H_

#include <stdint.h>
#include <stdlib.h>

typedef const char* httpc_static_string_t;
typedef char* httpc_string_t;

typedef enum _htppc_method_e {
    HTTPC_GET,
    HTTPC_POST,
    HTTPC_PUT,
    HTTPC_DELETE,
    HTTPC_OPTIONS,
} httpc_method_t;

httpc_method_t httpc_method_from_string(httpc_static_string_t method);
httpc_static_string_t httpc_method_to_string(httpc_method_t method);

typedef struct _httpc_header_s {
    httpc_string_t key;
    httpc_string_t value;
    struct _httpc_header_s* next;
} httpc_header_t;

httpc_header_t* httpc_header_new(httpc_static_string_t key, httpc_static_string_t value);
void httpc_header_free(httpc_header_t* header);

httpc_string_t httpc_header_to_string(httpc_header_t* header);
httpc_header_t* httpc_header_from_string(httpc_static_string_t header);

typedef struct _httpc_req_s {
    httpc_string_t url;
    httpc_method_t method;
    httpc_header_t* headers;
    httpc_string_t body;
} httpc_request_t;

// char* httpc_req_to_string(httpc_request_t* req, size_t* out_size); // outside of scope for now
httpc_request_t* httpc_request_from_string(httpc_static_string_t req);
void httpc_request_free(httpc_request_t* req);

typedef struct _httpc_res_s {
    uint16_t status_code;
    httpc_static_string_t status_text;
    httpc_header_t* headers;
    httpc_string_t body;
    size_t body_size;
} httpc_response_t;

httpc_string_t httpc_response_to_string(httpc_response_t* res, size_t* out_size);
// httpc_response_t* httpc_response_from_string(const char* res); // outside of scope for now

httpc_response_t* httpc_response_new(httpc_static_string_t status_text, uint16_t status_code);
void httpc_response_free(httpc_response_t* res);

void httpc_response_add_header(httpc_response_t* res, httpc_static_string_t key, httpc_static_string_t value);
char* httpc_get_header_value(httpc_header_t* headers, httpc_static_string_t key);
void httpc_response_set_body(httpc_response_t* res, uint8_t* body, size_t body_size);

#endif