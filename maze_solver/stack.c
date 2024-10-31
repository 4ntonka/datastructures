#include "stack.h"

#include <stdio.h>
#include <stdlib.h>

typedef int keytype;

/* Handle to stack */
struct stack {
  keytype *data;
  size_t capacity;
  size_t used;
};

/* Return a pointer to a stack data structure with a maximum capacity of
 * 'capacity' if successful, otherwise return NULL. */
struct stack *stack_init(size_t capacity) {
  struct stack *s = malloc(sizeof(struct stack));
  if (s == NULL) {
    return NULL;
  }

  s->data = malloc(capacity * sizeof(keytype));
  if (s->data == NULL) {
    free(s);
    return NULL;
  }

  s->capacity = capacity;
  s->used = 0;
  return s;
}

/* Cleanup stack. */
void stack_cleanup(struct stack *s) {
  if (s != NULL) {
    free(s->data);
    free(s);
  }
}

/* Print stack statistics to stderr.
 * The format is: 'stats' num_of_pushes num_of_pops max_elements */
void stack_stats(const struct stack *s) {
  if (s == NULL) {
    return;
  }
  fprintf(stderr, "stats %zu %zu %zu\n", s->used, s->used, s->capacity);
}

int stack_push(struct stack *s, int c) {
  if (s == NULL) {
    return 1;
  }
  if (s->used == s->capacity) {
    return 1;
  }
  s->data[s->used++] = c;
  return 0;
}

/* Pop item from stack and return it.
 * Return top item if successful, -1 otherwise. */
int stack_pop(struct stack *s) {
  if (s == NULL) {
    return -1;
  }
  if (s->used == 0) {
    return -1;
  }
  return s->data[--s->used];
}

/* Return top of item from stack. Leave stack unchanged.
 * Return top item if successful, -1 otherwise. */
int stack_peek(const struct stack *s) {
  if (s == NULL) {
    return -1;
  }
  if (s->used == 0) {
    return -1;
  }
  return s->data[s->used - 1];
}

/* Return 1 if stack is empty, 0 if the stack contains any elements and
 * return -1 if the operation fails. */
int stack_empty(const struct stack *s) {
  if (s == NULL) {
    return -1;
  }
  if (s->used == 0) {
    return 1;
  }
  return 0;
}

/* Return the number of elements stored in the stack. */
size_t stack_size(const struct stack *s) {
  if (s == NULL) {
    return -1;
  }
  return s->used;
}
