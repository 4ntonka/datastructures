

#include "array.h"

#include <stdlib.h>

struct array {
  int *data;
  unsigned long size;
  unsigned long capacity;
};

struct array *array_init(unsigned long initial_capacity) {
  struct array *a = malloc(sizeof(struct array));
  if (!a) return NULL;

  a->data = malloc(initial_capacity * sizeof(int));
  if (!a->data) {
    free(a);
    return NULL;
  }

  a->size = 0;
  a->capacity = initial_capacity;
  return a;
}

void array_cleanup(struct array *a) {
  if (a) {
    free(a->data);
    free(a);
  }
}

int array_get(const struct array *a, unsigned long index) {
  if (!a || index >= a->size) {
    return -1;
  }
  return a->data[index];
}

int array_append(struct array *a, int elem) {
  if (!a) return 1;

  // If we're at capacity, resize
  if (a->size == a->capacity) {
    unsigned long new_capacity = a->capacity * 2;
    int *new_data = realloc(a->data, new_capacity * sizeof(int));
    if (!new_data) return 1;

    a->data = new_data;
    a->capacity = new_capacity;
  }

  a->data[a->size++] = elem;
  return 0;
}

unsigned long array_size(const struct array *a) { return a ? a->size : 0; }
