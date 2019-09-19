/** \file
 * \brief CRiSP 1.6-style delay calculations */

/* definitions for the new (V1.6) dam delay computations.
 */
#ifndef __DELAY_H__
#define __DELAY_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Probability of delay given the conditions */
extern double delay_prob(float alpha_day, float alpha_night,
		  float beta1, float beta2, float beta3,
		  float daylight_proportion,
		  float velocity, float spill, int julian);

/** Probability of delay at a dam given the conditions */
extern double dam_delay_prob(struct river_segment* seg, equation *delay_eqn,
			     int julian, int season_slice,
			     int season_step);
    
#ifdef __cplusplus
}
#endif

/* definitions for the delay equation */
#define DELAY_ALPHA_DAY   param[0].val
#define DELAY_ALPHA_NIGHT param[1].val
#define DELAY_BETA1	  param[2].val
#define DELAY_BETA2	  param[3].val
#define DELAY_BETA3	  param[4].val
 
#endif /* __DELAY_H__ */
