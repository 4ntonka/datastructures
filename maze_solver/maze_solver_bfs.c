#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "maze.h"
#include "queue.h"

#define NOT_FOUND -1
#define ERROR -2
#define MAX_STACK_SIZE 40000

/* Solves the maze m.
 * Returns the length of the path if a path is found.
 * Returns NOT_FOUND if no path is found and ERROR if an error occured.
 */
int bfs_solve(struct maze *m) {
  if (m == NULL) {
    return ERROR;
  }

  // Initialize queue and parent array
  struct queue *q = queue_init((size_t)(maze_size(m) * maze_size(m)));
  int *parent = calloc((size_t)(maze_size(m) * maze_size(m)), sizeof(int));
  if (q == NULL || parent == NULL) {
    free(parent);
    queue_cleanup(q);
    return ERROR;
  }

  // Get start position and add it to queue
  int start_r, start_c;
  maze_start(m, &start_r, &start_c);
  int start_idx = maze_index(m, start_r, start_c);
  queue_push(q, start_idx);
  parent[start_idx] = start_idx;

  bool found = false;
  int dest_idx = -1;

  // BFS main loop
  while (!queue_empty(q)) {
    int current = queue_pop(q);
    int r = maze_row(m, current);
    int c = maze_col(m, current);

    if (maze_at_destination(m, r, c)) {
      found = true;
      dest_idx = current;
      break;
    }

    // Check all neighbors
    for (int i = 0; i < N_MOVES; i++) {
      int new_r = r + m_offsets[i][0];
      int new_c = c + m_offsets[i][1];

      if (maze_valid_move(m, new_r, new_c)) {
        char cell = maze_get(m, new_r, new_c);
        if (cell == FLOOR || maze_at_destination(m, new_r, new_c)) {
          int new_idx = maze_index(m, new_r, new_c);
          queue_push(q, new_idx);
          parent[new_idx] = current;
          if (!maze_at_destination(m, new_r, new_c)) {
            maze_set(m, new_r, new_c, VISITED);
          }
        }
      }
    }
  }

  // If path found, reconstruct it from destination to start
  int path_length = 0;
  if (found) {
    int current = dest_idx;
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

  // Cleanup
  free(parent);
  queue_cleanup(q);

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
