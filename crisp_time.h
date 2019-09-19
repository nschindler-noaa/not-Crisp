/** \file
 * \brief Time definitions only used for is_night().
 */

#ifndef __TIME_H__
#define __TIME_H__

#ifdef __cplusplus
extern "C" {
#endif

/* NOTE: These defines are ONLY used here and do not determine the
 * length of day/night periods. In spill.c there are the day/night
 * definitions used for spill and there both periods are half a
 * day (12 hours). */

#define night_start 	(2000)		/** Time at which night begins for is_night() */
#define night_end      	(600)		/** Time at which night ends for is_night() */

/** is_night() takes in a dam time slice (that is on [0, DAM_TIME_SLICE), i.e. slice % DAM_TIME_SLC)
 * \return TRUE if the slice is during the night, FALSE otherwise. */
extern int is_night(int time);

#if 0
/* Inlined version of is_night()
 * This would slow down compilation a bit, but should provide a tiny speed boost
 * if is_night() is being used a lot. I only implemented this because I thought
 * is_night() was a macro, but it wasn't. I doubt the difference is noticeable. */
#define	is_night(time) (((step_time(time) >= night_start && step_time(time+1) <= night_end) \
	|| (step_time(time) < night_end && night_end - step_time(time) >= step_time(1)/2) \
	|| (step_time(time+1) > night_start && step_time(time+1) - night_start >= step_time(1)/2)) ? TRUE : FALSE)
#endif

#ifdef __cplusplus
}
#endif

#endif /* __TIME_H__ */
