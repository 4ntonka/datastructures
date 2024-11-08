/*
 * Name : A. Smirnov
 * UvAnetID : 13272225
 * Study : BSc Informatica
 */

#include "queue.h"

#include <stdio.h>
#include <stdlib.h>

/* Handle to queue */
struct queue {
  int *data;            // Array to store queue elements
  size_t capacity;      // Maximum capacity of queue
  size_t size;          // Current number of elements
  size_t front;         // Index of front element
  size_t rear;          // Index of rear element
  size_t pushes;        // Total number of push operations
  size_t pops;          // Total number of pop operations
  size_t max_elements;  // Maximum number of elements at any time
};

/* Return a pointer to a queue data structure with a maximum capacity of
 * 'capacity' if successful, otherwise return NULL. */
struct queue *queue_init(size_t capacity) {
  struct queue *q = malloc(sizeof(struct queue));
  if (!q) return NULL;

  q->data = malloc(capacity * sizeof(int));
  if (!q->data) {
    free(q);
    return NULL;
  }

  q->capacity = capacity;
  q->size = 0;
  q->front = 0;
  q->rear = 0;
  q->pushes = 0;
  q->pops = 0;
  q->max_elements = 0;
  return q;
}

/* Cleanup queue. */
void queue_cleanup(struct queue *q) {
  if (q) {
    free(q->data);
    free(q);
  }
}

/* Print queue statistics to stderr.
 * The format is: 'stats' num_of_pushes num_of_pops max_elements */
void queue_stats(const struct queue *q) {
  if (q) {
    fprintf(stderr, "stats %zu %zu %zu\n", q->pushes, q->pops, q->max_elements);
  }
}

/* Push item the end of the queue.
 * Return 0 if successful, 1 otherwise. */
int queue_push(struct queue *q, int e) {
  if (!q || q->size >= q->capacity) {
    return 1;
  }

  q->data[q->rear] = e;
  q->rear = (q->rear + 1) % q->capacity;
  q->size++;
  q->pushes++;

  if (q->size > q->max_elements) {
    q->max_elements = q->size;
  }

  return 0;
}

/* Remove the first item from queue and return it.
 * Return the first item if successful, -1 otherwise. */
int queue_pop(struct queue *q) {
  if (!q || q->size == 0) {
    return -1;
  }

  int value = q->data[q->front];
  q->front = (q->front + 1) % q->capacity;
  q->size--;
  q->pops++;

  return value;
}

/* Return the first item from queue. Leave queue unchanged.
 * Return the first item if successful, -1 otherwise. */
int queue_peek(const struct queue *q) {
  if (!q || q->size == 0) {
    return -1;
  }
  return q->data[q->front];
}

/* Return 1 if queue is empty, 0 if the queue contains any elements and
 * return -1 if the operation fails. */
int queue_empty(const struct queue *q) {
  if (!q) {
    return -1;
  }
  return q->size == 0 ? 1 : 0;
}

/* Return the number of elements stored in the queue. */
size_t queue_size(const struct queue *q) { return q ? q->size : 0; }
