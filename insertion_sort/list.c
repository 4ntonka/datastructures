/*
 * Name : A. Smirnov
 * UvAnetID : 13272225
 * Study : BSc Informatica
 *
 * This file contains the implementation of the linked list and its nodes, for
 * function descriptions look for list.h.
 */

#include "list.h"

struct node {
  int value;
  struct node *next;
};

struct list {
  struct node *head;
  struct node *tail;
  size_t size;
};

struct list *list_init(void) {
  struct list *l = (struct list *)malloc(sizeof(struct list));
  if (l == NULL) {
    return NULL;
  }
  l->head = NULL;
  l->tail = NULL;
  l->size = 0;
  return l;
}

struct node *list_new_node(int num) {
  struct node *n = (struct node *)malloc(sizeof(struct node));
  if (n == NULL) {
    return NULL;
  }
  n->value = num;
  n->next = NULL;
  return n;
}

struct node *list_head(const struct list *l) {
  if (l == NULL) {
    return NULL;
  }
  return l->head;
}

struct node *list_next(const struct node *n) {
  if (n == NULL) {
    return NULL;
  }
  return n->next;
}

int list_add_front(struct list *l, struct node *n) {
  if (l == NULL || n == NULL) {
    return 1;
  }
  n->next = l->head;
  l->head = n;
  if (l->tail == NULL) {
    l->tail = n;
  }
  l->size++;
  return 0;
}

struct node *list_tail(const struct list *l) {
  if (l == NULL) {
    return NULL;
  }
  return l->tail;
}

struct node *list_prev(const struct list *l, const struct node *n) {
  struct node *current = l->head;
  while (current != NULL && current->next != n) {
    current = current->next;
  }
  return current;
}

int list_add_back(struct list *l, struct node *n) {
  if (l == NULL || n == NULL) {
    return 1;
  }
  if (l->head == NULL) {
    l->head = n;
    l->tail = n;
  } else {
    l->tail->next = n;
    l->tail = n;
  }
  l->size++;
  return 0;
}

int list_node_get_value(const struct node *n) {
  if (n == NULL) {
    return 0;
  }
  return n->value;
}

int list_node_set_value(struct node *n, int value) {
  if (n == NULL) {
    return 1;
  }
  n->value = value;
  return 0;
}

int list_unlink_node(struct list *l, struct node *n) {
  if (l == NULL || n == NULL || l->head == NULL) {
    return 1;
  }
  if (l->head == n) {
    l->head = n->next;
    if (l->tail == n) {
      l->tail = NULL;
    }
  } else {
    struct node *prev = list_prev(l, n);
    if (prev == NULL) {
      return 1;
    }
    prev->next = n->next;
    if (l->tail == n) {
      l->tail = prev;
    }
  }
  n->next = NULL;
  l->size--;
  return 0;
}

void list_free_node(struct node *n) {
  if (n != NULL) {
    free(n);
  }
}

int list_cleanup(struct list *l) {
  if (l == NULL) {
    return 1;
  }

  struct node *current = l->head;
  while (current != NULL) {
    struct node *next = current->next;
    list_free_node(current);
    current = next;
  }
  free(l);
  return 0;
}

int list_node_present(const struct list *l, const struct node *n) {
  if (l == NULL || n == NULL) {
    return -1;
  }
  struct node *current = l->head;
  while (current != NULL) {
    if (current == n) {
      return 1;
    }
    current = current->next;
  }
  return 0;
}

int list_insert_after(struct list *l, struct node *n, struct node *m) {
  if (l == NULL || n == NULL || m == NULL) {
    return 1;
  }
  if (list_node_present(l, n) == 1 || list_node_present(l, m) != 1) {
    return 1;
  }
  n->next = m->next;
  m->next = n;
  if (l->tail == m) {
    l->tail = n;
  }
  l->size++;
  return 0;
}

int list_insert_before(struct list *l, struct node *n, struct node *m) {
  if (l == NULL || n == NULL || m == NULL) {
    return 1;
  }
  if (list_node_present(l, n) == 1 || list_node_present(l, m) != 1) {
    return 1;
  }
  if (l->head == m) {
    n->next = m;
    l->head = n;
  } else {
    struct node *prev = list_prev(l, m);
    if (prev == NULL) {
      return 1;
    }
    prev->next = n;
    n->next = m;
  }
  l->size++;
  return 0;
}

size_t list_length(const struct list *l) {
  if (l == NULL) {
    return 0;
  }
  return l->size;
}

struct node *list_get_ith(const struct list *l, size_t i) {
  if (l == NULL || i >= l->size) {
    return NULL;
  }
  struct node *current = l->head;
  for (size_t j = 0; j < i; j++) {
    current = current->next;
  }
  return current;
}

struct list *list_cut_after(struct list *l, struct node *n) {
  if (l == NULL || n == NULL || !list_node_present(l, n)) {
    return NULL;
  }
  struct list *second = list_init();
  if (second == NULL) {
    return NULL;
  }
  second->head = n->next;
  if (second->head != NULL) {
    second->tail = l->tail;
    second->size = l->size - (size_t)(list_get_ith(l, l->size - 1) == n);
    l->tail = n;
    n->next = NULL;
    l->size = l->size - second->size;
  }
  return second;
}
