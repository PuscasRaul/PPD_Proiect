#define _GNU_SOURCE
#include <string.h>
#include "request.h"

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

static enum http_methods string_to_enum(const char *str) {
    size_t num_methods = sizeof(method_table) / sizeof(method_table[0]);
    for (size_t i = 0; i < num_methods; i++) {
        if (strcmp(str, method_table[i].name) == 0) {
            return method_table[i].method;
        }
    }
    return UNRECOGNIZED;
}

static int header_eq(hnode *lhs, hnode *rhs) {
  http_header *first = container_of(lhs, http_header, node);
  http_header *second = container_of(rhs, http_header, node);

  return string_cmp(&first->name, &second->name); 
}

http_request *init_http_request(http_request *req) {
  if (req == NULL)
    return NULL;

  if (init_string(&req->raw_data, INIT_HTTP_REQ_SIZE) == NULL)
    return NULL;

  if (init_htab(&req->header_tab, MAX_HTTP_HEADERS) == NULL) {
    deinit_string(&req->raw_data);
    return NULL;
  }
  
  return req;
}

void deinit_http_request(http_request *req) {
  if (req != NULL) {
    deinit_string(&req->raw_data);
    deinit_str_view(&req->body);
    deinit_str_view(&req->status_line);
    deinit_str_view(&req->uri);
    deinit_str_view(&req->body);
    deinit_htab(&req->header_tab);
  }
}

int http_extract_req_line(
    const char *data,
    const size_t len,
    http_request *req
    ) {

  char *status_line = NULL;
  int offset = 0;
  const char *iterator = NULL;
  char tmp_buffer[512];
  if (data == NULL || req == NULL)
    return -1;

  status_line = memmem(data, len, "\r\n", 2);
  if (status_line == NULL) /* Not enough data in buffer yet */
    return -1;

  offset = status_line - data; /* This should start on CR */

  /* 
   * format is:
   * METHOD SP Request-URI SP HTTP_VERSION CRLF
   *
   */

  /* copy raw data into request string */
  if (string_cpy(&req->raw_data, data) == NULL) 
    return -1;

  /* initialize string view for status line */
  init_str_view(&req->status_line, req->raw_data.buffer, len);
   
  /* extract the method */
  iterator = data; 
  while (*iterator != ' ')
    iterator++;
  offset = iterator - data;
  memcpy(tmp_buffer, data, offset);
  tmp_buffer[offset] = '\0';
  req->method = string_to_enum(tmp_buffer);

  /* extract the uri and initialize string view */
  data = iterator;
  while (*iterator != ' ')
    ++iterator;

  offset = iterator - data;
  init_str_view(&req->uri, data, offset);

  return 0;
}

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
