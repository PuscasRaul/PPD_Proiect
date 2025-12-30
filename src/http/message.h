#ifndef HTTP_MESSAGE_H
#define HTTP_MESSAGE_H

#include "../utils/my_string.h"
#include "../utils/string_view.h"
#include "methods.h"

typedef struct http_message http_message;
typedef struct http_header http_header;

#ifndef MAX_HTTP_HEADERS
#define MAX_HTTP_HEADERS 30
#endif

struct http_header {
  string_t name; /* Header name */
  string_t value; /* header value */
};

struct http_message {
  string_t raw_message; /* The entirety of the request/ response */
  http_header headers[MAX_HTTP_HEADERS]; /* All the headers */
  string_view body; /* Body, it represents only a view over raw_message */
  string_view head; /* head, it represents only a view over raw_message */
  string_view uri; /* uri, it represents only a view over raw_message */
  http_methods method; /* method */
};

int http_parse(const char *data, size_t len, http_message *msg);

#endif // HTTP_MESSAGE_H
