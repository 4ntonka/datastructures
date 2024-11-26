#include "hash_table.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "array.h"

struct table {
  /* The (simple) array used to index the table */
  struct node **array;
  /* The function used for computing the hash values in this table */
  unsigned long (*hash_func)(const unsigned char *);

  /* Maximum load factor after which the table array should be resized */
  double max_load_factor;
  /* Capacity of the array used to index the table */
  unsigned long capacity;
  /* Current number of elements stored in the table */
  unsigned long load;
};

/* Note: This struct should be a *strong* hint to a specific type of hash table
 * You may implement other options, if you can build them in such a way they
 * pass all tests. However, the other options are generally harder to code. */
struct node {
  /* The string of characters that is the key for this node */
  char *key;
  /* A resizing array, containing the all the integer values for this key */
  struct array *value;

  /* Next pointer */
  struct node *next;
};

/* ... SOME CODE MISSING HERE ... */

struct table *table_init(unsigned long capacity, double max_load_factor,
                         unsigned long (*hash_func)(const unsigned char *)) {
  /* Note: The 'array' member of struct table is a pointer to a block of
   * memory that contains pointers to struct nodes. Make sure that the struct
   * node pointers in this block are all set to NULL. A good way to do this is
   * with the calloc() function. Check the manual page 'man calloc' for
   * its usage. */
  if (capacity == 0 || max_load_factor <= 0.0 || hash_func == NULL) {
    return NULL;
  }
  struct table *t = malloc(sizeof(struct table));
  if (!t) {
    return NULL;
  }
  t->array = calloc(capacity, sizeof(struct node *));
  if (!t->array) {
    free(t);
    return NULL;
  }
  t->hash_func = hash_func;
  t->max_load_factor = max_load_factor;
  t->capacity = capacity;
  t->load = 0;
  return t;
}

int table_insert(struct table *t, const char *key, int value) {
  if (!t || !key) {
    return 1;
  }
  // printf("Current load factor: %.2f, Capacity: %lu\n",
  //        (double)t->load / t->capacity, t->capacity);
  if ((double)(t->load + 1) / t->capacity > t->max_load_factor) {
    unsigned long new_capacity = t->capacity * 2;
    struct node **new_array = calloc(new_capacity, sizeof(struct node *));
    if (!new_array) {
      return 1;
    }
    for (unsigned long i = 0; i < t->capacity; i++) {
      struct node *current = t->array[i];
      while (current) {
        struct node *next = current->next;
        unsigned long new_hash =
            t->hash_func((const unsigned char *)current->key) % new_capacity;
        current->next = new_array[new_hash];
        new_array[new_hash] = current;
        current = next;
      }
    }
    free(t->array);
    t->array = new_array;
    t->capacity = new_capacity;
  }
  unsigned long hash = t->hash_func((const unsigned char *)key) % t->capacity;
  struct node *current = t->array[hash];
  while (current) {
    if (strcmp(current->key, key) == 0) {
      return array_append(current->value, value);
    }
    current = current->next;
  }
  struct node *new_node = malloc(sizeof(struct node));
  if (!new_node) {
    return 1;
  }
  new_node->key = strdup(key);
  if (!new_node->key) {
    free(new_node);
    return 1;
  }
  new_node->value = array_init(4);
  if (!new_node->value) {
    free(new_node->key);
    free(new_node);
    return 1;
  }
  if (array_append(new_node->value, value) != 0) {
    array_cleanup(new_node->value);
    free(new_node->key);
    free(new_node);
    return 1;
  }
  new_node->next = t->array[hash];
  t->array[hash] = new_node;
  t->load++;
  return 0;
}

struct array *table_lookup(const struct table *t, const char *key) {
  if (!t || !key) {
    return NULL;
  }
  unsigned long hash = t->hash_func((const unsigned char *)key) % t->capacity;
  struct node *current = t->array[hash];
  while (current) {
    if (strcmp(current->key, key) == 0) {
      return current->value;
    }
    current = current->next;
  }
  return NULL;
}

double table_load_factor(const struct table *t) {
  if (!t) {
    return -1.0;
  }
  return (double)t->load / t->capacity;
}

int table_delete(struct table *t, const char *key) {
  if (!t || !key) {
    return -1;
  }
  unsigned long hash = t->hash_func((const unsigned char *)key) % t->capacity;
  struct node *current = t->array[hash];
  struct node *prev = NULL;
  while (current) {
    if (strcmp(current->key, key) == 0) {
      if (prev) {
        prev->next = current->next;
      } else {
        t->array[hash] = current->next;
      }
      array_cleanup(current->value);
      free(current->key);
      free(current);
      t->load--;
      return 0;
    }
    prev = current;
    current = current->next;
  }
  return 1;
}

void table_cleanup(struct table *t) {
  if (!t) {
    return;
  }
  for (unsigned long i = 0; i < t->capacity; i++) {
    struct node *current = t->array[i];
    while (current) {
      struct node *next = current->next;
      array_cleanup(current->value);
      free(current->key);
      free(current);
      current = next;
    }
  }
  free(t->array);
  free(t);
}
