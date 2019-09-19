#include "test.h"

int main(int argc, char *argv) {
	test_start("Assertions");
	assert(1);
	assert(-1);
	assert(!0);
	assert('a');
	assert(1.0f);
	assert(1.0);
	assert(1 == 1);
	return test_pass();
}
