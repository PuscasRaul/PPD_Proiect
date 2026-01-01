#define _GNU_SOURCE
#include <string.h>
#include "request.h"

http_request *init_http_request(http_request *req) {
  if (req == NULL)
    return NULL;

  if (init_string(&req->raw_data, INIT_HTTP_REQ_SIZE) == NULL)
    return NULL;
  
  return req;
}

void deinit_http_request(http_request *req) {
  if (req != NULL) {
    deinit_string(&req->raw_data);
    deinit_str_view(&req->body);
    deinit_str_view(&req->status_line);
    deinit_str_view(&req->uri);
    deinit_str_view(&req->body);
  }
}

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
