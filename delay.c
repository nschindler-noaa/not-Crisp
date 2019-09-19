/** \file
 * \brief components of the dam delay algorithm
 *
 * \section new_dam_delay See the Word document on the new dam delay algorithm
 */

#include <math.h>
#include "crisp.h"
#include "log.h"
#include "delay.h"

/** new delay equation - daylight_proportion is portion of desired
 * timestep spent in daylight; velocity in mi/day;
 * spill is proportion of river spilled; julian is julian date; and
 * the rest are dam dependent parameters.  */

double delay_prob(float alpha_day, float alpha_night,
		  float beta1, float beta2, float beta3,
		  float daylight_proportion,
		  float velocity, float spill, int julian)
{
    double p = daylight_proportion;
    double lambda = p*alpha_day + (1.0 - p)*alpha_night
	+ beta1*velocity + beta2*spill + beta3*julian;
	double dprob = exp(-lambda/DAM_TIME_SLC);
    return ( (dprob <= 1.0) ? dprob: 1.0 );
}

/** routine to compute the delay for a given dam, julian day, dam time
 * slice, and system time step (both counting from the beginning of the year)
 */

double dam_delay_prob(struct river_segment *seg, equation *delay_eqn,
		      int julian, int season_slice, int season_step)
{
	switch (delay_eqn->id) {
		case EQ_DELAY0:
		case EQ_DELAY1:
		case EQ_DELAY3:
		    if (seg->up && seg->up->seg_type == REACH_SEG) {
			return (delay_prob(delay_eqn->DELAY_ALPHA_DAY,
			   delay_eqn->DELAY_ALPHA_NIGHT,
			   delay_eqn->DELAY_BETA1,
			   delay_eqn->DELAY_BETA2,
			   delay_eqn->DELAY_BETA3,
			   seg->current->dam.daylight_proportion[season_slice],
			   /* convert velocity to mi/day */
			   seg->up->current->reach.velocity[season_step] * STEPS_PER_DAY,
			   seg->current->dam.spill[season_slice],
			   julian));
			}
			else {
				log_msg(L_ERROR, "can't compute passage delay at dam %s; no upstream reach velocity available.\n", seg->seg_name);
				return 0;
			}
		default:
			return 0.0;
	}
}
