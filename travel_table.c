/** \file
 * \brief implementation of a travel table using the vectable
 * module.
 */

#include <malloc.h>
#include <math.h>
#include "crisp_defs.h"
#include "vectable.h"
#include "strtools.h"
#include "travel_table.h"



/** static definition of the bins, containing initializations of the
 * upper bounds and slotsizes only, null-terminated with empty bin.
 * The tt_calib_mode bins create a very small travel table so that 
 * travel times are always calculated rather than looked up.
 */

static struct bin i_bins[] = {
    {0,   1.0,  0.25, 0},
    {0,   5.0,  0.5,  0},
    {0,  10.0,  1.0,  0},
    {0,  20.0,  2.0,  0},
    {0,  50.0,  5.0,  0},
    {0, 100.0, 10.0,  0},
    {0,   0,    0,    0}
};

static struct bin i_bins_tt_calib_mode[] = {
    {0,   0.1,  0.1, 0},
    {0,   0,    0,    0}
};

static struct bin j_bins[] = {
    {0,   1.0,  0.25, 0},
    {0,   5.0,  0.5,  0},
    {0,  10.0,  1.0,  0},
    {0,  20.0,  2.0,  0},
    {0,  50.0,  5.0,  0},
    {0, 100.0, 10.0,  0},
    {0,   0,    0,    0}
};

static struct bin j_bins_tt_calib_mode[] = {
    {0,   0.1,  0.1, 0},
    {0,   0,    0,    0}
};

/*********************************************************************/
/* INTERNAL MODULE ROUTINES 					     */
/*********************************************************************/

#ifdef WIN32
/** The PC doesn't have erf() or erfc() in it's math libraries. So this
 *  routine is provided. Copied from Numerical Recipes in C, 2nd ed, p 221
 */
/* TODO implement this properly someday. */
double erfc( double x )
{
    double t, z, ans;
    z = fabs( x );
    t = 1.0 / (1.0 + 0.5 * z);
    ans = t * exp( -z*z - 1.26551223 + t * (1.00002368 + t * (0.37409196 +
           t * (0.09678418 + t * (-0.18628806 + t * (0.27886807 +
           t * (-1.13520398 + t * (1.48851587 + t * (-0.82215223 +
           t * 0.17087277)))))))));
    return x >= 0.0 ? ans : 2.0 - ans;
}

double erf( double x )
{
    return 1.0 - erfc( x );
}
#endif



/**  Travel time algorithm -- computes the number of fish arriving
 *  during a given time interval.  allocates the vector and returns it.
 *  parameters are mu, lambda, and default vector length to
 *  generate.
 */

static double *arrival(mu, lam, dflt_len)
        double   mu,lam; /* model parameters */
	int dflt_len;
{
        double   	t;
        double   *ptr;
        double   first, second;
        double  y, z;
        double  fac1, fac2;
        double   d0 = 0.2316419;
        double   d1 = 0.319381530;
        double   d2 = -0.356563782;
        double   d3 = 1.781477937;
        double   d4 = -1.821255978;
        double   d5 = 1.330274429;
        double   qz;
	int vsize;
	double sqrt_2pi;
    double mu_sqrt_t;

	static double *start=0;
	static int vlen = 0;

	sqrt_2pi = sqrt(2*M_PI);

	/* always keep a local vector of adequate size */
	if (!start || dflt_len > vlen) {
	    if (start)
		free(start);
	    vlen = dflt_len;
	    start = (double *) malloc ((dflt_len+2) * sizeof (double));
	    
	    if (!start)
		return 0;
	}
    ptr = start;
    *ptr = 0;

	/* t corresponds to timesteps, as does dflt_len.  t is incremented
	 * by 1 each iteration in filling out the vector.
	 */
    t = 0;
    while(*ptr < 0.999 && t < dflt_len) {
        ++ptr;
		t+=1;
        mu_sqrt_t = mu * sqrt( t );
        y = lam*(t-mu)/mu_sqrt_t;
        z = lam*(t+mu)/mu_sqrt_t;
 
        if (z<4){
            qz = 1/(1+d0*z);
            fac1 = (exp(-(y*y)/2))/(sqrt_2pi);
            fac2 = qz * (d1 + qz * (d2 + qz * (d3 + qz * (d4 + qz * d5))));
        }
        else /* if (z>=4) */ {
            qz = 1/(z*z);
            fac1 = (exp(-(y*y)/2))/(sqrt_2pi)/z;
            fac2 = 1 + qz * (-1 + qz * (3 + qz * (-3*5 + qz * (3*5*7
                      + qz * (-3*5*7*9 + qz * (3*5*7*9*11 
                      - qz * 3*5*7*9*11*13))))));
        }
        second = fac1*fac2;
        first = phi(y);
        *ptr = first + second;
    }
	++ptr;
    *ptr = 1.0;

    /* Since the 0'th element of the vector is zero by definition, and
     *  furthermore is never accessed by the consumers of this data, we
     *  will use it to store the length of the significant part of the
     *  vector, for optimization purposes. */
    *start = ((ptr - start) + 1);

	/* return a vector of the necessary size */
    vsize = ((ptr - start) + 1) * sizeof(double);
    ptr = (double *) malloc(vsize);
	if (!ptr)
	    return 0;
	bcopy((char *) start, (char *) ptr, vsize);

	return ptr;
}

/** travel table vector create func.  just an interface routine for
 * portability.
 */

void *tvec_create(mu, lam)
    double mu, lam;
{
    return (void *) arrival(mu, lam, STEPS_IN_RUN);
}

/*********************************************************************/
/* EXTERNAL ROUTINES (user callable) 				     */
/*********************************************************************/

/** the travel table */

static struct vec_table *travel = 0;

/** set up the table if not already.
 * return 0 on error, travel table ptr on success.
 */

struct vec_table *init_travel_table()
{
    if (!travel) {
/* we don't need to scale the bins, and we get a tighter table without
 * it.  eventually, we should initialize the bins to more appropriate
 * values than currently used.
 *
 *      scale_bins(i_bins, (double) STEPS_PER_DAY);
 *	scale_bins(j_bins, sqrt((double) STEPS_PER_DAY));
 */
        /* Here, we create a very small travel table if in tt calib mode.  This is 
        necessary for memory management */
        if (tt_calib_mode)
			travel = create_vector_table(i_bins_tt_calib_mode, 0.0, 
			                    j_bins_tt_calib_mode, 0.0, tvec_create);
		else
			travel = create_vector_table(i_bins, 0.0, j_bins, 0.0, tvec_create);
    }
    return travel;
}

void destroy_travel_table()
{
    if (travel) {
	destroy_vector_table(travel);
	travel = 0;
    }
}

