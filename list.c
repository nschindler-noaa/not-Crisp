/* utility routines for list manipulation */

#include <stdlib.h>

#include "list.h"


/** routine to malloc a list entry.  returns the blank entry if successfull,
 * else null.  
 */

LIST *make_list_entry()
{
    LIST *listp;

    if ( ! (listp = (LIST *) malloc(sizeof(LIST))))
	return (NULL);

    listp->next = listp->prev = NULL;
    listp->item = NULL;
    return (listp);
}


/** routine to free an entire list.  takes as parameters the list
 * and a (possibly null) user function to call to do related clean up.
 * the data item is passed as parameter to the function.
 */

void free_list(listp, user_fn)
    LIST *listp;
    void (*user_fn)();
{
    LIST *entry;
    
    while ((entry = listp) != NULL)
    {
	if (user_fn)
	    (*user_fn)(entry->item);
	listp = listp->next;
	free(entry);
    }
}


/** append a node to the end of a list.  takes the address of the head
 * of the list as parameter.  assumes that pointer fields in node are
 * correct (either data or null, as appropriate).
 */

void list_append(headp, node)
    LIST **headp;
    LIST *node;
{
    LIST *last;
    
    if (! *headp) {
	*headp = node;
	return;
    }
    last = *headp;
    while (last->next)
	last = last->next;
    last->next = node;
    node->prev = last;
}

/**
 * insert a node in the list.  parameters are a pointer to the
 * (possibly null) head-of-list pointer, and pointer to new list
 * node.  no guarantees are made about location of insertion;
 * currently prepend is used for convenience.
 */

void list_insert(headp, node)
    LIST **headp;
    LIST *node;
{
    LIST *head = *headp;

    node->next = head;
    node->prev = NULL;
    if (head)
	head->prev = node;
    *headp = node;
}   

/** insert an item into the list.  uses make_list_entry() and list_insert()
 * to construct a new list entry and insert it.  returns a pointer to the
 * list entry, or NULL on failure.
 */

LIST *list_insert_item(headp, item)
    LIST **headp;
    void *item;
{
    LIST *entry = make_list_entry();
    if (entry) {
	entry->item = item;
	list_insert(headp, entry);
    }
    return entry;
}


/**
 * delete a list node.  takes a pointer to the head-of-list pointer,
 * a pointer to the node to delete, and a (possibly null) user fn to
 * do related cleanup.  it is assumed that the node does in fact
 * point to a member of the list - no check is made.
 */

void list_delete(headp, node, delete_fn)
    LIST **headp;
    LIST *node;
    void (*delete_fn)();
{
    if (!node)
	return;
    
    if (node->prev)
	node->prev->next = node->next;
    if (node->next)
	node->next->prev = node->prev;
    if (node == *headp)
	*headp = node->next;

    if (delete_fn)
	(*delete_fn)(node->item);
    free(node);
}

/**
 * routine to search a list for a given item by a simple comparison of
 * the item pointers.  returns a pointer to the list entry if found,
 * otherwise NULL.
 */

LIST *item_in_list(listp, item)
    LIST *listp;
    void *item;
{
    while (listp && listp->item != item)
	listp = listp->next;
    
    return(listp);
}

/**
 * find an item in a list using a user comparison fn.  takes a pointer
 * to the list, the item to search for, and a pointer to the comp fn.
 * comp fn should take two void * pointers and return non-zero on
 * match, 0 on non-match.  this find fn returns a pointer to the list
 * entry if found, otherwise NULL.  search is currently implemented as
 * linear.
 */

LIST *list_find(list, item, comp_fn)
    LIST *list;
    void *item;
    int (*comp_fn)();
{
    while (list) {
	if ((*comp_fn)(item, list->item))
	    return list;
	else
	    list = list->next;
    }
    return NULL;
}
    
