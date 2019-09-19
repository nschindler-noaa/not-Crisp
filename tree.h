/** \file
 *	\brief Definitions for the generic binary tree module
 */

#ifndef __TREE_H__
#define __TREE_H__  1

typedef void * data_type;

struct tree_node {
    struct tree_node *left;
    struct tree_node *right;
    struct tree_node *next;
    struct tree_node *prev;
    data_type datum;
};

typedef struct {
    char is_ptr;
    unsigned int size;
    unsigned int count;
    int (*comparison)();
    void (*datum_free)();
    struct tree_node *root;
    struct tree_node *first;
    struct tree_node *last;
    struct tree_node *current;
} TREE;

extern int get_tree_size( TREE *t );
extern int tree_current_item_number( TREE *t );
extern int tree_remove(TREE *tree, data_type value);
extern int tree_insert(TREE *tree, data_type value);
extern TREE *tree_init(int (*compare)(), void (*datum_free)(), char value_is_ptr, unsigned int datum_size);
extern data_type tree_array(TREE *tree);
extern void tree_free(TREE *tree);

#endif /* __TREE_H__ */

