/* headw.c
 *	this file handles the creation of modulated unregulated headwater
 *	flows based on a formula which simulates historical flows.  used
 *	in scenario mode only at explicit request from the user.
 */

#include "crisp.h"
#include "sample.h" 	/*  already #includes <math.h>  */

#define OMEGA  (2*M_PI/365)

/* compute the headwater flow based on a bunch of user parameters,
 * including mean flow and weighting for this year (proportion).  output
 * is calculated daily flow for the season.
 */

static void compute_headwater_flows(hdw, proportion, hdwflow)
     struct headwater *hdw;		/* from the river_segment struct */
     float  proportion;			/* allows designation of hi/low flow */
     float *hdwflow;			/* output flow, size DAYS_IN_SEASON */
{
  int day;
  float base_year[365];

  for(day = 0; day < 365; day++)
      base_year[day] = proportion * hdw->flow_mean;  
  
  for( day = 0; day < DAYS_IN_SEASON; day++ )
    hdwflow[day] = hdw->flow_mean;
  return;
}
  

/*
 * void modulate_unreg_headwaters()
 *	compute the modulated historical flows for all unregulated
 *	headwaters and storage basins.
 */

void modulate_unreg_headwaters()
{
    struct river_segment *seg, *downseg;
    
    for (seg = river_mouth; seg; seg = seg->next)
    {
	if (seg->seg_type == HEADWATER_SEG)
	{
	    /* see if this headwater is unregulated, or above a storage basin */
	    downseg = seg->down;
	    while (downseg && !downseg->fork && !(downseg->seg_type == DAM_SEG))
		downseg = downseg->down;
	    if (!downseg || downseg->fork
		|| (downseg->seg_type == DAM_SEG
		    && downseg->current->dam.storage_basin))
	    {
		/* seg is unregulated or storage basin; do the modulation */
		compute_headwater_flows(&seg->current->headwater,
				        current->flow_proportion,
					seg->current->headwater.flow);
	    }
	}
    }
}
