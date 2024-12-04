#include <fcntl.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "prioq.h"

#define BUF_SIZE 1024

static char buf[BUF_SIZE];

struct config {
  /* You can ignore these options until/unless you implement the */
  /* bonus features. */

  /* Set to 1 if -y is specified, 0 otherwise. */
  int year;
};

static int parse_options(struct config *cfg, int argc, char *argv[]);

typedef struct {
  char *name;
  int age;
  int duration;
} patient_t;

static int compare_patient_name(const void *a, const void *b) {
  return strcmp(((const patient_t *)a)->name, ((const patient_t *)b)->name);
}

static int compare_patient_age(const void *a, const void *b) {
  const patient_t *p1 = (const patient_t *)a;
  const patient_t *p2 = (const patient_t *)b;

  if (p1->age != p2->age) {
    return p1->age - p2->age;  // Sort by age first
  }
  return strcmp(p1->name, p2->name);  // If ages equal, sort by name
}

int main(int argc, char *argv[]) {
  char *token, *name_cpy;
  prioq *queue;
  struct config cfg;

  if (parse_options(&cfg, argc, argv) != 0) {
    return EXIT_FAILURE;
  }

  if (cfg.year) {
    queue = prioq_init(&compare_patient_age);
  } else {
    queue = prioq_init(&compare_patient_name);
  }

  if (!queue) {
    return EXIT_FAILURE;
  }

  for (int iterations = 0;;) {
    while (1) {
      char *s = fgets(buf, BUF_SIZE, stdin);
      if (s == NULL) {
        fprintf(stderr, "Unexpected end of file. exiting\n");
        prioq_cleanup(queue, free);
        return EXIT_FAILURE;
      }

      // Remove newline if present
      size_t len = strlen(buf);
      if (len > 0 && buf[len - 1] == '\n') {
        buf[len - 1] = '\0';
      }

      // Check for end of input period
      if (buf[0] == '.') {
        break;
      }

      // Parse input line
      patient_t *patient = malloc(sizeof(patient_t));
      if (!patient) {
        prioq_cleanup(queue, free);
        return EXIT_FAILURE;
      }

      // Get name
      token = strtok(buf, " ");
      if (!token) {
        free(patient);
        prioq_cleanup(queue, free);
        return EXIT_FAILURE;
      }

      name_cpy = strdup(token);
      if (!name_cpy) {
        free(patient);
        prioq_cleanup(queue, free);
        return EXIT_FAILURE;
      }
      patient->name = name_cpy;

      // Get age
      token = strtok(NULL, " ");
      if (!token) {
        free(patient->name);
        free(patient);
        prioq_cleanup(queue, free);
        return EXIT_FAILURE;
      }
      patient->age = atoi(token);

      // Get duration if present
      token = strtok(NULL, " ");
      patient->duration = token ? atoi(token) : 1;

      if (prioq_insert(queue, patient) != 0) {
        free(patient->name);
        free(patient);
        prioq_cleanup(queue, free);
        return EXIT_FAILURE;
      }
    }

    // Process one patient if queue not empty
    patient_t *next = prioq_pop(queue);
    if (next) {
      printf("%s\n", next->name);
      free(next->name);
      free(next);
    } else {
      printf(".\n");
    }

    if (++iterations == 10) {
      // End of day - process remaining patients
      while ((next = prioq_pop(queue))) {
        printf("%s\n", next->name);
        free(next->name);
        free(next);
      }
      break;
    }
  }

  prioq_cleanup(queue, NULL);

  return EXIT_SUCCESS;
}

int parse_options(struct config *cfg, int argc, char *argv[]) {
  memset(cfg, 0, sizeof(struct config));
  int c;
  while ((c = getopt(argc, argv, "y")) != -1) {
    switch (c) {
      case 'y':
        cfg->year = 1;
        break;
      default:
        fprintf(stderr, "invalid option: -%c\n", optopt);
        return 1;
    }
  }
  return 0;
}
