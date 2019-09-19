#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "crisp.h"

#include "log.h"
#include "probability.h"

/* routine to allocate a display table for trans equations.  also select the
 * initial equations to display.  returns a pointer to the table, or null if
 * failure.
 */

/*
 * float compute_trans_mort(float tt, equation *eqn, float base_mort)
 *	compute the mort from the given trans mort equation and base mort.
 */


float compute_trans_mort(tt, eqn, base_mort)
    float tt;
    equation *eqn;
    float base_mort;
{
    double tt1 = eqn->param[0].val;
    double m1  = eqn->param[1].val + base_mort;
    double tt2 = eqn->param[2].val;
    double m2  = eqn->param[3].val + base_mort;

    BOUND_PROB(m1);
    BOUND_PROB(m2);
    
    if (tt <= tt1)
	return m1;
    else if (tt > tt2)
	return m2;
    else
	return (m1 + (tt - tt1) * ((m2 - m1)/(tt2 - tt1)));
    
}

