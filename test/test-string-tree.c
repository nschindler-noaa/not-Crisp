#include "test.h"
#include "tree.h"
#include "crisp.h"
#include <string.h>

int main(int argc, char *argv) {
	test_start("Binary tree of strings");

	test_category("Creation");
	TREE *string_tree = tree_init(&strcmp, &free, TRUE, sizeof(char*));
	assert(string_tree != NULL);

	
	test_category("Initial size");
	assert(get_tree_size(string_tree) == 0);

	test_category("Adding single element");
	tree_insert(string_tree, strdup("testa"));
	assert(get_tree_size(string_tree) == 1);

	test_category("Removal");
	tree_remove(string_tree, "testa");
	assert(get_tree_size(string_tree) == 0);

	test_category("Redundant removals");
	tree_remove(string_tree, "testa");
	tree_remove(string_tree, "testa");
	tree_remove(string_tree, "testa");
	assert(get_tree_size(string_tree) == 0);

	test_category("Adding multiple elements");
	tree_insert(string_tree, strdup("testa"));
	tree_insert(string_tree, strdup("testb"));
	tree_insert(string_tree, strdup("testc"));
	tree_insert(string_tree, strdup("test"));
	tree_insert(string_tree, strdup("tes"));
	assert(get_tree_size(string_tree) == 5);

	test_category("Removal");
	tree_remove(string_tree, "tes");
	assert(get_tree_size(string_tree) == 4);
	tree_remove(string_tree, "tes");
	tree_remove(string_tree, "tes");
	tree_remove(string_tree, "tes");
	assert(get_tree_size(string_tree) == 4);
	tree_remove(string_tree, "testa");
	assert(get_tree_size(string_tree) == 3);
	tree_remove(string_tree, "testa");
	tree_remove(string_tree, "testa");
	tree_remove(string_tree, "testa");
	assert(get_tree_size(string_tree) == 3);

	test_category("Deallocation");
	tree_free(string_tree);

	return test_pass();
}


