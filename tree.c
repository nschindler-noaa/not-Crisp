/** \file
 *
 *	\brief Implementation of a generic binary tree.
 *
 * This tree also maintains
 *	a doubly linked list of its entries to ease walking the tree.
 *	In addition, the concept of a "current" tree element is maintained,
 *	in order to allow programmatic traversal of the tree external to
 *	this package and ease certain package operations.  This particular
 *	tree does not allow insertion of multiple equivalent-valued items.
 *
 *	The tree is designed to work with pointers as its data values.
 *	It may also work for some <= sizeof(ptr) data values, but
 *	assuming that it will always work for such data values is
 *	dangerous.
 */

#include <stdio.h>
#include <stdlib.h>
#include "tree.h"
#include "strtools.h"
#include "unused.h"

/**
 * \brief The default comparison routine between two data
 *	values.
 *
 * It is a simple integer comparison returning 1 (left leaf)
 *	if val2 is greater than val1, 0 if val2 is equal to val1, and -1
 *	if val2 is less than val1.
 */

static int default_comparison(val1, val2)
	data_type val1, val2;
{
    if ((long) val2 < (long) val1) return -1;
    else return ((long) val2 > (long)val1);
}

/**
 * \brief The simple value way to free data_type value.  (Do nothing.)
 */

static void dont_free(data_type UNUSED(value))
{
    ;
}

/**
 * \brief The simple pointer way to free data_type value.  (Just free it.)
 */
static void ptr_free(value)
	data_type value;
{
    free(value);
}

/**
 * \brief The routine to initialize the tree package.
 
 * Returns a tree
 *	pointer.  The parameters are compare, a routine to compare two data
 *	and datum_free, the routine to free a datum.  Defaults are provided
 *	if these function pointers are null.
 *
 *	The compare function should return 0 if value2 is computationally
 *	equivalent to value1, less than zero (typically -1) if value2 is
 *	computationally less than value1 and greater than zero (typically 1)
 *	if value2 is computationally greater than value1.
 *
 *	The datum_free function should free all associated data within datum.
 *
 *	The value_is_ptr parameter tells whether the tree_value is a pointer
 *	to a value or simply a value.
 *
 *	datum_size is the size of the datum if it is a value, or the size
 *	of the value pointed to, if it is a pointer.
 *
 *	This is getting unwieldy.  A better method (parameter list) would be
 *	nice.
 */

TREE *tree_init(compare, datum_free, value_is_ptr, datum_size)
	int (*compare)();
	void (*datum_free)();
	char value_is_ptr;
	unsigned int datum_size;
{
    TREE *ret_tree;

    if (!(ret_tree = (TREE *) malloc(sizeof(TREE))))
	return (TREE *) 0;
    bzero(ret_tree, sizeof(TREE));
    if (compare) ret_tree->comparison = compare;
    else ret_tree->comparison = default_comparison;
    if (!datum_free) ret_tree->datum_free = (value_is_ptr) ? ptr_free :
	dont_free;
    else ret_tree->datum_free = datum_free;
    ret_tree->is_ptr = value_is_ptr;
    ret_tree->size = datum_size;
    return ret_tree;
}

/**
 * \brief  Reset the current element pointer.
 */
void tree_reset(tree)
	TREE *tree;
{
    tree->current = tree->first;
}

/**
 * \brief  Increment the current element pointer,
 *	setting it to NULL if there are no more elements.
 */
void tree_increment(tree)
	TREE *tree;
{
    if (!tree) return;

    tree->current = (tree->current == tree->last || !tree->current) ? 0
	: tree->current->next;
}

/**
 * \brief  Returns the data pointer
 *	for the current tree node.
 */
data_type get_current_tree_datum(tree)
	TREE *tree;
{
    return (tree && tree->current) ? tree->current->datum : 0;
}

/**
 * \brief  Returns the number of elements in the
 *	tree.
 */
int get_tree_size(tree)
	TREE *tree;
{
    return (tree) ? tree->count : 0;
}

/**
 * \brief  Returns an integer
 *	corresponding to the number of the current tree node, with
 *	0 as the first (comparatively least) node and tree->count - 1
 *	as the last (comparatively greatest) node.
 */
int tree_current_item_number(tree)
	TREE *tree;
{
    int i;
    struct tree_node *node;

    if (!(tree && tree->current)) return -1;
    for (i = 0, node = tree->first ; node ; node = node->next, ++i) {
	if ((*tree->comparison)(node->datum, tree->current->datum) == 0) break;
    }
    return (node) ? i : -1;
}

/**
 * \brief  The routine to deallocate a tree structure.
 */
void tree_free(tree)
	TREE *tree;
{
    struct tree_node *node, *old;

    if (!(tree && tree->first)) return;
    for (node=tree->first ; node ; old=node, node=old->next , free(old))
	(*tree->datum_free)(node->datum);
    free(tree);
}

/**
 * \brief allocates a node for the tree passed in
 */
struct tree_node *node_alloc()
{
    struct tree_node *node;

    if (!(node = (struct tree_node *) malloc(sizeof(struct tree_node))))
	return (struct tree_node *) 0;
    bzero(node, sizeof(struct tree_node));
    return node;
}

/**
 * \brief frees the node of the tree passed in
 */
void node_free(tree, node)
	TREE *tree;
	struct tree_node *node;
{
    if (!(tree && node)) return;
    tree->datum_free(node->datum);
    free(node);
}

/** store the parent for the last node_element_exists check,
 * in case we need to use it.
 */
static struct tree_node *exist_parent = 0;

/**
 * \brief  Internal only function, which actually
 *	traverses the tree
 */
static struct tree_node *node_element_exists(tree, node, datum)
	TREE *tree;
	struct tree_node *node;
	data_type datum;
{
    int comp_result;

    if ((comp_result = (*tree->comparison)(node->datum, datum)) == 0) {
	return node;
    } else {
	exist_parent = node;
	if (comp_result < 0) {
	    if (node->left)
		return node_element_exists(tree, node->left, datum);
	} else {
	    if (node->right)
		return node_element_exists(tree, node->right, datum);
	}
    }
    /* No match and no element to compare against, match does not exist */
    exist_parent = 0;
    return 0;
}

/**
 * \brief  Returns true
 *	if an element comparatively equal to datum exists in tree, false
 *	otherwise.
 */
int tree_element_exists(tree, datum)
	TREE *tree;
	data_type datum;
{
    if (!(tree && tree->root)) return 0;
    return (tree->current = node_element_exists(tree, tree->root, datum)) != 0;
}

/**
 * \brief  Recursive function to insert node in tree starting from root.
 */

static void node_insert(tree, root, node)
	TREE *tree;
	struct tree_node *root;
	struct tree_node *node;
{
    if ((*tree->comparison)(root->datum, node->datum) < 0) {
	if (root->left) node_insert(tree, root->left, node);
	else {
	    if (root->prev) {
		root->prev->next = node;
		node->prev = root->prev;
	    } else tree->first = node;
	    root->prev = node;
	    root->left = node;
	    node->next = root;
	}
    } else {
	if (root->right) node_insert(tree, root->right, node);
	else {
	    if (root->next) {
		root->next->prev = node;
		node->next = root->next;
	    } else tree->last = node;
	    root->next = node;
	    root->right = node;
	    node->prev = root;
	}
    }
    tree->current = node;
}

/**
 * \brief  Insert an item with value value into tree tree
 *	Returns -1 on error, 0 if a new tree was created, 1 on
 *	normal insert.  The tree's current pointer is automatically
 *	set to the last element inserted.
 */
int tree_insert(tree, value)
	TREE *tree;
	data_type value;
{
    struct tree_node *new_node;

    if (!tree) return -1;
    if (!(new_node = node_alloc())) return -1;
    new_node->datum = value;
    ++tree->count;
    if (!tree->root) {
	tree->current = tree->first = tree->last = tree->root = new_node;
	return 0;
    }
    node_insert(tree, tree->root, new_node);
    return 1;
}

#ifdef NEEDED
/**
 * \brief  Returns the node
 *	which is the parent of its parameter.
 *
 */
static struct tree_node *find_parent(node)
	struct tree_node *node;
{
}
#endif

/**
 * \brief  replace
 *	the specified node with the an appropriate child, moving nodes
 *	so that a valid binary tree results.
 *
 *	Returns the node which takes the spot of the original node in
 *	the tree.
 */
static struct tree_node *tree_replace_node(node)
	struct tree_node *node;
{
    /* Use the right node by default, not worrying about balance */
    struct tree_node *replacement = node->right;

    /* No right child of node, so no work required. */
    if (!replacement) return node->left;

    /* If left child of node exists, we need to make sure it doesn't
     * get lost in the shuffle.
     */
    if (node->left) {

	/* If left child of replacement exists, we have to make sure that
	 * it *also* doesn't get lost in the shuffle.
	 */
	if (replacement->left) {
	    struct tree_node *new_left_tree_home = node->left;

	    while (new_left_tree_home->right)
		new_left_tree_home = new_left_tree_home->right;

	    new_left_tree_home->right = replacement->left;
	}

	replacement->left = node->left;
    }

    return replacement;
}

/**
 * \brief  Remove the node matching
 *	value from the tree, if any such matches exist.
 */
int tree_remove(tree, value)
	TREE *tree;
	data_type value;
{
    struct tree_node *current;
    struct tree_node *parent;

    if (!tree) return 0;

    if (!(current = node_element_exists(tree, tree->root, value)))
	return 0;

    --tree->count;

    if (current->next)
	current->next->prev = current->prev;
    else
	tree->last = current->prev;

    if (current->prev)
	current->prev->next = current->next;
    else
	tree->first = current->next;
    /* current is now removed from the embedded list. */

    if (current == tree->root)
	tree->root = tree_replace_node(current);
    else {
    /* exist_parent is a static stored from the node_element_exists search */
	parent = exist_parent;

	if (parent->left == current)
	    parent->left = tree_replace_node(current);
	else
	    parent->right = tree_replace_node(current);
    }

    if (current == tree->current)
	tree->current = 0;

    /* current is now removed from references within tree, and can be
     * safely freed */

    node_free(tree, current);
    return 1;
}

/**
 * \brief  Create a malloc'ed array of the tree
 *	data.
 *
 *	WARNING: The bcopy may not work for tree->size < sizeof(datum_type),
 *	depending on byte order.
 */

data_type tree_array(tree)
	TREE *tree;
{
    char *array;
    struct tree_node *step;
    unsigned int i;

    if (!tree->count) return (data_type) 0;
    if (!(array = (char *) malloc(tree->count * tree->size)))
	return (data_type) 0;
    if (!tree->is_ptr)
	for (step=tree->first, i=0 ;
	     step && i<tree->count ;
	     step=step->next, i++) {
	    bcopy(&step->datum, array+i*tree->size, tree->size);
	}
    else
	for (step=tree->first, i=0 ;
	     step && i<tree->count ;
	     step=step->next, i++) {
	    bcopy(step->datum, array+i*tree->size, tree->size);
	}
    if (i < tree->count) {
	printf("Bogus programming!\n");
	abort();
    }
    return (data_type) array;
}

