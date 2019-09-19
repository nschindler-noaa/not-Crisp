#include "test.h"
#include "tree.h"
#include "crisp.h"
#include <stdlib.h>
#include <math.h>

#define NUM 1000

long compare_longs(long a, long b) {
	return a < b;
}
void long_free() {
}

int main(int argc, char *argv) {
	TREE *long_tree;
	long i;

	test_start("Binary tree of longs");

	test_category("Initialization");
	long_tree = tree_init(&compare_longs, &long_free, FALSE, sizeof(long));
	assert(long_tree != NULL);
	assert(get_tree_size(long_tree) == 0);
	test_category("Insertions");
	for (i=0; i < NUM; i++) {
		tree_insert(long_tree, (data_type)i);
	}
	assert(get_tree_size(long_tree) == NUM);

	test_category("Removals");
	/* TODO */

	test_category("Deallocation");
	tree_free(long_tree);

	return test_pass();
}


