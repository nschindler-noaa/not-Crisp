/* output tracking in scenario mode */

#include "crisp.h"
#include "output_flags.h"

/* set the output flags for a given seg */

void set_scenario_output_flags(seg)
    struct river_segment *seg;
{
    seg->output_flags = PASSAGE_FLAG;	/* always track passage in scenario */

#ifdef ROUTE_TRACKING
	if (seg->seg_type == DAM_SEG || seg->seg_type == REACH_SEG
		  || seg->seg_type == HEADWATER_SEG)
		seg->output_flags |= seg->output_settings;
#endif /* ROUTE_TRACKING */
}

