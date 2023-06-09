/** \file
 * \brief Implementation of statistical functions. */

#include <math.h>
#include <string.h>
#include <time.h>
#ifdef WIN32
#include <stdio.h>
/* TODO handle this better someday. */
#pragma warning (disable: 4244)
#endif
#include "log.h"
#include "sample.h"

/* minimum standard linear congruential from Park and Miller,
 * slightly optimized to eliminate extra local variables and to
 * substitute mults for divs.  much faster than drand48().
 */

static double dseed = 1;

#define R_A 16807.0
#define R_M 2147483647.0
#define R_Q 127773.0
#define R_R 2836.0
#define R_IM (1.0 / R_M)
#define R_IQ (1.0 / R_Q)

static double drand()
{
    double hi;

    hi = floor(dseed * R_IQ);
    dseed = R_A * (dseed - R_Q * hi) - R_R * hi;
    if (dseed < 0)
	dseed += R_M;
    return (dseed * R_IM);
}
    
/** seed the random number generator from the clock */

void init_seed()
{
    dseed = (double) time(0); 
}


/** return a sample from the normal distribution, from "Numerical Recipes" */

double normal_sample()
{
        static int iset = 0;
        static double gset;
        register double r, v1, v2, fac;

        if (iset == 0){
           do {
               v1 = 2.0 * drand() - 1.0;
               v2 = 2.0 * drand() - 1.0;
               r = v1*v1 + v2*v2;
              } while (r >= 1.0);
           fac = sqrt(-2.0 * log(r)/r);
           gset = v1 * fac;
           iset = 1;
           return v2*fac;
        } else {
           iset = 0;
           return gset;
        }
}


/** return a sample from the uniform distribution.
 * cv is the coefficient of variation, defined as
 *	cv = stdv/mean
 * and confined to the range 0 <= cv <= 1/sqrt(3)
 * values out of range give undefined results
 */

double uniform_sample(mean, cv)
    register    double	mean;
    register    double	cv;
{
    static int set_flag = 0;
    static double sqrt3;
    static double sqrt12;
    register double a, b;

    if (!set_flag)
    {
	sqrt3 = sqrt(3.0);
	sqrt12 = sqrt(12.0);
	set_flag = 1;
    }

    a = mean * (1.0 - sqrt3 * cv);
    b = sqrt12 * mean * cv;

    return(a + b * drand());
}

    

#define x_mid	0.50
/** \brief "A probability density function for a stochastic variable can be
 * described by a broken stick cumulative distribution."  
 *
 * The #defs for the cumulative distribution shaping factors are
 * provided here for easy reference (only one at present).
 *
 * NOTE: As an optimization, the range check to insure that
 *	low <= mean <= high
 * was removed.  All params must be checked in the routine
 * stochastic_param_check() before every model run.
 *
 * NOTE: this routine has a macro interface for speed, broken_stick()
 */

double broken_stick_sample(mean, low, high)
    double mean;
    double low;
    double high;
{
    register double x;
    register double median;

    median = 2.0*mean - (low + high)/2.0;
    x = drand();
    
    if (x <= x_mid)
    {
	return(low + (x * (median - low) / x_mid));
    }
    else 
    {
	return(median + ((x - x_mid) * (high - median) / x_mid));
    }
}

/** 
 *	Utility routine to implement a stochastic parameter check.
 *	checks to see if the low, mean, and high make sense, and if the
 *	mean falls within the acceptable range for computation of the median
 *	as required by the broken stick calculation.
 *
 *	also returns an error message in caller supplied variable.
 * 
 *	Return:	0 no error
 *		1 error
 */

int param_error(float low, float mean, float high, char* msg)
{
    if (low == high)
	return 0;
    else if (low > mean || mean > high)
    {
	strncpy(msg, "range must be low <= mean <= high", 256);
	return 1;
    }
    else
    {
	register double median;

	median = 2.0*mean - (low + high)/2.0;
	if (median - low < 0.0 || high - median < 0.0)
	{
	    float q1 = (high - low)/4 + low;
	    float q3 = 3*(high - low)/4 + low;
	    sprintf(msg, "mean %f not in range %f <= mean <= %f", mean, q1, q3);
	    return 1;
	}
	else return 0;
    }
}

