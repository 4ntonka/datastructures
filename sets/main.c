/*
 * Name : A. Smirnov
 * UvAnetID : 13272225
 * Study : BSc Informatica
 *
 * File main.c implements user interface. It handles user input commands ('+'
 * for insert, '-' for remove, '?' for find, 'p' for print) and uses the Set
 * API (from set.c) to perform the requested operations
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "set.h"

#define BUF_SIZE 256

void cleanup_and_fail(char *buf, struct set *s) {
  if (s) {
    set_cleanup(s);
  }
  free(buf);
  exit(EXIT_FAILURE);
}

int main(void) {
  char *buf = malloc(BUF_SIZE);
  if (!buf) {
    perror("Could not allocate input buffer");
    return EXIT_FAILURE;
  }
  struct set *s = set_init(0); /* initialize set with turbo turned off. */

  if (!s) {
    printf("Could not initialize set\n");
    free(buf);
    return EXIT_FAILURE;
  }

  while (fgets(buf, BUF_SIZE, stdin)) {
    char *endptr;
    char *command;
    char *num_str;
    int num;

    command = strtok(buf, " ");
    if (strchr("+-?", *command)) {
      num_str = strtok(NULL, "\n");
      if (!num_str) {
        printf("set operation '%c' requires integer operand\n", *command);
        cleanup_and_fail(buf, s);
      }
      num = (int)strtol(num_str, &endptr, 10);
      if (*endptr != '\0') {
        printf("error converting '%s' to an integer\n", num_str);
        cleanup_and_fail(buf, s);
      }
    }
    switch (*command) {
      case '+':
        if (set_insert(s, num) == 1) {
        }
        break;
      case '-':
        if (set_remove(s, num) == 1) {
        }
        break;
      case '?':
        if (set_find(s, num)) {
          printf("found: %d\n", num);
        } else {
          printf("not found: %d\n", num);
        }
        break;
      case 'p':
        set_print(s);
        break;
      default:
        printf("Unknown command '%c'\n", *command);
        cleanup_and_fail(buf, s);
    }
  }
  if (set_verify(s)) {
    fprintf(stderr, "Set implementation failed verification!\n");
  }
  free(buf);
  set_cleanup(s);
  return EXIT_SUCCESS;
}
