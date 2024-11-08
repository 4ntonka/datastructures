/*
 * Name : A. Smirnov
 * UvAnetID : 13272225
 * Study : BSc Informatica
 *
 * This program solves a maze using Breadth-First Search (BFS) and uses a queue
 * to keep track of the path.
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "maze.h"
#include "queue.h"

#define NOT_FOUND -1
#define ERROR -2

/* Solves the maze m.
 * Returns the length of the path if a path is found.
 * Returns NOT_FOUND if no path is found and ERROR if an error occured.
 */
int bfs_solve(struct maze *m) {
  if (m == NULL) {
    return ERROR;
  }

  // Initialize queue for BFS and array to track each cell's parent
  // Queue size is maze_size squared since we need space for all cells
  struct queue *q = queue_init((size_t)(maze_size(m) * maze_size(m)));
  int *parent = calloc((size_t)(maze_size(m) * maze_size(m)), sizeof(int));

  // Return error if memory allocation failed
  if (q == NULL || parent == NULL) {
    free(parent);
    queue_cleanup(q);
    return ERROR;
  }

  // Get starting position coordinates and convert to 1D index
  int start_r, start_c;
  maze_start(m, &start_r, &start_c);
  int start_idx = maze_index(m, start_r, start_c);

  // Add start position to queue and set its parent to itself
  queue_push(q, start_idx);
  parent[start_idx] = start_idx;

  // Variables to track if destination is found and store destination index
  bool found = false;
  int dest_idx = -1;

  // Main BFS loop - continues while queue is not empty
  while (!queue_empty(q)) {
    // Get next cell from queue
    int current = queue_pop(q);
    int r = maze_row(m, current);
    int c = maze_col(m, current);

    // If current cell is destination, mark as found and exit loop
    if (maze_at_destination(m, r, c)) {
      found = true;
      dest_idx = current;
      break;
    }

    // Check all 4 neighboring cells (up, right, down, left)
    for (int i = 0; i < N_MOVES; i++) {
      // Calculate neighbor coordinates
      int new_r = r + m_offsets[i][0];
      int new_c = c + m_offsets[i][1];

      // If move is valid (within bounds and not a wall)
      if (maze_valid_move(m, new_r, new_c)) {
        char cell = maze_get(m, new_r, new_c);
        // If cell is unvisited floor or destination
        if (cell == FLOOR || maze_at_destination(m, new_r, new_c)) {
          int new_idx = maze_index(m, new_r, new_c);
          // Add neighbor to queue and set its parent
          queue_push(q, new_idx);
          parent[new_idx] = current;
          // Mark as visited unless it's the destination
          if (!maze_at_destination(m, new_r, new_c)) {
            maze_set(m, new_r, new_c, VISITED);
          }
        }
      }
    }
  }

  // If path was found, trace back from destination to start
  int path_length = 0;
  if (found) {
    int current = dest_idx;
    // Follow parent pointers back to start, marking path
    while (current != start_idx) {
      int r = maze_row(m, current);
      int c = maze_col(m, current);
      if (!maze_at_destination(m, r, c)) {
        maze_set(m, r, c, PATH);
      }
      current = parent[current];
      path_length++;
    }
  }

  // Free allocated memory
  free(parent);
  queue_cleanup(q);

  // Return path length if found, NOT_FOUND otherwise
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
  int path_length = bfs_solve(m);
  if (path_length == ERROR) {
    printf("bfs failed\n");
    maze_cleanup(m);
    return 1;
  } else if (path_length == NOT_FOUND) {
    printf("no path found from start to destination\n");
    maze_cleanup(m);
    return 1;
  }
  printf("bfs found a path of length: %d\n", path_length);

  /* print maze */
  maze_print(m, false);
  maze_output_ppm(m, "out.ppm");

  maze_cleanup(m);
  return 0;
}
