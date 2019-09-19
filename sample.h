/** \file 
 * \brief  macro definition of sample() which resamples from a normal distrubution
 * until a value within range is found.
 */

#include <math.h>
#include "crisp.h"
#include "run_state.h"

extern double normal_sample();
extern double uniform_sample();
extern double broken_stick_sample();
extern int param_error(float low, float mean, float high, char* msg);

#define sample(rslt, mean, var) 					\
    /* float *rslt; float mean; float var; */				\
    									\
    while ( (*(rslt) = (mean) + sqrt(var) * normal() / 10.0) < 0.0	\
	   || *(rslt) > 1.0)		/* empty body */

/* no trailing ; (user supplies) */


/* the following routines are interfaces to the actual sampling routines.
 * initial checks are performed here for speed.
 */

#ifdef NOVAR

#define normal()							\
    ((current->flags & VAR_SUPPRESS_FLAG) ? 0.0 : normal_sample())

#define uniform(mean, cv) 						\
    ((current->flags & VAR_SUPPRESS_FLAG)				\
     ? (mean) : uniform_sample((mean), (cv)))

#define broken_stick(mean, low, high)					\
    ((((low) == (high)) || (current->flags & VAR_SUPPRESS_FLAG))	\
     ? (mean) : broken_stick_sample((mean), (low), (high)))

#else

#define normal() (normal_sample())

#define uniform(mean, cv) (uniform_sample((mean), (cv)))

#define broken_stick(mean, low, high)					\
    (((low) == (high)) ? (mean) : broken_stick_sample((mean), (low), (high)))

#endif

