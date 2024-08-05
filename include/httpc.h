/**
 * @file httpc.h
 * @brief HTTP parser and builder
 * 
 * This file contains structures and functions for parsing and building HTTP messages.
 */

#ifndef _HTTPC_H_
#define _HTTPC_H_

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/**
 * @brief Enum for HTTP methods
 * 
 * This enum is used to represent the different HTTP methods that can be used in a request.
 */
typedef enum _htppc_method_e {
    HTTPC_GET,
    HTTPC_HEAD,
    HTTPC_POST,
    HTTPC_PUT,
    HTTPC_DELETE,
    HTTPC_OPTIONS,
    HTTPC_PATCH,
} HttpcMethodType;

/**
 * @brief Convert a string to a HTTP method
 * 
 * @param method The string to convert
 * @return The HTTP method
 */
HttpcMethodType httpc_method_from_string(const char* method);

/**
 * @brief Convert a HTTP method to a string
 * 
 * This function converts a HTTP method to a string literal.
 * Returned string must not be freed by the caller.
 * 
 * @param method The HTTP method to convert
 * @return The string representation of the HTTP method
 */
const char* httpc_method_to_string(HttpcMethodType method);

/**
 * @brief Struct for a HTTP header
 * 
 * Singly-linked list that represents headers in HTTP messages.
 */
typedef struct _httpc_header_s {
    char* key;
    char* value;
    struct _httpc_header_s* next;
} HttpcHeader;

/**
 * @brief Create a new HTTP header
 * 
 * This function creates a new HTTP header.
 * Both key and value are assumed to be valid, null-terminated strings.
 * Both key and value are copied to structure using `strdup`.
 * Resulting header must be freed by the caller using `httpc_header_free`.
 * 
 * @param key The key of the header
 * @param value The value of the header
 * @return The new HTTP header
 */
HttpcHeader* httpc_header_new(const char* key, const char* value);

/**
 * @brief Free a HTTP header
 * 
 * This function frees a HTTP header and all its children recursively.
 * 
 * @param header The header to free
 */
void httpc_header_free(HttpcHeader* header);

/**
 * @brief Convert a HTTP header to a string
 * 
 * This function converts a HTTP header to a `key: value` string (without the "\r\n" bytes).
 * Returned string must be freed by the caller.
 * 
 * @param header The header to convert
 * @return The string representation of the header
 */
char* httpc_header_to_string(HttpcHeader* header);

/**
 * @brief Convert a string to a HTTP header
 * 
 * This function tries to convert a string to a HTTP header by
 * simply splitting the string by the first `:` character.
 * 
 * @param header The string to convert
 * @return The HTTP header
 */
HttpcHeader* httpc_header_from_string(const char* header);

/**
 * @brief Add a header to a header list
 * 
 * This function adds a header to a header list.
 * Argument `header` is going to be added to the end of the list, and should no longer
 * be used or freed by the caller. To free a header list, use `httpc_header_free` on its head.
 * 
 * @param header_list The header list to add the header to
 * @param header The header to add
 */
void httpc_add_header_h(HttpcHeader** header_list, HttpcHeader* header);

/**
 * @brief Add a header to a header list
 * 
 * This function adds a header to a header list. Both key and value are copied to the list.
 * 
 * @param headers The header list to add the header to
 * @param key The key of the header
 * @param value The value of the header
 */
void httpc_add_header_v(HttpcHeader** headers, const char* key, const char* value);

/**
 * @brief Add a header to a header list
 * 
 * This function adds a header to a header list. Value is formatted using `vsnprintf`.
 * 
 * @param headers The header list to add the header to
 * @param key The key of the header
 * @param format The format string of the value
 * @param ... The arguments for the format string
 */
void httpc_add_header_f(HttpcHeader** headers, const char* key, const char* format, ...);

/**
 * @brief Get the value of a header
 * 
 * This function gets the value of a header from a header list.
 * Resulting string is pointer to the value in the header list, and must not be freed directly.
 * 
 * @param headers The header list to search in
 * @param key The key of the header to search for
 * @return The value of the header or NULL if not found
 */
char* httpc_get_header_value(HttpcHeader* headers, const char* key);

/**
 * @brief Struct for a HTTP request
 * 
 * This struct represents a HTTP request.
 */
typedef struct _httpc_req_s {
    char* url;
    HttpcMethodType method;
    HttpcHeader* headers;
    char* body;
    size_t body_size;
} HttpcRequest;

/**
 * @brief Convert a HTTP request to a string
 * 
 * This function converts a HTTP request to array of bytes.
 * Returned byte pointer must be freed by the caller.
 * 
 * @param req The request to convert
 * @param out_size The size of the resulting string, can be set to `NULL` to ignore.
 * @return The string representation of the request
 * @warning This function automatically adds the `Content-Length` header to resulting string, but not to the request itself.
 */
char* httpc_request_to_string(HttpcRequest* req, size_t* out_size);

/**
 * @brief Create a new HTTP request
 * 
 * This function creates a new HTTP request with empty body and no headers.
 * Result must be freed by the caller using `httpc_request_free`.
 * 
 * @param url The URL of the request
 * @param method The HTTP method of the request
 * @return The new HTTP request
 */
HttpcRequest* httpc_request_new(const char* url, HttpcMethodType method);

/**
 * @brief Parse a string to a HTTP request
 * 
 * This function tries to parse a string to a HTTP request.
 * If parsing fails, the function returns NULL.
 * 
 * @param req The string to parse
 * @param size The size of the string
 * @return The HTTP request or NULL if parsing failed
 */
HttpcRequest* httpc_request_from_string(const uint8_t* req, size_t size);

/**
 * @brief Free a HTTP request
 * 
 * This function frees memory allocated for a HTTP request and all its members.
 * 
 * @param req The request to free
 */
void httpc_request_free(HttpcRequest* req);

/**
 * @brief Set the body of a HTTP request
 * 
 * This function sets the body of a HTTP request. The body is copied to the request.
 * 
 * @param req The request to set the body of
 * @param body The body to set
 * @param size The size of the body
 * 
 * @warning Old body will be freed if not NULL.
 */
void httpc_request_set_body(HttpcRequest* req, const uint8_t* body, size_t size);

/**
 * @brief Struct for a HTTP response
 * 
 * This struct represents a HTTP response.
 */
typedef struct _httpc_res_s {
    uint16_t status_code;
    char* status_text;
    HttpcHeader* headers;
    char* body;
    size_t body_size;           
} HttpcResponse;

/**
 * @brief Convert a HTTP response to a string
 * 
 * This function converts a HTTP response to array of bytes.
 * Returned string must be freed by the caller.
 * 
 * @param res The response to convert
 * @param out_size The size of the resulting string, can be set to `NULL` to ignore.
 * @return The string representation of the response
 */
char* httpc_response_to_string(HttpcResponse* res, size_t* out_size);

/**
 * @brief Parse a string to a HTTP response
 * 
 * This function tries to parse a string to a HTTP response.
 * If parsing fails, the function returns NULL.
 * 
 * @param res The string to parse
 * @param size The size of the string
 * @return The HTTP response or NULL if parsing failed
 */
HttpcResponse* httpc_response_from_string(const uint8_t* res, size_t size);

/**
 * @brief Create a new HTTP response
 * 
 * This function creates a new HTTP response with empty body and no headers.
 * Result must be freed by the caller using `httpc_response_free`.
 * 
 * @param status_text The status text of the response
 * @param status_code The status code of the response
 * @return The new HTTP response
 */
HttpcResponse* httpc_response_new(const char* status_text, uint16_t status_code);

/**
 * @brief Free a HTTP response
 * 
 * This function frees memory allocated for a HTTP response and all its members.
 * 
 * @param res The response to free
 */
void httpc_response_free(HttpcResponse* res);

/**
 * @brief Set the body of a HTTP response
 * 
 * This function sets the body of a HTTP response. The body is copied to the response.
 * 
 * @param res The response to set the body of
 * @param body The body to set
 * @param size The size of the body
 * 
 * @warning Old body will be freed if not NULL.
 */
void httpc_response_set_body(HttpcResponse* res, const uint8_t* body, size_t size);

#endif
