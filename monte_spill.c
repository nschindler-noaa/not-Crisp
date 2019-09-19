/* spill calculations in the monte carlo mode */

#include "crisp.h"
#include "crisp_time.h"
#include "monte.h"
#include "log.h"
#include "unused.h"
#include "spill.h"


/* local structure type for passing data around to user-functions */

struct period_spill_data
{
    float day;
    float night;
    float overgen;
};

/* Forward definition. */
static int compute_spill_sched(
    struct river_segment *dam,		/* dam pointer */
    struct archive_header *archive_data,/* archive header data */
    struct archive_spill *game_spill,	/* archive spills this dam */
    unsigned int arch_spill_flag);	/* use archive planned spill if set */
    
/* routine called from monte carlo module which fills in spill
 * schedules
 */

void monte_compute_spill_schedules(archive_data, game_spills, arch_spill_flag)
    struct archive_header *archive_data; 	/* archive header data */
    struct archive_spill **game_spills;		/* list of archive spills */
    unsigned int arch_spill_flag; 	/* use archive planned spill if set */
{
    int i;

    /* only fill in schedules for dams specified in archive */
    for (i = 0; i < archive_data->ndams; i++)
    {
	if (archive_data->dam_list[i] != (struct river_segment *) -1)
	    compute_spill_sched(archive_data->dam_list[i],
				archive_data, game_spills[i], arch_spill_flag);
    }
}

/* Helper function for compute_flow_archive_spill_schedule */
static int compute_flow_archive_spill_schedule_time(struct river_segment *dam, struct archive_header *archive_data, struct archive_spill *game_spill, PERIOD_LIST** spill_period, int night) {
	int i;
	PERIOD_LIST *period;

	PERIOD_LIST *new_period;

	/* Whether we're dealing with day or night */
	night = (spill_period == &dam->current->dam.planned_spill_night);

	/* Create new spill schedule (delete the old one if it exists) */
	if (*spill_period == NULL) destroy_period_struct(*spill_period);

	*spill_period = (PERIOD_LIST*) malloc(sizeof(PERIOD_LIST));
	new_period = *spill_period;
	if (new_period == NULL) return -1;

	new_period->prev = NULL;

	/* Create new periods with values from the flow archive data */
	for (i = 0, period = archive_data->periods; i < archive_data->nperiods; i++, period = period->next) {
		if (i > 0) {
			/* Append period (the first one was already created) */
			new_period->next = (PERIOD_LIST*) malloc(sizeof(PERIOD_LIST));
			if (new_period->next == NULL) return -1;

			new_period->next->prev = new_period;
			new_period = new_period->next;
		}

		new_period->sub_periods = NULL;
		new_period->next = NULL;
		new_period->start = period->start;
		new_period->end = period->end;
		new_period->value = night ? game_spill[i].night : game_spill[i].day;
	}

	return 0;
}

/*
 * Note that this function will replace the dam spill schedule periods with ones
 * that match the flow archive periods.
 *
 * returns non-zero on error
 */
static int compute_flow_archive_spill_schedule(struct river_segment *dam, struct archive_header *archive_data, struct archive_spill *game_spill) {
	int rc = 0;
	bzero((char *) dam->current->dam.planned_spill, sizeof(dam->current->dam.planned_spill));

	rc |= compute_flow_archive_spill_schedule_time(dam, archive_data, game_spill, &dam->current->dam.planned_spill_day, FALSE);
	rc |= compute_flow_archive_spill_schedule_time(dam, archive_data, game_spill, &dam->current->dam.planned_spill_night, TRUE);

	return rc;
}

/* routine to compute spill schedule for a dam 
 * based on information in active flow archive file and
 * user specified spill schedules already in river struct.
 * returns non-zero on error
 */
static int compute_spill_sched(dam, archive_data, game_spill, arch_spill_flag)
    struct river_segment *dam;		/* dam pointer */
    struct archive_header *archive_data;/* archive header data */
    struct archive_spill *game_spill;	/* archive spills this dam */
    unsigned int arch_spill_flag;	/* use archive planned spill if set */
{
	int rc = 0;
	if (arch_spill_flag) {
	        /* Overwrite input spill schedule with the data from the flow archive file */
	        rc |= compute_flow_archive_spill_schedule(dam, archive_data, game_spill);
	}
	
	/* Set planned spill from schedule */
	set_dam_planned_spill(dam);

	/* TODO: Apply overgen here once it is decided upon (since overgen should not go into the schedules) */


	return rc;
}

