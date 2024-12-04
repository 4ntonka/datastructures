#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "array.h"
#include "prioq.h"

static struct heap *heap_init(int (*compare)(const void *, const void *)) {
  struct heap *h = malloc(sizeof(struct heap));
  if (!h) return NULL;

  h->array = array_init(1);
  if (!h->array) {
    free(h);
    return NULL;
  }

  h->compare = compare;
  return h;
}

prioq *prioq_init(int (*compare)(const void *, const void *)) {
  return heap_init(compare);
}

long int prioq_size(const prioq *q) {
  if (!q) return -1;
  return array_size(q->array);
}

static int heap_cleanup(struct heap *h, void free_func(void *)) {
  if (!h) return -1;
  array_cleanup(h->array, free_func);
  free(h);
  return 0;
}

int prioq_cleanup(prioq *q, void free_func(void *)) {
  return heap_cleanup(q, free_func);
}

static void heapify_up(struct heap *h, int index) {
  while (index > 0) {
    int parent = (index - 1) / 2;
    if (h->compare(array_get(h->array, index), array_get(h->array, parent)) <
        0) {
      void *temp = array_get(h->array, index);
      array_set(h->array, index, array_get(h->array, parent));
      array_set(h->array, parent, temp);
      index = parent;
    } else {
      break;
    }
  }
}

static void heapify_down(struct heap *h, int index) {
  int size = array_size(h->array);
  while (1) {
    int smallest = index;
    int left = 2 * index + 1;
    int right = 2 * index + 2;

    if (left < size && h->compare(array_get(h->array, left),
                                  array_get(h->array, smallest)) < 0) {
      smallest = left;
    }

    if (right < size && h->compare(array_get(h->array, right),
                                   array_get(h->array, smallest)) < 0) {
      smallest = right;
    }

    if (smallest != index) {
      void *temp = array_get(h->array, index);
      array_set(h->array, index, array_get(h->array, smallest));
      array_set(h->array, smallest, temp);
      index = smallest;
    } else {
      break;
    }
  }
}

static int heap_insert(struct heap *h, void *p) {
  if (!h) return -1;

  if (array_append(h->array, p) != 0) {
    return -1;
  }

  heapify_up(h, array_size(h->array) - 1);
  return 0;
}

int prioq_insert(prioq *q, void *p) { return heap_insert(q, p); }

static void *heap_pop(struct heap *h) {
  if (!h || array_size(h->array) == 0) return NULL;

  void *result = array_get(h->array, 0);
  void *last = array_pop(h->array);

  if (array_size(h->array) > 0) {
    array_set(h->array, 0, last);
    heapify_down(h, 0);
  } else {
    result = last;
  }

  return result;
}

void *prioq_pop(prioq *q) { return heap_pop(q); }
