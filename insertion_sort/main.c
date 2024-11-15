/*
 * Name : A. Smirnov
 * UvAnetID : 13272225
 * Study : BSc Informatica
 *
 * This program implements an insertion sort algorithm using a linked list.
 * It reads integers from standard input and inserts them into a sorted list.
 * Additional options modify the output:
 *   -d: Sort in descending order instead of ascending
 *   -c: Combine adjacent pairs of numbers by adding them
 *   -o: Remove odd numbers from the final output
 *   -z: Zip alternating elements from first/second half of list
 *
 * Please note: this program requires a sorted list to work correctly provided
 * in list.c and list.h files.
 *
 * Example usage:
 * $ make clean && make && echo "4 2 3 1" | ./mysort
 * 1
 * 2
 * 3
 * 4
 */

// TODO sideeffcts

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

/* Parses the command-line options and sets the corresponding flags in the
 * config structure. */
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

/* Inserts a number into a sorted linked list while maintaining the sort order.
 *
 * Arguments:
 *   a) l: Pointer to the input linked list to process. Must be already sorted.
 *   b) num: The integer value to insert into the list
 *   c) descending: Flag controlling sort order, 0 = ascending, 1 = descending.
 *
 * Expected behavior:
 *   a) If the list is empty, the new node is added as the only node in the
 * list. b) If the new node's value is less than or equal to the head node's
 * value (when descending is 0) or greater than or equal to the head node's
 * value (when descending is 1), the new node is added at the front of the list.
 */
void insert_sorted(struct list *l, int num, int descending) {
  struct node *new_node = list_new_node(num);
  if (!new_node) return;
  if (list_head(l) == NULL ||
      (!descending && num <= list_node_get_value(list_head(l))) ||
      (descending && num >= list_node_get_value(list_head(l)))) {
    list_add_front(l, new_node);
    return;
  }
  struct node *current = list_head(l);
  while (current != NULL && list_next(current) != NULL) {
    int next_val = list_node_get_value(list_next(current));
    if ((!descending && num <= next_val) || (descending && num >= next_val)) {
      break;
    }
    current = list_next(current);
  }
  list_insert_after(l, new_node, current);
}

/* Combines adjacent pairs of numbers in a linked list by adding them together.
 *
 * Arguments:
 *   l: Pointer to the input linked list to process.
 *
 * Expected behavior:
 *   a) If the list is empty, the function does nothing.
 *   b) If the list has an odd number of nodes, the last node is added as-is to
 *      the new list.
 */
void combine_pairs(struct list *l) {
  if (l == NULL || list_head(l) == NULL) {
    return;
  }
  struct list *new_list = list_init();
  if (new_list == NULL) {
    return;
  }
  struct node *current = list_head(l);
  while (current != NULL) {
    int sum;
    if (list_next(current) != NULL) {
      sum = list_node_get_value(current) +
            list_node_get_value(list_next(current));
      current = list_next(list_next(current));
    } else {
      sum = list_node_get_value(current);
      current = NULL;
    }
    struct node *new_node = list_new_node(sum);
    if (new_node != NULL) {
      list_add_back(new_list, new_node);
    }
  }
  struct node *old_head = list_head(l);
  while (old_head != NULL) {
    struct node *next = list_next(old_head);
    list_unlink_node(l, old_head);
    list_free_node(old_head);
    old_head = next;
  }
  struct node *new_head = list_head(new_list);
  while (new_head != NULL) {
    struct node *next = list_next(new_head);
    list_unlink_node(new_list, new_head);
    list_add_back(l, new_head);
    new_head = next;
  }
  list_cleanup(new_list);
}

/* Removes all odd numbers from a linked list, modifying it in place.
 *
 * Arguments:
 *   l: Pointer to the input linked list to process.
 *
 * Expected behavior:
 *   a) If the list is empty, the function does nothing.
 *   b) If the list has no odd numbers, the list remains unchanged.
 *   c) If the list has odd numbers, they are removed and the remaining nodes
 * are reconnected in a continuous sequence.
 */
void remove_odd_numbers(struct list *l) {
  if (l == NULL) {
    return;
  }
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

/* Rearranges elements in a linked list by alternating between elements from
 * the first and second half of the list.
 *
 * Arguments:
 *   l: Pointer to the input linked list to process.
 *
 * Expected behavior:
 *   a) If the list is empty, the function does nothing.
 *   b) If the list has no odd numbers, the list remains unchanged.
 */
void zip_alternating(struct list *l) {
  if (l == NULL) {
    return;
  }
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

/* Main function that processes command line arguments and performs operations
 * on a linked list.
 *
 * Arguments:
 *   a) argc: Number of command line arguments
 *   b) argv: Array of command line argument strings
 *
 * Expected behavior:
 *   a) If the command line arguments are invalid, the program prints an error
 *      message and exits with status 1.
 *   b) The program initializes a linked list, reads integers from standard
 *      input, and inserts them into the list in sorted order.
 */
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
