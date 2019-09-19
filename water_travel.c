/* routines to compute water particle travel time */

#include "crisp.h"

/* compute water particle tt for a single reach segment.  takes start
 * and ending days of the season as parameters.  returns travel time
 * in days.  assumes valid length, velocity, etc. in segment.  called
 * from compute_flow().
 */

float water_tt(seg, first, last)
    struct river_segment *seg;
    int first, last;		/* first and last days of season */
{
    int i;
    float avg_vel;
    
    if (seg->seg_type != REACH_SEG)
	return 0;

    /* convert to time steps */
    first *= STEPS_PER_DAY;
    last  *= STEPS_PER_DAY;

    avg_vel = 0;
    for (i = first; i <= last && i < STEPS_IN_SEASON; ++i) 
	avg_vel += seg->current->reach.velocity[i];
    avg_vel /= (i-first + 1);

    /* return average water particle travel time through this seg in days */
    return ((seg->current->reach.length / avg_vel) / STEPS_PER_DAY);
}

/* compute average system water particle travel time using the
 * parameters specified in the global water_travel struct.  assumes
 * that all the individual reservoir time calculations are up to date.
 * called from compute_all_flow().  places the result in the global
 * water_travel struct.
 */

void set_system_water_tt()
{
    float total = 0;
    struct river_segment *seg;

    seg = current->water_travel.upper;
    while (seg && seg != current->water_travel.lower) {
	if (seg->seg_type == REACH_SEG)
	    total += seg->current->reach.water_particle_tt;
	seg = seg->down;
    }
    /* add in the last one */
    if (seg == current->water_travel.lower && seg->seg_type == REACH_SEG)
	total += seg->current->reach.water_particle_tt;

    current->water_travel.time = total;
}

