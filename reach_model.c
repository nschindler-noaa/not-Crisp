#include <stdlib.h>
#include "crisp.h"
#include "sample.h"
#include "species.h"
#include "log.h"
#include "output_flags.h"
#include "gas_mort.h"
#include "nsat.h"
#include "pred.h"
#include "passage.h"


void reach_model(river_seg, game)
	struct river_segment *river_seg;
    	int game;
{
#define this_reach	(river_seg->current->reach)	/* abbreviation */    
	struct release *release;
	int		run_length;
	float		pactivity, predpopmean, trin, trinlen;
	float		pred_rt[STEPS_IN_SEASON];
	float		drop_ratio[STEPS_IN_SEASON];
	int		start_date;
	int 		smolt_onset;
	int 		smolt_finish;
	int		i, is, j;
    int    reach_class;
    float  drift_variance;
    double  *sqrt_factor;
	extern float do_seg_new();

	static float	*input = 0;
    static float    *length_in = 0;
    static float    *dam_spill_in = 0;
	static float	*nsat_in = 0;
	static float	*nsat_out = 0;
    static float	*nmix = 0;
    static float	*ndiff = 0;

    static double    **p_gmort_vec = NULL;
    static double   **p_sqrt_vec = NULL;
    static int      *gmort_vec_valid = NULL;

    reach_class = this_reach.rclass;

	/* allocate temporary arrays */
	if (!input)
	    if ( ! (input = (float *) malloc((sizeof(float) * STEPS_IN_RUN))))
	    {	log_msg(L_ERROR, "Error allocating mem in reach_model\n");
		return;
	    }
	if (!length_in)
	    if ( ! (length_in = (float *) malloc((sizeof(float) * STEPS_IN_RUN))))
	    {	log_msg(L_ERROR, "Error allocating mem in reach_model\n");
		return;
	    }
	if (!dam_spill_in)
	    if ( ! (dam_spill_in = (float *) malloc((sizeof(float) * STEPS_IN_RUN))))
	    {	log_msg(L_ERROR, "Error allocating mem in reach_model\n");
		return;
	    }
	if (!nsat_in)
	    if ( ! (nsat_in =(float *) malloc((sizeof(float)*STEPS_IN_SEASON))))
	    {	log_msg(L_ERROR, "Error allocating mem in reach_model\n");
		return;
	    }
	if (!nsat_out)
	    if ( ! (nsat_out =(float *)malloc((sizeof(float)*STEPS_IN_SEASON))))
	    {	log_msg(L_ERROR, "Error allocating mem in reach_model\n");
		return;
	    }
    if (!nmix)
	if ( ! (nmix=(float *)malloc((sizeof(float)*DAM_SLICES_IN_SEASON))))
	{	log_msg(L_ERROR, "Error allocating mem in reach_model\n");
	return;
	}
    if (!ndiff)
	if ( ! (ndiff=(float *)malloc((sizeof(float)*DAM_SLICES_IN_SEASON))))
	{	log_msg(L_ERROR, "Error allocating mem in reach_model\n");
	return;
	}

    /* These arrays will cache gas mortality vectors so that they can
     *  be reused between releases. NOTE that we do not need to bother
     *  with any of this if we are not calculating gas mortality.
     */
    if (DO_GAS_MORT && p_gmort_vec == NULL) {
        p_gmort_vec = (double **)malloc( sizeof(double *) * num_specs );
        if (p_gmort_vec == NULL) {
            log_msg(L_ERROR, "Error allocating mem in reach_model\n");
            return;
        }
        gmort_vec_valid = (int *)malloc( sizeof(int) * num_specs );
        if (gmort_vec_valid == NULL) {
            log_msg(L_ERROR, "Error allocating mem in reach_model\n");
            return;
        }
        for (i = 0; i < num_specs; i++) {
            p_gmort_vec[i] = (double *)malloc( 
                            sizeof(double) * DAM_SLICES_IN_SEASON );
            if (p_gmort_vec[i] == NULL) {
                log_msg(L_ERROR, "Error allocating mem in reach_model\n");
                return;
            }
        }
    }

    /* These arrays will cache the square root factor needed in X-T
     *  predation mortality so that they can be reused between releases
     *  of the same species. NOTE that they are kept per species.
     */
    if (p_sqrt_vec == NULL) {
        p_sqrt_vec = (double **)malloc( sizeof(double *) * num_specs );
        if (p_sqrt_vec == NULL) {
            log_msg(L_ERROR, "Error allocating mem in reach_model\n");
            return;
        }
        for (i = 0; i < num_specs; i++) {
            p_sqrt_vec[i] = (double *)malloc( 
                            sizeof(double) * STEPS_IN_RUN );
            if (p_sqrt_vec[i] == NULL) {
                log_msg(L_ERROR, "Error allocating mem in reach_model\n");
                return;
            }
        }
    }

    /* mark gmort_vec cache as empty */
    if (DO_GAS_MORT) {
        if (p_gmort_vec == NULL || gmort_vec_valid == NULL)
            return;
        for (i = 0; i < num_specs; i++)
            gmort_vec_valid[i] = 0;
    }

    /* mark the sqrt_vec cache as empty. */
    for( i = 0; i < num_specs; i++)
        for (j = 0; j < STEPS_IN_RUN; j++)
            p_sqrt_vec[i][j] = 0.0;

#ifdef NOTDEF    
    if (quantify_is_running()) {
	quantify_stop_recording_data();
	if (!strcmp(river_seg->seg_name, "John Day Pool")) {
	    quantify_clear_data();
	    quantify_stop_recording_system_calls();
	    quantify_start_recording_data();
	}
    }
#endif    
    /* compute gas nmix and ndiff vector values for this gas input */
    gas_mix_and_diff(this_reach.gas_in, nmix, ndiff);

    if (DO_PRED_MORT) {
	    for (i=0; i < STEPS_IN_SEASON; i++) {
	        /* calculate volume adjustment for predator activity.
             *  NOTE that this calculation is new for v1.6 */
	        if (current->flags & PRED_VOL_FLAG)
	        {
		        drop_ratio[i] = (this_reach.cur_volume[i] == 0.0) ? 0.0
		            : exp( this_reach.pred_dist_coef *
                       (1.0 - this_reach.cur_volume[i] / this_reach.volume) );
	        }
	        else
                drop_ratio[i] = 1.0;
	    }
    }

   	for (release = run->release_list; release; release = release->next)
	{
	    struct rls_seg *rls_seg = release->rls_seg;
	    int top_seg;

        is = species_index( release->species );
        if (is == -1) {
            /* Should never happen! */
            log_msg(L_ERROR, "Species not found in reach_model\n");
            return;
        }

	    while (rls_seg && rls_seg->river_segment != river_seg)
		rls_seg = rls_seg->down;
	    if ( ! rls_seg)
		continue;		/* doesn't pass this point in river */

	    top_seg = (rls_seg == release->rls_seg); /* first seg in release? */
	    run_length = DAYS_IN_SEASON - release->start_date;
	    if (run_length > DAYS_IN_RUN)
		run_length = DAYS_IN_RUN;

	    if (top_seg)
		log_msg(L_DBGMSG, "Processing release %s, river mile %.2f along %s\n",
			release->site->name, release->site->seg_mile,
			release->site->seg->seg_name);
	    
        /* Drift variance now depends on reach class, so must be
         *  sampled in each reach for each release. */
	    drift_variance = release->stock->mvcoef[reach_class];

	    /* get samples for predation and gas mortality coefficients */

        if (DO_PRED_MORT) {
	        pactivity = 0.0000001 * release->species->reach_pcoef[reach_class];
            predpopmean = this_reach.species[is].pred_pop_mean;
		    
	        /* compute predator activity */
	        for (i=0; i < STEPS_IN_SEASON; i++) {
	            /* calculate volume adjustment for predator activity */
		        pred_rt[i] = pred_rate(pactivity, 
                                predpopmean * drop_ratio[i],
					            this_reach.water_temp[i],
					            &(release->stock->pred_temp_response_eqn[reach_class]));
	        }
        }

	    /* adjust start date and smolt date for STEPS PER DAY */
	    start_date = release->start_date * STEPS_PER_DAY;
	    smolt_onset = release->smolt_onset * STEPS_PER_DAY;
	    smolt_finish = release->smolt_finish * STEPS_PER_DAY;

	    /* set input array */
	    bcopy((char *) (top_seg ? release->number :
			    rls_seg->up->data[game]->output),
		  (char *) input, sizeof(float) * STEPS_IN_RUN);

	    /* set input length array */
        if (top_seg) {
            for (i = 0; i < STEPS_IN_RUN; i++)
                length_in[i] = release->length;
        }
        else {
	        bcopy(rls_seg->up->data[game]->length,
		      (char *) length_in, sizeof(float) * STEPS_IN_RUN);
        }

	    /* set input dam_spill array for Z15 model.
		 * If the release is made below a dam, then these are set to zero.
		 * If a dam is upstream then the proportion of fish that are spilled is used. 
		 * As the fish leave the segment, a weighted average of their fish spilled
		 * proportions is calculated and may be copied into the next segment. */
		if (top_seg) {
			/* Try to find the upstream dam and fill in some theoretical values */
			struct river_segment *dm = rls_seg->river_segment->up;
			
			if (dm && dm->seg_type == DAM_SEG) {
				/* Try and guess what the spill proportion would be at this dam (this is used for migration rate calculations) */
				struct dam_surv dmprb;
				int step;

				/* Remember to account for the start date in dam_spill_in[], but not in the dam arrays since they are for the whole time span */
				for (step = start_date; step < STEPS_IN_RUN + start_date; step++) {
					pass_surv(dm, release->species->index, release->stock, step/STEPS_PER_DAY, step % STEPS_PER_DAY, 0.0, release->smolt_onset, &dmprb, TRUE, FALSE);
					dam_spill_in[step-start_date] = dmprb.spill_surv + dmprb.rsw_surv;
				}
			}
			else {
				/* There is no dam to fill in theoretical values, so use the specified value */
				for (i=0; i < STEPS_IN_RUN; i++) {
					dam_spill_in[i] = release->initial_spill_experience;
				}
			}
		}
		else if (rls_seg->up
				&& rls_seg->up->river_segment->seg_type == DAM_SEG) {
			/* Calculate spill experience values for the Z15 model from the dam if
			 * this segment is immediately following a dam */
			int step;
			struct rls_seg* dm = rls_seg->up;

			for (step=0; step < STEPS_IN_RUN; step++) {
				if (dm->data[game]->output[step] != 0) {
					dam_spill_in[step] = (dm->data[game]->spillway_pass[step] + dm->data[game]->rsw_pass[step]) / dm->data[game]->output[step];
				}
			}
		}
		else {
	        bcopy(rls_seg->up->data[game]->dam_spill,
		      (char *) dam_spill_in, sizeof(float) * STEPS_IN_RUN);
		}

		/* allocate delayed mortality rate data space if needed. */
		if (river_seg->down && river_seg->down->seg_type == DAM_SEG && rls_seg->data[game]->delayed_survival_rate == NULL) {
			if (!(rls_seg->data[game]->delayed_survival_rate =
			  (float *)malloc((sizeof(float) * STEPS_IN_RUN))))
			{   log_msg(L_ERROR,
				"Error allocating delayed_survival_rate mem in reach_model\n");
			return;
			}
			bzero((char *)rls_seg->data[game]->delayed_survival_rate,
			  sizeof(float) * STEPS_IN_RUN);
		}

        if (rls_seg->data[game]->transport_in) {
            for (i=0; i < STEPS_IN_RUN; i++) {
                trin = rls_seg->data[game]->transport_in[i];
                trinlen = rls_seg->data[game]->transport_in_length[i];
                if (input[i] + trin > 0.00001) {
                    length_in[i] = (input[i] * length_in[i]
                                    + trin * trinlen)
                                                 / (input[i] + trin);
                }
		        input[i] += trin;
            }
        }

/*	    nsat_ptr = (release->species->gmort_eqn.id == EQ_GMORT0)
		? nsat_out + start_date
		    : nsat_in + start_date;
	    */
#ifdef ROUTE_TRACKING
	    /* if run-time tracking flag turned on, and tracking on at
	     * this reach...
	     */

	    if (TRACK_PASSAGE(river_seg)) 
	    {
		if (! rls_seg->data[game]->project_in)
		{
		    if (!(rls_seg->data[game]->project_in =
			  (float *)malloc((sizeof(float) * STEPS_IN_RUN))))
		    {   log_msg(L_ERROR,
				"Error allocating route tracking in reach model\n");
			return;
		    }
		    bzero((char *)rls_seg->data[game]->project_in,
			  sizeof(float) * STEPS_IN_RUN);
		}

		/* we have to save the input data before the output is
		 * calculated, since the output routine may actually
		 * modify this input array.
		 */
		bcopy((char *)input, (char *)rls_seg->data[game]->project_in,
		      sizeof(float) * STEPS_IN_RUN);
	    }
#endif	

        /* Retrieve the gas mortality vector from the cache, or if
         *  necessary calculate it here.
         */
        if (DO_GAS_MORT) {
            if (p_gmort_vec[is] == NULL)
                return;
            if (!gmort_vec_valid[is]) {
                compute_gas_mort_vec(p_gmort_vec[is], river_seg,
                            river_seg->current->reach.gas_in,
			                nmix, ndiff,
			                &release->species->gmort_eqn,
			                &release->species->fdens_eqn );
                gmort_vec_valid[is] = 1;

            }
        }

        /* retrieve the sqrt factor cache vector needed
         *  by X-T predation mortality */
        sqrt_factor = p_sqrt_vec[ is ];

	    /* Calculate the output of the current river segment */

	    rls_seg->data[game]->nopass =
		do_seg_new(game, input, length_in, dam_spill_in, rls_seg->data[game]->transport_in,
			   rls_seg->data[game]->output,
			   rls_seg->data[game]->length,
			   rls_seg->data[game]->dam_spill, rls_seg->data[game]->output_transport,
			   ((river_seg->down && river_seg->down->seg_type == DAM_SEG) ? rls_seg->data[game]->delayed_survival_rate : NULL),
			   (top_seg ? release->site->seg_mile
			    	    : this_reach.length),
			   this_reach.velocity + start_date,
			   this_reach.flow + (start_date / STEPS_PER_DAY),
               this_reach.water_temp + start_date,
               this_reach.turbidity + start_date,
			   this_reach.cur_volume + start_date,
			   this_reach.length,
			   this_reach.width,
               DO_GAS_MORT ? 
                    p_gmort_vec[is] + (start_date * SLICES_IN_STEP) :
                    NULL,
               pred_rt + start_date,
			   drift_variance,
			   &(release->stock->migration_eqn[reach_class]),
			   release->stock->migr_b1fac[reach_class] + start_date,
			   release->stock->vvar[reach_class],
			   run_length * STEPS_PER_DAY,
			   start_date,
			   smolt_onset,
			   smolt_finish,
			   top_seg,
               reach_class,
			   release,
               sqrt_factor,
			   &rls_seg->data[game]->inriver_latent_mort,
			   &rls_seg->data[game]->transport_latent_mort,
			   &rls_seg->data[game]->differential_return,
			   &rls_seg->data[game]->inriver_return,
			   &rls_seg->data[game]->transport_return,
			   &rls_seg->data[game]->overall_return,
			   &rls_seg->data[game]->post_bonneville_total,
			   default_transport_target == rls_seg->river_segment ? &rls_seg->data[game]->d : NULL,
			   /* input_dam_spill should be used for custom_survival unless this is the top segment and it's bounded at the upstream end by a dam */
			   !(top_seg && release->rls_seg->river_segment->up && release->rls_seg->river_segment->up->seg_type == DAM_SEG));
	}
#ifdef NOTDEF
    if (quantify_is_running()) {
	quantify_stop_recording_data();
    }
#endif    
}
