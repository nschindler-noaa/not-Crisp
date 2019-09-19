#include "crisp.h"
#include "passage.h"
#include "pred.h"
#include "spill.h"
#include "rsw.h"
#include "crisp_time.h"
#include "delay.h"

/* defintion to convert day and time (dam slice) to STEPS_IN_SEASON index */
#define SEASON_STEP(day, time) ((int)(STEPS_PER_DAY * 			\
				      (day+ (float)time/DAM_TIME_SLC)))

extern float compute_fge();
extern float compute_fge_new();

/** Estimate dam passage */
void pass_surv(struct river_segment *seg, int spc, StockT *stock, int day,
	int time, double collection_eff, double smolt_age,
	struct dam_surv *dmprb, int tracking_flag, int delay)
{
	struct dam *dm = &seg->current->dam;
	struct dam_species *dmspc = &dm->species[spc];
	struct dam *phouse;

    int season_step = SEASON_STEP(day, time);
    int slice = day*DAM_TIME_SLC + time;
    double total_powerhouse_flows;

	double spillway_prob,		non_spillway_prob;

	/* The delay on these routes must be averaged so that they may differ by powerhouse */
	dmprb->bypass_prob = 0;
	dmprb->bypass_surv = 0;
	dmprb->bypass_extra = 0;
	dmprb->bypass_day = 0;
	dmprb->sluiceway_prob = 0;
	dmprb->sluiceway_surv = 0;
	dmprb->sluiceway_extra = 0;
	dmprb->sluiceway_day = 0;
	dmprb->turbine_prob = 0;
	dmprb->turbine_surv = 0;
	dmprb->turbine_extra = 0;
	dmprb->turbine_day = 0;

	/* Forebay predation */
    if (DO_PRED_MORT) {
		int rc;
		double forebay_pred_activity;
		double tailrace_pred_activity;
		double pred_dens;
	    double dtemp;

        /* forebay predation */
        forebay_pred_activity = dmspc->pred[time]
	    * 0.0000001 * specs[spc].forebay_pcoef; 
    
        SET_PRED_DENS(&pred_dens,
                     dm->species[spc].pred_pop_mean, 
                     dm->drop_ratio[day]);
        /* NOTE we assume the upstream segment is a reach. */
        rc = seg->up->current->reach.rclass;
        dtemp = -pred_rate(forebay_pred_activity, pred_dens,
                               dm->water_temp[season_step],
				               &(stock->pred_temp_response_eqn[rc]));
        dmprb->pred_surv = exp(dtemp/DAM_TIME_SLC);
        dmprb->pred_quarter_surv = exp( dtemp / (4 * DAM_TIME_SLC) );
        dmprb->fb_pred_surv 
                = exp( dtemp * dmspc->mean_forebay_transit_time / 24 );    

        /* tailrace predation */
	    tailrace_pred_activity = dmspc->pred[time]
	        * 0.0000001 * specs[spc].tailrace_pcoef; 
	    SET_PRED_DENS(&pred_dens,
                      dm->species[spc].pred_pop_tailrace,
                      dm->tr_drop_ratio[day]);

	    /* the tailrace predation calculation is scaled to the length
         * of time in days that the flow from this timestep is in the
         * tailrace.  Recall that fish are always passed through the
         * dam (and tailrace) in one timestep, regardless of the
         * results of this apparent residence calculation.
         *  NOTE the downstream segment is also (assumed to be) a reach.
	     */
        rc = seg->down->current->reach.rclass;
	    dmprb->tr_pred_surv =
	        exp(-pred_rate(tailrace_pred_activity, pred_dens, dm->water_temp[season_step],
			          &(stock->pred_temp_response_eqn[rc])));
    
    }
    else /* not doing DO_PRED_MORT */
        dmprb->pred_surv = dmprb->pred_quarter_surv 
              = dmprb->fb_pred_surv = dmprb->tr_pred_surv = 1.0;

	/* Fish entering the dam are first either spilled or not spilled */
	spillway_prob = spill_prob(dm, spc, day, time);
	non_spillway_prob = 1.0 - spillway_prob;

	/* Spilled fish either go through RSWs or conventional spillways */
	dmprb->rsw_prob = spillway_prob * rsw_prob(dm, spc, day, time);
	dmprb->rsw_surv = dmprb->rsw_prob * (1.0 - dmspc->rsw_mort);

	dmprb->spill_prob = spillway_prob - dmprb->rsw_prob;
	dmprb->spill_surv = dmprb->spill_prob * (1.0 - dmspc->spill_mort);



	/* Get flow fraction for all powerhouses */
	total_powerhouse_flows = 0;
	for (phouse = dm; phouse; phouse = (phouse->additional_pwrhs) ? &phouse->additional_pwrhs->current->dam : NULL) total_powerhouse_flows += phouse->flow_fraction[slice];

	/* Fish that aren't spilled pass through the powerhouses */
    for (phouse = dm; phouse; phouse = (phouse->additional_pwrhs) ? &phouse->additional_pwrhs->current->dam : NULL) {
		double sluiceway_prob,		non_sluiceway_prob;
		double fge;

		double psluiceway_prob,		psluiceway_surv;
		double pbypass_prob,		pbypass_surv;
		double pturbine_prob,		pturbine_surv;
		double flow_fraction, flow_through_phouse;

		/* flow_fraction is used to partition fish between powerhouses, so all that matters is the flow through this powerhouse relative to ALL the powerhouses at this dam (which is stored in total_powerhouse_flows) */
		flow_fraction = phouse->flow_fraction[slice] / total_powerhouse_flows;
		dmspc = &(phouse->species[spc]);
		if (flow_fraction == 0.0) continue;
		//NEW FGE EQUATION
		if(dmspc->fge_eqn.id == 22){
			if (dm->forebay_depth[day] >= dm->bypass_height) {
				flow_through_phouse = phouse->flow[day]*(1-phouse->spill[slice])*flow_fraction;
				fge = compute_fge_new(&dmspc->fge_eqn, is_night(time), flow_through_phouse, day, phouse->water_temp[season_step]);
				/*if (current->flags & AGE_DEP_FGE_FLAG) {
					fge = compute_fge(fge, dm->flow[day*DAM_TIME_SLC + time], day, dm->water_temp[day*DAM_TIME_SLC + time]);
				}*/
			}
		}
		
		//OLD FGE EQUATION
		else{
			if (dm->forebay_depth[day] >= dm->bypass_height) {
				if (is_night(time)) fge = dmspc->night_fge;
				else fge = dmspc->day_fge;
	
				if (current->flags & AGE_DEP_FGE_FLAG) {
					fge = compute_fge(fge, &dmspc->fge_eqn, smolt_age, dm->fp_fb_depth - dm->forebay_depth[day]);
				}
			}
		}

		sluiceway_prob = dmspc->sluiceway_prop;
		non_sluiceway_prob = 1.0 - sluiceway_prob;
		
		/* Sluiceways are taken into account BEFORE fish go to turbine/bypass */
		psluiceway_prob = non_spillway_prob * flow_fraction * sluiceway_prob;
		psluiceway_surv = psluiceway_prob * (1.0 - dmspc->sluiceway_mort);
		dmprb->sluiceway_prob += psluiceway_prob;
		dmprb->sluiceway_surv += psluiceway_surv;

		pbypass_prob = non_spillway_prob * flow_fraction * non_sluiceway_prob * fge;
		pbypass_surv = pbypass_prob * (1.0 - dmspc->bypass_mort) * (1.0 - collection_eff);
		dmprb->bypass_prob += pbypass_prob;
		dmprb->bypass_surv += pbypass_surv;

		pturbine_prob = non_spillway_prob * flow_fraction * non_sluiceway_prob * (1.0 - fge);
		pturbine_surv = pturbine_prob * (1.0 - dmspc->turbine_mort);
		dmprb->turbine_prob += pturbine_prob;
		dmprb->turbine_surv += pturbine_surv;

		dmprb->turbine_extra += pturbine_surv * dmspc->turbine_delay;
		dmprb->turbine_day += pturbine_surv * dmspc->turbine_day;

		dmprb->sluiceway_extra += psluiceway_surv * dmspc->sluiceway_delay;
		dmprb->sluiceway_day += psluiceway_surv * dmspc->sluiceway_day;

		dmprb->bypass_extra += pbypass_surv * dmspc->bypass_delay;
		dmprb->bypass_day += pbypass_surv * dmspc->bypass_day;

		/* Divide the delay values to yield weighted averages */
		if (pbypass_surv) {
			dmprb->bypass_extra /= pbypass_surv;
			dmprb->bypass_day /= pbypass_surv;
		}
		if (psluiceway_surv) {
			dmprb->sluiceway_extra /= psluiceway_surv;
			dmprb->sluiceway_day /= psluiceway_surv;
		}
		if (pturbine_surv) {
			dmprb->turbine_extra /= pturbine_surv;
			dmprb->turbine_day /= pturbine_surv;
		}
	}

	dmspc = &dm->species[spc];

    /* set collection probability, for transportation */
    dmprb->collection_prob = dmprb->bypass_prob * collection_eff;
    
    /* compute passage prob; note that total dam flow is used */
    if (delay) dmprb->pass_prob = 1.0 - dam_delay_prob(seg, &dmspc->delay_eqn, day, slice,
					    season_step);

}
