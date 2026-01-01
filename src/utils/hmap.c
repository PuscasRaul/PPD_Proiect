#include "hmap.h"
#include <stdlib.h>
#include "logger.h"

htab *h_init(htab *htab, size_t n) {
  if (htab == NULL) {
    log_info("Tried initializing a null htab");
    return NULL;
  }

  /* Make sure n is a power of 2 */
  if (n <= 0 || ((n - 1) & n) != 0) {
    log_info("Tried initializing with non-power of 2 mask");
    return NULL;
  }

  htab->size = 0;
  htab->mask = n - 1;
  if ((htab->tab = calloc(n, sizeof(hnode*))) == NULL) {
    log_error("Could not allocate memory for htab->tab");
    return NULL;
  }

  return htab;
}

void deinit_htab(htab *htab) {
  if (htab) {
    if (htab->tab)
      free(htab->tab);

    htab->tab = NULL;
    htab->mask = 1;
    htab->size = 0;
  }
}

hnode *h_insert(htab *htab, hnode *node) {
  if (htab == NULL || node == NULL) {
    log_info("Tried inserting into null htab or null node");
    return NULL;
  }

  size_t pos = node->hcode & htab->mask;
  hnode *next = htab->tab[pos];
  node->next = next;
  htab->tab[pos] = node;
  htab->size++;
  return node;
}

hnode **h_lookup(
    htab *htab,
    hnode *key,
    int (*eq)(hnode *, hnode *)
    ) {

  if (htab == NULL || key == NULL) {
    log_info("Tried searching into NULL htab or for NULL key");
    return NULL;
  }

  size_t pos = key->hcode & htab->mask;
  hnode **from = &htab->tab[pos];
  for (hnode *node = *from;
      (node = *from) != NULL;
      from = &(node->next)
      ) 
    if (key->hcode == node->hcode && eq(key, node))
      return from;
  
  log_info("Could not find element with hcode: [%d]", key->hcode);
  return NULL;
}

hnode *h_delete(htab *htab, hnode *key, int (*eq)(hnode*, hnode*)) {
  /* if htab is NULL, lookup will be NULL */
  hnode **from = h_lookup(htab, key, eq);
  if (from == NULL)
    return NULL;

  hnode *node = *from;
  *from = node->next;
  htab->size--;
  return node;
}
