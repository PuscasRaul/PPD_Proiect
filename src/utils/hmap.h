#ifndef UTILS_HMAP_H
#define UTILS_HMAP_H

#include <stdint.h>
#include <stddef.h>

typedef struct hnode hnode;
typedef struct htab htab;
typedef struct hmap hmap;

struct hnode {
  hnode *next;
  uint64_t hcode;
};

struct htab {
  hnode **tab; /* Array of buckets */
  size_t mask; /* 2^x - 1 mask */
  size_t size; /* number of keys */
};

/*
 * Initializes a fixed-size htable
 * param: htab: hash table to be initialized
 * param: mask: non-zero power of 2 used for efficient position calc?
 *
 * return: NULL on failure
 *         htab address on success
 * NOTE: must be deinitialized via a call to deinit_htab
 */
htab *init_htab(htab *htab, size_t mask);

/*
 * Deinitializes htable
 * param: htab: htable to be initialized
 * NOTE: safe to call with NULL htab
 */
void deinit_htab(htab *htab);

/*
 * Inserts a node into the htable
 * param: htab: hashtable to insert into
 * param: node: node to be inserted
 * return: NULL on failure
 *         address of node on success
 */
hnode *h_insert(htab *htab, hnode *node); 

/*
 * Checks for existence of node into hash table
 * param: htab: hash table in which we check
 * param: key: searched node
 * param: eq: equality function between node for which we check
 *
 * return: NULL on failure
 *         address of node* on success
 */
hnode **h_lookup(
    htab *htab,
    hnode *key,
    int (*eq)(hnode *, hnode *)
    ); 

/*
 * Detaches node from hash table
 * param: htab: hash table from which we detach
 * param: key: node we want to delete 
 * param: eq: equality function between node for which we check
 *
 * return: NULL on failure
 *         deleted node on success
 */
hnode *h_delete(htab *htab, hnode *key, int (*eq)(hnode*, hnode*)); 

#endif // UTILS_HMAP_H
