/** \file
 *
 *	\brief support routines for spill calculations
 */

#include <math.h>
#include <stdlib.h>
#include "crisp.h"
#include "period.h"
#include "spill.h"
#include "log.h"
#include "sample.h"
#include "phouse.h"
#include "rsw.h"
#include "crisp_time.h"

#define SPILL_NIGHT_END	6
#define SPILL_NIGHT_START	18

/* macro to limit sample to 0 <= x <= 1
 * this is kludgy, but in this macro we assign to a local var first
 * to avoid multiple analysis of the expression.  a good compiler would
 * handle this, but we'd better not count on it.  worse, we assume a
 * local double tmp_fl to be declared within the scope of the macro.
 */
#define prob(x)	(tmp_fl = (x), tmp_fl < 0.0 ? 0 : tmp_fl > 1.0 ? 1 : tmp_fl)

/**
 *	computes the spill probability for the given day and dam
 *	time slice for the given dam and species.
 */

double	spill_prob(dm, spc, day, time)
    struct	dam	*dm;
    int		spc;
    int 	day, time;
{
    register double x;
    register double err;
    register double tmp_fl;		/* ugh.  var for use by prob() macro */
    struct dam_species *dmspc = &(dm->species[spc]);

    /* change range to 0<= x <=100 */
    x = dm->spill[day*DAM_TIME_SLC + time] * 100.0;
	
    if (x == 0.0)
	return(0);


    err = sqrt(dmspc->spill_eqn.param[2].val) * normal();
    switch(dmspc->spill_eqn.id)
    {
	case EQ_RSW:
	{
		const float a = dmspc->spill_eqn.RSW_A_VAL;
		const float b = dmspc->spill_eqn.RSW_B_VAL;

		const double exp_term = exp(a + b*log(x/(1 - x)));
		return exp_term / (1 + exp_term);
	}
	case EQ_SPILL_EFFICIENCY:
	{
		double y = dm->flow[day*DAM_TIME_SLC + time];
		const float B0 = dmspc->spill_eqn.param[0].val;
		const float B1 = dmspc->spill_eqn.param[1].val;
		const float B2 = dmspc->spill_eqn.param[2].val;
		const float B3 = dmspc->spill_eqn.param[3].val;
		x = dm->spill[day*DAM_TIME_SLC + time]; /* 0.0-1.0, not 0-100 */
		if (x <= 0.0) return 0.0;
		if (x >= 1.0) return 1.0;
		else {
			const double logit = exp(B0 + (B1*log(x/(1 - x))) + (B2*y) + (B3*log(x/(1 - x))*y));
			return logit / (1 + logit);
		}
	}
    case EQ_EXP0:
	return(prob(dmspc->spill_eqn.param[1].val
		    * exp(dmspc->spill_eqn.param[0].val * x + err)/100));
		       
    case EQ_POW0:
	return(prob(dmspc->spill_eqn.param[1].val
		    * (pow(x, dmspc->spill_eqn.param[0].val + err))/100));
	
    case EQ_LIN2:
    case EQ_EXP2:
    case EQ_POW2:
	err = x*err;
	break;
		    
    case EQ_LIN3:
    case EQ_EXP3:
    case EQ_POW3:
	err = x*x*err;
	break;
	
    default:
	break;
    }

    switch(dmspc->spill_eqn.id)
    {
    case EQ_LIN0:
    case EQ_LIN1:
    case EQ_LIN2:
    case EQ_LIN3:
	return(prob((dmspc->spill_eqn.param[0].val
		     + dmspc->spill_eqn.param[1].val * x + err)/100));

    case EQ_EXP1:
    case EQ_EXP2:
    case EQ_EXP3:
	return(prob((dmspc->spill_eqn.param[1].val
		     * exp(dmspc->spill_eqn.param[0].val * x) + err)/100));

    case EQ_POW1:
    case EQ_POW3:
	return(prob((dmspc->spill_eqn.param[1].val
		     * pow(x,dmspc->spill_eqn.param[0].val) + err)/100));
    case EQ_POW2:
	return(prob((dmspc->spill_eqn.param[1].val
		     * pow(x/100.0,dmspc->spill_eqn.param[0].val) + err)/100));

    case EQ_LOG0:
	return(prob((dmspc->spill_eqn.param[0].val
		     + dmspc->spill_eqn.param[1].val * log(x) + err)/100));

    case EQ_CUB0:
    {
	float a = dmspc->spill_eqn.param[0].val;
	float b = dmspc->spill_eqn.param[1].val;
	float c = dmspc->spill_eqn.param[2].val;
	err = sqrt(dmspc->spill_eqn.param[3].val) * normal();

	return(prob((a*x + b*x*x + c*x*x*x + err)/100));
    }
    case EQ_CUB1:
    {
	float a = dmspc->spill_eqn.param[0].val;
	float b = dmspc->spill_eqn.param[1].val;
	float c = dmspc->spill_eqn.param[2].val;
	err = sqrt(dmspc->spill_eqn.param[3].val) * normal() / 100.0;

	return prob((a*x/100 + b*x*x/10000 + c*x*x*x/1000000) + err);
    }

    default:
		log_msg(L_ERROR, "Invalid spill efficiency equation number\n");
	return(0);
    }
}

/**
 *	set the planned_spill[] item in the specified dam
 *	based on the spill schedules at that dam.  for each day in 
 *	planned_spill_days, get the value (spill fraction) from that
 *	period list item.  this represents the instantaneous spill.
 *	all corresponding slices in spill[] are then set to day or night
 *	values depending on if the slice is during the day or night.
 *
 * note: perhaps the spill value should represent the average daily spill,
 *	instead of the instantaneous spill.  if so, we will change that later
 *	to pro-rate it into the proper spill hours.
 */

void set_dam_planned_spill(seg)
    struct river_segment *seg;
{
    struct dam *dam = &seg->current->dam;
    PERIOD_LIST *plan;
    int slice;
    int day;
    
    bzero((char *) dam->planned_spill, sizeof(dam->planned_spill));

	/* Fill in planned spill (day) values */
    for (plan = dam->planned_spill_day; plan; plan = plan->next)
    {
		if (plan->value > 1.0)
		{
			plan->value = 1.0;
			log_msg(L_ERROR, "excessive planned spill (day) at %s, setting to 1.0\n",
				seg->seg_name);
		}
		for (day = plan->start; day <= plan->end && day < DAYS_IN_SEASON; day++) {
			for (slice = day*DAM_TIME_SLC; slice < (day+1)*DAM_TIME_SLC; slice++) {
				if (!is_night(slice % DAM_TIME_SLC)) dam->planned_spill[slice] = plan->value;
			}
		}
    }

	/* Fill in planned spill (night) values */
    for (plan = dam->planned_spill_night; plan; plan = plan->next)
    {
		if (plan->value > 1.0)
		{
			plan->value = 1.0;
			log_msg(L_ERROR, "excessive planned spill (night) at %s, setting to 1.0\n",
				seg->seg_name);
		}
		for (day = plan->start; day <= plan->end && day < DAYS_IN_SEASON; day++) {
			for (slice = day*DAM_TIME_SLC; slice < (day+1)*DAM_TIME_SLC; slice++) {
				if (is_night(slice % DAM_TIME_SLC)) dam->planned_spill[slice] = plan->value;
			}
		}
    }
}

/**
 *	run through the dam list and set all the spill from the schedules.
 *	this may be overridden at some dams in monte mode.
 */

void set_planned_spill()
{
    int i;

    for (i = 0; i < num_dams; i++) {
		set_legacy_spill(dams[i]); /* Convert from legacy data structure to the new one (this does nothing if no legacy data structures were input) */
		set_dam_planned_spill(dams[i]);
	}
}

/** \brief Converts legacy (fish spill hours) spill schedules to the new day/night format. */
void set_legacy_spill(struct river_segment *seg) {
    struct dam *dam = &seg->current->dam;
    PERIOD_LIST *plan;
    PERIOD_LIST *hours;
    PERIOD_LIST *day = NULL;
    PERIOD_LIST *night = NULL;

	/* Don't overwrite anything if we're already using the new format */
	if (dam->planned_spill_day || dam->planned_spill_night) {
		if (dam->legacy_planned_spill || dam->legacy_fish_spill) {
			log_msg(L_ERROR, "Mixed new and legacy format for spill schedule at %s, using new format only\n", seg->seg_name);
		}
		return;
	}

	for (plan = dam->legacy_planned_spill, hours = dam->legacy_fish_spill; hours && plan; hours = hours->next) {
		do {
			/* Keep track of sums of fish spill hours */
			PERIOD_LIST *spill_hours;
			int day_hours = 0;
			int night_hours = 0;
			int period_start = hours->start >= plan->start ? hours->start : plan->start;
			int period_end = hours->end <= plan->end ? hours->end : plan->end;

			/* Create a new period for the day and night spill */
			{
				PERIOD_LIST *last_day = day;
				day = (PERIOD_LIST*) malloc(sizeof(PERIOD_LIST));
				if (last_day) last_day->next = day;
				day->prev = last_day;
				day->sub_periods = NULL;
				day->start = period_start;
				day->end = period_end;
				day->next = NULL;
			}

			{
				PERIOD_LIST *last_night = night;
				night = (PERIOD_LIST*) malloc(sizeof(PERIOD_LIST));
				if (last_night) last_night->next = night;
				night->prev = last_night;
				night->sub_periods = NULL;
				night->start = period_start;
				night->end = period_end;
				night->next = NULL;
			}

			/* Sum hours during day and night */
			for (spill_hours = hours->sub_periods; spill_hours; spill_hours = spill_hours->next) {
				int hour;
				for (hour = spill_hours->start; hour < spill_hours->end; hour++) {
					if (hour < SPILL_NIGHT_END || hour >= SPILL_NIGHT_START) night_hours++;
					else if (hour >= SPILL_NIGHT_END && hour < SPILL_NIGHT_START) day_hours++;
				}
			}

			if (plan->value <= 0.0f || (day_hours <= 0 && night_hours <= 0)) {
				day->value = 0.0f;
				night->value = 0.0f;
			}
			else {
				day->value = plan->value * day_hours / 12.0f;
				night->value = plan->value * night_hours / 12.0f;

				/* Move extra spill to the other period if necessary */
				if (day->value > 1.0f) {
					night->value += day->value - 1.0f;
					day->value = 1.0f;
					log_msg(L_ERROR, "Too much day spill (%s: %d to %d), spill at 100% during the day, remaining spill added to night\n", seg->seg_name, period_start, period_end);
				}
				else if (night->value > 1.0f) {
					day->value += night->value - 1.0f;
					night->value = 1.0f;
					log_msg(L_ERROR, "Too much night spill (%s: %d to %d), spill at 100% during the day, remaining spill added to day\n", seg->seg_name, period_start, period_end);
				}

				if (day->value > 1.0f || night->value > 1.0f) {
					log_msg(L_ERROR, "Too much spill from legacy inputs (%s: %d to %d)\n", seg->seg_name, period_start, period_end);
				}
			}

			/* Move to next planned spill proportion if necessary */
			if (plan->end <= hours->end) plan = plan->next;
		} while (plan && plan->end <= hours->end);
	}

	/* Replace NULL planned_spill_day/night here */
	for (; day && day->prev != NULL; day = day->prev);
	dam->planned_spill_day = day;
	for (; night && night->prev != NULL; night = night->prev);
	dam->planned_spill_night = night;

	/* Remove legacy data now */
   destroy_period_struct(dam->legacy_planned_spill);
   dam->legacy_planned_spill = NULL;
   destroy_period_struct(dam->legacy_fish_spill);
   dam->legacy_fish_spill = NULL;
}

/**
 *	this routine sets the final spill and powerhouse fractions
 *	for the given dam.  done on a per day basis.  assumes
 * 	that flow and planned spill are already in place.  handles multiple
 *	powerhouses, assuming that they are already configured and scheduled
 *	properly.  the algorithm for allocating phouse flow is:
 *		1) start with lowest (best) priority powerhouse (priority 0)
 *		2) fill to flow_min (otherwise emit warning about not enough flow)
 *		3) spill to planned spill
 *		4) add flow until flow_min for next phouse
 *		5) first phouse up to hydraulic capacity
 *		6) second phouse to hydraulic capacity
 *		7) spill until spill cap
 *		8) spill remaining and emit warning of exceeding spill cap
 */

void set_dam_flow_routing(dam, day)
    struct river_segment *dam;
    int day;
{
    int i;
    int slice;
    float remaining_flow;
    struct river_segment *phouse;
    struct dam *damptr = DAMPTR(dam);
    
    for (slice = day*DAM_TIME_SLC; slice < (day+1)*DAM_TIME_SLC; slice++)
    {
	damptr->spill[slice] = 0.0;
	for (phouse = dam; phouse; phouse = DAMPTR(phouse)->additional_pwrhs) DAMPTR(phouse)->flow_fraction[slice] = 0.0;
	if (damptr->flow[day] <= 0.0)
	    continue;
	remaining_flow = damptr->flow[day];

	i = 0;
	phouse = find_priority_phouse(dam, i);
	while (remaining_flow > 0.0 && phouse != NULL) {
		struct river_segment *next_phouse = find_priority_phouse(dam, i + 1);

		if (dam_slice_in_schedule(DAMPTR(phouse)->phouse_schedule, slice)) {
			/* Flow min at this powerhouse */
			if (remaining_flow < DAMPTR(phouse)->flow_min_project) {
				/* Not enough flow through the dam */
				/* Note that the powerhouse numbers in this error message start at 1 (main dam), so a secondary powerhouse is powerhouse 2 */
				log_msg(L_WARNING, "Not enough flow on day %d through %s for powerhouse %d (flow_min not reached!)\n", day, dam->seg_name, i + 1);
				DAMPTR(phouse)->flow_fraction[slice] += remaining_flow / damptr->flow[day];
				remaining_flow = 0;
				break;
			}

			DAMPTR(phouse)->flow_fraction[slice] += DAMPTR(phouse)->flow_min_project / damptr->flow[day];
			remaining_flow -= DAMPTR(phouse)->flow_min_project;
	
			if (i == 0) {
				/* Main powerhouse only */
	
				/* Spill up to the planned spill fraction */
				if (remaining_flow < damptr->planned_spill[slice] * damptr->flow[day]) {
					damptr->spill[slice] += remaining_flow / damptr->flow[day];
					remaining_flow = 0.0;
				}
				else {
					damptr->spill[slice] += damptr->planned_spill[slice];
					remaining_flow -= damptr->planned_spill[slice] * damptr->flow[day];
				}
			}
	
			/* If there isn't enough flow to turn on the next powerhouse (or there is no next powerhouse), fill this one to hydraulic capacity.
			 * Additionally, if there IS more than the next powerhouse's flow_min, fill this one to hydraulic capacity */
			if (remaining_flow > 0.0) {
				if ((next_phouse == NULL || !dam_slice_in_schedule(DAMPTR(next_phouse)->phouse_schedule, slice)) || remaining_flow < DAMPTR(next_phouse)->flow_min_project) {
					if (remaining_flow < (DAMPTR(phouse)->powerhouse_capacity - DAMPTR(phouse)->flow_min_project)) {
						DAMPTR(phouse)->flow_fraction[slice] += remaining_flow / damptr->flow[day];
						remaining_flow = 0.0;
					}
					else {
						DAMPTR(phouse)->flow_fraction[slice] += (DAMPTR(phouse)->powerhouse_capacity - DAMPTR(phouse)->flow_min_project) / damptr->flow[day];
						remaining_flow -= DAMPTR(phouse)->powerhouse_capacity - DAMPTR(phouse)->flow_min_project;
					}
				}
				else if (next_phouse != NULL && dam_slice_in_schedule(DAMPTR(next_phouse)->phouse_schedule, slice) && remaining_flow > DAMPTR(next_phouse)->flow_min_project) {
					const float excess = remaining_flow - DAMPTR(next_phouse)->flow_min_project;
					if (excess > DAMPTR(phouse)->powerhouse_capacity - DAMPTR(phouse)->flow_min_project) {
						DAMPTR(phouse)->flow_fraction[slice] += (DAMPTR(phouse)->powerhouse_capacity - DAMPTR(phouse)->flow_min_project) / damptr->flow[day];
						remaining_flow -= DAMPTR(phouse)->powerhouse_capacity - DAMPTR(phouse)->flow_min_project;
					}
					else {
						DAMPTR(phouse)->flow_fraction[slice] += excess / damptr->flow[day];
						remaining_flow -= excess;
					}
				}
				else {
				}
			}
		}

		++i;
		phouse = next_phouse;
	}

	if (remaining_flow > 0.0) {
		/* Spill extra flow */
		damptr->spill[slice] += remaining_flow / damptr->flow[day];
		remaining_flow = 0.0;

		if (damptr->spill[slice] * damptr->flow[day] + remaining_flow <= damptr->spill_cap) {
			log_msg(L_DBGMSG, "More than planned spill on day %d at %s due to high flow\n", day, dam->seg_name);
		}
		else {
			/* Too much flow and no place to put it! */
			log_msg(L_WARNING, "Too much flow on day %d through %s (spill cap exceeded!)\n", day, dam->seg_name);
		}
	}
    }
}

/**
 *	remove spill scheds from all dams.
 */

void remove_all_spill_scheds()
{
   struct river_segment *seg;

   for (seg = river_mouth; seg; seg = seg->next)
   {
       if (seg->seg_type == DAM_SEG)
       {
	   destroy_period_struct(DAMPTR(seg)->planned_spill_day);
	   DAMPTR(seg)->planned_spill_day = NULL;
	   destroy_period_struct(DAMPTR(seg)->planned_spill_night);
	   DAMPTR(seg)->planned_spill_night = NULL;
	   destroy_period_struct(DAMPTR(seg)->legacy_planned_spill);
	   DAMPTR(seg)->legacy_planned_spill = NULL;
	   destroy_period_struct(DAMPTR(seg)->legacy_fish_spill);
	   DAMPTR(seg)->legacy_fish_spill = NULL;
       }
   }
}

