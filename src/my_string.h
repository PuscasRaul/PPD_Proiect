#ifndef MY_STRING_H
#define MY_STRING_H

#include <stddef.h>
#include <stdlib.h>

typedef struct string_t string_t;
struct string_t {
  char *buffer; 
  size_t len; 
  size_t capacity;
};

/*
 * Initializes a string for a given capacity
 * param: str: the string to be initialized
 * param: capacity: capacity of initialized string
 * return: NULL on failure
 *         address of str on success
 * NOTE: deinitialize through via call to deinit_string
 */
string_t *init_string(string_t *str, size_t capacity);

/*
 * Initializes the string from a message.
 * The initialized string will have a capacity of strlen(msg) + 1
 * param: str: string to be initialized
 * param: msg: base message
 * return: NULL on failure
 *         address of str on success.
 * NOTE: deinitialize through a to deinit_string
 */
string_t *init_with_msg(
    string_t *str,
    const char * restrict msg
    );

/*
 * Deinitializes a string
 * param: str: string to be deinitialized
 * NOTE: safe to call with NULL str
 */
void deinit_string(string_t *str);

/*
 * Allocates on the heap space for a new string and initializes.
 * NOTE: more details available at init_string()
 */
static inline string_t *new_string(size_t capacity) {
  return init_string(malloc(sizeof(string_t)), capacity);
}

/*
 * Deinitializes and free's str.
 * NOTE: safe to call with NULL ptr
 */
static inline void delete_string(string_t *str) {
  if (str) {
    deinit_string(str);
    free(str);
  }
}

/*
 * Resizes string to a new capacity.
 * param: str: string to be resized
 * param: new_capacity: new capacity of string
 * return: NULL on failure
 *         address of str on success
 */
string_t *string_resize(string_t *str, size_t new_capacity);

/*
 * Copies src buffer into dest->buffer.
 * param: dest: string destination
 * param: src: source data
 *
 * NOTE: if dest->capacity is less than strlen(src), a resize will be
 *       performed.
 *       The first strlen(src) bytes of dest->buffer will be overwritten
 */
string_t *string_cpy(
    string_t * restrict dest,
    const char * restrict src
    );

/*
 * Copies n butes from src buffer into dest->buffer.
 * param: dest: string destination
 * param: src: source data
 *
 * NOTE: if dest->capacity is less than n, a resize will be
 *       performed with new_capacity set to (n + 1).
 *       The first n bytes of dest->buffer will be overwritten
 */
string_t *string_ncpy(
    string_t * restrict dest,
    const char * restrict src,
    size_t n
    );

/*
 * Resets the strings.
 * param: str: string to be resetted
 * NOTE: will keep the capacity, but the length will be set to 0
 */
void string_clear(string_t *str);

/* 
 * Compares two strings. 
 * param: lstr: left-hand string
 * param: rstr: right-hand string
 *
 * Return: <0 if (lstr < rstr ||
 *                lstr->len < rstr->len ||
 *                lstr != NULL && rstr == NULL
 *                )
 *         > 0 if (lstr > rstr  || 
 *                 lstr->len > rstr->len ||
 *                 (lstr == NULL && rstr != NULL)
 *                 )
 *         0 if lstr = rstr
 *         two NULL strings are considered to be equal by default
 */
int string_cmp(string_t *lstr, string_t *rstr);

/*
 * Concatenates data to end of dst.
 * param: dst: string destination
 * param: data: data to be concatenated
 * return: 0 on success
 *         any non-zero value otherwise
 */
int string_cat(string_t *dst, const char *data);

/*
 * Concatenates data to end of dst.
 * param: dst: string destination
 * param: data: data to be concatenated
 * param: len: length of data to be concatenated
 * return: 0 on success
 *         any non-zero value otherwise
 */
int string_ncat(string_t *dst, const char *data, size_t len);
#endif
