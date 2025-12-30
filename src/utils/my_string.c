#include "my_string.h"
#include <string.h>

string_t *init_string(string_t *str, size_t capacity) {
  if (str == NULL)
    return NULL;

  str->capacity = capacity;
  str->buffer = malloc(capacity);
  str->holds_data = 1;
  if (str->buffer == NULL)
    return NULL;
  str->len = 0;

  return str;
}

string_t *init_with_msg(string_t *str, char *msg) {
  if (str == NULL)
    return NULL;

  if (msg == NULL) {
    str->capacity = str->len = 0;
    return str;
  }
  
  str->capacity = strlen(msg) + 1;
  str->len = strlen(msg);
  str->buffer = malloc(str->capacity);
  str->holds_data = 1;
  memcpy(str->buffer, msg, str->len);
  str->buffer[str->len] = '\0';

  return str;
}

void deinit_string(string_t *str) {
  if (str != NULL) {
    if (str->buffer)
      free(str->buffer);
    str->capacity = str->len = 0;
    str->buffer = NULL;
  }
}
  
string_t *string_resize(string_t *str, size_t new_capacity) {
  if (str == NULL)
    return NULL;

  deinit_string(str);
  return init_string(str, new_capacity);
}

string_t *string_cpy(
    string_t *restrict dest,
    string_t *restrict src
    ) {
  if (dest == NULL || src == NULL)
    return NULL;

  /* do a long ass copy */
  if (dest->capacity < src->len) {
    deinit_string(dest);
    return init_with_msg(dest, src->buffer);
  }

  memcpy(dest->buffer, src->buffer, src->len);
  dest->buffer[src->len] = '\0';
  dest->len = src->len;
  dest->holds_data = 1;
  return dest;
}

