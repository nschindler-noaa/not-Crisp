#include <malloc.h>
#include "crisp.h"
#include "log.h"
#include "period.h"

/* Forward definitions. */
int period_data_equal(PERIOD_LIST *p1, PERIOD_LIST *p2);

/**
 * \brief  Allocates a new
 *	PERIOD_LIST and bzeros it.
 */

PERIOD_LIST *new_period_struct ()
{
    PERIOD_LIST *per = (PERIOD_LIST *)
	malloc (sizeof(PERIOD_LIST));

    if (!per)
    {
	log_msg(L_ERROR, "No memory for period structure\n");
	return((PERIOD_LIST *)0);
    }
    bzero ( (char *)per, sizeof(PERIOD_LIST));

    return (per);
}

/**
 * \brief allocates and returns
 * a deep copy of rhs.
 *
 */

PERIOD_LIST *make_deep_copy(rhs)
	PERIOD_LIST *rhs;
{
    PERIOD_LIST *lhs;
    
    lhs = (PERIOD_LIST *) 0;
    
    if (rhs)
    {
	lhs = new_period_struct();
        bcopy(rhs, lhs, sizeof(PERIOD_LIST));
	lhs->next = make_deep_copy(rhs->next);
	if (lhs->next)
	    lhs->next->prev = lhs;
	lhs->sub_periods = make_deep_copy(rhs->sub_periods);
	if (lhs->sub_periods)
	    lhs->sub_periods->prev = lhs;
    }
    return lhs;
}

/**
 * \brief Frees
 *	a period list and everything associated with it.
 */

void destroy_period_struct (period)
	PERIOD_LIST *period;
{
    void free_period_struct();

    if (period)
    {
	destroy_period_struct (period->next);
	free_period_struct(period);
    }
}

/**
 * \brief Frees
 *	the period struct and everything associated with it.
 */

void free_period_struct(period)
	PERIOD_LIST *period;
{
    if (period) {
	if (period->sub_periods)
	    destroy_period_struct(period->sub_periods);
	free(period);
    }
}

/**
 * \brief collapse a period
 * list to the smallest representation.
 */

void collapse_period_list(per)
    PERIOD_LIST *per;
{
    PERIOD_LIST *pnext;
    
    if (per && per->sub_periods)
	collapse_period_list(per->sub_periods);

    while (per) {
	int collapsed = 0;
	if ((pnext = per->next)) {
	    if (pnext->sub_periods)
		collapse_period_list(pnext->sub_periods);
	    if (per->end == pnext->start
		|| per->end+1 == pnext->start) {

		/* make a shallow copy of per, but with pnext dates */
		PERIOD_LIST tmp;
		tmp.start = pnext->start;
		tmp.end = pnext->end;
		tmp.value = per->value;
		tmp.sub_periods = per->sub_periods;

		/* determine if these two adjacent periods collapsible */
		if (period_data_equal(&tmp, pnext)) {
		    per->end = pnext->end;
		    per->next = pnext->next;
		    if (per->next)
			per->next->prev = per;
		    free_period_struct(pnext);
		    collapsed = 1;
		}
	    }
	}
	if (!collapsed)
	    per = per->next;
    }
}
		    
/**
 * \brief determines if
 * two periods contain equivalent data, disregarding their prev and next
 * pointers.  sub-period lists must be identical.
 *            return: 0 not equal
 *		      1 equal
 */
int period_data_equal(p1, p2)
    PERIOD_LIST *p1;
    PERIOD_LIST *p2;
{
    int subs_eq = 1;

    if (!p1 && !p2)
	return 1;
    else if (!p1 || !p2)
	return 0;

    if (p1->sub_periods || p2->sub_periods) {
	PERIOD_LIST *sub1 = p1->sub_periods;
	PERIOD_LIST *sub2 = p2->sub_periods;

	do {
	    if (!period_data_equal(sub1, sub2))
		subs_eq = 0;
	    else {
		sub1 = sub1->next;
		sub2 = sub2->next;
	    }
	} while (subs_eq && (sub1 || sub2));
    }
    if (subs_eq
	&& p1->start == p2->start
	&& p1->end == p2->end
	&& p1->value == p2->value
	)
	return 1;
    else
	return 0;
}

/**
 * \brief	takes a period list which consists of days with subperiods as
 *	hours.  confirms that days and hours are within range.
 *		return: 0  ok
 *			1  error
 */

int check_daily_period_list(day)
    PERIOD_LIST *day;
{
    PERIOD_LIST *hour;
    
    while (day)
    {
	if (day->start < 0 || day->end > 365)
	    return 1;
	hour = day->sub_periods;
	while (hour)
	{
	    if (hour->start < 0 || hour->end > 24)
		return 1;
	    hour = hour->next;
	}
	day = day->next;
    }
    return 0;
}

/**
 * \brief examines a schedule consisting of a period list for days
 *	with subperiods for hours.  determines if the given dam time
 *	slice (from beginning of season) falls within the range of
 *	the list. note that >= 50% of a dam time slice must fall during
 *	a scheduled hour range to qualify.
 *		return:	0  slice not within schedule
 *			1  slice in schedule
 */

int dam_slice_in_schedule(sched, slice)
    PERIOD_LIST *sched;			/* schedule */
    int slice;				/* dam slices since start of season */
{
    int day   = slice / DAM_TIME_SLC;
    int step = slice % DAM_TIME_SLC;
    static int slice_size = 24/DAM_TIME_SLC;
    int first_hour, last_hour;

    /* find the day */

    while (sched && sched->end < day)
	sched = sched->next;
    if (!sched || sched->start > day)
	return 0;

    /* find the hour containing >= 50% of the dam time slice; use the
     * bounding hours of the slice
     */

    sched = sched->sub_periods;
    first_hour = step * slice_size;
    last_hour = first_hour + slice_size;
    while (sched && sched->end - first_hour <= slice_size/2.0)
	sched = sched->next;
    if (sched && last_hour - sched->start >= slice_size/2.0) 
	return 1;
    else
	return 0;
}

