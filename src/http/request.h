#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include "../utils/my_string.h"
#include "../utils/string_view.h"
#include "core.h"

#ifndef MAX_HTTP_HEADERS
#define MAX_HTTP_HEADERS 32
#endif

/* might be unnecessary */
#ifndef INIT_HTTP_REQ_SIZE
#define INIT_HTTP_REQ_SIZE 4096
#endif

typedef struct http_header http_header;
typedef struct http_request http_request;

struct http_header {
  string_view name;
  string_view value;
};

struct http_request {
  string_t raw_data; /* Raw request data */

  string_view uri; /* Destination uri, view over raw_data */
  string_view status_line; /* Status_line of request, view over raw_data */
  string_view body; /* Body of request, view over raw_data */

  http_methods method; /* Method */

  http_header header_tab[MAX_HTTP_HEADERS]; /* Headers contained by request */
  size_t headers_amt; /* Amount of headers that the request contains */
};

http_request *init_http_request(http_request *req);
void deinit_http_request(http_request *req);

int http_extract_req_line(
    const char *data,
    const size_t len,
    http_request *req
    );

int http_extract_headers(
    const char *data,
    const size_t len,
    http_request *req
    );

int http_extract_body(
    const char *data,
    const size_t len,
    http_request *req
    );
#endif // HTTP_REQUEST_H
