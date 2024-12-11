/*
 * Name : A. Smirnov
 * UvAnetID : 13272225
 * Study : BSc Informatica
 *
 * File set.c contains the implementation of the Set abstract data type. It
 * provides a clean API for set operations while internally using a binary
 * search tree (implemented in tree.c) as its underlying data structure.
 */

#include "set.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tree.h"

struct set {
  struct tree *tree;
};

struct set *set_init(int turbo) {
  struct set *s = malloc(sizeof(struct set));
  if (!s) {
    return NULL;
  }
  s->tree = tree_init(turbo);
  if (!s->tree) {
    free(s);
    return NULL;
  }
  return s;
}

int set_insert(struct set *s, int num) {
  if (!s || !s->tree) {
    return -1;
  }
  int result = tree_insert(s->tree, num);
  if (result == 0) return 0;
  if (result == 1) return 1;
  return -1;
}

int set_find(struct set *s, int num) {
  if (!s || !s->tree) {
    return 0;
  }
  return tree_find(s->tree, num);
}

int set_remove(struct set *s, int num) {
  if (!s || !s->tree) {
    return 1;
  }
  return tree_remove(s->tree, num);
}

void set_cleanup(struct set *s) {
  if (!s) {
    return;
  }
  if (s->tree) {
    tree_cleanup(s->tree);
  }
  free(s);
}

void set_print(const struct set *s) {
  if (!s || !s->tree) {
    return;
  }
  tree_print(s->tree);
}

int set_verify(const struct set *s) {
  if (!s || !s->tree) {
    return 0;
  }
  return tree_check(s->tree);
}
