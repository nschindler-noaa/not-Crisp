#include "test.h"

int main(int argc, char *argv) {
	test_start("Floating point comparisons");

	test_category("Floating point equality");
	assert_equal(1.0, 1.0);
	test_category("Floating point inequality");
	assert_not_equal(1.0, 1.0 + 0.001);
	assert_not_equal(1.0, 0.0);
	assert_not_equal(0.0, 1.0);
	assert_not_equal(0.0, 0.0000000001);
	test_category("Floating point equality with error");
	assert_equal(1.0, 1.0 + TEST_MAX_ERROR/2);
	assert_equal(1.0, 1.0 - TEST_MAX_ERROR/2);
	assert_equal(1.0 + TEST_MAX_ERROR/2, 1.0);
	assert_equal(1.0 - TEST_MAX_ERROR/2, 1.0);

	test_category("Floating point inequality");
	assert_less_than(0.0, 1.0);
	assert_less_than(0.9, 1.0);
	assert_less_than(0.9999, 1.0);
	test_category("Floating point inequality with error");
	assert_less_than(1.0, 1.0 + TEST_MAX_ERROR*2);
	assert_less_than(1.0 - TEST_MAX_ERROR*2, 1.0);
	assert_less_than(0.9 + TEST_MAX_ERROR/2, 1.0);
	assert_less_than(0.9, 1.0 - TEST_MAX_ERROR/2);

	return test_pass();
}
