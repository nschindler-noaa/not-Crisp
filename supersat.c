#include <math.h>
#include <stdio.h>
#include "crisp.h"
#include "log.h"
#include "nsat.h"
#include "crisp_time.h"
#include "run_mode.h"

/* routine to return pointer to output GasDistribution for any seg */

GasDistribution *
get_gas_dist(seg)
    struct river_segment *seg;
{
    switch(seg->seg_type)
    {
    case HEADWATER_SEG:
	return(seg->current->headwater.gas_out);
	
    case REACH_SEG:
	return(seg->current->reach.gas_out);

    case DAM_SEG:
	return(seg->current->dam.gas_out);

    default:
	log_msg(L_FATAL, "cannot get gas distribution for seg %s\n",
		seg->seg_name);
	return((GasDistribution *) NULL);
    }
	
}

/**********************************************************************
 * the following function(s) deal with actually calculating nitrogen
 * supersaturation while the model is running.
 **********************************************************************
 */

/* ------------------------------------------------------------------------- *
 *  NOTE: this routine returns the concentration of gas in the spill flow,
 *  without further mixing.
 *
 *  n_conc = N2 concentration of total flow, after mixing; mg/liter.
 *  n_spill = N2 concentration leaving the stilling basin; mg/liter.
 *  n_forebay = N2 concentration in the forebay; mg/liter.
 *  pct_forebay = %nitrogen saturation in forebay (not supersaturation) 
 *  	= 100*n_forebay/N_EQ(temp)
 *  N_EQ(temp) = equilibrium concentration of N2 in water under air at
 *    	1.0 atm as a function of temperature;  mg/(liter atm).
 *
 *  total_flow = total flow through project; ft^3/sec (cfs).
 *  spill_flow = flow over spillway; ft^3/sec (cfs).
 *	these are input in kcfs, for consistency with the rest of COMPASS,
 *	and converted locally to cfs for consistency with gasspill and
 *	for numerical stability.
 *  
 *  hydro_pressure = average hydrostatic pressure in the spilling basin; atm.
 *  BARO_PRESSURE = barometric pressure at site; atm  (assumed = 1.0).
 *  delta = differential pressure factor; atm^(1/3).
 *
 *  k_entrainment = bubble entrainment coefficient; ft /(sec * atm^(1/3) ).
 *  k_entrainment = k_20 * ( 1.028 )^( T - 20 ), T in degrees C.
 *  k_20, used to set k_entrainment.There are three versions:
 *
 *  k_20 = 10^a * E^b * pct_forebay^c, ( EQ_NSAT1 ) used at Bonneville.
 *  k_20 = a + b*E + c*pct_forebay,    ( EQ_NSAT2 ) used elsewhere.
 *  k_20 = a*E^b, original WRE model.  ( EQ_NSAT3 )
 *
 *  a, b, c, dam_dependent parameters;  messy dimensions.
 *
 *  ALPHA = density of mainflow; atm / ft (ALPHA = 0.0295 atm/ft).
 *  ALPHA_0 = density of roller; atm/ft, (ALPHA_0 = sgr * ALPHA).
 *  sgr = specific gravity of roller, 0.0 <= sgr <= 1.0 ; dimensionless.
 *      
 *  width = spillway width; ft.
 *  length = stilling basin length; ft.
 *  depth = depth of stilling basin; ft.
 *        ( depth = tailwater elevation - basin floor elevation )
 *
 *  head = total assumed head at entrance to stilling basin;  ft
 *       ( head = forebay elevation - basin floor elevation ).
 * -------------------------------------------------------------------------
 */


	/* --------------------------------------------------------- *
	 *							     
	 *    To compute nsat over 100% use			  
	 *       n_supersat = 100 * ( (n_conc / N_EQ) - 1.0 );
	 *
	 *    To compute concentration use
	 *       n_conc = ( nsat / 100  + 1.0 ) *  N_EQ;
	 *
	 * ---------------------------------------------------------
	 */

/* routine to return N concentration.  returns -1 on error */

float
compute_n_conc(total_flow, spill_flow, temp, head,
	       depth, n_forebay, pergate, ngates, gate_width,
	       length, sgr, nsat_eqn)
     
    double   total_flow;
    double   spill_flow;
    double   temp;
    double   head;
    double   depth;
    double   n_forebay;
    double   pergate, ngates, gate_width;	/* dam-dependent constants  */
    double   length, sgr;			/* these, too */
    equation *nsat_eqn;
{
    double   y_zero, hydro_pressure, delta;
    double   total_head_loss, res_time;
    double   energy_loss_rate, k_20, k_entrainment;  
    double   n_spill;
    double   pct_forebay;
    double   open_gates, width;
    double   a, b, c;			/* adjustable dam_dependent params */

    a = nsat_eqn->param[0].val;
    b = nsat_eqn->param[1].val;
    c = nsat_eqn->param[2].val;
    pct_forebay = 100*n_forebay/N_EQ(temp);

    if ( spill_flow < pergate )
	open_gates = 1.0;
    else
	open_gates = spill_flow / pergate; 

    if ( open_gates > ngates )
	open_gates = ngates;
    width = open_gates * gate_width;
    
    total_flow *= 1000;
    spill_flow *= 1000;
      
    if (head - depth < 0.0)
    {
	log_msg(L_ERROR, "negative head\n");
	return (-1);
    }
  
    if (spill_flow <= 100.0)
	n_spill = n_forebay;
    else
    {
	y_zero = spill_flow / (width * sqrt(2 * G * head) ) ;
    
	hydro_pressure = BARO_PRESSURE + sgr*ALPHA*(depth - y_zero)*.5 +
	    ALPHA*(depth + y_zero)*.25;
    
	delta = pow( hydro_pressure +.25 * ALPHA *(depth + y_zero) , 1/3.0 )
	    - pow( hydro_pressure -.25 * ALPHA *(depth + y_zero) , 1/3.0 ) ;
    
	total_head_loss =head-depth-(0.5/G)*pow((spill_flow/(width*depth)),2.0);
	if( total_head_loss < 0 )
	    total_head_loss = 0;
	res_time = width * depth * length / spill_flow;
	energy_loss_rate = total_head_loss / res_time;

	/* Three versions of k_20 */

	switch(nsat_eqn->id)
	{
	case EQ_NSAT1:
	    k_20 = pow(10.0,a) * pow(energy_loss_rate, b) * pow(pct_forebay, c);
	    break;
	case EQ_NSAT2:
	    k_20 = a + b*energy_loss_rate + c*pct_forebay;
	    break;
	case EQ_NSAT3:
	    k_20 = a * pow( energy_loss_rate, b );
	    break;
	case	EQ_LIN0:
		return 0;
	default:
	    log_msg(L_ERROR, "bad spill eqn\n");
	    return(-1);
	}
    
	k_entrainment = k_20 * pow( 1.028, temp - 20 );
    
	n_spill = N_EQ(temp) * hydro_pressure
	    - ( N_EQ(temp) * hydro_pressure - n_forebay )
		* exp(-1 * k_entrainment * width * length * delta / spill_flow);
	if (n_spill < N_EQ(temp)) n_spill = N_EQ(temp);
    }
  
    return((float) n_spill);
}


/* entry point for nitrogen supersat calculation.  returns % sat >
 * 100, or -1 on error.  note that the nsat returned corresponds to
 * the supersaturation in the spill flow, without further mixing.
 */
float
compute_nsat(flow, spill, wtemp, fb_depth, tr_depth, fb_nsat, eqn, seg)
    double flow;
    double spill;
    double wtemp;
    double fb_depth;
    double tr_depth;
    double fb_nsat;
    equation *eqn;
    struct river_segment *seg;
{
    if (flow == 0) return 0;
    if (spill == 0) return (float) fb_nsat;
    
    switch(eqn->id) {
    case EQ_NSAT5:
    {
	float nsat = 0;
	if (spill < eqn->param[2].val || spill > eqn->param[3].val) {
	    if (eqn->id == seg->current->dam.nsat_backup_eqn.id)
		return -1;	/* error condition */
	    else
		return compute_nsat(flow, spill, wtemp, fb_depth,
				    tr_depth, fb_nsat,
				    &seg->current->dam.nsat_backup_eqn,
				    seg);
	}
	nsat = eqn->param[0].val + spill*eqn->param[1].val;
	if (nsat < fb_nsat) {
/*	    log_msg(L_DBGMSG, "using fb gas %f (spill gas %f) at %s\n",
		    fb_nsat, nsat, seg->seg_name); */
	    nsat = fb_nsat;
	}
	return (nsat > 0) ? nsat : 0;
    }
    break;
    case EQ_NSAT6:
    {
	float nsat = 0;
	if (spill < eqn->param[3].val || spill > eqn->param[4].val) {
	    if (eqn->id == seg->current->dam.nsat_backup_eqn.id)
		return -1;	/* error condition */
	    else
		return compute_nsat(flow, spill, wtemp, fb_depth,
				    tr_depth, fb_nsat,
				    &seg->current->dam.nsat_backup_eqn,
				    seg);
	}
	nsat = eqn->param[0].val
	    + eqn->param[1].val*exp(spill*eqn->param[2].val);

	if (nsat < fb_nsat) {
/*	    log_msg(L_DBGMSG, "using fb gas %f (spill gas %f) at %s\n",
		    fb_nsat, nsat, seg->seg_name); */
	    nsat = fb_nsat;
	}
	return (nsat > 0) ? nsat : 0;
    }
    break;
    case EQ_NSAT0:
    {
	float nsat = (((eqn->param[1].val)*spill
		+ (eqn->param[0].val)*(1.0 - exp(-(eqn->param[2].val)*spill))));
	if (nsat < fb_nsat)
	    nsat = fb_nsat;
	return nsat;
	break;
    }
    case EQ_NSAT4:
    {
	float nsat = (((eqn->param[1].val)*spill
		 + (eqn->param[0].val)*spill/((eqn->param[2].val) + spill)));
	if (nsat < fb_nsat)
	    nsat = fb_nsat;
	return nsat;
	break;
    }
    default:
    {
	float n_conc;
  
	n_conc = compute_n_conc(flow, spill, wtemp, fb_depth, tr_depth,
				(0.01 * fb_nsat + 1.0) * N_EQ(wtemp),
				seg->current->dam.pergate,
				seg->current->dam.ngates,
				seg->current->dam.gate_width,
				seg->current->dam.basin_length,
				seg->current->dam.sgr,
				eqn);
  
	if (n_conc < 0)
	    return (-1);
	else {
	    float nsat = (n_conc / N_EQ(wtemp) - 1.0) * 100;
	    /* remove rounding error */
	    if (nsat < 0)
		nsat = 0;
	    return(nsat);
	}
    }
    }
}


/* routine to compute a supersat for a specific dam, but assuming no
 * forebay nsat, no drawdown, and generic flow and temperature.  this
 * is used by the equation window.
 */

#ifdef __cplusplus
extern "C" {
#endif

float compute_generic_nsat(float spill, equation *eqn, void *params)
{
    return compute_nsat(200.0, spill, 15.0, 112.0, 37.0, 0.0, eqn,
			(struct river_segment *) params);
}

#ifdef __cplusplus
}
#endif

/* compute the gas output vectors for a dam for the entire year.  
 *
 * preconditions:  passed in river segment is a dam;
 *		   physical characteristics fields in dam current
 *			(flow, temp, spill, elev, depths, etc.);
 *		   gas_in struct properly formed and with current data;
 *		   gas_out struct properly formed (all vectors exist);
 *		   gas_out struct contains current data in all fields
 *			except "gas";
 *
 * postconditions: all preconditions remain invariant;
 *		   "gas" vectors in gas_out struct updated;
 *
 * return: 	   0 on success; non-zero on error.
 */

int
compute_dam_gas(seg)
    struct river_segment *seg;
{
    struct dam *dam = &seg->current->dam;
    equation *nsat_eqn;
    double nsat, Nmix, Ndiff, gas_left, gas_right;
    double frLold, frLnew, frdiff, Nl_in, Nr_in;
    int i, day;
    int erc=0;
    FlowLocation spill_side = dam->spill_side;
    FlowLocation phouse_side = (spill_side == FLOW_LEFT) ? FLOW_RIGHT : FLOW_LEFT;    
    GasDistribution* gas_in = dam->gas_in;
    GasDistribution* gas_out = dam->gas_out;
    
    /* step through the season on the dam time slice. */
    for (i=0; i < DAM_SLICES_IN_SEASON; ++i) {
    if (run_mode.compute_flow_break)
        return 1;
	day = (int) (i / DAM_TIME_SLC);

	/* get the input and output left side fractions and compute diff */
	frLold = (gas_in->fraction_side == FLOW_LEFT)
	    ? gas_in->flow_fraction[i] : 1.0 - gas_in->flow_fraction[i];
	frLnew = (spill_side == FLOW_LEFT)
	    ? dam->spill[i] : 1.0 - dam->spill[i];
	frdiff = frLnew - frLold;

	/* get the input gas levels and modify as appropriate */
	Nl_in = gas_in->gas[FLOW_LEFT][i];
	Nr_in = gas_in->gas[FLOW_RIGHT][i];
	if (frdiff != 0.0) {	    
	    if (frdiff >= 0.0) {
		Nl_in = (Nl_in * frLold + Nr_in * frdiff) / frLnew;
		/* Nr_in remains unchanged */
	    }
	    else {
		Nr_in = (Nr_in * (1.0 - frLold) - Nl_in * frdiff) / (1.0 - frLnew);
		/* Nl_in remains unchanged */
	    }
	}

	/* proceed with gas calculations based on spill */
	gas_out->gas[FLOW_LEFT][i] = Nl_in;
	gas_out->gas[FLOW_RIGHT][i] = Nr_in;

	nsat_eqn = is_night(i % DAM_TIME_SLC) ? &dam->nsat_night_eqn : &dam->nsat_eqn;
	nsat = compute_nsat(dam->flow[day],
			    dam->spill[i] * dam->flow[day],
			    dam->water_temp[day*STEPS_PER_DAY],
			    dam->forebay_depth[day],
			    dam->tailrace_depth[day],
			    (spill_side == FLOW_LEFT) ? Nl_in : Nr_in,
			    nsat_eqn,
			    seg);
	if (nsat == -1) {
	    nsat = 0;
	    log_msg(L_ERROR, "Nsat error at %s\n", seg->seg_name);
	    ++erc;
	}
	gas_out->gas[spill_side][i] = nsat;

	/* A non-zero k_entrain allows for gassing of the powerhouse
	 * flow as a function of the difference between the forebay and
	 * spill gas levels and the fraction of flow spilled.  The
	 * gassing of the powerhouse flow is caused by entraiment of
	 * this flow by the spillwater in the tailrace.  This is an
	 * empirical function.
	 */

    if (gas_out->gas[spill_side][i] > gas_out->gas[phouse_side][i]) {
        gas_out->gas[phouse_side][i] +=
             ( 1 - exp(-1*dam->k_entrain*dam->spill[i]*dam->flow[i/DAM_TIME_SLC]) )
              * (gas_out->gas[spill_side][i] - gas_out->gas[phouse_side][i]);
        }
	
	/* now do some tailrace mixing.  dam_theta of 0 will result in
           no tailrace mixing. */

	gas_left = gas_out->gas[FLOW_LEFT][i];
	gas_right = gas_out->gas[FLOW_RIGHT][i];
	Nmix = frLnew * gas_left + (1.0 - frLnew) * gas_right;
	Ndiff = gas_left - gas_right;
	gas_out->gas[FLOW_LEFT][i] = Nmix + (1.0 - frLnew) * Ndiff * exp(-dam->gas_theta);
	gas_out->gas[FLOW_RIGHT][i] = Nmix - frLnew * Ndiff * exp(-dam->gas_theta);	
    }
    return erc;
}

/* compute the gas output for a reach for a single dam timestep over an
 * arbitrary length.  handles the mixing and dissipation of gas in two
 * flows.  see preconditions for compute_reach_gas().  no data is
 * modified by this routine.  results are returned in the floats given
 * by the last two parameters.
 */

void
dissipate_gas(gas_in, n, length, rchptr, gas_out_left, gas_out_right)
    GasDistribution *gas_in;	/* input gas levels */
    int n;			/* output dam timestep */
    float length;		/* length in miles */
    struct reach *rchptr;	/* reach */
    float *gas_out_left;	/* output ptr */
    float *gas_out_right;	/* output ptr */
{
    double fr1, fr2;
    double dissp_exp, k, nmix, ndiff;
    float *vel = rchptr->velocity;	/* velocity vector,
					   units miles/reach_timestep,
					   length STEPS_IN_SEASON */
    float daily_vel;
    FlowLocation s1 = gas_in->fraction_side;
    FlowLocation s2 = FLOW_OTHER_SIDE(s1);
    
    /* determine source of gas */
    int i=0;
    float dist = vel[n/SLICES_IN_STEP] / SLICES_IN_STEP;
    while ((n-i) && dist < length) {
	++i;
	/* velocity in miles/step */
	dist += vel[(n-i)/SLICES_IN_STEP] / SLICES_IN_STEP;	
    }
    fr1 = gas_in->flow_fraction[n-i];
    fr2 = 1.0 - fr1;
	
    /* obtain velocity in mi/day */
    daily_vel = vel[(n-i)/SLICES_IN_STEP] * STEPS_PER_DAY; 
	
    /* mix and dissipate the gas in two flows */
    dissp_exp = NSAT_DISSP_EXP(current->gas_dissp_exp,
			       daily_vel,
			       rchptr->cur_volume[n/DAM_TIME_SLC]
			       / rchptr->width);
    k = dissp_exp / daily_vel;
    nmix = fr1 * gas_in->gas[s1][n-i] + fr2 * gas_in->gas[s2][n-i];
    ndiff = gas_in->gas[s1][n-i] - gas_in->gas[s2][n-i];

    if (s1 == FLOW_LEFT) {
	*gas_out_left = 
	    (nmix - N_EQ_ZERO + ndiff * fr2 * exp(-rchptr->gas_theta*length))
	    * exp(-k * length) + N_EQ_ZERO;
	*gas_out_right = 
	    (nmix - N_EQ_ZERO - ndiff * fr1 * exp(-rchptr->gas_theta*length))
	    * exp(-k * length) + N_EQ_ZERO;
    }
    else {
	*gas_out_right = 
	    (nmix - N_EQ_ZERO + ndiff * fr2 * exp(-rchptr->gas_theta*length))
	    * exp(-k * length) + N_EQ_ZERO;
	*gas_out_left = 
	    (nmix - N_EQ_ZERO - ndiff * fr1 * exp(-rchptr->gas_theta*length))
	    * exp(-k * length) + N_EQ_ZERO;
    }
}

/* Interface routine to allow C++-based GUI to display reach gas
 *  dissipation profiles. */
void compute_gas_eqn_val(struct river_segment *seg, int step, 
                          float length, 
                          float *gas_out_left, float *gas_out_right) {
    struct reach *rchptr = &(seg->current->reach);
    dissipate_gas( rchptr->gas_in, step, length, rchptr, 
                                gas_out_left, gas_out_right );
}

/* compute gas output of a reach for the entire season.  
 *
 * preconditions:  seg must be a reach;
 *		   gas_in must contain current input values in all fields;
 *		   gas_out flow_fraction and fraction_side values must
 *		       match gas_in values;
 * postconditions: preconditions invariant;
 *		   gas_out contains output gas values for this reach;
 * return: 0 on success; non-0 on error.
 */

int
compute_reach_gas(gas_out, gas_in, seg)
    GasDistribution *gas_out;	/* result */
    GasDistribution *gas_in;	/* input gas dist */
    struct river_segment *seg;
{
    int n;
    
    for (n=0; n < DAM_SLICES_IN_SEASON; ++n) {
	if (seg->current->reach.flow[n/DAM_TIME_SLC] == 0.0) {
	    gas_out->gas[FLOW_LEFT][n] = gas_out->gas[FLOW_RIGHT][n] = 0.0;
	}
	else {
        if (run_mode.compute_flow_break)
            return 1;
	    dissipate_gas(gas_in, n, seg->current->reach.length,
			  &(seg->current->reach),
			  &(gas_out->gas[FLOW_LEFT][n]),
			  &(gas_out->gas[FLOW_RIGHT][n]));
	}
    }
    return 0;
}

/* instantaneously mix the gas values in a distribution, and return
 * the result in the supplied vector.  the result vector may be the
 * same storage as one of the input vectors.  this fn also takes two
 * temperature vectors corresponding to the two flows.  If the
 * pointers to the temp vecs are identical, then a simple temperature
 * independent mixing algorithm is used, otherwise a more complicated
 * one is used.  note that in the temperature dependent version, input
 * temperatures are also instantaneously mixed to determine a final
 * mixed gas value.
 *
 * preconditions:  input gas distribution valid in all fields;
 *		   temp vecs of size STEPS_IN_SEASON (reach time step);
 *		   result vector size DAM_SLICES_IN_SEASON;
 * postconditions: supplied gas vector updated with mixed value;
 * return: 	   none.
 */

void
mix_gas_instantaneously(gas, gas_in, left_temp, right_temp)
    float *gas;
    GasDistribution *gas_in;
    float *left_temp;
    float *right_temp;
{
    int i;

    if (left_temp == right_temp) {
	/* temperature independent mixing */
	for (i=0; i < DAM_SLICES_IN_SEASON; ++i) {
	    gas[i] = (gas_in->gas[gas_in->fraction_side][i]
		      * gas_in->flow_fraction[i]) 
		+ (gas_in->gas[FLOW_OTHER_SIDE(gas_in->fraction_side)][i]
		   * (1.0 - gas_in->flow_fraction[i]));
	}
    }
    else {
	/* temperature dependent mixing */
	float	neq, l_nconc, r_nconc, nconc;

	for (i=0; i < DAM_SLICES_IN_SEASON; ++i) {
	    if (gas_in->gas[FLOW_LEFT][i] > 0.0
		|| gas_in->gas[FLOW_RIGHT][i] > 0.0) {

		int day = (int) (i / DAM_TIME_SLC);
		int step = (int) (i / SLICES_IN_STEP);

		float r_flow =
		    ((gas_in->fraction_side == FLOW_RIGHT)
		     ? gas_in->flow_fraction[i]
		     : 1.0 - gas_in->flow_fraction[i])
		    * gas_in->flow[day];
		float l_flow = gas_in->flow[day] - r_flow;
	    
		/* convert nsat to concentration, average, convert back */
		neq = N_EQ(left_temp[step]);
		l_nconc = gas_in->gas[FLOW_LEFT][i] * neq / 100.0 + neq;
		neq = N_EQ(right_temp[step]);
		r_nconc = gas_in->gas[FLOW_RIGHT][i] * neq / 100.0 + neq;
		nconc = (l_flow * l_nconc + r_flow * r_nconc)
		    / (gas_in->flow[day]);
		neq = N_EQ((l_flow*left_temp[step] + r_flow*right_temp[step])
			   / gas_in->flow[day]);
		gas[i] = 100.0 * (nconc - neq) / neq;
		if (gas[i] < 0.0)
		    gas[i] = 0.0;
	    }
	    else gas[i] = 0.0;
	}
    }
}

/* utility routine to compute nmix and ndiff vector values for a gas
 * dist.  empty nmix and ndiff vectors of size DAM_SLICES_IN_SEASON
 * should be supplied as input parameters, as well as a well-formed
 * GasDistribution struct.
 */

void gas_mix_and_diff(gas_in, nmix, ndiff)
    GasDistribution *gas_in;
    float *nmix;
    float *ndiff;
{
    double fr1, fr2;
    int i;

    FlowLocation s1 = gas_in->fraction_side;
    FlowLocation s2 = FLOW_OTHER_SIDE(s1);

    for (i=0; i < DAM_SLICES_IN_SEASON; ++i) {
	fr1 = gas_in->flow_fraction[i];
	fr2 = 1.0 - fr1;

	nmix[i] = fr1 * gas_in->gas[s1][i] + fr2 * gas_in->gas[s2][i];
	ndiff[i] = gas_in->gas[s1][i] - gas_in->gas[s2][i];
    }
}
