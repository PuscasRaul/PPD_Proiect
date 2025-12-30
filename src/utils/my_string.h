#ifndef UTILS_MY_STRING_H
#define UTILS_MY_STRING_H

#include <stddef.h>
#include <stdlib.h>

typedef struct string_t string_t;
struct string_t {
  char *buffer;
  size_t len;
  size_t capacity;
  int holds_data;
};

string_t *init_string(string_t *str, size_t capacity);
string_t *init_with_msg(string_t *restrict str, char *restrict msg);
void deinit_string(string_t *str);

static inline string_t *new_string(size_t capacity) {
  return init_string(malloc(sizeof(string_t)), capacity);
}

static inline void delete_string(string_t *str) {
  if (str) {
    deinit_string(str);
    free(str);
  }
}

string_t *string_resize(string_t *str, size_t new_capacity);
string_t *string_cpy(
    string_t *restrict dest,
    string_t *restrict src
    );
#endif
