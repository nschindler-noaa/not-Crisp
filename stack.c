/** \file
 * \brief Stack implementation. */

#include <stdio.h>
#include <stdlib.h>
#include "stack.h"
#include "strtools.h"

void free_stack(stack)
	STACK **stack;
{
    while (*stack)
	free_stack(&(*stack)->next);
    free(stack);
}

void push(stack, value)
	STACK **stack;
	int	value;
{
    STACK *new_entry;

    new_entry = (STACK *) malloc(sizeof(STACK));
    bzero(new_entry, sizeof(STACK));
    new_entry->value = value;
    new_entry->next = *stack;
    *stack = new_entry;
}

int pop(stack)
	STACK **stack;
{
    STACK *top_item;
    int ret_val;
   
    if (!stack) {
	fprintf(stderr, "stack underflow\n");
	return 0;
    }

    top_item = (*stack);

    *stack = (*stack)->next;
    ret_val = top_item->value;
    free(top_item);
    return ret_val;
}

/** To be used only with a stack on unique values.. */
void remove_item(stack, value)
	STACK **stack;
    	int value;
{
    STACK **cur, *goner;

    for (cur = stack; cur && *cur; cur = &((*cur)->next))
	if ((*cur)->value == value) {
	    goner = *cur;
	    *cur = (*cur)->next;
	    free(goner);
	}
}
