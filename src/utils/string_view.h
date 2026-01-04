#ifndef UTILS_STRING_VIEW_H
#define UTILS_STRING_VIEW_H

#include <stddef.h>
#include "my_string.h"

typedef struct string_view string_view;
struct string_view {
  char *buffer;
  size_t len;
};

/*
 * Initializes a string view with a specified length on buffer.
 * param: str_v: pointer to string_view
 * param: buffer: buffer on which we initialize the view
 * param: len: len for the view
 * return: NULL on failure(str_v is NULL)
 *         pointer to str_v on success
 */
string_view *init_str_view(
    string_view *str_v,
    const char *buffer,
    size_t len
    );

/*
 * Deinitializes a string view.
 * param: str_v: pointer to string view
 * NOTE: safe to call with NULL ptr
 */
void deinit_str_view(string_view *str_v);

/*
 * Allocates on heap a new string view and initializes it.
 * param: buffer: buffer on which we initialize the view
 * param: len: len for the view
 *
 * NOTE: internally, it calls init_str_view, more info there
 */
static inline string_view *new_str_view(
    const char *buffer,
    size_t len
    ) {
  return init_str_view(malloc(sizeof(string_view)), buffer, len);
}

/*
 * Deinitializes and free's memory pointed to by str_v
 * NOTE: safe to call with NULL ptr
 */
static inline void free_str_view(string_view *str_v) {
  if (str_v) {
    deinit_str_view(str_v);
    free(str_v);
  }
}

#endif
