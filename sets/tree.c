/*
 * Name : A. Smirnov
 * UvAnetID : 13272225
 * Study : BSc Informatica
 *
 * This is the low-level implementation of a binary search tree data structure
 * and its operations and is used by set.c.
 */

#include "tree.h"

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

  /* ... SOME CODE MISSING HERE ... */
};
typedef struct node node;

/* Unique id's for numbering nodes in dot format. */
static int global_node_counter = 0;

/* Helper function: Allocate a new tree node and initialise it with
 * the given parameters. Return a pointer to the new node or NULL on
 * failure. */
static node *make_node(int data) {
  node *new_node = (node *)malloc(sizeof(node));
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
  /* ... OPTIONALLY IMPLEMENT TREE CHECKING HERE ... */
  return 0;
}

struct tree *tree_init(int turbo) {
  struct tree *new_tree = (struct tree *)malloc(sizeof(struct tree));
  if (!new_tree) {
    return NULL;
  }
  new_tree->root = NULL;
  return new_tree;
}

int tree_insert(struct tree *tree, int num) {
  node **current = &(tree->root);
  while (*current) {
    if (num < (*current)->data) {
      current = &((*current)->lhs);
    } else if (num > (*current)->data) {
      current = &((*current)->rhs);
    } else {
      return 1;  // Already exists
    }
  }
  *current = make_node(num);
  return *current ? 0 : -1;  // Success or failure
}

int tree_find(struct tree *tree, int num) {
  node *current = tree->root;
  while (current) {
    if (num < current->data) {
      current = current->lhs;
    } else if (num > current->data) {
      current = current->rhs;
    } else {
      return 1;  // Found
    }
  }
  return 0;  // Not found
}

int tree_remove(struct tree *tree, int num) {
  node **current = &(tree->root);
  while (*current) {
    if (num < (*current)->data) {
      current = &((*current)->lhs);
    } else if (num > (*current)->data) {
      current = &((*current)->rhs);
    } else {
      // Node found, handle removal
      node *to_remove = *current;
      if (!to_remove->lhs) {
        *current = to_remove->rhs;
      } else if (!to_remove->rhs) {
        *current = to_remove->lhs;
      } else {
        // Node with two children: find the inorder successor
        node **successor = &(to_remove->rhs);
        while ((*successor)->lhs) {
          successor = &((*successor)->lhs);
        }
        to_remove->data = (*successor)->data;
        to_remove = *successor;
        *successor = (*successor)->rhs;
      }
      free(to_remove);
      return 0;  // Success
    }
  }
  return 1;  // Not found
}

void inorder_print(node *n) {
  if (!n) return;
  inorder_print(n->lhs);
  printf("%d\n", n->data);
  inorder_print(n->rhs);
}

void tree_print(const struct tree *tree) { inorder_print(tree->root); }

void free_nodes(node *n) {
  if (!n) return;
  free_nodes(n->lhs);
  free_nodes(n->rhs);
  free(n);
}

void tree_cleanup(struct tree *tree) {
  free_nodes(tree->root);
  free(tree);
}
