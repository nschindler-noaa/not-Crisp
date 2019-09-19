#include "test.h"
#include "tree.h"
#include "crisp.h"

/* This is not a very complete test, it's more for testing the testing framework */

int compare_ints(int a, int b) {
	return a < b;
}
void int_free() {
}

int main(int argc, char *argv) {
	TREE *int_tree;

	test_start("Binary tree of ints");

	test_category("Creation");
	int_tree = tree_init(&compare_ints, &int_free, FALSE, sizeof(int));
	assert(int_tree != NULL);

	test_category("Size, Insertion, Deletion");
	assert(get_tree_size(int_tree) == 0);
	tree_insert(int_tree, (data_type)1);
	assert(get_tree_size(int_tree) == 1);
	tree_remove(int_tree, (data_type)1);
	assert(get_tree_size(int_tree) == 0);

	test_category("Deallocation");
	tree_free(int_tree);

	return test_pass();
}

