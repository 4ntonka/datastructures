#include "tree.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* C files can be modified anywhere.
 * So you can change or remove these structs to suit your needs. */
struct tree {
  struct node *root;
};

struct node {
  int data;
  struct node *lhs;
  struct node *rhs;
  int height;
};
typedef struct node node;

/* Unique id's for numbering nodes in dot format. */
static int global_node_counter = 0;

/* Helper function: Allocate a new tree node and initialise it with
 * the given parameters. Return a pointer to the new node or NULL on
 * failure. */
static node *make_node(int data) {
  node *new_node = malloc(sizeof(node));
  if (!new_node) {
    return NULL;
  }
  new_node->data = data;
  new_node->lhs = NULL;
  new_node->rhs = NULL;
  return new_node;
}

static int print_tree_dot_r(node *root, FILE *dotf) {
  int left_id, right_id, my_id = global_node_counter++;
  if (root == NULL) {
    fprintf(dotf, "    %d [shape=point];\n", my_id);
    return my_id;
  }
  fprintf(dotf, "    %d [color=%s label=\"%d\"]\n", my_id, "black", root->data);
  left_id = print_tree_dot_r(root->lhs, dotf);
  fprintf(dotf, "    %d -> %d [label=\"l\"]\n", my_id, left_id);
  right_id = print_tree_dot_r(root->rhs, dotf);
  fprintf(dotf, "    %d -> %d [label=\"r\"]\n", my_id, right_id);
  return my_id;
}

void tree_dot(const struct tree *tree, const char *filename) {
  node *root = tree->root;
  global_node_counter = 0;
  FILE *dotf = fopen(filename, "w");
  if (!dotf) {
    printf("error opening file: %s\n", filename);
    exit(1);
  }
  fprintf(dotf, "digraph {\n");
  if (root) {
    print_tree_dot_r(root, dotf);
  }
  fprintf(dotf, "}\n");
  fclose(dotf);
}

int tree_check(const struct tree *tree) {
  if (!tree) {
    return -1;
  }
  if (!tree->root) {
    return 0;
  }
  // Check BST property recursively
  node *current = tree->root;
  if (current->lhs && current->lhs->data >= current->data) {
    return -1;
  }
  if (current->rhs && current->rhs->data <= current->data) {
    return -1;
  }
  // Check subtrees recursively
  if (current->lhs &&
      tree_check_node(current->lhs, INT_MIN, current->data) != 0) {
    return -1;
  }
  if (current->rhs &&
      tree_check_node(current->rhs, current->data, INT_MAX) != 0) {
    return -1;
  }

  return 0;
}

struct tree *tree_init(int turbo) {
  struct tree *t = malloc(sizeof(struct tree));
  if (!t) {
    return NULL;
  }
  t->root = NULL;
  return t;
}

int tree_insert(struct tree *tree, int data) {
  node *new_node = make_node(data);
  if (!new_node) {
    return 0;
  }
  if (!tree->root) {
    tree->root = new_node;
    return 1;
  }
  node *current = tree->root;
  while (1) {
    if (data < current->data) {
      if (!current->lhs) {
        current->lhs = new_node;
        return 1;
      }
      current = current->lhs;
    } else if (data > current->data) {
      if (!current->rhs) {
        current->rhs = new_node;
        return 1;
      }
      current = current->rhs;
    } else {
      // Duplicate value, free the new node and return failure
      free(new_node);
      return 0;
    }
  }
}

int tree_find(struct tree *tree, int data) {
  if (!tree || !tree->root) {
    return 0;
  }
  node *current = tree->root;
  while (current) {
    if (data < current->data) {
      current = current->lhs;
    } else if (data > current->data) {
      current = current->rhs;
    } else {
      return 1;  // Found the value
    }
  }
  return 0;  // Value not found
}

static node *find_min(node *root) {
  while (root->lhs) {
    root = root->lhs;
  }
  return root;
}

static node *remove_node(node *root, int data, int *success) {
  if (!root) {
    *success = 0;
    return NULL;
  }

  if (data < root->data) {
    root->lhs = remove_node(root->lhs, data, success);
  } else if (data > root->data) {
    root->rhs = remove_node(root->rhs, data, success);
  } else {
    *success = 1;
    if (!root->lhs) {
      node *temp = root->rhs;
      free(root);
      return temp;
    } else if (!root->rhs) {
      node *temp = root->lhs;
      free(root);
      return temp;
    }

    node *temp = find_min(root->rhs);
    root->data = temp->data;
    root->rhs = remove_node(root->rhs, temp->data, success);
  }
  return root;
}

int tree_remove(struct tree *tree, int data) {
  if (!tree) {
    return 0;
  }
  int success = 0;
  tree->root = remove_node(tree->root, data, &success);
  return success;
}

static void print_inorder(node *root) {
  if (!root) {
    return;
  }
  print_inorder(root->lhs);
  printf("%d ", root->data);
  print_inorder(root->rhs);
}

void tree_print(const struct tree *tree) {
  if (!tree || !tree->root) {
    printf("Empty tree\n");
    return;
  }
  print_inorder(tree->root);
  printf("\n");
}

static void cleanup_node(node *root) {
  if (!root) {
    return;
  }
  cleanup_node(root->lhs);
  cleanup_node(root->rhs);
  free(root);
}

void tree_cleanup(struct tree *tree) {
  if (!tree) {
    return;
  }
  cleanup_node(tree->root);
  free(tree);
}
