/* Do not edit this file. */
#include <stddef.h>

/* Handle to stack */
struct stack;

/* Return a pointer to a stack data structure with a maximum capacity of
 * 'capacity' if successful, otherwise return NULL. */
struct stack *stack_init(size_t capacity);

/* Cleanup stack. */
void stack_cleanup(struct stack *s);

/* Print stack statistics to stderr.
 * The format is: 'stats' num_of_pushes num_of_pops max_elements */
void stack_stats(const struct stack *s);

/* Push item onto the stack.
 * Return 0 if successful, 1 otherwise. */
int stack_push(struct stack *s, int c);

/* Pop item from stack and return it.
 * Return top item if successful, -1 otherwise. */
int stack_pop(struct stack *s);

/* Return top of item from stack. Leave stack unchanged.
 * Return top item if successful, -1 otherwise. */
int stack_peek(const struct stack *s);

/* Return 1 if stack is empty, 0 if the stack contains any elements and
 * return -1 if the operation fails. */
int stack_empty(const struct stack *s);

/* Return the number of elements stored in the stack. */
size_t stack_size(const struct stack *s);
