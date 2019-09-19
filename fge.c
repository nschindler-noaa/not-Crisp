#ifdef WIN32
#include <stdlib.h>
#endif

#include <math.h>
#include <stdio.h>

#include "crisp.h"

#include "log.h"
#include "math.h"

#define _USE_MATH_DEFINES
/*
 * float compute_fge(float fge0, equation *eqn, float age, float elev)
 *	compute the fge from the given fge, equation, fish age since
 *	smoltification in days, and drawdown (elev >= 0).
 */

#define LOG_HALF (-0.693)		/* log(0.5) */

float compute_fge(fge0, eqn, age, elev)
    float fge0;
    equation *eqn;
    float age;
    float elev;
{
    register double x, z;
    register double z0 = eqn->param[0].val;
    register double z1 = eqn->param[1].val;
    register double t0 = eqn->param[2].val;
    register double dt = eqn->param[3].val;


    if (age <= t0)
	z = z0;
    else if (age < dt + t0)
	z = z0 + (z1 - z0) * (age - t0) / dt;
    else
	z = z1;
    
    x = (z0 * (fge0 < 1.0 ? log(1 - fge0) : -999999.9999) / LOG_HALF) - elev;
	//x = (z0 * (fge0 < 1.0 ? log(1 - fge0) : -(HUGE_VAL) ) / LOG_HALF) - elev;

    return((x < 0) ? 0.0 : 1.0 - exp(LOG_HALF * x / z));
}

float compute_fge_new(eqn, X1, X2, X3, X4)
    equation *eqn;
	int X1;    //Day vs Night
    float X2; // Flow through powerhouse
    int X3;   // Day
	float X4; // Temp
{
    register double expterm;
    register double B0 = eqn->param[0].val;
    register double B1 = eqn->param[1].val;
    register double B2 = eqn->param[2].val;
    register double B3 = eqn->param[3].val;
	register double B4 = eqn->param[4].val;

    expterm = exp(B0 + (B1*X1) + (B2*X2) + (B3*X3) + (B4*X4));
    return (expterm/(1+expterm));
}

/*
 * float compute_fge_eqn_val(float age, equation *eqn, int range)
 *	computes the value for the fge equation display.
 *	the range field specifies whether
 *	to return a value representing the mean, low, or high.  values for
 *	these are 0, 1, and 2, respectively.
 */

#ifdef __cplusplus
extern "C" {
#endif

float compute_fge_eqn_val(float age, equation *eqn, int dm, int spc, int range)
{
    struct river_segment *seg = phouses[dm];
    struct dam_species *dmspc = &seg->current->dam.species[spc];
    float fge0 = dmspc->night_fge;
    float *ddwn;
    int date;
    int plot_var = 1;

	variable_parameter *night_fge_dist = get_variable_parameter(&dmspc->night_fge);

    if (night_fge_dist && night_fge_dist->parameters.broken_stick.low == night_fge_dist->parameters.broken_stick.high)
	plot_var = 0;
    
	if (night_fge_dist) {
    switch (range)
    {
    case 0:
	fge0 = night_fge_dist->parameters.broken_stick.mean;
	break;
    case 1:
	if (!plot_var)
	    return(-1);
	else
	    fge0 = night_fge_dist->parameters.broken_stick.low;
	break;
    case 2:
	if (!plot_var)
	    return(-1);
	else
	    fge0 = night_fge_dist->parameters.broken_stick.high;
	break;
    }
	}
	/* else fge0 = dmspc->night_fge; (this is its original assignment, see above) */

    /* we need to find the dam itself, which is above a secondary phouse */
    if (seg->seg_type == POWERHOUSE_SEG)
	seg = phouses[dm-1];

    /* account for bypass elevation */
    date = (int) (age + eqn->param[4].val);
    if (date >= DAYS_IN_SEASON)
	date = DAYS_IN_SEASON - 1;

    if (seg->current->dam.forebay_depth[date] < seg->current->dam.bypass_height)
	    return(0.0);
    else if (!(current->flags & AGE_DEP_FGE_FLAG))
    	return(100 * fge0);
    else
    {
	    if (seg->up->seg_type == HEADWATER_SEG)
	        ddwn = seg->up->current->headwater.elev_change;
	    else 
	        ddwn = seg->up->current->reach.elev_change;

	    return(100 * compute_fge(fge0, eqn, age, -ddwn[date]));
    }
}

#ifdef __cplusplus
}
#endif

