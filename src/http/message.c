#define _GNU_SOURCE
#include <string.h>
#include "message.h"

/*
 * Returns the offset inside of data 
 * Of where the status_line ends and headers begin 
 * NOTE: use memmem for better handling compared to strstr
 */
static int http_extract_status_line(
    const char *data,
    const int data_len,
    http_message *msg
    ) {
  char *tmp_buffer;
  size_t offset = 0;
  tmp_buffer = memmem(data, data_len, "\r\n", 2);

  if (tmp_buffer == NULL) 
    return 0;

  if (tmp_buffer == data)
    return 0; /* there exists no status_line */

  offset = tmp_buffer - data; /* take the offset of CRLF inside of it */
  if (string_cpy(&msg->raw_data, data) == NULL)
    return 0;

  /* setup the string view */
  if (init_str_view(
        &msg->start_line,
        msg->raw_data.buffer,
        offset) == NULL
    ) {
    string_clear(&msg->raw_data);
    return 0;
  }

  return offset;
}

int http_extract_headers(
    const char *data,
    const int data_len,
    http_message *msg
    ) {
  return 0;
}

int http_extract_body(
    const char *data,
    const int data_len,
    http_message *msg
    ) {
  return 0;
}
