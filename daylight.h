/* \file
 * \brief This routine sets the daylight proportions at a dam for all dam time slices*/

#ifndef __DAYLIGHT_H__
#define __DAYLIGHT_H__

#ifdef __cplusplus
extern "C" {
#endif

extern void compute_daylight_proportions(struct river_segment *seg);
extern double get_daylight_fraction( double latitude, int timestep );

#ifdef __cplusplus
}
#endif

#endif /* __DAYLIGHT_H__ */
