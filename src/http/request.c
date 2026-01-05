#define _GNU_SOURCE
#include <string.h>
#include "request.h"
#include "../utils/logger.h"

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

http_request *init_http_request(http_request *req) {
  if (req == NULL)
    return NULL;

  if (init_string(&req->raw_data, INIT_HTTP_REQ_SIZE) == NULL)
    return NULL;

  req->headers_amt = 0;

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
    ) {

  char *status_line = NULL;
  int offset = 0, i = 0, delimiter = 0;
  char tmp_buffer[512];
  if (data == NULL || req == NULL)
    return -1;

  /* 
   * STATUS LINE format is:
   * METHOD SP URI SP HTTP_VERSION CRLF
   */
  status_line = memmem(data, len, "\r\n", 2);
  if (status_line == NULL) /* Not enough data in buffer yet */
    return -1;

  offset = status_line - data; /* This should start on CR */
  string_ncpy(&req->raw_data, data, offset); 
  delimiter = i;
  while (data[delimiter] != ' ')
    delimiter++;

  memcpy(tmp_buffer, data, delimiter - i);
  tmp_buffer[delimiter - i] = '\0';
  req->method = string_to_enum(tmp_buffer); 
  
  delimiter++;
  i = delimiter;
  while (data[delimiter] != ' ')
    delimiter++;

  init_str_view(
      &req->uri,
      req->raw_data.buffer + i,
      delimiter - i);

  memcpy(tmp_buffer, req->raw_data.buffer + i, req->uri.len);
  tmp_buffer[req->uri.len] = '\0';

  delimiter++;
  init_str_view(
      &req->http_version,
      req->raw_data.buffer + delimiter,
      offset - delimiter
      );

  return 0;
}

int http_extract_headers(
    const char *data,
    const size_t len,
    http_request *req
    ) {

  if (req == NULL || data == NULL)
    return -1;

  char *header_line = NULL;
  int offset = 0, i = 0;
  int header_delimiter = 0, name_delimiter = 0;
  size_t status_line_len = req->raw_data.len;
  header_line = memmem(data, len, "\r\n\r\n", 4);
  if (header_line == NULL)
    return -1; /* Not enough data yet probably */

  /*
   * Header format is :
   * HEADER_NAME:  (SP)*  HEADER_VALUE CRLF
   *
   * One approach is, extract via CRLF all of the different header_line
   * For each header line
   * iterate up to ":", and then take separately the name and value
   */

  offset = header_line - data;
  if (string_ncat(&req->raw_data, data, offset) != 0)
    return -1; /* Could not append headers to existing status-line */

  while (i < offset) {
    /* advance untill we find a CRLF */
    header_delimiter = i;
    while (
        data[header_delimiter] != '\r' 
        && data[header_delimiter + 1] != '\n'
        && header_delimiter <= offset)
      ++header_delimiter;

    name_delimiter = i;
    while (data[name_delimiter] != ':')
      ++name_delimiter;

    /* the upper bound of the value is the header delimiter itself */
    init_str_view(
        &req->header_tab[req->headers_amt].name,
        req->raw_data.buffer + status_line_len, 
        name_delimiter
        );

    ++name_delimiter;
    init_str_view(
        &req->header_tab[req->headers_amt].value,
        req->raw_data.buffer + status_line_len + name_delimiter,
        header_delimiter - name_delimiter 
        );

    ++req->headers_amt;
    i = header_delimiter + 2; /* move i past CRLF */ 
  }

  return 0;
}

int http_extract_body(
    const char *data,
    const size_t len,
    http_request *req
    ) {

  if (data == NULL || req == NULL)
    return -1;

  char *body = memmem(data, len, "\r\n", 2);
  int offset = 0;
  if (body == NULL)
    return -1; /* No existing CRLF to top off body */

  offset = body - data;
  int header_len = req->raw_data.len;
  if (string_ncat(&req->raw_data, data, len) != 0)
    return -1; /* Can not append body to data */

  init_str_view(&req->body, req->raw_data.buffer + header_len, offset);
  return 0;
}

