/** \file 
 * \brief simple macro to bound a probability. */

/** simple macro to bound a probability.
 *
 * it is a single statement,
 * with the caller expected to supply the trailing semicolon.  the
 * paramater is a variable whose value should be bounded to lie between
 * 0 and 1.
 */

#define BOUND_PROB(x)			\
do { if (x<0) x=0; else if (x>1) x=1; } while (0)
