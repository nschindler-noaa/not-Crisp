/** \file
 * \brief Gas-related mortality calculations */

#ifndef GAS_MORT_H
#define GAS_MORT_H

#include "crisp.h"

/* gas mort 0-2 eqn param names */
#define gm_mlow	param[0].val
#define gm_mhi	param[1].val
#define gm_crit	param[2].val
#define gm_mcr	param[3].val
#define gm_len	param[4].val
#define gm_vel	param[5].val
#define gm_depth param[6].val

/* gas mort 3 eqn param names */
#define gm3_mlow	param[0].val
#define gm3_mhi		param[1].val
#define gm3_ncrit	param[2].val
#define gm3_tdg2	param[3].val
#define gm3_splfr	param[4].val
#define gm3_theta	param[5].val
#define gm3_len		param[6].val
#define gm3_vel		param[7].val

/* compute the gas mortality fraction (daily rate);
 * sat==Nsupersaturation, m_low==low_slope, m_hi==high_slope,
 * crit==critical value
 */
#define COMPUTE_GMORT0(sat, m_low, m_hi, crit)  			\
(((sat) <= (crit)) ? (m_low) * (sat)					\
 : (m_low) * (sat) + (m_hi) * ((sat) - (crit)))

extern double compute_gmort1();
extern double compute_gmort2(double sat, double m_low, double m_hi, double crit,
		double k, double len, double vel, double u0, double ub, double udepth);

extern void update_gas_mort_eqn();

void compute_gas_mort_vec(
    double *gas_mort,   	/* result vector */
    struct river_segment *seg,  /* reach characteristics */
    
    GasDistribution *gas,	/* input gas of reach */
    float *Nmix, 		/* mixed input gas vector */
    float *Ndiff, 		/* diff of input gas vectors */
    
    equation	*gmort_eqn,
    equation	*fdens_eqn
    );


#endif
