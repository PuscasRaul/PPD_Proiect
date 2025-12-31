#ifndef HTTP_MESSAGE_H
#define HTTP_MESSAGE_H

#include <stdlib.h>

#include "../utils/my_string.h"
#include "../utils/string_view.h"

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
  string_t raw_data; /* The raw data received over the network */

  string_view uri; /* uri */
  string_view start_line; /* Request/ response line */
  http_header headers[MAX_HTTP_HEADERS]; /* Headers array */
  string_view body; /* Message body */
};

/*
 * Initializes the http message given it's raw format.
 * param: msg: the http message to be created
 * param: raw_msg: the raw string to be converted
 *
 * return: NULL on failure
 *         address of msg on success
 *
 * NOTE: must be destroyed via a call to deinit_http_message
 */

http_message *init_http_message(http_message *msg, size_t msg_len); 
void deinit_http_message(http_message *msg);

static inline http_message *new_http_message(size_t msg_len) {
  return init_http_message(malloc(sizeof(http_message)), msg_len);
}

void static inline free_http_message(http_message *msg) {
  if (msg) {
    deinit_http_message(msg);
    free(msg);
  }
}

/*
 * Parses the data buffer and populates the http_message fields
 * param: data: the data read over the network
 * param: len: length of data
 * param: msg: http_msg to be populated
 *
 * return: NULL on failure(poorly formatted message/ msg is NULL)
 *         address of msg on success
 */
http_message *http_parse(
    const char *data,
    size_t len,
    http_message *msg
    );

int http_extract_status_line(const char *data, http_message *msg);
int http_extract_headers(const char *data, http_message *msg);
int http_extract_body(const char *data, http_message *msg);

#endif // HTTP_MESSAGE_H
