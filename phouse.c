
/* phouse.c - support routines for multiple powerhouse operation */

#include "crisp.h"
#include "log.h"
#include "phouse.h"

/* NOTE: powerhouse fractions depend on spill and are set in the unified
 * method set_dam_flow_routing() in spill.c. */

/*
 * void clear_powerhouse_fractions(struct river_segment *dam)
 *	clears the phouse fractions for all powerhouses of this dam
 */

void clear_powerhouse_fractions(dam)
    struct river_segment *dam;
{
    while (dam)
    {
	bzero((char *) DAMPTR(dam)->flow_fraction,
	      sizeof(DAMPTR(dam)->flow_fraction));
	dam = DAMPTR(dam)->additional_pwrhs;
    }
}

/*
 * struct river_segment *find_priority_phouse(struct river_segment *dam, int i)
 *	finds the phouse of the given dam with priority i.  the best priority
 *	is zero, followed by increasing positive numbers.  returns null if
 *	not found.
 */

struct river_segment *find_priority_phouse(dam, i)
    struct river_segment *dam;
    int i;
{
    if (i >= DAMPTR(dam)->num_powerhouses)
	return(NULL);
    while (dam && DAMPTR(dam)->phouse_priority != i)
	dam = DAMPTR(dam)->additional_pwrhs;
    return(dam);
}

/*
 * PERIOD_LIST *make_default_phouse_schedule()
 *	returns a default powerhouse schedule.  the current default is
 *	on for 24 hours a day, 365 days a year.
 */

PERIOD_LIST *make_default_phouse_schedule()
{
    PERIOD_LIST *sched;

    sched = new_period_struct();
    sched->start = 0;
    sched->end   = 365;
    sched->sub_periods = new_period_struct();
    sched->sub_periods->start = 0;
    sched->sub_periods->end = 24;
    return(sched);
}

/*
 * void remove_all_phouse_scheds()
 *	remove phouse scheds from all dams and powerhouses.
 */

void remove_all_phouse_scheds()
{
   struct river_segment *seg;

   for (seg = river_mouth; seg; seg = seg->next)
       if (seg->seg_type == DAM_SEG || seg->seg_type == POWERHOUSE_SEG)
       {
	   destroy_period_struct(DAMPTR(seg)->phouse_schedule);
	   DAMPTR(seg)->phouse_schedule = NULL;
       }
}
