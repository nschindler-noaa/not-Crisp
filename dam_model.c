#include <stdlib.h>
#include <math.h>

#include "crisp.h"
#include "passage.h"
#include "log.h"
#include "output_flags.h"
#include "transport.h"
#include "crisp_time.h"

#define	MONTE(mean, var)	(mean)	/* for now */
#define active_seg temp			/* access temp field of release struct*/
#define RESOLUTION (0.05)		/* less than this many input fish
					 * are passed with no mortality
					 */

/* Compute the dam passage for the given dam seg.  The outer loop is time
 * steps (system).  Within, each release is examined.  This is so that daily
 * totals crucial to the transportation calculation may be maintained.
 * The dam module maintains its own daily time step;  this is the innermost
 * loop.
 *
 * The routine returns 0 on success, -1 on any error.
 *
 * WARNING:  The dam time step DAM_TIME_SLC must be greater than (i.e. more
 * steps per day) or equal to the system time step STEPS_PER_DAY for proper
 * operation.  Also, if the former is an integer multiple of the latter then
 * things work a little better.  Otherwise, parameters at some dam time slices
 * will never be examined (all fish will pass in other time slices).
 */

int dam_model(river_seg, game)
    struct river_segment *river_seg;
    int game;
{
    struct release *release;
    struct rls_seg *rls_seg;
    register struct dam	*dm;
	struct dam_species *dmspc;
    register double tmp1;
    register int i, spc, max_steps, step, step_slice;
    register StockT *stock;
    int first_slice, last_slice;
    register float *input;
    register double this_pass;
    register double nopass;
    register double dam_input;
    int		day;
    int		full_trans_day;
    int		step_in_day;
    int		collecting;
    int		transport_complete;
    int		transport_low_days;
    int		end_transport_ok = TRUE;
    int 	transport_starts;
    float	smolt_age;
    float	daily_count;
    float	*partial_counts;
    float	collection_eff;
    float 	total1, total2;
    struct transport	*trns;
    struct dam_surv dmprb;

    trns = river_seg->current->dam.transport;
    dm = &river_seg->current->dam;

    max_steps = DAYS_IN_SEASON * STEPS_PER_DAY;
    
    /* mark active releases;
     * compute total number of partially transported fish
     */
    if (!(partial_counts = (float *) malloc(sizeof(float) * DAYS_IN_SEASON)))
    {	log_msg(L_ERROR, "Error allocating mem in dam_model\n");
	return -1;
    }
    bzero((char *)partial_counts, sizeof(float) * DAYS_IN_SEASON);
    total1 = 0.0;
    for (release = run->release_list; release; release = release->next)
    {
	rls_seg = release->rls_seg;
	while (rls_seg && rls_seg->river_segment != river_seg)
	    rls_seg = rls_seg->down;
	release->active_seg = (char *) rls_seg;
	if (rls_seg && trns && release->species == trns->hfl_spec)
	{
	    int rls_start_step = release->start_date * STEPS_PER_DAY;
	    input = (rls_seg == release->rls_seg ? release->number : rls_seg->up->data[game]->output);
	    for (i=0; i < STEPS_IN_RUN && (rls_start_step+i) < max_steps; i++)
	    {
		partial_counts[(rls_start_step + i)/STEPS_PER_DAY] += input[i];
		total1 += input[i];
	    }
	}
    }
    /* compute last day for partial transport, if applicable */
    full_trans_day = 0;
    if (trns && total1 > 0.0 && trns->hfl_passed_perc > 0.0)
    {
	total1 = total1 * trns->hfl_passed_perc;
	total2 = 0.0;
	i=0;
	while (i < DAYS_IN_SEASON && total2 <= total1)
	{
	    total2 += partial_counts[i];
	    ++i;
	}
	full_trans_day = i-1;
    }
    free(partial_counts);

    collecting = FALSE;
    transport_complete = FALSE;
    transport_starts = 0;
    daily_count = 0;
    transport_low_days = 0;
    for (step=0; step < max_steps; step++)
    {
	day = step / STEPS_PER_DAY;
	step_in_day = step % STEPS_PER_DAY;

	/* start transport? note that transport_complete is true when
	 * all permissible restarts have been performed.
	 */
	if (trns && step_in_day == 0 && !collecting && !transport_complete)
	{
	    if (transport_starts == 0)
	    {
		/* first start */
		if (trns->start_by_date)
		{
		    if (day == trns->start_date)
			collecting = TRUE;
		}
		else if (daily_count > trns->start_count)
		    /* this is yesterday's count!  */
		    collecting = TRUE;
	    }
	    else
	    {
		/* restart */
		if (trns->restart_by_date)
		{
		    if (day == trns->restart_date)
			collecting = TRUE;
		}
		else if (daily_count > trns->restart_count)
		    /* this is yesterday's count!  */
		    collecting = TRUE;
	    }
	    if (collecting)
	    {
		/* new transport just started */
		++transport_starts;
		end_transport_ok = FALSE;
		transport_low_days = 0;
		log_msg(L_MSG, "transportation started at %s, day %d\n",
			river_seg->seg_name, day);
	    }
	}

	if (step_in_day==0)
	    daily_count = 0;
		
	for (release = run->release_list; release; release = release->next)
	{
	    if ( ! (rls_seg = (struct rls_seg *) release->active_seg))
		continue;		/* doesn't pass this point in river */

	    /* set index into output hists, accounting for start
	     * date offset.  
	     */
	    i = step - (release->start_date * STEPS_PER_DAY);
	    
	    /* continue if start date not yet reached or if end of run */
	    if (i < 0 || i >= STEPS_IN_RUN)
		continue;

	    if (i == 0)			/* first time in this seg */
	    {
		rls_seg->data[game]->nopass = 0;
		
        /* allocate length data space if needed. */
        if (rls_seg->data[game]->length == NULL) {
		    if (!(rls_seg->data[game]->length =
			  (float *)malloc((sizeof(float) * STEPS_IN_RUN))))
		    {   log_msg(L_ERROR,
				"Error allocating length mem in dam_model\n");
			return -1;
		    }
		    bzero((char *)rls_seg->data[game]->length,
			  sizeof(float) * STEPS_IN_RUN);
        }
		
        /* allocate dam_spill data space if needed. */
        if (rls_seg->data[game]->dam_spill == NULL) {
		    if (!(rls_seg->data[game]->dam_spill =
			  (float *)malloc((sizeof(float) * STEPS_IN_RUN))))
		    {   log_msg(L_ERROR,
				"Error allocating dam_spill mem in dam_model\n");
			return -1;
		    }
		    bzero((char *)rls_seg->data[game]->dam_spill,
			  sizeof(float) * STEPS_IN_RUN);
        }

		/* allocate transport data space */
		if (trns && rls_seg->data[game]->transport_out == NULL)
		{		
		    if (!(rls_seg->data[game]->transport_out =
			  (float *)malloc((sizeof(float) * STEPS_IN_RUN))))
		    {   log_msg(L_ERROR,
				"Error allocating trns mem in dam_model\n");
			return -1;
		    }
		    bzero((char *)rls_seg->data[game]->transport_out,
			  sizeof(float) * STEPS_IN_RUN);
		}
#ifdef ROUTE_TRACKING
		/* if run-time tracking flag turned on, and tracking on at
		 * this dam...
		 */

		if (TRACK_ROUTING(river_seg)) 
		{
		    if (! rls_seg->data[game]->project_in)
		    {
			if (!(rls_seg->data[game]->project_in =
			      (float *)malloc((sizeof(float) * STEPS_IN_RUN))))
			{   log_msg(L_ERROR,
			    "Error allocating route tracking in dam_model\n");
			    return -1;
			}
			bzero((char *)rls_seg->data[game]->project_in,
			      sizeof(float) * STEPS_IN_RUN);
		    }
		    if (! rls_seg->data[game]->turbine_in)
		    {
			if (!(rls_seg->data[game]->turbine_in =
			      (float *)malloc((sizeof(float) * STEPS_IN_RUN))))
			{   log_msg(L_ERROR,
			    "Error allocating route tracking in dam_model\n");
			    return -1;
			}
			bzero((char *)rls_seg->data[game]->turbine_in,
			      sizeof(float) * STEPS_IN_RUN);
		    }
		    if (! rls_seg->data[game]->sluiceway_in)
		    {
			if (!(rls_seg->data[game]->sluiceway_in =
			      (float *)malloc((sizeof(float) * STEPS_IN_RUN))))
			{   log_msg(L_ERROR,
			    "Error allocating route tracking in dam_model\n");
			    return -1;
			}
			bzero((char *)rls_seg->data[game]->sluiceway_in,
			      sizeof(float) * STEPS_IN_RUN);
		    }
		    if (! rls_seg->data[game]->bypass_in)
		    {
			if (!(rls_seg->data[game]->bypass_in =
			      (float *)malloc((sizeof(float) * STEPS_IN_RUN))))
			{   log_msg(L_ERROR,
			    "Error allocating route tracking in dam_model\n");
			    return -1;
			}
			bzero((char *)rls_seg->data[game]->bypass_in,
			      sizeof(float) * STEPS_IN_RUN);
		    }
		    if (! rls_seg->data[game]->spillway_in)
		    {
			if (!(rls_seg->data[game]->spillway_in =
			      (float *)malloc((sizeof(float) * STEPS_IN_RUN))))
			{   log_msg(L_ERROR,
			    "Error allocating route tracking in dam_model\n");
			    return -1;
			}
			bzero((char *)rls_seg->data[game]->spillway_in,
			      sizeof(float) * STEPS_IN_RUN);
		    }
		    if (! rls_seg->data[game]->rsw_in)
		    {
			if (!(rls_seg->data[game]->rsw_in =
			      (float *)malloc((sizeof(float) * STEPS_IN_RUN))))
			{   log_msg(L_ERROR,
			    "Error allocating route tracking in dam_model\n");
			    return -1;
			}
			bzero((char *)rls_seg->data[game]->rsw_in,
			      sizeof(float) * STEPS_IN_RUN);
		    }

		    if (! rls_seg->data[game]->sluiceway_pass)
		    {
			if (!(rls_seg->data[game]->sluiceway_pass =
			      (float *)malloc((sizeof(float) * STEPS_IN_RUN))))
			{   log_msg(L_ERROR,
			    "Error allocating sluiceway mem in dam_model\n");
			    return-1;
			}
			bzero((char *)rls_seg->data[game]->sluiceway_pass,
			      sizeof(float) * STEPS_IN_RUN);
		    }
		    if (! rls_seg->data[game]->bypass_pass)
		    {
			if (!(rls_seg->data[game]->bypass_pass =
			      (float *)malloc((sizeof(float) * STEPS_IN_RUN))))
			{   log_msg(L_ERROR,
			    "Error allocating bypass mem in dam_model\n");
			    return-1;
			}
			bzero((char *)rls_seg->data[game]->bypass_pass,
			      sizeof(float) * STEPS_IN_RUN);
		    }
		    if (! rls_seg->data[game]->turbine_pass)
		    {
			if (!(rls_seg->data[game]->turbine_pass =
			      (float *)malloc((sizeof(float) * STEPS_IN_RUN))))
			{   log_msg(L_ERROR,
			    "Error allocating turbine mem in dam_model\n");
			    return -1;
			}
			bzero((char *)rls_seg->data[game]->turbine_pass,
			      sizeof(float) * STEPS_IN_RUN);
		    }
		    if (! rls_seg->data[game]->spillway_pass)
		    {
			if (!(rls_seg->data[game]->spillway_pass =
			      (float *)malloc((sizeof(float) * STEPS_IN_RUN))))
			{   log_msg(L_ERROR,
			    "Error allocating spillway mem in dam_model\n");
			    return -1;
			}
			bzero((char *)rls_seg->data[game]->spillway_pass,
			      sizeof(float) * STEPS_IN_RUN);
		    }
		    if (! rls_seg->data[game]->rsw_pass)
		    {
			if (!(rls_seg->data[game]->rsw_pass =
			      (float *)malloc((sizeof(float) * STEPS_IN_RUN))))
			{   log_msg(L_ERROR,
			    "Error allocating spillway mem in dam_model\n");
			    return -1;
			}
			bzero((char *)rls_seg->data[game]->rsw_pass,
			      sizeof(float) * STEPS_IN_RUN);
		    }
		}
		if (TRACK_ROUTING(river_seg) || TRACK_TRANSPORT(river_seg)) 
		{
		    if (! rls_seg->data[game]->transport_pass)
		    {
			if (!(rls_seg->data[game]->transport_pass =
			      (float *)malloc((sizeof(float) * STEPS_IN_RUN))))
			{   log_msg(L_ERROR,
			    "Error allocating transport mem in dam_model\n");
			    return -1;
			}
			bzero((char *)rls_seg->data[game]->transport_pass,
			      sizeof(float) * STEPS_IN_RUN);
		    }
		}
		if (TRACK_ROUTING(river_seg) || TRACK_TRANSPORT(river_seg)) 
		{
		    if (! rls_seg->data[game]->transport_arrived)
		    {
			if (!(rls_seg->data[game]->transport_arrived =
			      (float *)malloc((sizeof(float) * STEPS_IN_RUN))))
			{   log_msg(L_ERROR,
			    "Error allocating transport mem in dam_model\n");
			    return -1;
			}
			bzero((char *)rls_seg->data[game]->transport_arrived,
			      sizeof(float) * STEPS_IN_RUN);
		    }
		}
#endif  /* ROUTE_TRACKING */		
	    }
		    
	    spc = release->species->index;
		dmspc = &dm->species[spc];
	    stock = release->stock;
	    smolt_age = ((float) day) - release->smolt_onset;
		
	    input = (rls_seg == release->rls_seg ? release->number
		     : rls_seg->up->data[game]->output);
	    nopass = rls_seg->data[game]->nopass;

	    /* speed optimizations */
	    if (input[i] == 0.0 && nopass == 0.0)
		continue;

	    if (collecting)
	    {
		if (day < full_trans_day && dm->flow[day] >= trns->hi_flow)
		    collection_eff = 1.0 - dm->species[spc].separation_prob;
		else
		    collection_eff = 1.0;
	    }
	    else
		collection_eff = 0.0;

	    /* loop through the dam time slices for this system time step */
	    dam_input = input[i] / SLICES_IN_STEP;
	    first_slice = step_in_day * SLICES_IN_STEP;
	    last_slice  = first_slice + SLICES_IN_STEP;
	    for (step_slice= first_slice; step_slice < last_slice; step_slice++)
	    {
		pass_surv(river_seg, spc, stock, day, step_slice, collection_eff,
			  smolt_age, &dmprb
#ifdef ROUTE_TRACKING
			  , TRACK_ROUTING(river_seg)
#endif			  
			  , TRUE);

        /* Apply forebay processes: fish approaching the dam are subjected
         *  to mortality, then those fish which are passing straight through
         *  are subjected to 1/4 slice of dam-face predator mortality before
         *  being combined with the passing-out proportion of the delayed
         *  fish ("nopass" from previous slices) and subjected to in-dam
         *  processes. The approaching fish that don't go straight through
         *  are added to the further-delayed fish and subjected to a full
         *  slice of dam-face predator mortality.
         */
        tmp1 = dam_input * dmprb.fb_pred_surv; /* approach-phase mortality */
        this_pass = (tmp1 * dmprb.pred_quarter_surv + nopass)
                      * dmprb.pass_prob;  /* fish input to dam processes. */
        nopass = (tmp1 + nopass) * (1.0 - dmprb.pass_prob)
                   * dmprb.pred_surv;

		/* if the delayed fish are too few, go ahead and pass
                   them, as an optimization */
		if (nopass < RESOLUTION) {
		    this_pass += nopass;
		    nopass = 0;
		}
	
/* This is code for the new dam delay stuff */

		/* Here is where the new route-specific and day/night-specific delay is computed (it is used just below) */
		{
			/* This array of structures holds the parameters that are needed for the new route/day varying dam delay model for each passage route.
			 * It is terminated by an entry that has NULL in/out members.
			 *
			 * Note that the spill routes are shared between powerhouses so they are not returned from the call to pass_surv(). */
			const struct route_delay_parameters {
				int		is_bypass; /* Whether or not this is the bypass route (for transportation purposes) */
				double	extra;	/* Extra delay */
				double	day;	/* Day delay */
				double	prob;
				double	surv;
				float*	in;
				float*	out;
			} *route_parameters, route_delay_parameters[] = {
				{FALSE,	dmspc->spill_delay,		dmspc->spill_day,		dmprb.spill_prob,		dmprb.spill_surv,		rls_seg->data[game]->spillway_in,	rls_seg->data[game]->spillway_pass},
				{TRUE,	dmprb.bypass_extra,		dmprb.bypass_day,		dmprb.bypass_prob,		dmprb.bypass_surv,		rls_seg->data[game]->bypass_in,		rls_seg->data[game]->bypass_pass},
				{FALSE,	dmprb.sluiceway_extra,	dmprb.sluiceway_day,	dmprb.sluiceway_prob,	dmprb.sluiceway_surv,	rls_seg->data[game]->sluiceway_in,	rls_seg->data[game]->sluiceway_pass},
				{FALSE,	dmprb.turbine_extra,	dmprb.turbine_day,		dmprb.turbine_prob,		dmprb.turbine_surv,		rls_seg->data[game]->turbine_in,	rls_seg->data[game]->turbine_pass},
				{FALSE,	dmspc->rsw_delay,		dmspc->rsw_day,			dmprb.rsw_prob,			dmprb.rsw_surv,			rls_seg->data[game]->rsw_in,		rls_seg->data[game]->rsw_pass},
				{0, 0, 0, 0, 0, NULL, NULL}
			};

			/* Loop through each passage route and calculate when the fish will enter/exit, then add to total and route counts */
			for (route_parameters = route_delay_parameters; route_parameters->in && route_parameters->out; route_parameters++) {
				double extra_steps = route_parameters->extra * STEPS_PER_DAY; /* Converted to model steps (the delay) */
				int extra_base = floor(extra_steps);	/* Minimum number of steps fish will be delayed */
				double extra_prob[2];	/* Probability of passing on either the first or second step */
				int extra_step;	/* Which of the two steps is currently being processed (0 or 1) */

				/* Fish pass in one of two steps (this is so that on average, for example, 1.2 steps of delay can be incurred by delaying 20% of the fish for an extra step) */
				extra_base = floor(extra_steps);
				extra_prob[1] = extra_steps - extra_base;
				extra_prob[0] = 1.0 - extra_prob[1];

				for (extra_step=0; extra_step < 2; extra_step++) {
					double day_steps = 0.0;
					int day_base;
					double day_prob[2];
					int day_step;

					/* If the fish are arriving during the day then there may be additional delay which must be partintioned further into 2 other steps */
					if (is_night((i + extra_base + extra_step) % STEPS_PER_DAY) == FALSE) day_steps = route_parameters->day / 2 * STEPS_PER_DAY; /* Divided by 2 since it is assumed that half of a 24 hour period is day */

					/* Fish pass in one of two steps */
					day_base = floor(day_steps);
					day_prob[1] = day_steps - day_base;
					day_prob[0] = 1.0 - day_prob[1];

					for (day_step=0; day_step < 2; day_step++) {
						const int s = i + extra_base + extra_step + day_base + day_step; /* Which model step these fish will enter/exit on */

						const double delayed_survival_rate = rls_seg->up->data[game]->delayed_survival_rate ? rls_seg->up->data[game]->delayed_survival_rate[i] : 0.0; /* Rate of mortality due to delayed passage */
						const double delay_surv = exp(delayed_survival_rate * ((double)(s-i)) / STEPS_PER_DAY); /* Survival after the effects of mortality due to delayed passage */

						const double in = this_pass * extra_prob[extra_step] * day_prob[day_step] * route_parameters->prob;
						const double surv = this_pass * extra_prob[extra_step] * day_prob[day_step] * dmprb.tr_pred_surv * delay_surv * route_parameters->surv;

						if (s < max_steps) {
							/* Fish are assumed not to grow while passing through dam. */
							if (surv > 0) rls_seg->data[game]->length[s] = ((rls_seg->data[game]->length[s] * rls_seg->data[game]->output[s]) + (rls_seg->up->data[game]->length[i] * surv)) / (rls_seg->data[game]->output[s] + surv);

							/* Pass the fish */
							rls_seg->data[game]->project_in[s] += in;
							rls_seg->data[game]->output[s] += surv;
							route_parameters->in[s] += in;
							route_parameters->out[s] += surv;

							/* Transported fish are collected in the bypass system, so they are delayed like bypassed fish */
							if (route_parameters->is_bypass) {
								if (trns && !transport_complete)
								{
									if (collecting)
									{
									rls_seg->data[game]->transport_out[s] += extra_prob[extra_step] * day_prob[day_step] * this_pass * dmprb.collection_prob;
									daily_count += extra_prob[extra_step] * day_prob[day_step] * this_pass * dmprb.collection_prob;
									}
									else
									daily_count += extra_prob[extra_step] * day_prob[day_step] * this_pass * dmprb.bypass_prob;
								}
							}
						}
						else {
							/* Censor these fish since they don't ever leave the dam in the model run */
							/* TODO: I don't think this is effectively censoring the fish, but this probably won't ever happen */
							rls_seg->data[game]->nopass += surv;
						}
					}
				}
			}
		}

	    }
	    /* fish left for next time step, or censored if last step */
	    rls_seg->data[game]->nopass = nopass;	
	}
	
	/* end transportation? */
	if (trns && step_in_day == (STEPS_PER_DAY - 1) 
	    && !transport_complete && collecting)
	{
	    /* end by date? */
	    if (day == trns->end_date)
		collecting = FALSE;
	    else
	    {
		/* make sure to go above low count before ending */
		if (daily_count > trns->end_count) 		
		    end_transport_ok = TRUE;		
	    
		if (daily_count < trns->end_count && end_transport_ok)
		    ++transport_low_days;
		else
		    transport_low_days = 0;
			
		if (transport_low_days >= trns->num_low_days)
		    collecting = FALSE;
	    }
	    if (!collecting)
	    {
		/* transport just ended, check if restarts allowed */
		if (trns->max_restarts >= 0) /* -1 means infinite restarts */
		    if (transport_starts > trns->max_restarts)
			transport_complete = TRUE;
		log_msg(L_MSG, "transportation stopped at %s, day %d\n",
			river_seg->seg_name, day);
	    }
	}
    }

    /* do transportation */
    if (trns)
    {
        for (release = run->release_list; release; release = release->next)
	{
	    if ( ! (rls_seg = (struct rls_seg *) release->active_seg))
		continue;       /* doesn't pass this point in river */

	    transport_fish(release, rls_seg, trns, game);
	}
    }
	return 0;
}
