#include "test.h"
#include <stdlib.h>
#include <stdio.h>

/* Default test name */
const char *name = "unnamed";
/* Default test category */
const char *category = "default";

void test_start(const char *_name) {
	name = _name;
}
void test_category(const char *_category) {
	category = _category;
}

static void fail() {
	fprintf(stderr, "FAILED:\t%s (%s)\n", name, category);
	exit(TEST_FAIL);
}
void assert(int expr) {
	if (!expr) fail();
}
static int test_equal(double expected, double observed) {
	double error = expected - observed;
	double denom = (expected > observed ? expected : observed);
	error = error / ((denom > 0 || denom < 0) ? denom : 1.0);
	if (error < 0) error = 0.0 - error;

	return error <= TEST_MAX_ERROR;
}
void assert_equal(double expected, double observed) {
	if (!test_equal(expected, observed)) {
		printf("ERROR:\tExpected %0.8e but got %0.8e\n", expected, observed);

		fail();
	}
}
void assert_not_equal(double expected, double observed) {
	if (test_equal(expected, observed)) {
		printf("ERROR:\tExpected anything but %0.8e but got %0.8e\n", expected, observed);

		fail();
	}
}
void assert_less_than(double expected, double observed) {
	double min = (observed - expected);
	double denom = (expected > observed ? expected : observed);
	min /= (denom > 0 || denom < 0) ? denom : 1.0;
	min += TEST_MAX_ERROR;

	if (min < 0) {
		printf("ERROR:\tExpected value < %0.8e but got %0.8e\n", expected, observed);

		fail();
	}
}

int test_pass() {
	fprintf(stderr, "PASSED:\t%s\n", name);
	return TEST_OK;
}

