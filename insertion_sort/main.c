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
 * The function:
 *   1. Creates a new node containing the number
 *   2. Traverses the list to find the correct insertion position by comparing
 *      values
 *   3. Inserts the new node either:
 *      a) At the front if it belongs before all existing nodes based on sort
 *         order
 *      b) After the appropriate node to maintain ascending/descending order
 *   4. Handles error cases like failed node creation gracefully
 *   5. Maintains the sorted property of the list after insertion */
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
 * The function:
 *   1. Creates a new temporary list to store the combined values
 *   2. Processes the input list by:
 *      a) Taking pairs of adjacent nodes (i.e. nodes at positions 0&1, 2&3,
 *         etc)
 *      b) Adding their values together c) Inserting the sum into the temporary
 *         list
 *   3. If there's an odd number of nodes, the last unpaired node is added as-is
 *   4. Cleans up the original list by unlinking and freeing all nodes
 *   5. Moves all nodes from the temporary list back to the original list
 *   6. Cleans up the temporary list structure
 *
 * Example:
 * Input list:  1 -> 2 -> 3 -> 4 -> 5
 * Step 2:      3 (1+2) -> 7 (3+4) -> 5
 * Final list:  3 -> 5 -> 7. */
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
    insert_sorted(new_list, sum, 0);
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
 * The function:
 *   1. Iterates through each node in the list
 *   2. For each node:
 *      a) Gets its value and checks if it's odd (not divisible by 2)
 *      b) If odd: Unlinks the node from the list and frees its memory
 *      c) If even: Leaves the node in place and continues
 *   3. Updates list structure (size, head/tail pointers) via unlink operations
 *
 * Example:
 * Input list:  1 -> 2 -> 3 -> 4 -> 5
 * Output list: 2 -> 4 */
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
 * The function:
 *   1. Calculates length and finds midpoint of list
 *   2. Splits list into two halves at midpoint using list_cut_after()
 *   3. Creates temporary result list to store reordered elements
 *   4. Alternately takes elements from first and second half:
 *      a) Takes first element from first half, adds to result
 *      b) Takes first element from second half, adds to result
 *      c) Repeats until both halves are empty
 *   5. Moves all elements from result back to original list
 *   6. Cleans up temporary lists
 *
 * Example:
 * Input list:  1 -> 2 -> 3 -> 4 -> 5 -> 6
 * After split: (1 -> 2 -> 3) and (4 -> 5 -> 6)
 * Output list: 1 -> 4 -> 2 -> 5 -> 3 -> 6 */
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
 * Returns:
 *   0 on successful execution
 *   1 if initialization fails or invalid options are provided. */
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
