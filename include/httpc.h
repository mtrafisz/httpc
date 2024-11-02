#ifndef _HTTPC_H_
#define _HTTPC_H_

#include <allocator.h>
#include <array.h>
#include <list.h>
#include <macros.h>

extern const allocator_t httpc_allocator;

typedef enum _httpc_method_e {
    HTTP_OPTIONS = 0,
    HTTP_GET,
    HTTP_HEAD,
    HTTP_POST,
    HTTP_PUT,
    HTTP_DELETE,
    HTTP_TRACE,
    HTTP_CONNECT,
} httpc_method_t;

httpc_method_t httpc_method_from_string(const char* method);
const char const* httpc_method_to_string(httpc_method_t method);

typedef struct _httpc_header_s {
    char* key;
    char* value;
} httpc_header_t;

httpc_header_t* httpc_header_new(const char* key, const char* value);
void httpc_header_free(httpc_header_t* header);

// Polymorphism!!?? IN MY C??

typedef list_t(httpc_header_t) httpc_header_list_t;  // TODO: replace with hash table?

bool httpc_header_list_add_v(httpc_header_list_t* list, const char* key, const char* value);
bool httpc_header_list_add_h(httpc_header_list_t* list, httpc_header_t* header);
bool httpc_header_list_add_f(httpc_header_list_t* list, const char* key, const char* fmt, ...);
char* httpc_header_list_get(httpc_header_list_t* list, const char* key);

typedef struct _httpc_request_s {
    httpc_method_t method;
    char* url;
    httpc_header_list_t headers;
    array_t(byte) body;
} httpc_request_t;

httpc_request_t httpc_request_new(httpc_method_t method, const char* url);
void httpc_request_free(httpc_request_t* request);
array_t(byte) httpc_request_serialize(httpc_request_t* request);
httpc_request_t httpc_request_deserialize(const array_t(byte) * data);

typedef struct _httpc_response_s {
    int status_code;
    char* status_message;
    httpc_header_list_t headers;
    array_t(byte) body;
} httpc_response_t;

httpc_response_t httpc_response_new(const char* status_message, int status_code);
void httpc_response_free(httpc_response_t* response);
array_t(byte) httpc_response_serialize(httpc_response_t* response);
httpc_response_t httpc_response_deserialize(const array_t(byte) * data);

// INFO: request and response bodies are to be interacted with directly by the user, using array_* functions.

#endif
