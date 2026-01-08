#define _GNU_SOURCE
#include <string.h>
#include "request.h"
#include "logger.h"

#include <stdint.h>


typedef struct {
  const char *name;
    enum http_methods method;
} http_method_map;

static const http_method_map method_table[] = {
    {"OPTIONS", OPTIONS},
    {"GET",     GET},
    {"HEAD",    HEAD},
    {"POST",    POST},
    {"PUT",     PUT},
    {"DELETE",  DELETE},
    {"TRACE",   TRACE},
    {"CONNECT", CONNECT}
};

static http_methods string_to_enum_n(const char *data, size_t len) {
    if (len < 3) return UNRECOGNIZED;

    // We can interpret the first 4 bytes as a 32-bit integer.
    // Note: This works best on Little Endian (x86/ARM).
    // Using a simple switch on length + first characters:
    switch (len) {
        case 3:
            if (data[0] == 'G' && data[1] == 'E' && data[2] == 'T') 
                return GET;
            break;
        case 4:
            if (*(uint32_t*)data == *(uint32_t*)"POST") return POST;
            break;
        case 6:
            if (memcmp(data, "DELETE", 6) == 0) return DELETE;
            break;
        // Add other methods as needed
    }

    return UNRECOGNIZED;
}


http_request *init_http_request(http_request *req) {
  if (req == NULL)
    return NULL;

  req->headers_amt = 0;
  return req;
}

void deinit_http_request(http_request *req) {
  if (req != NULL) {
    deinit_str_view(&req->body);
    deinit_str_view(&req->status_line);
    deinit_str_view(&req->uri);
    deinit_str_view(&req->body);
  }
}

int http_extract_req_line(const char *data, size_t len, http_request *req) {
    if (!data || !req) return -1;

    // 1. Find the boundary of the Request Line
    const char *line_end = memmem(data, len, "\r\n", 2);
    if (!line_end) return NEED_MORE_DATA;

    size_t line_len = line_end - data;
    init_str_view(&req->status_line, data, line_len);

    const char *cur = data;
    const char *end = line_end;

    // 2. Extract Method
    const char *sp1 = memchr(cur, ' ', end - cur);
    if (!sp1) return -1; // Malformed: No SP after method
    
    // Instead of memcpy, pass pointer and length to your enum converter
    req->method = string_to_enum_n(cur, sp1 - cur); 
    
    cur = sp1;
    while (cur < end && *cur == ' ') cur++; // Skip spaces

    // 3. Extract URI
    const char *sp2 = memchr(cur, ' ', end - cur);
    if (!sp2) return -1; // Malformed: No SP after URI
    
    init_str_view(&req->uri, cur, sp2 - cur);
    
    cur = sp2;
    while (cur < end && *cur == ' ') cur++; // Skip spaces

    // 4. Extract Version
    init_str_view(&req->http_version, cur, end - cur);

    return (int) line_len; 
}

int http_extract_headers(
    const char *data,
    const size_t len,
    http_request *req
    ) {
    if (data == NULL || req == NULL) return -1;

    // 1. Find the final boundary of the header section
    const char *header_end = memmem(data, len, "\r\n\r\n", 4);
    if (header_end == NULL) return NEED_MORE_DATA;

    const char *cur = data;
    const char *line_end;

    // 2. Iterate line by line until we reach the double CRLF
    while (cur < header_end) {
        line_end = memmem(cur, header_end - cur, "\r\n", 2);
        if (line_end == NULL) break; 

        // Safety: Prevent array overflow
        if (req->headers_amt >= MAX_HTTP_HEADERS) {
            log_error("Too many headers in request");
            return -1; 
        }

        const char *sep = memchr(cur, ':', line_end - cur);
        if (sep == NULL) {
            // This handles the empty line or malformed headers
            cur = line_end + 2;
            continue;
        }

        // Initialize Name View
        init_str_view(
            &req->header_tab[req->headers_amt].name,
            cur,
            sep - cur
            );

        // Move past ':' and trim leading spaces for the value
        sep++; 
        while (sep < line_end && *sep == ' ') sep++;

        // Initialize Value View
        init_str_view(
            &req->header_tab[req->headers_amt].value,
            sep,
            line_end - sep
            );

        req->headers_amt++;
        cur = line_end + 2; // Move to the start of the next line
    }

    // Return total bytes consumed (headers + the \r\n\r\n)
    return (int)(header_end - data + 4);
}

int http_extract_body(
    const char *data,
    const size_t len,
    http_request *req
    ) {

  if (data == NULL || req == NULL)
    return -1;

  char *body = memmem(data, len, "\r\n", 2);
  if (body == NULL)
    return -1; /* No existing CRLF to top off body */

  init_str_view(&req->body, data, body - data);
  return 0;
}

int lookup_header_value(
    const char *data,
    int data_size,
    const char *header_name,
    string_view *value
    ) {
  
  /*
   * FIX: this will fail for any malitious client sending '\0' 
   * in req_line and headers
   *
   * But they're malicious so fuck off
   */
  char *delim = strcasestr(data, header_name);
  if (delim == NULL) 
    return -1; 

  /* find the beggining of the header value */
  char *separator = memchr(delim, ':', data_size - (delim - data));
  if (separator == NULL)
    return -1;

  ++separator;
  /* find start of CRLF */
  delim = memchr(separator, '\r', data_size - (separator - data));
  if (delim == NULL)
    return -1;
  init_str_view(value, separator, delim - separator);
  return 0;
}

