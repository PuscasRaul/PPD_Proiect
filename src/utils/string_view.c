#include "string_view.h"

string_view *init_str_view(
    string_view *str_v,
    const char *buffer,
    size_t len
    ) {
  if (str_v == NULL)
    return NULL;

  str_v->buffer = buffer;
  str_v->len = len;

  return str_v;
}

void deinit_str_view(string_view *str_v) {
  if (str_v) {
    init_str_view(str_v, NULL, 0);
  }
}
