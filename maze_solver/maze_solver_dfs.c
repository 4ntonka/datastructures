/*
 * Name : A. Smirnov
 * UvAnetID : 13272225
 * Study : BSc Informatica
 *
 * This program solves a maze using Depth-First Search (DFS) and uses a stack to
 * keep track of the path.
 */

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

  // Initialize stack for DFS with maximum size
  struct stack *s = stack_init(MAX_STACK_SIZE);
  if (s == NULL) {
    return ERROR;
  }

  // Get starting position coordinates and add to stack
  int start_r, start_c;
  maze_start(m, &start_r, &start_c);
  stack_push(s, maze_index(m, start_r, start_c));

  // Variables to track path length and if destination is found
  int path_length = 0;
  bool found = false;

  // Main DFS loop - continues while stack is not empty
  while (!stack_empty(s)) {
    // Look at current cell on top of stack
    int current = stack_peek(s);
    int r = maze_row(m, current);
    int c = maze_col(m, current);

    // If we reached the destination, mark as found and exit loop
    if (maze_at_destination(m, r, c)) {
      found = true;
      break;
    }

    // Try to find any unvisited neighbor cell
    bool has_unvisited = false;
    // Check all 4 directions (up, right, down, left)
    for (int i = 0; i < N_MOVES; i++) {
      // Calculate coordinates of neighbor cell
      int new_r = r + m_offsets[i][0];
      int new_c = c + m_offsets[i][1];

      // If move is valid (within bounds and not a wall)
      if (maze_valid_move(m, new_r, new_c)) {
        char cell = maze_get(m, new_r, new_c);
        // If cell is unvisited floor or destination
        if (cell == FLOOR || maze_at_destination(m, new_r, new_c)) {
          // Add neighbor to stack
          stack_push(s, maze_index(m, new_r, new_c));
          // Mark as visited unless it's the destination
          if (!maze_at_destination(m, new_r, new_c)) {
            maze_set(m, new_r, new_c, VISITED);
          }
          has_unvisited = true;
          break;
        }
      }
    }

    // If no unvisited neighbors found, backtrack by removing current cell
    if (!has_unvisited) {
      stack_pop(s);
      maze_set(m, r, c, VISITED);
    }
  }

  // If path was found, mark path and calculate its length
  if (found) {
    // Pop each cell from stack and mark it as part of path
    while (!stack_empty(s)) {
      int index = stack_pop(s);
      int r = maze_row(m, index);
      int c = maze_col(m, index);
      if (!maze_at_start(m, r, c) && !maze_at_destination(m, r, c)) {
        maze_set(m, r, c, PATH);
      }
      path_length++;
    }
    path_length--;
  }

  // Clean up stack and return result
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
