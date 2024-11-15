/*
 * Name : A. Smirnov
 * UvAnetID : 13272225
 * Study : BSc Informatica
 *
 *
 */

#include <getopt.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"
#define BUF_SIZE 1024

char buf[BUF_SIZE];

struct config {
  /* You can ignore these options until you implement the
     extra command-line arguments. */

  /* Set to 1 if -d is specified, 0 otherwise. */
  int descending_order;

  // Set to 1 if -c is specified, 0 otherwise.
  int combine;

  // Set to 1 if -o is specified, 0 otherwise.
  int remove_odd;

  /* Set to 1 if -z is specified, 0 otherwise. */
  int zip_alternating;
};

int parse_options(struct config *cfg, int argc, char *argv[]) {
  memset(cfg, 0, sizeof(struct config));
  int c;
  while ((c = getopt(argc, argv, "dcoz")) != -1) {
    switch (c) {
      case 'd':
        cfg->descending_order = 1;
        break;
      case 'c':
        cfg->combine = 1;
        break;
      case 'o':
        cfg->remove_odd = 1;
        break;
      case 'z':
        cfg->zip_alternating = 1;
        break;
      default:
        fprintf(stderr, "invalid option: -%c\n", optopt);
        return 1;
    }
  }
  return 0;
}

// Helper function to insert a number into the sorted list
void insert_sorted(struct list *l, int num, int descending) {
  struct node *new_node = list_new_node(num);
  if (!new_node) return;

  struct node *current = list_head(l);
  struct node *prev = NULL;

  while (current != NULL) {
    int curr_val = list_node_get_value(current);
    if ((!descending && num <= curr_val) || (descending && num >= curr_val)) {
      break;
    }
    prev = current;
    current = list_next(current);
  }

  if (prev == NULL) {
    list_add_front(l, new_node);
  } else {
    list_insert_after(l, new_node, prev);
  }
}

// Apply combine option: add adjacent pairs
void combine_pairs(struct list *l) {
  struct node *current = list_head(l);
  struct list *new_list = list_init();

  while (current != NULL && list_next(current) != NULL) {
    int sum =
        list_node_get_value(current) + list_node_get_value(list_next(current));
    insert_sorted(new_list, sum, 0);
    current = list_next(list_next(current));
  }

  if (current != NULL) {
    insert_sorted(new_list, list_node_get_value(current), 0);
  }

  // Copy new_list to l
  struct node *n = list_head(l);
  while (n != NULL) {
    struct node *next = list_next(n);
    list_unlink_node(l, n);
    list_free_node(n);
    n = next;
  }

  n = list_head(new_list);
  while (n != NULL) {
    struct node *next = list_next(n);
    list_unlink_node(new_list, n);
    list_add_back(l, n);
    n = next;
  }

  list_cleanup(new_list);
}

// Remove odd numbers from the list
void remove_odd_numbers(struct list *l) {
  struct node *current = list_head(l);
  while (current != NULL) {
    struct node *next = list_next(current);
    if (list_node_get_value(current) % 2 != 0) {
      list_unlink_node(l, current);
      list_free_node(current);
    }
    current = next;
  }
}

// Zip alternating elements from first and second half
void zip_alternating(struct list *l) {
  size_t len = list_length(l);
  if (len <= 1) return;

  size_t mid = (len + 1) / 2;
  struct node *mid_node = list_get_ith(l, mid - 1);
  struct list *second_half = list_cut_after(l, mid_node);

  if (!second_half) return;

  struct node *first = list_head(l);
  struct node *second = list_head(second_half);
  struct list *result = list_init();

  while (first != NULL || second != NULL) {
    if (first != NULL) {
      struct node *next_first = list_next(first);
      list_unlink_node(l, first);
      list_add_back(result, first);
      first = next_first;
    }
    if (second != NULL) {
      struct node *next_second = list_next(second);
      list_unlink_node(second_half, second);
      list_add_back(result, second);
      second = next_second;
    }
  }

  // Copy result back to l
  first = list_head(result);
  while (first != NULL) {
    struct node *next = list_next(first);
    list_unlink_node(result, first);
    list_add_back(l, first);
    first = next;
  }

  list_cleanup(second_half);
  list_cleanup(result);
}

int main(int argc, char *argv[]) {
  struct config cfg;
  if (parse_options(&cfg, argc, argv) != 0) {
    return 1;
  }

  struct list *l = list_init();
  if (!l) {
    return 1;
  }

  char *token;
  while (fgets(buf, BUF_SIZE, stdin)) {
    token = strtok(buf, " \t\n");
    while (token != NULL) {
      char *endptr;
      long num = strtol(token, &endptr, 10);
      if (*endptr == '\0' && num >= 0) {
        insert_sorted(l, (int)num, cfg.descending_order);
      }
      token = strtok(NULL, " \t\n");
    }
  }

  if (cfg.combine) combine_pairs(l);
  if (cfg.remove_odd) remove_odd_numbers(l);
  if (cfg.zip_alternating) zip_alternating(l);

  struct node *current = list_head(l);
  while (current != NULL) {
    printf("%d\n", list_node_get_value(current));
    current = list_next(current);
  }

  list_cleanup(l);
  return 0;
}
