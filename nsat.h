/** \file
 * \brief Nitrogen supersaturation definitions. */

#ifndef NSAT_H
#define NSAT_H

#include "flow.h"
#include <math.h>

/* Gas Distribution type */

typedef struct GasDistribution {
    float *gas[NUM_FLOW_PARTITIONS];  /* size DAM_SLICES_IN_SEASON */
    float *flow;			/* size DAYS_IN_SEASON */
    float *flow_fraction;		/* fraction of flow on one side,
					   size DAM_SLICES_IN_SEASON */
    FlowLocation fraction_side;		/* which side the fraction refers to */
} GasDistribution;

/* definitions for nitrogen supersaturation calculations */

#define ALPHA           ( 0.0295 )
#define G               ( 32.0 )
#define BARO_PRESSURE   ( 1.0 )
/* Nick replaced this 1/13/03 DHS #define N_EQ(temp)      ( 21.1 - 0.3125 * (temp) ) */
#define N_EQ(temp)      ( 25.47 - 0.3868 * (temp) )
#define N_EQ_ZERO	(0.0)		/* supersat equilibrium at 0 */
/*#define N_THETA 	(0.075)	*/	/* yields 95% mix at 40 miles */
    
/* dissipation calculation */

#define NSAT_DISSP_EXP(dissp_exp, velocity, depth) \
(sqrt((dissp_exp) * 0.0001 * (velocity) / ((depth)*(depth)*(depth))) * 700.75)

/* This is used in place of NSAT_DISSP_EXP where pre-calculating 1/depth^3
 *  can be profitable. Note also that the multiplication by 0.0001 has
 *  been moved outside the sqrt() call and combined with the 700.75 constant.
 */
#define NSAT_DISSP_EXP_DM3(dissp_exp, velocity, depth_m3) \
(sqrt((dissp_exp) * (velocity) * (depth_m3)) * 7.0075)


#define NSAT_DISSP(old_nsat, exponent, length, velocity) \
((old_nsat) * exp(-(exponent) * (length) / (velocity)))


/* definitions for nsat equation display */

#define NSAT_XMIN 0.0
#define NSAT_XMAX 400.0
#define NSAT_YMIN 0.0
#define NSAT_YMAX 100.0

#define NSAT_EQ_HEIGHT 470
#define NUM_NSAT_EQNS 6

/* externally callable routines */
    
extern float compute_nsat();		/* compute a single nsat value */
extern GasDistribution *get_gas_dist();	/* returns output gas for a seg */
extern FlowLocation determine_side();	/* places a fork on the right or left */
extern void mix_gas_instantaneously();  /* mix gas from two flows */
extern int compute_reach_gas();		/* compute gas output for a reach */
extern int compute_dam_gas();		/* compute all gas output for a dam */
#ifdef __cplusplus
extern "C" {
extern void compute_gas_eqn_val(struct river_segment *seg, int step, 
                          float length, 
                          float *gas_out_left, float *gas_out_right);
}
#endif
extern void dissipate_gas();		/* dissipate/mix gas for one timestep */
extern void gas_mix_and_diff();		/* compute mix and diff vectors */
#endif
