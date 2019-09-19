/* qIdcSchedule.cpp   Derived classes for interface data controllers which control
 *  a set of lists of values, selected by a single index. These represent
 *  PERIOD_LISTS in the underlying database. The sets are
 *  selected by two strings (feature and species, although in principle they
 *  could be anything).
 */

#include "qIdcSchedule.h"
#include "crisp.h"
#include "resource.h"
#include "qUtils.h"
extern "C" {
#include "crisp_time.h"
}

/* planned spill fractions, per day, at dams.
 */
qIdcPlannedSpillDay::qIdcPlannedSpillDay() : qIdcPlannedSpill() {
	setHelpPage( 66 );
}

qDatContainer *qIdcPlannedSpillDay::NewData() {
	return new qDatContainer( DAYS_IN_SEASON, 2, DAYS_IN_SEASON );
}

int qIdcPlannedSpillDay::FillData(qDatContainer *dat) {
	river_segment *seg;
	
	seg = getRiverSegmentHandle( dat->getString(0), DAM_SEG );
	if (seg == NULL)
		return 0;

	getFromList( dat, seg->current->dam.planned_spill_day );
	dat->clearDirty();
	return 1;
}

void qIdcPlannedSpillDay::ApplyData(qDatContainer *dat) {
	river_segment *seg;
	
	seg = getRiverSegmentHandle( dat->getString(0), DAM_SEG );
	if (seg == NULL)
		return;

	putToList( dat, &(seg->current->dam.planned_spill_day) );

	dat->clearDirty();
	qIdcScheduleBase::ApplyData( dat );
}

void qIdcPlannedSpillDay::putToList( qDatContainer *dat, PERIOD_LIST **ppl ) {
	putToListWithVolume(dat, ppl, FALSE);
}

/* A single global, static object does the interfacing for this type of
 *  data.
 */
qIdcPlannedSpillDay qIdcPlannedSpillDay_;

/* ******************************************************************** */

/* planned spill fractions, per day, at dams.
 */
qIdcPlannedSpillNight::qIdcPlannedSpillNight() : qIdcPlannedSpill() {
	setHelpPage( 66 );
}

qDatContainer *qIdcPlannedSpillNight::NewData() {
	return new qDatContainer( DAYS_IN_SEASON, 2, DAYS_IN_SEASON );
}

int qIdcPlannedSpillNight::FillData(qDatContainer *dat) {
	river_segment *seg;
	
	seg = getRiverSegmentHandle( dat->getString(0), DAM_SEG );
	if (seg == NULL)
		return 0;

	getFromList( dat, seg->current->dam.planned_spill_night );
	dat->clearDirty();
	return 1;
}

void qIdcPlannedSpillNight::ApplyData(qDatContainer *dat) {
	river_segment *seg;
	
	seg = getRiverSegmentHandle( dat->getString(0), DAM_SEG );
	if (seg == NULL)
		return;

	putToList( dat, &(seg->current->dam.planned_spill_night) );

	dat->clearDirty();
	qIdcScheduleBase::ApplyData( dat );
}

void qIdcPlannedSpillNight::putToList( qDatContainer *dat, PERIOD_LIST **ppl ) {
	putToListWithVolume(dat, ppl, TRUE);
}

/* A single global, static object does the interfacing for this type of
 *  data.
 */
qIdcPlannedSpillNight qIdcPlannedSpillNight_;

/* ******************************************************************** */

/* housr of operation, for each day, at powerhouses.
 */
qIdcPhouseSchedList::qIdcPhouseSchedList() : qIdcScheduleBase( ID_SPILLSCHEDULE ) {
	setHelpPage( 68 );
}

qDatContainer *qIdcPhouseSchedList::NewData() {
	return new qDatContainer( 0, 2, DAYS_IN_SEASON * 24 );
}

/* Base class behavior lists dams - we want to list powerhouses.
 */
void qIdcPhouseSchedList::Setup( void ) {
	int i;
	if (getNumSpecies() == 0) {
		insertSpecies( "all" );
		for (i = 0; i < num_powerhouses; i++)
			insertFeature( phouses[i]->seg_name );
	}
}

int qIdcPhouseSchedList::FillData(qDatContainer *dat) {
	river_segment *seg;

	seg = getRiverSegmentHandle( dat->getString(0), POWERHOUSE_SEG );
	if (seg == NULL)
		seg = getRiverSegmentHandle( dat->getString(0), DAM_SEG );
	if (seg == NULL)
		return 0;

	getFromList( dat, seg->current->dam.phouse_schedule );
	dat->clearDirty();
	return 1;
}

void qIdcPhouseSchedList::ApplyData(qDatContainer *dat) {
	river_segment *seg;
	
	seg = getRiverSegmentHandle( dat->getString(0), POWERHOUSE_SEG );
	if (seg == NULL)
		seg = getRiverSegmentHandle( dat->getString(0), DAM_SEG );
	if (seg == NULL)
		return;

	putToList( dat, &(seg->current->dam.phouse_schedule) );
	dat->clearDirty();
	qIdcScheduleBase::ApplyData( dat );
}

/* A single global, static object does the interfacing for this type of
 *  data.
 */
qIdcPhouseSchedList qIdcPhouseSchedList_;

/* ******************************************************************** */

/* Accept either a volume or proportion of flow */
void qIdcPlannedSpill::putToListWithVolume( qDatContainer *dat, PERIOD_LIST **ppl, int night ) {
	PERIOD_LIST *npl;
	int i;

	struct river_segment *seg = getRiverSegmentHandle( dat->getString(0), DAM_SEG );
	if (seg == NULL)
		return;
	
	destroy_period_struct( *ppl );
	*ppl = NULL;

	if (dat->lengthDoubleVec() == DAYS_IN_SEASON) {
		/* Single level list. */

		/* Create a PERIOD_LIST structure for the last day.
		 */
		npl = new_period_struct();
		npl->start = npl->end = DAYS_IN_SEASON - 1;
		npl->sub_periods = npl->next = npl->prev = NULL;

		/* Crawl backwards through the season, from last day to first,
		 *  constructing period lists of adjacent days with equal values.
		 */
		for (i = DAYS_IN_SEASON - 1; i >= 0; i--) {
			double value;
			if (dat->getInt(i)) {
				/* The integer indicates whether the value is a volume rate or not.
				 * Such a volume will be set as a fraction such that the average spill
				 * rate over the period (day or night) should yield the correct volume. */
				double flow = seg->current->dam.flow[i];

				/* Don't allow the value to go over 1.0 */
				value = dat->getDouble(i) > flow ? 1.0 : dat->getDouble( i ) / flow;
			}
			else {
				/* Otherwise we assume this is a flow fraction */
				value = dat->getDouble(i);
			}
			if (*ppl) {
				if (value == (*ppl)->value)
					(*ppl)->start = i;
				else {
					/* Value changed: create a new list element and weave it in.
					 */
					npl = new_period_struct();
					npl->start = npl->end = i;
					npl->value = value;
					npl->sub_periods = NULL;
					npl->prev = NULL;
					npl->next = *ppl;
					(*ppl)->prev = npl;
					*ppl = npl;
				}
			}
			else *ppl = npl;
		}
	}
}

