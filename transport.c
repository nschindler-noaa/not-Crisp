/** \file
 * \brief Implementations of transport operation functions. */
#include <stdlib.h>
#include <stdio.h>
#include "crisp.h"
#include "sample.h"
#include "log.h"
#include "output_flags.h"
#include "batch.h"
#include "transport.h"
#include "species.h"

extern float river_distance();		/* defined below */

/** \brief Send the fish away */

int transport_fish(release, rls_seg, trns, game)
    struct release *release;
    struct rls_seg *rls_seg;
    struct transport *trns;
    int game;
{
    struct river_segment *tseg = trns->target_seg;
    struct rls_seg *trls;
    float mort;
    float time;
    equation *eqn;
    struct dam_species *dmspec;
/*    float total_in = 0;
 *    float total_out = 0;
 */
    float surv, shipped;
    int	i;

    extern float compute_trans_mort();
    
    for (trls = rls_seg; trls && trls->river_segment != tseg; trls = trls->down)
	;
    if (!trls)
    {
	    log_msg(L_ERROR, "unable to find transport target seg!\n");
	    return -1;
    }
    if (!trls->data[game]->transport_in)
    {
	    if (!(trls->data[game]->transport_in =
	          (float *)malloc((sizeof(float) * STEPS_IN_RUN))))
	    {
            log_msg(L_ERROR, "Error allocating mem in transport\n");
	        return -1;
	    }
	    bzero((char *)trls->data[game]->transport_in,
	          sizeof(float) * STEPS_IN_RUN);
    }
    if (!trls->data[game]->transport_in_length)
    {
	    if (!(trls->data[game]->transport_in_length =
	          (float *)malloc((sizeof(float) * STEPS_IN_RUN))))
	    {
            log_msg(L_ERROR, "Error allocating mem in transport len\n");
	        return -1;
	    }
	    bzero((char *)trls->data[game]->transport_in_length,
	          sizeof(float) * STEPS_IN_RUN);
    }
    
    /* compute mort */

    dmspec = &trns->source_seg->current->
	dam.species[species_index(release->species)];
    eqn = &dmspec->trans_eqn;
    mort = compute_trans_mort(current->water_travel.time, eqn,
			      dmspec->transport_mort);
/*    low = mort - (dmspec->transport_mort_mean - dmspec->transport_mort_low);
    low = (low < 0.0) ? 0.0 : low;
    hi  = mort + (dmspec->transport_mort_high - dmspec->transport_mort_mean);
    hi  = (hi > 1.0) ? 1.0 : hi;

    mort = broken_stick(mort, low, hi);*/

/*    mort = broken_stick(trns->source_seg->current->
				dam.species[species_index(release->species)]
					.transport_mort_mean,
			trns->source_seg->current->
				dam.species[species_index(release->species)]
					.transport_mort_low,
			trns->source_seg->current->
				dam.species[species_index(release->species)]
					.transport_mort_high);
*/
    /* compute travel time in days */
    time = river_distance(trns->source_seg, trns->target_seg, 0.0) / trns->rate;
    /* change to time steps */
    time = (float) ((int) (time * (float)STEPS_PER_DAY));
    
    for (i=0; i < STEPS_IN_RUN; i++)
    {
	
	shipped = rls_seg->data[game]->transport_out[i];
	surv = shipped * (1.0 - mort);
	if (i + time >= STEPS_IN_RUN)
	    trls->data[game]->nopass += surv;
	else
	{
        float num, len, lenin;
        num = trls->data[game]->transport_in[i+(int)time];
        len = trls->data[game]->transport_in_length[i+(int)time];
        lenin = rls_seg->data[game]->length[i];
        if (num + surv > 0.00001)
            trls->data[game]->transport_in_length[i+(int)time] =
                  (surv * lenin + num * len) / (surv + num);

	    trls->data[game]->transport_in[i+(int)time] += surv;
	    /* total_in += surv; */

#ifdef ROUTE_TRACKING
	    /* if compile time tracking and tracking this dam */
	    if (TRACK_ROUTING(trns->source_seg)
		|| TRACK_TRANSPORT(trns->source_seg)) 

		/* this tracks post-mort transport by release */
		rls_seg->data[game]->transport_pass[i] = surv;

		/* This array is the same as transport_pass except that it's indexed by date of arrival */
		rls_seg->data[game]->transport_arrived[i+(int)time] = surv;
#endif
	}
	/* total_out += shipped; */
    }

   	return 0;
}

