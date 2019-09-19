/** \file
 * \brief  Migration eqn macros */

/* definitions for the equation parameters */
#define migr_bmin 	param[0].val
#define migr_bmax 	param[1].val
#define migr_bflow 	param[2].val
#define migr_alpha 	param[3].val
#define migr_tinf 	param[4].val
#define migr_vel 	param[5].val	/* non-data param - velocity */
#define migr_rls 	param[6].val	/* non-data param - release date */
#define migr_alfa2	param[7].val	/* hidden parameter */
#define migr_blen	param[8].val	/* hidden parameter */

/* the actual equation computations */

/* first, a few internal defs */

#define MB0(min, max) (2*(min) - (max))
#define MB2(min, max) (2 * ((max)-(min)))

/* This quantity is precalculated and stored for speed. */
extern float mb1factor( equation *eqn, double t );

/* migration rate in miles/day.  velocity is same units.
 * tmig is (date - migration_onset).  scale is a final scaling
 * factor to apply to the result.
 */
#define MIGR_RT(Bmin, Bmax, alfa2, mb1fac, velocity, tmig, Blenfac, scale) \
((MB0((Bmin),(Bmax)) + (velocity) * (mb1fac)				\
  + MB2((Bmin),(Bmax)) / (1 + exp(-(alfa2)*(tmig)))			\
  + Blenfac  )					  			\
 / (scale))
/* This version includes a temperature term */
#define MIGR_RT2(Bmin, Bmax, alfa2, mb1fac, Btemp, velocity, temperature, tmig, Blenfac, scale) \
((MB0((Bmin),(Bmax)) + (velocity) * (mb1fac) + (temperature) * (Btemp)			\
  + MB2((Bmin),(Bmax)) / (1 + exp(-(alfa2)*(tmig)))			\
  + Blenfac  )					  			\
 / (scale))

/* This routine is used to calculate migration rates for both display and
 *  simulation. All migration rate calculations are done here.
 */
extern double compute_migr_eqn( equation *eqn, double mb1fac, 
                        double velocity, double temperature, 
                        double tmig, double fish_length, double dam_spill,
						double julian_day, double scale );
