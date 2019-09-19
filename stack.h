/** \file
 * \brief Stack implementation. */
 

#ifndef NULL
#define NULL 0
#endif

typedef struct stack STACK;

struct stack {
    STACK *next;
    int	value;
};

extern void free_stack(/* STACK **stack */);
extern void push(/* STACK **stack, int value*/);
extern int  pop(/* STACK **stack*/);
extern void remove_item(/* STACK **stack */);

