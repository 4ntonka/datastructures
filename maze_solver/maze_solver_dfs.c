#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "maze.h"
#include "stack.h"

#define NOT_FOUND -1
#define ERROR -2
#define MAX_STACK_SIZE 40000

/* Solves the maze m.
 * Returns the length of the path if a path is found.
 * Returns NOT_FOUND if no path is found and ERROR if an error occured.
 */
int dfs_solve(struct maze *m) {
  if (m == NULL) {
    return ERROR;
  }

  struct stack *s = stack_init(MAX_STACK_SIZE);
  if (s == NULL) {
    return ERROR;
  }

  // Get start position and push it to stack
  int start_r, start_c;
  maze_start(m, &start_r, &start_c);
  stack_push(s, maze_index(m, start_r, start_c));

  int path_length = 0;
  bool found = false;

  while (!stack_empty(s)) {
    int current = stack_peek(s);
    int r = maze_row(m, current);
    int c = maze_col(m, current);

    if (maze_at_destination(m, r, c)) {
      found = true;
      break;
    }

    // Try to find an unvisited neighbor
    bool has_unvisited = false;
    for (int i = 0; i < N_MOVES; i++) {
      int new_r = r + m_offsets[i][0];
      int new_c = c + m_offsets[i][1];

      if (maze_valid_move(m, new_r, new_c)) {
        char cell = maze_get(m, new_r, new_c);
        if (cell == FLOOR || maze_at_destination(m, new_r, new_c)) {
          stack_push(s, maze_index(m, new_r, new_c));
          if (!maze_at_destination(m, new_r, new_c)) {
            maze_set(m, new_r, new_c, VISITED);
          }
          has_unvisited = true;
          break;
        }
      }
    }

    // If no unvisited neighbors, backtrack
    if (!has_unvisited) {
      stack_pop(s);
      maze_set(m, r, c, VISITED);
    }
  }

  // If path found, mark it and count length
  if (found) {
    while (!stack_empty(s)) {
      int index = stack_pop(s);
      int r = maze_row(m, index);
      int c = maze_col(m, index);
      if (!maze_at_start(m, r, c) && !maze_at_destination(m, r, c)) {
        maze_set(m, r, c, PATH);
      }
      path_length++;
    }
    path_length--;  // Don't count the start position
  }

  stack_cleanup(s);
  return found ? path_length : NOT_FOUND;
}

int main(void) {
  /* read maze */
  struct maze *m = maze_read();
  if (!m) {
    printf("Error reading maze\n");
    return 1;
  }

  /* solve maze */
  int path_length = dfs_solve(m);
  if (path_length == ERROR) {
    printf("dfs failed\n");
    maze_cleanup(m);
    return 1;
  } else if (path_length == NOT_FOUND) {
    printf("no path found from start to destination\n");
    maze_cleanup(m);
    return 1;
  }
  printf("dfs found a path of length: %d\n", path_length);

  /* print maze */
  maze_print(m, false);
  maze_output_ppm(m, "out.ppm");

  maze_cleanup(m);
  return 0;
}
