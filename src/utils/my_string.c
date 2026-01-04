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

  char *tmp_buffer = realloc(str->buffer, new_capacity);
  if (tmp_buffer == NULL) 
    return NULL;

  str->buffer = tmp_buffer;
  str->capacity = new_capacity;
  return str;
}

string_t *string_cpy(
    string_t *restrict dest,
    const char *restrict src
    ) {
  if (dest == NULL || src == NULL)
    return NULL;
  
  size_t str_len = strlen(src);

  if (dest->capacity < str_len) 
    string_resize(dest, str_len + 1);

  memcpy(dest->buffer, src, str_len);
  dest->len = str_len;
  return dest;
}

string_t *string_ncpy(
    string_t * restrict dest,
    const char * restrict src,
    size_t n
    ) {
  if (dest == NULL || src == NULL)
    return NULL;


  if (dest->capacity < n) 
    string_resize(dest, n + 1);

  memcpy(dest->buffer, src, n);
  dest->len = n;
  return dest;
}

void string_clear(string_t *str) {
  str->len = 0;
}

int string_cmp(string_t *lstr, string_t *rstr) {  
  if (lstr == NULL && rstr != NULL)
    return -1;

  if (lstr != NULL && rstr == NULL)
    return 1;

  if (lstr == NULL && rstr == NULL)
    return 0;

  if (lstr->len > rstr->len)
    return 1;

  if (lstr->len < rstr->len)
    return -1;

  for (register size_t i = 0; i < lstr->len; i++) {
    if (lstr->buffer[i] == rstr->buffer[i])
      continue;

    if (lstr->buffer[i] > rstr->buffer[i])
      return 1;
    else 
      return -1;
  }

  return 0;
}

int string_cat(string_t *dst, const char *data) {
  if (dst == NULL || data == NULL)
    return -1;

  if (dst->capacity < dst->len + strlen(data))
    if (string_resize(dst, dst->len + strlen(data) + 1) == NULL)
      return NULL;

  memcpy((dst->buffer + dst->len), data, strlen(data));
  return 0;
}
