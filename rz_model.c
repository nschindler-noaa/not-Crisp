#include <stdlib.h>
#include <math.h>
#include <malloc.h>
#include <stdio.h>
#include "crisp.h"
#include "sample.h"
#include "log.h"
#include "gas_mort.h"
#include "migration.h"
#include "nsat.h"
#include "pred.h"
#include "fdens.h"
#include "travel_table.h"
#include "monte_output.h"
#include "growth.h"
#include "custom_survival.h"
#include "delayed_mort.h"
#include "return.h"
#include "pbn.h"

#define	RESOLUTION	0.000001

/* we need to add age and health values and probably shift
 * to a season-offset base rather than a release-offset base
 *  NOTE that we may or may not apply gas or predation
 *  mortality to the fish.
 */

float
do_seg_new(game, input, input_length, input_dam_spill, input_transport, output, output_length, output_dam_spill, output_transport, output_delayed_survival_rate,
       seg_length,velocity, flow, water_temp, turbidity,
       volume,reach_length,width,
	   gmort_vec, pred_rate, drift_variance,
	   migr_eqn, migr_b1fac,
	   vvar,run_length,start_date,smolt_onset,smolt_finish,
	   top_seg, reach_class, rls, sqrt_factor, inriver_latent_mort_ptr, transport_latent_mort_ptr, differential_return_ptr, inriver_return_ptr,
	   transport_return_ptr, overall_return_ptr,
	   post_bonneville_total_ptr, delayed_mortality_table_ptr,
	   custom_survival_using_input_dam_spill)
	int game;
    float	input[];		/* by reservoir step */
    float	input_length[];	/* by reservoir step */
    float	input_dam_spill[];	/* by reservoir step */
    float	input_transport[];	/* by reservoir step */
    float	output[];		/* by reservoir step */
    float	output_length[];/* by reservoir step */
    float	output_dam_spill[];/* by reservoir step */
    float	output_transport[];/* by reservoir step */
	float	output_delayed_survival_rate[];/* by reservoir step */
    float	seg_length;		/* length travelled by fish in miles */
    float	velocity[];		/* by reservoir step */
    float	flow[];		/* by day */
    float   water_temp[];   /* by reservoir step, deg C */
    float   turbidity[];   /* by reservoir step */
    float	volume[];		/* by reservoir step, but no length */
    float	reach_length;		/* miles */
    float	width;			/* feet */
    double  gmort_vec[];    /* by dam time slice. */
    float 	pred_rate[];		/* by reservoir step */
    float	drift_variance;		/* drift variance */
    equation 	*migr_eqn;		/* migration equation */
    float	migr_b1fac[];		/* by step; part of the migration eqn */
    const float	vvar;			/* vvar */
    int		run_length;		/* in reservoir steps */
    int 	start_date;		/* in steps */
    int		smolt_onset;		/* in steps */
    int		smolt_finish;		/* in steps */
    int 	top_seg;		/* top segment in this release? */
    int     reach_class;    /* What type of reach is this? */
    struct release *rls;		/* pointer to this release */
    double  sqrt_factor[];  /* caches calculations for X-T mortality. */
	float	*inriver_latent_mort_ptr;
	float	*transport_latent_mort_ptr;
	float	*differential_return_ptr;
	float	*inriver_return_ptr;
	float	*transport_return_ptr;
	float	*overall_return_ptr;
	float	*post_bonneville_total_ptr;
	delayed_mortality_table	**delayed_mortality_table_ptr;

	/* There is an anomally in that custom survival doesn't use input_dam_spill
	 * if this is the top segment and this segment is bounded at the top by a
	 * dam. */
	int		custom_survival_using_input_dam_spill;
{
    int		i, j, k, l;
	int     xt_pred_mort;   /* flag - 0 = time only; 1 = X-T */
	double	flux;
    double		flx1, flx0; /* Temporaries used to calculate flux */
    double	inp_factor; /* This factor applies some mortality calculations so they need not be repeated */
    double	censored; /* Fish that don't exit the segment during the entire simulation */
    double	*inptr;	/* pointer to arrival dist'n   */
    int		cv_len; /* Number of steps in the cumulative velocity, etc. so that the average can be calculated */
    double		cum_vel; /* Cumulative velocity, needed to calculate average velocity over migration */
    double      cum_temp; /* Cumulative temperature, need for same reason as cum_vel */
    double	mb1fac;
    double 	gmort;
    double  dfact, tfact;  /* Temporaries for X-T predation so they need not be recalculated */
    double  pred[STEPS_IN_RUN];  /* cumulated predation rate, X-T and COMPASS only, used to store cumulative temp. for DO_CUSTOM */
	double	pred_capt[STEPS_IN_RUN]; /* accumulated predator capture distance factor, COMPASS only */
    double	cum_mort[STEPS_IN_RUN]; /* cumulated mortality */
    double	cum_mort_exp[STEPS_IN_RUN][2]; /* ... & exponentials thereof */
    double	cum_flow[DAYS_IN_RUN]; /* accumulated flow for obtaining average flow for DO_CUSTOM */
    double  fish_length[STEPS_IN_RUN]; /* used in growth/migration code. */
    double  p, w, dw, m_cum;
	 /* mu is the migration rate. It is used as an offset for accessing the output array
	  * for when the fish are exiting this river segment. Since it depends on the average
	  * velocity over the migration, mu and cv_len are tightly coupled (since changing the
	  * migration rate changes the average travel time which in turn affects the average
	  * velocity, thus affecting the migration rate) */
    double	mu;
	double	lam;	/* travel time parameter  */
    float   out; /* Temporary that holds the number of fish exiting during a specific day */
    int 	smolt_offset = start_date - smolt_onset;
    int     doing_growth; /* flag, != 0 implies we calculate fish growth. */
    
    static struct vec_table *travel_table = 0;

	double error; /* Error term for survival. The product of deterministic survival and this give the resultant survival */
    
	/* Reset accumulators/base cases */
    cum_mort[0] = 0.0;
    pred[0] = 0.0;
	pred_capt[0] = 0.0;
	cum_flow[0] = 0.0;
    cum_mort_exp[0][0] = 1.0;
    cum_mort_exp[0][1] = 1.0;

    lam = seg_length/sqrt(vvar/STEPS_PER_DAY);
	/* The COMPASS XT (DO_XT) and XT pred/gas models use the distance factor differently
	 * since there is no distance coefficient in the COMPASS version of the XT model */
	if (DO_XT)
		dfact = reach_length * reach_length;
	else
		dfact = reach_length * reach_length 
				   * rls->stock->distance_coeff[reach_class];
    tfact = rls->stock->time_coeff[reach_class] /
                 (STEPS_PER_DAY * STEPS_PER_DAY);

	/* Set flags for optional models */
	xt_pred_mort = (rls->stock->distance_coeff[reach_class] != 0.0)
		             || (rls->stock->time_coeff[reach_class] != 1.0);
    doing_growth = rls->species->pprime_a[reach_class] != 0.0 || 
                        rls->species->pprime_b[reach_class] != 0.0;

    if (! travel_table)
    {
        travel_table = init_travel_table();
	if (!travel_table)
	    log_msg(L_FATAL, "unable to initialize travel table.\n");
    }

    if (DO_GAS_MORT || DO_PRED_MORT) {
		/* Accumulate predation/mortality values */
        for (i = 1; i < run_length; ++i)
        {
	        /* compute simple average of gmort values for the dam slices
	         * in this reach timestep.
	         */
	        gmort = 0;
            if (DO_GAS_MORT) {
	            for (j=0; j < SLICES_IN_STEP; ++j) {
	                gmort += gmort_vec[i*SLICES_IN_STEP + j];
	            }
	            gmort /= SLICES_IN_STEP;
            }
	    
            /* time-based predation mortality is
             *  included in "cum_mort". X-T predation mortality is
             *  both time and distance based and so must be handled
             *  separately
             */
			pred[i] = pred[i-1] + pred_rate[i];
	        cum_mort[i] = cum_mort[i-1]
                + ((!xt_pred_mort ? pred_rate[i] : 0.0) + gmort)
                                / STEPS_PER_DAY;
	        cum_mort_exp[i][0] = exp(cum_mort[i]);
	        cum_mort_exp[i][1] = 1/cum_mort_exp[i][0];
        }
	}
	if (DO_XT) {
		for (i=1; i < run_length; i++) {
			/* pred[] is used differently for the DO_XT model since it's time and distance-dependent */
			pred[i] = pred[i-1] + pred_temp_response(&rls->stock->pred_temp_response_eqn[reach_class], water_temp[i]);

			/* The DO_XT model additionally uses a capture distance equation */
			pred_capt[i] = pred_capt[i-1] + pred_capture_dist(&rls->stock->pred_capture_dist_eqn[reach_class], turbidity[i]);
		}
	}
	else if (DO_CUSTOM) {
		for (i=1; i < run_length; i++) {
			/* pred[] is used to store the cumulative water temperature for DO_CUSTOM
			 * Later, average temperature, theta-bar = (pred[j]-pred[i])/(j-i) */
			pred[i] = pred[i-1] + water_temp[i];
		}
		for (i=1; i < run_length / STEPS_PER_DAY; i++) {
			/* cum_flow[] is used to store the cumulative flow for DO_CUSTOM,
			 * Later, the average flow, assuming j-i > STEPS_PER_DAY is
			(pred[j/STEPS_PER_DAY]-pred[i/STEPS_PER_DAY])/(j/STEPS_PER_DAY - i/STEPS_PER_DAY) */
			cum_flow[i] = cum_flow[i-1] + flow[i];
		}
	}

	/* Sample error term for stochastic reach survival, currently only for XT and
	 * CUSTOM (which is really the new COMPASS model) */
	if (DO_XT || DO_CUSTOM) {
		error = exp(normal() * rls->stock->sigma_d[reach_class] * rls->stock->sigma_d[reach_class] * reach_length);
	}
	else {
		error = 1;
	}

	/* Perform delayed mortality table calculations here if this is the delayed mortality segment */
	if (delayed_mortality_table_ptr) {
		create_delayed_mortality_table(rls, delayed_mortality_table_ptr, game);
		/* TODO: do something with that table */

		/* Calculate return rates */
		if (inriver_latent_mort_ptr && transport_latent_mort_ptr && inriver_return_ptr && transport_return_ptr && differential_return_ptr) {
			int step;
			/* For return rates */
			double inriver_num = 0.0;
			double transport_num = 0.0;

			/* For arrival fractions */
			double inriver_in_total = 0.0;
			double transport_in_total = 0.0;

			double total = 0.0;

			/* The S3 vs. WTT algorithm calculates inriver_latent_mortality from and equation */
			if (run->post_bonneville_hypothesis == POST_BONNEVILLE_S3_VS_WTT) *inriver_latent_mort_ptr = s3_vs_wtt_inriver_latent_mort(&rls->species->inriver_latent_mort_eqn, current->water_travel.time);
			else *inriver_latent_mort_ptr = rls->species->inriver_latent_mort;

			*transport_latent_mort_ptr = rls->species->transport_latent_mort;
			*differential_return_ptr = rls->species->differential_return;

			for (step=0; step < run_length; step++) {
				inriver_num += input[step] * return_prob(&rls->stock->inriver_return_eqn, step + start_date);
				transport_num += input[step] * return_prob(&rls->stock->transport_return_eqn, step + start_date);

				/* Totals for fraction transported/inriver arrivals */
				if (input_transport) {
					transport_in_total += input_transport[step];
					inriver_in_total += input[step] - input_transport[step];
				}
				else {
					inriver_in_total += input[step];
				}
				total += input[step];
			}

			if (total > 0) {

				*inriver_return_ptr = inriver_num / total;
				rls->stats.inriver_return = *inriver_return_ptr;
				*transport_return_ptr = transport_num / total;
		        rls->stats.transport_return = *transport_return_ptr;

				if (overall_return_ptr) {
					const double transport_in_fraction = transport_in_total / total;
					const double inriver_in_fraction = inriver_in_total / total;

					*overall_return_ptr = inriver_in_fraction * (*inriver_return_ptr)
										+ transport_in_fraction * (*transport_return_ptr);
			        rls->stats.overall_return = *overall_return_ptr;
				}

				if (post_bonneville_total_ptr) {
					*post_bonneville_total_ptr = (1.0 - rls->species->inriver_latent_mort) * inriver_in_total
											+ (1.0 - rls->species->transport_latent_mort) * transport_in_total;
			        rls->stats.post_bonneville_total = *post_bonneville_total_ptr;
				}
			}
		}
	}

    /* deal with migration onset issues if this is the first river
     * segment in the release.
     */
    if (top_seg)
    {
	struct rls_seg_data rls_data; /* tmp for migr onset calc below */
	
	/* handle fish which have been released but not smolted.  
	 * we modify the input array, which should therefore be a dummy.
	 */
	if (smolt_finish > start_date)
	{
	    /* prevent div by zero */
	    float smolt_range =
		(smolt_finish == smolt_onset) ? 1.0f : smolt_finish - smolt_onset;

	    /* make everything relative to start_date = 0 */
	    smolt_onset -= start_date;
	    smolt_finish -= start_date;

	    for (i=0; i < smolt_finish && i < run_length-1; ++i)
	    {
		float stuck_fish = 0;

		/* compute probability of migrating */
		float p = (i - smolt_onset) / smolt_range;
		if (p < 0) p = 0;
		else if (p > 1) p = 1;

		/* (1 - p) is the fraction which doesn't migrate, which means
		 * we shove them further down the input array.
		 */
		stuck_fish = (p > 0) ? input[i] * (1-p) : input[i];
        if (!xt_pred_mort)
		    input[i+1] += stuck_fish * cum_mort_exp[i][0] * cum_mort_exp[i+1][1];
        else /* if (xt_pred_mort) */
            input[i+1] += stuck_fish * cum_mort_exp[i][0] * cum_mort_exp[i+1][1]
                            * exp( -sqrt(tfact) * (pred[i+1] - pred[i]) );
		input[i] -= stuck_fish;
	    }
	}
	/* compute some basic stats on this (possibly adjusted) initial
	 * release data.
	 */
	bzero((char *) &rls_data, sizeof(rls_data));
	rls_data.output = input;
	rls_data.stats.done = FALSE; /* mark as not done */
	compute_stats(&rls_data, rls->rls_seg->river_segment,
		      rls->start_date, 0);
	/* rls->stats = rls_data.stats;	 this updates the initial rls stats */
	rls->migr_onset_median = rls_data.stats.total.median;
    }
    censored = 0;

    /*  TOP OF MAIN LOOP. Loop through releases from the upstream site. */
    for (i=0, cv_len=1; i < run_length; ++i)
    {
	float tmig;

	/* the cumulative velocity "cum_vel" is kept as a sum of the
	 * next "cv_len" number of velocities, starting with the
	 * current timestep.  it is initially set and adjusted here,
	 * at the top of the timestep loop.  the formula for the index
	 * of the last velocity in the cumulative is (cv_len + i - 1)
	 */

	if (i==0) {
	    cum_vel = velocity[i];
        cum_temp = water_temp[i];
	    cv_len = 1;
	}
	else {
	    /* adjust the cum_vel by removing the previous day, and
	     * adding a new one, to keep the cv_len the same as in
	     * the previous iteration if possible, accounting for
	     * boundary conditions. Do the same for cumulative water temperature.
	     */
	    cum_vel -= velocity[i-1];
        cum_temp -= water_temp[i-1];
        if (cv_len+i-1 < run_length) {
            cum_vel += velocity[cv_len+i-1];
            cum_temp += water_temp[cv_len+i-1];
        }
	    else
            --cv_len;
	}

	/* if there are no fish, iterate */
	if (input[i] == 0)
	    continue;
	
	flx0 = 0.0;
   	/* cum_mort_exp[x][0] is factored out from the output[x] calc
	 * below, for speed. */
   	inp_factor = input[i] * cum_mort_exp[i][0];
	
	/*  Begin setting mu, the migration rate */

	/*  The parameter mu depends on the average flow during the average */
	/*  travel time. 						    */

	tmig = ((float)(i+start_date) / STEPS_PER_DAY) - rls->migr_onset_median;
	if (tmig < 0)
	    tmig = 0;
	mb1fac = migr_b1fac[i];


	//printf("Min mgr is %f\n",current->min_migr_rate);
	/* Initial calculation of mu, assuming that the cv_len of previous
     *  loop is a good starting point. NOTE: we may or may not take
     *  fish growth into account. */
    if (!doing_growth) {
        mu = seg_length / (compute_migr_eqn(migr_eqn, mb1fac,
				                STEPS_PER_DAY*cum_vel/cv_len,
                                cum_temp/cv_len,
				                tmig, fish_length[i], input_dam_spill[i], (i + start_date) / STEPS_PER_DAY, STEPS_PER_DAY)
			    * drift_variance);
    }
    else {
        /* First, calculate fish length on days of interest, for fish
         *  which entered on day "i" */
        fish_length[i] = input_length[i];
        for (j = i+1; j < cv_len+i; j++) {
            w = length_to_weight( fish_length[j-1] );
            p = pvalue( rls->species->pprime_a[reach_class], 
                        rls->species->pprime_b[reach_class],
                        water_temp[j] );
            dw = growth_fn( w, water_temp[j], p ) / STEPS_PER_DAY;
            fish_length[j] = weight_to_length( w + dw );
        }

        /* Then we calculate mu by adding up the migration values over
         *  the timesteps [i .. cv_len+i-1]. */
        for (j = i, m_cum = 0.0; j < cv_len+i; j++) {
            m_cum += compute_migr_eqn(migr_eqn, mb1fac,
				                STEPS_PER_DAY*velocity[j],
                                water_temp[j],
				                tmig, fish_length[j], input_dam_spill[i], (i + start_date) / STEPS_PER_DAY, STEPS_PER_DAY);
        }
        mu = seg_length * cv_len / (drift_variance * m_cum);
    }


	/* we want the number of steps contained in the cumulative velocity
	 * to be just above mu.  (cv_len+i-1) is the formula for the index of
	 * the last velocity currently used in the cumulative.
	 */
	if (!(cv_len >= mu && cv_len-1 < mu)) { 
	    if (cv_len > mu) {
		/* if cv_len > mu by too much, reduce them and recompute */
		double last_mu;
		while (cv_len > mu && cv_len > 1) {
		    last_mu = mu;
		    cum_vel -= velocity[cv_len+i-1];
		    cum_temp -= water_temp[cv_len+i-1];
		    --cv_len;
            if (!doing_growth) {
    	        mu = seg_length / (compute_migr_eqn(migr_eqn, mb1fac,
				                        STEPS_PER_DAY*cum_vel/cv_len, 
                                        cum_temp/cv_len, tmig, 
                                        fish_length[i], input_dam_spill[i], (i + start_date) / STEPS_PER_DAY, STEPS_PER_DAY)
			            * drift_variance);
            }
            else {
                m_cum -= compute_migr_eqn(migr_eqn, mb1fac,
				                STEPS_PER_DAY*velocity[cv_len+i],
                                water_temp[cv_len+i],
				                tmig, fish_length[cv_len+i], input_dam_spill[cv_len+i], (cv_len+i + start_date) / STEPS_PER_DAY, 
                                STEPS_PER_DAY);
                mu = seg_length * cv_len / (drift_variance * m_cum);
            }
		}
		/* if we stepped below mu (the typical case) bump back up by
		 * one, and use the previously computed mu value.
		 */
		if (cv_len < mu) {
		    ++cv_len;
		    cum_vel += velocity[cv_len+i-1];
		    cum_temp += water_temp[cv_len+i-1];
		    mu = last_mu;
		}
	    }
	    else {
		/* cv_len < mu, so increase */
		while (cv_len < mu && (cv_len+i-1) < run_length) {
		    ++cv_len;
		    cum_vel += velocity[cv_len+i-1];
		    cum_temp += water_temp[cv_len+i-1];
            if (!doing_growth) {
                mu = seg_length / (compute_migr_eqn(migr_eqn, mb1fac,
				                        STEPS_PER_DAY*cum_vel/cv_len, 
                                        cum_temp/cv_len, tmig, 
                                        fish_length[i], input_dam_spill[i], (i + start_date) / STEPS_PER_DAY, STEPS_PER_DAY)
			            * drift_variance);
            }
            else {
                w = length_to_weight( fish_length[cv_len+i-2] );
                p = pvalue( rls->species->pprime_a[reach_class], 
                            rls->species->pprime_b[reach_class],
                            water_temp[cv_len+i-1] );
                dw = growth_fn( w, water_temp[cv_len+i-1], p ) / STEPS_PER_DAY;
                fish_length[cv_len+i-1] = weight_to_length( w + dw );
                m_cum += compute_migr_eqn(migr_eqn, mb1fac,
				                STEPS_PER_DAY*velocity[cv_len+i-1],
                                water_temp[cv_len+i-1],
				                tmig, fish_length[cv_len+i-1], input_dam_spill[cv_len+i-1], (cv_len+i-1 + start_date) / STEPS_PER_DAY, 
                                STEPS_PER_DAY);
                mu = seg_length * cv_len / (drift_variance * m_cum);
            }
		}
	    }
	}
	/*  end setting mu  */

	if (mu < 0)
	{
	    mu = seg_length/0.1;
	    log_msg(L_WARNING, "negative migration rate.  Try increasing Bmin, Bmax, or Bflow.\n");
	}

	/* Here is where the models are applied, note that there are
	 * differences between the deterministic and standard model
	 * that are accounted for with if (vvar <= 0.0) {} blocks.
	 * there is essentially no code duplication now */

	/* Initialize values/tables as necessary */
	if (vvar <= 0.0) {
	    j = (int)(mu) + i + 1;
	}
	else {
	    int erc=0;
	    
	    /* grab trav. time vector	*/
	    inptr = (double *) get_vector(travel_table, mu, lam, &erc); 
	    if (!inptr) {
		if (erc == VECTAB_MEM_ERR)
		    log_msg(L_FATAL,"out of memory error in travel table\n");
		else if (erc == VECTAB_BOUNDS_ERR)
		    log_msg(L_FATAL,"parameter bounds error in travel table\n");
		else
		    log_msg(L_FATAL,"unknown error in travel table\n");
	    }

	    j = i;
	}

	/* May need to lengthen array of fish lengths since they will
	 * need to be known later */
	if (doing_growth) {
		/* Different boundary conditions for deterministic and standard */
		if (vvar <= 0.0) {
			l = j;
		}
		else {
            /* The first element of the vector is the length of the
			 * significant part of the vector
             */
            l = (int)(*inptr + 0.1);  /* beware roundoff error. */
            if (l + i > run_length)
                l = run_length - i;

			l += i;
		}
		for (k = cv_len+i; k <= l; k++) {
			w = length_to_weight( fish_length[k-1] );
			p = pvalue( rls->species->pprime_a[reach_class], 
						rls->species->pprime_b[reach_class],
						water_temp[k] );
			dw = growth_fn( w, water_temp[k], p ) / STEPS_PER_DAY;
			fish_length[k] = weight_to_length( w + dw );
		}
	}
	/* Calculate square root factors (for XT pred mort or DO_XT) if necessary.
	 * Notice that only one square root factor is needed for the
	 * deterministic version */
	if (DO_XT || xt_pred_mort) {
		if (vvar <= 0.0) {
			k = j-i;
		}
		else {
			/* The first element of the vector is the length of the
			 * significant part of the vector
			 */
			k = (int)(*inptr + 0.1);  /* beware roundoff error. */
			if (k >= run_length)
				k = run_length-1;
		}
        while (k >= 0 && sqrt_factor[k] == 0.0) {
            sqrt_factor[k] = sqrt( dfact + tfact*k*k );
			if (vvar <= 0.0) break; /* Only one is needed for deterministic version */

            k--;
        }
	}
	/* Here are the final reservoir survival model calculations */
	do {
		if (vvar <= 0.0) {
			if (j >= run_length) {
				flx1 = 0.0;
				break;
			}

			flux = 1.0;
		}
		else {
			++inptr;
			++j;
			if (j >= run_length) break;
			flx1 = *inptr;
			flux = flx1 - flx0;	    
			flx0 = flx1;
		}
		
		/* Select between possible models */
		if (DO_XT) {
		    out = (flux * input[i])
                * exp( -rls->stock->reach_survival_coef[reach_class]
					*((pred[j]-pred[i])*(pred_capt[j]-pred_capt[i])/(j-i)) * sqrt_factor[j-i] );
		}
		else if (DO_CUSTOM) {
			/* Custom survival is a function of average temperature, average flow, distance, and time */
			int jday = j / STEPS_PER_DAY; /* flow[] is referenced by day */
			int iday = i / STEPS_PER_DAY;
		    out = (flux * input[i])
				* custom_survival(&rls->stock->custom_survival_eqn[reach_class],
					(pred[j]-pred[i])/(j-i), /* pred[] stores cumulative temperature if DO_CUSTOM */
					jday - iday > 0 ? (cum_flow[jday]-cum_flow[iday])/(jday - iday) : 0.0,
					custom_survival_using_input_dam_spill ? input_dam_spill[i] : 0,
					reach_length,
					((float) j-i)/STEPS_PER_DAY);
		}
		else if (!xt_pred_mort) {
            out = (flux * cum_mort_exp[j][1] * inp_factor);
		}
		else /* if (xt_pred_mort) */ {
		    out = (flux * cum_mort_exp[j][1] * inp_factor)
                * exp( -((pred[j]-pred[i])/(j-i)) * sqrt_factor[j-i] );
		}

		if (doing_growth) {
            if (output[j] + out > 0.00001) {
                output_length[j] = (output_length[j] * output[j]
                                     + fish_length[j] * out) 
                                            / (output[j] + out);
			}
		}
		else {
            output_length[j] = input_length[i];
		}

		/* Keep track of spill experienced by fish for Z15 model */
		if (DO_CUSTOM) {
            if (output[j] + out > RESOLUTION) {
                output_dam_spill[j] = (output_dam_spill[j] * output[j]
                                     + input_dam_spill[i] * out) 
                                            / (output[j] + out);
			}
		}
		else {
			output_dam_spill[j] = input_dam_spill[i];
		}

		/* Calculate mortality rate for fish that are delayed at the downstream dam */
		if (output_delayed_survival_rate != NULL) {
			double current_delayed_survival_rate = log(out/input[i]) / (((double)(j-i))/STEPS_PER_DAY); /* Calculation for current time step */

            if (output[j] + out > RESOLUTION) {
                output_delayed_survival_rate[j] = (output_delayed_survival_rate[j] * output[j]
                                     + current_delayed_survival_rate * out) 
                                            / (output[j] + out);
			}
		}

		output[j] += out * error;
		if (input_transport != NULL) output_transport[j] += out * error * input_transport[i] / input[i]; /* This keeps track of passage of transported vs. inriver fish at a transport destination */

		if (vvar <= 0.0) {
			flx1 = 1.0;
			break; /* deterministic model only requires 1 output */
		}
    } while (*inptr < 1.0);

	/* track any fish which went off the end - these censored fish use 
     *  the predation from the last time step. The length of censored
     *  fish is not recorded.
	 */
	if (DO_XT) {
        if (i == run_length-1) {
            if (sqrt_factor[1] == 0.0)
                sqrt_factor[1] = sqrt( dfact + tfact );
			censored += input[i]
				* exp( -rls->stock->reach_survival_coef[reach_class]
				*((pred[run_length-1]-pred[run_length-2])*(pred_capt[run_length-1]-pred_capt[run_length-2])) * sqrt_factor[1] );
		}
        else if (flx1 < 1.0) {
           if (sqrt_factor[(run_length-1)-i] == 0.0)
                sqrt_factor[(run_length-1)-i] = 
                                sqrt( dfact + tfact*((run_length-1)-i)
                                                *((run_length-1)-i) );
	        censored += (1.0 - flx1) * input[i]
				* exp( -rls->stock->reach_survival_coef[reach_class]
				*((pred[run_length-1]-pred[i])*(pred_capt[run_length-1]-pred_capt[i])/((run_length-1)-i)) * sqrt_factor[(run_length-1)-i] );
        }
	}
	else if (DO_CUSTOM) {
		if (i == run_length-1) {
			/* Custom survival is a function of average temperature, average flow, distance, and time */
			int jday = (run_length-1) / STEPS_PER_DAY; /* flow[] is referenced by day */
			int iday = (run_length-2) / STEPS_PER_DAY;
			out = input[i]
				* custom_survival(&rls->stock->custom_survival_eqn[reach_class],
					(pred[run_length-1]-pred[run_length-2]),
					jday - iday > 0 ? (cum_flow[jday]-cum_flow[iday])/(jday - iday) : 0.0,
					custom_survival_using_input_dam_spill ? input_dam_spill[i] : 0,
					reach_length,
					1);
		}
		else if (flx1 < 1.0) {
			/* Custom survival is a function of average temperature, average flow, distance, and time */
			int jday = (run_length-1) / STEPS_PER_DAY; /* flow[] is referenced by day */
			int iday = i / STEPS_PER_DAY;
			out = ((1.0 - flx1) * input[i])
				* custom_survival(&rls->stock->custom_survival_eqn[reach_class],
					(pred[run_length-1]-pred[i])/((run_length-1)-i), /* pred[] stores cumulative temperature if DO_CUSTOM */
					jday - iday > 0 ? (cum_flow[jday]-cum_flow[iday])/(jday - iday) : 0.0,
					custom_survival_using_input_dam_spill ? input_dam_spill[i] : 0,
					reach_length,
					((float) (run_length-1)-i)/STEPS_PER_DAY);
		}
	}
    else if (!xt_pred_mort) {
	    if (i == run_length-1)
	        censored += input[i] * cum_mort_exp[run_length-2][0]
		    * cum_mort_exp[run_length-1][1];
	    else if (flx1 < 1.0)
	        censored += (1.0 - flx1) * inp_factor * cum_mort_exp[run_length-1][1];
    }
    else /* if (xt_pred_mort) */ {
        if (i == run_length-1) {
            if (sqrt_factor[1] == 0.0)
                sqrt_factor[1] = sqrt( dfact + tfact );
	        censored += input[i] * cum_mort_exp[run_length-2][0]
		    * cum_mort_exp[run_length-1][1]
            * exp( -(pred[run_length-1]-pred[run_length-2])
                      * sqrt_factor[1] );
        }
        else if (flx1 < 1.0) {
           if (sqrt_factor[(run_length-1)-i] == 0.0)
                sqrt_factor[(run_length-1)-i] = 
                                sqrt( dfact + tfact*((run_length-1)-i)
                                                *((run_length-1)-i) );
	        censored += (1.0 - flx1) * inp_factor * cum_mort_exp[run_length-1][1]
                * exp( -((pred[run_length-1]-pred[i])/((run_length-1)-i))
                           * sqrt_factor[(run_length-1)-i] );
        }
    }
    }
    return(censored);
}

