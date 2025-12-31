#include "my_string.h"
#include <string.h>

string_t *init_string(string_t *str, size_t capacity) {
  if (str == NULL)
    return NULL;

  str->capacity = capacity;
  str->buffer = malloc(capacity);
  if (str->buffer == NULL)
    return NULL;
  str->len = 0;

  return str;
}

string_t *init_with_msg(string_t *str, const char * restrict msg) {
  if (str == NULL)
    return NULL;

  if (msg == NULL) {
    str->capacity = str->len = 0;
    return str;
  }
  
  str->capacity = strlen(msg) + 1;
  str->len = strlen(msg);
  str->buffer = malloc(str->capacity);
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
    const char *restrict src
    ) {
  if (dest == NULL || src == NULL)
    return NULL;
  
  size_t str_len = strlen(src);

  /* do a long ass copy */
  if (dest->capacity < str_len) {
    deinit_string(dest);
    return init_with_msg(dest, src);
  }

  memcpy(dest->buffer, src, str_len);
  dest->buffer[str_len] = '\0';
  dest->len = str_len;
  return dest;
}

void string_clear(string_t *str) {
  str->buffer -= (str->len - 1); /* WARN: might cause a SEG fault */
  str->len = 0;
}
