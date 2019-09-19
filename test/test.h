/** \file
 * \brief Test framework header.
 *
 * This file defines the testing framework. It is a very simple framework that consists of categorized assertion tests. */

/**

\mainpage
This is a self-contained testing framework for testing C programs. It is not tied in any way to a single project and should be portable while remaining very simple.

See test.h for the available functions.

\section c Creating Test Cases
The Makefile assumes that all files matching test-*.c are test cases and it attempts to build and run them.

After creating a file like test-example.c the form of the test case is very simple and may look like the following:
\code
#include "test.h"

int main(int argc, char *argv) {
	test_start("Floating point comparisons");

	test_category("Assertion test");
	assert(1 == 1);
	assert(1 + 1 == 2);

	test_category("Floating point equality");
	assert_equal(1.0, 1.0);
	assert_equal(0.0, 0.0);

	test_category("Floating point inequality");
	assert_not_equal(1.0, 0.0);
	assert_not_equal(0.0, 1.0);
	assert_not_equal(1.0, 1.0 + 0.001);
	assert_not_equal(0.0, 0.0000000001);

	return test_pass();
}
\endcode

If the test case needs to use external objects, then simply set the foreign source directory (SRC_FOREIGN in the Makefile) to the directory and list the objects in EXTRA_FOREIGN. Local code may also be linked in to test cases using EXTRA_LOCAL.

Note that all foreign objects get linked into every test case. This could lead to large test executables but it keeps the framework very simple.

\section o Output
When the Makefile is run the tests will automatically be run and an error will be generated if an assertion fails or a test crashes.

The results of the run are appended to a file in the directory called results.txt in a tab-separated form. The format is as follows:

\code
<TIMESTAMP>	<SUCCESSES>	<FAILURES>
\endcode

- TIMESTAMP is the number of seconds since 12:00 a.m. January 1st, 1970 
- SUCCESSES is the number of successful tests run
- FAILURES is the number of failed tests.

\section v Visualization

Using the results.txt output the results can be visualized. Here is an example script for R:

\code
x <- scan("results.txt", list(0,0,0))
pdf("ratio.pdf")
plot(x[[1]], x[[2]]/(x[[2]]+x[[3]]), type='l', main="Success vs. Time", xlab="Seconds in Epoch", ylab="Success Ratio")
dev.off()
pdf("success.pdf")
plot(x[[1]], x[[2]], type='l', main="Successes vs. Time", xlab="Seconds in Epoch", ylab="Number of Tests Passed")
dev.off()
pdf("coverage.pdf")
plot(x[[1]], x[[2]] + x[[3]], type='l', main="Coverage vs. Time", xlab="Seconds in Epoch", ylab="Total Number of Tests")
dev.off()
\endcode

 */

#ifndef	__TEST_H
#define	__TEST_H

#ifdef __cplusplus
extern "C" {
#endif

/** The test passed */
#define TEST_OK 0

/** The test failed */
#define TEST_FAIL 1

/** Set the name of the test fixture
 * \param name The name of the test fixture */
extern void test_start(const char *name);

/** Start a named category within the test fixture (to help identify failures).
 * \param section The name of the category of tests */
extern void test_category(const char *section);

/** Check a condition, if the value is false (i.e. 0) then the failure will be reported.
 * \param expr The expression to be evaluated. */
extern void assert(int expr);

/** How much error is allowed in checking floating point comparisons. This is
 * given in a fraction of the largest number in the comparison. */
#define TEST_MAX_ERROR 0.00001

/** Check to see if the two floating point numbers are equal within the error
 * bound (the bound is a fraction of the largest input).
 * \param expected The expected value.
 * \param observed The observed value. */
extern void assert_equal(double expected, double observed);

/** Check to make sure that two floating point numbers are not equal (excluding error).
 * \param expected The expected value.
 * \param observed The observed value. */
extern void assert_not_equal(double expected, double observed);

/** Check to see if floating point number a is less than b within error,
 * \param a The value that should be lesser.
 * \param b The value which should be greater. */
extern void assert_less_than(double a, double b);

/** Return this function at the end to signify that the test fixture has been passed. */
extern int test_pass();

#ifdef __cplusplus
}
#endif

#endif
