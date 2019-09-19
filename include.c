#include <stdio.h>
#include <stdlib.h>
#include "log.h"
#include "include.h"
#include "parser_file.h"

/* define the stack type */
struct buf_stack
{
    void *buffer;
    struct buf_stack *next;
};

static struct buf_stack *stack = 0;

unsigned char can_pop_buf()
{
    return (stack ? 1 : 0);
}

void *pop_buf()
{
    struct buf_stack *old;
    void *buffer;

    if (!stack)
    {
	log_msg(L_DBGMSG,
		"I can't pop the buffer off the stack because %s\n%s\n",
		"there ain't no buffers there.", "Bozo alert!");
	return((FILE *) 0);
    }

    old=stack;
    stack=stack->next;

    buffer = old->buffer;
    free (old);

    return(buffer);
}

void push_buf(buffer)
	void *buffer;
{
    struct buf_stack *new =
	(struct buf_stack *)malloc(sizeof(struct buf_stack));

    if (!new) /* malloc error */
    {
	log_msg(L_ERROR, "Error allocating memory for buffer on stack.\n");
	return;
    }
    
    new->next = stack;
    new->buffer = buffer;
    stack = new;
    return;
    
}

