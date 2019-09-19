#include <math.h>
#include <stdio.h>

#ifdef WIN32
#include <stdlib.h>
#endif

#include "crisp.h"

#include "log.h"
#include "flow.h"
#include "compute_gas_mort.h"
#include "exp_table.h"
#include "gas_mort.h"
#include "nsat.h"
#include "fdens.h"
#include "nrutil_short.h"

/* This module assumes that flow, and hence velocity, is greater than
 * zero. If there is no flow, then there is no fish or TDG, and so the
 * mortality due to gas is set to zero.
 */

/* compute the gas mortality vector for a reach. */
void compute_gas_mort_vec(
    double *gas_mort,   	/* result vector */
    struct river_segment *seg,  /* reach characteristics */
    
    GasDistribution *gas,	/* input gas of reach */
    float *Nmix, 		/* mixed input gas vector */
    float *Ndiff, 		/* diff of input gas vectors */
    
    equation	*gmort_eqn,
    equation	*fdens_eqn
    )
{
    struct reach *reachp = &(seg->current->reach);
    ReachGasSpecs rs;
    FDensParams fd;
    GasMortParams gm;

    int i;

    /* make the exponent vector - returns immediately if already set up */
    make_exp_vec();
    
    /* set up all parameters which don't vary by timestep */
    /* reach gas specs */
    rs.length = reachp->length;

    /* fish density parameters */
    fd.zb = fdens_eqn->fd_dbot;
    fd.zm = fdens_eqn->fd_dmode;

    /* gas mort params */
    gm.a = gmort_eqn->gm3_mlow;
    gm.b = gmort_eqn->gm3_mhi;
    gm.ncrit = gmort_eqn->gm3_ncrit;

    /* Compute Reach Mortality */
    /*  To compute reach mortality the reservoir is divided up into
	longitudinal peices according to the initial conditions of the
	water at the beginning of the reach and water travel time.

	Initial conditions at a dam could change potentially every dam
	timestep. So the "influence" of a particular condition is
	velocity[i] over TAU. (If velocity[i] is in miles per day,
	then velocity[i] over TAU is the number of miles the water has
	travelled during timestep i). These divisions are identical
	for the left and right flow.
 
	The algorithm will step through the reservoir calculating the
	mortality of each peice. The mortality for the entire reach
	for timestep i is the volume weighted average of the peicewise
	mortalities.
 
	Throughout COMPASS the reach is broken up into two flows, the
	right and left bank flows, each flow has its own TDG levels
	and hence its own % mort due to TDG. The total % mort due to
	TDG will be a flow weighted average of the two separate %
	morts.

	The mortality for the entire reach for a particular julian day
	is the simple average over the mortalities of the reservoir
	for each timestep of the day.

	Reminder: The right bank is the right side of the river
	looking downstream. The % of the total flow that goes into the
	left and right flow is determined by the spill fraction at the
	closest upstream dam, with the exception of a confluence
	reach.  So if the spillway is on the right bank then the %
	flow in the right flow is just the spill fraction. fr_R
	records the % flow in the right flow and 1-fr_R is the % flow
	in the left flow. At a confluence the amount of flow in the
	left flow is just the amount flow of the river on the left
	branch of the two converging rivers.  Currently in COMPASS only
	two rivers can converge at once.

	*/

    
    for (i=0; i < DAM_SLICES_IN_SEASON; ++i) {
	int m, t;
	double x1, x2, len, daily_vel;
	double mA;
	double fr_R;
    double depth_m3;
	int slices_in_step = SLICES_IN_STEP; /* optimization */
	/* tau is the scalar to bring velocity into units of miles/(dam timestep) */
	int tau = slices_in_step;  
	int rchstep;
	
	gas_mort[i] = 0.0;

	/* compute timestep dependent parameters */
	rchstep = i / slices_in_step;

	/* 0 velocity means 0 mort */
	if (reachp->velocity[rchstep] == 0.0) {
	    continue;
	}
	
	/* length has been previously factored out of the volume */
	rs.depth = reachp->cur_volume[rchstep] / reachp->width;
    depth_m3 = 1 / (rs.depth * rs.depth * rs.depth);  /* optimization */

	/* fish density parameters */
	mA = FD_A(fd.zm, fd.zb, rs.depth);
	fd.zdep = fd.zb < rs.depth ? fd.zb : rs.depth;
	fd.m0 = FD_M0(mA, fd.zm, fd.zb);
	fd.mdiff = FD_Mdiff(mA, fd.zm, fd.zb);

	fr_R = ((gas->fraction_side == FLOW_RIGHT)
		? gas->flow_fraction[i] : 1.0 - gas->flow_fraction[i]);

	/* begin computing mort in different slugs of water */
	m = 0;
	t = i-m;
	x1 = 0.0;		/* Start at upstream part of reach*/
	/* Compute length of water in same slug */
	x2 = reachp->velocity[rchstep]/tau; 
	if (x2 > rs.length)
	    x2 = rs.length;

	while(x1 < rs.length) {
	    len = x2 - x1;	/* length of this portion of the reservoir */	

	    /* obtain velocity in mi/day */
	    daily_vel = reachp->velocity[t/slices_in_step] * STEPS_PER_DAY; 
	    /* rs.l = k in supersat.c */
	    rs.l = NSAT_DISSP_EXP_DM3(current->gas_dissp_exp,
				       daily_vel,
				       depth_m3) / daily_vel;
	    /* Mixing parameter */
	    rs.lam = rs.l + reachp->gas_theta;

	    if(gas->gas[FLOW_RIGHT][t] > 1.0 || gas->gas[FLOW_LEFT][t] > 1.0)
		    gas_mort[i] += (1.0 / rs.length)
		        * compute_gas_mort(x1,x2,
				       fr_R,
				       (double) Nmix[t],
				       (double) Ndiff[t],
				       len,&rs,&fd,&gm);
	    /* else
		    gas_mort[i] += 0.0; ** No TDG and no TDG mort in this slug */

	    m++;
	    t = i-m;
	    if (t < 0) t = 0;

	    x1 = x2;
	    x2 += reachp->velocity[t/slices_in_step]/tau;
	    if(x2 > rs.length)
		x2 = rs.length;
	}	
    }
}

