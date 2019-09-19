#include "delayed_mort.h"
#include "crisp.h"
#include "log.h"
#include <stdlib.h>

/* The functions below here calculate the delayed mortality table which is used to calculated delayed mortality */

/* Recursive function for calculating inriver bypass experience probabilities */
static void calculate_spill_experience(double *inriver_probs, struct rls_seg **dam_rsegs, double *bypass_probs, int game, int num_collector_dams, double prob, int num_bypasses, int dam_index) {
	if (dam_index >= num_collector_dams) {
		/* Base case */
		inriver_probs[num_bypasses] += prob;
	}
	else {
		/* Recursive cases, first if fish aren't bypassed here and second if they are */		
		calculate_spill_experience(inriver_probs, dam_rsegs, bypass_probs, game, num_collector_dams, prob * (1 - bypass_probs[dam_index]), num_bypasses, dam_index + 1);
		calculate_spill_experience(inriver_probs, dam_rsegs, bypass_probs, game, num_collector_dams, prob * bypass_probs[dam_index], num_bypasses + 1, dam_index + 1);
	}
}

void create_delayed_mortality_table(struct release *release, delayed_mortality_table **d, int game) {
	int num_collector_dams; /* Number of dams this release can pass through before getting to the delayed mortality calculation point */
	struct rls_seg *rseg; /* Temporary for locating path */

	struct rls_seg **dam_rsegs; /* Array of pointers to dam release segments so that data may be obtained from them */
	int dam_index; /* Index of the dam we are processing */

	/* Count dams */
	for (rseg = release->rls_seg, num_collector_dams = 0; rseg && rseg->river_segment != default_transport_target; rseg = rseg->down) {
		if (rseg->river_segment->seg_type == DAM_SEG && rseg->river_segment->current->dam.collector) {
			/* Check to make sure that the proper tracking data exists, otherwise this can't be properly calculated */
			/* TODO: SOME CODE SHOULD AUTOMATICALLY ENABLE THE NECESSARY TRACKING!!! */
			if (rseg->data[game]->bypass_in == NULL || rseg->data[game]->transport_arrived == NULL) {
				log_msg(L_ERROR, "Tracking is not turned on at %s, so delayed mortality calculations could not be performed!\n", rseg->river_segment->seg_name);
				return;
			}

			num_collector_dams++;
		}
	}
	if (rseg == NULL) {
		log_msg(L_ERROR, "Error calculating delayed mortality. The selected release doesn't pass through the transport destination\n");
		return;
	}

	if (*d) {
		delayed_mortality_table_free(*d);
		*d = NULL;
	}

	/* Get pointers to the dam release segments */
	dam_rsegs = malloc(num_collector_dams * sizeof(struct dam*));
	for (dam_index=0, rseg = release->rls_seg; rseg && rseg->river_segment != default_transport_target; rseg = rseg->down) {
		if (rseg->river_segment->seg_type == DAM_SEG && rseg->river_segment->current->dam.collector) dam_rsegs[dam_index++] = rseg;
	}

	/* Construct the table with a column for each dam and for each possible number of bypass experiences */
	*d = (delayed_mortality_table*) malloc(sizeof(delayed_mortality_table));
	if (!(*d)) return;

	(*d)->num_collector_dams = num_collector_dams;

	/* Note that all values are initialized to zero */
	(*d)->columns = (struct delayed_mortality_column*) calloc(num_collector_dams, sizeof(struct delayed_mortality_column));
	if (!(*d)->columns) return;

	/* Fill in the dam columns (the data is indexed by model step, so it must be
	 * accumulated to get values for an entire day). */
	for (dam_index=0; dam_index < num_collector_dams; dam_index++) {
		int day;

		/* Only do these calculations if this dam is transporting, otherwise all values are zero */
		if (dam_rsegs[dam_index]->river_segment->current->dam.transport) {
			for (day=0; day < DAYS_IN_SEASON; day++) {
				double transport_arrived = 0.0;
				double total = 0.0;
				int step_in_day;

				/* Accumulate over a day */
				for (step_in_day=0; step_in_day < STEPS_PER_DAY; step_in_day++) {
					transport_arrived += dam_rsegs[dam_index]->data[game]->transport_arrived[day*STEPS_PER_DAY + step_in_day];
					total += dam_rsegs[dam_index]->data[game]->output[day*STEPS_PER_DAY + step_in_day] + dam_rsegs[dam_index]->data[game]->transport_arrived[day*STEPS_PER_DAY + step_in_day];
				}

				/* Final calculation for a single cell */
				if (total > 0) (*d)->columns[dam_index].prob[day] = transport_arrived / total;
			}
		}
	}

	/* Fill in inriver columns */
	{
		/* Store bypass probabilities once so they are not recalculated */
		double *bypass_probs = (double*) calloc(num_collector_dams, sizeof(double));

		/* Calculate bypass probabilities averaged over the season */
		for (dam_index=0; dam_index < num_collector_dams; dam_index++) {
			/* Obtained an average bypass probability weighted by passage through the dam */
			const struct rls_seg_data *data = dam_rsegs[dam_index]->data[game];
			int step;
			double passage_total = 0.0;
			double sum = 0.0;

			for (step=0; step < STEPS_IN_SEASON; step++) {
				const double passage = data->output[step];
				passage_total += passage;
				if (passage > 0) sum += passage * (data->bypass_pass[step]/passage);
			}

			if (passage_total > 0) bypass_probs[dam_index] = sum / passage_total;
		}

		/* Calculate number of bypass probabilities */
		(*d)->prob_num_bypasses = (double*) calloc(num_collector_dams + 1, sizeof(double));

		calculate_spill_experience((*d)->prob_num_bypasses, dam_rsegs, bypass_probs, game, num_collector_dams, 1.0, 0, 0);

		free(bypass_probs);
	}
#if 0
/* TODO: REMOVE THIS CODE SINCE IT'S ONLY FOR DEBUGGING PURPOSES */
	/* Average the values of the days and print them out */
	printf("*** Delayed mortality table averages: ***\n");
	{
		int i;
		for (i=0; i < num_collector_dams; i++) {
			double value = 0.0;
			int day;
			for (day=0; day < DAYS_IN_SEASON; day++) {
				value += (*d)->columns[i].prob[day];
			}
			value /= DAYS_IN_SEASON;

			printf("%s:\t%f\n", dam_rsegs[i]->river_segment->seg_name, value);
		}
		for (i=0; i < num_collector_dams + 1; i++) {
			printf("For %d bypasses:\t%f\n", i, (*d)->prob_num_bypasses[i]);
		}
	}
/* REMOVE THE CODE ABOVE HERE */
#endif

	free(dam_rsegs);
}

void delayed_mortality_table_free(delayed_mortality_table *d) {
	free(d->columns);
	free(d->prob_num_bypasses);
	free(d);
}
