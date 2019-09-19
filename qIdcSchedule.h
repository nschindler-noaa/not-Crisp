/* qIdcSchedule.h   Derived classes for interface data controllers which control
 *  a set of lists of values, selected by a single index (they may be
 *  logically organized into rows and columns, in which case all of the
 *  first row is stored, then all of the second row, etc). The sets are
 *  selected by two strings (feature and species, although in principle they
 *  could be anything).
 */

#ifndef __qIdcSchedule__
#define __qIdcSchedule__

#include "qIdcScheduleBase.h"
#include "resource.h"

/* This is a base class for spill schedules that should be entered in terms of
 * either volume or a proportion of flow. */
class qIdcPlannedSpill : public qIdcScheduleBase {
public:
	qIdcPlannedSpill() : qIdcScheduleBase( ID_SPILLSCHEDULE ) { }

protected:
	void putToListWithVolume( qDatContainer *dat, PERIOD_LIST **ppl, int night );
};

class qIdcPlannedSpillDay : public qIdcPlannedSpill {

public:
	qIdcPlannedSpillDay();
	virtual int FillData(qDatContainer *dat);
	virtual void ApplyData(qDatContainer *dat);
	virtual qDatContainer *NewData();
	virtual void putToList( qDatContainer *dat, PERIOD_LIST **ppl ); /* Override to accept volume or fraction of flow */

};

extern qIdcPlannedSpillDay qIdcPlannedSpillDay_;

class qIdcPlannedSpillNight : public qIdcPlannedSpill {

public:
	qIdcPlannedSpillNight();
	virtual int FillData(qDatContainer *dat);
	virtual void ApplyData(qDatContainer *dat);
	virtual qDatContainer *NewData();
	virtual void putToList( qDatContainer *dat, PERIOD_LIST **ppl ); /* Override to accept volume or fraction of flow */

};

extern qIdcPlannedSpillNight qIdcPlannedSpillNight_;

class qIdcPhouseSchedList : public qIdcScheduleBase {
public:
	qIdcPhouseSchedList();
	virtual int FillData(qDatContainer *dat);
	virtual void ApplyData(qDatContainer *dat);
	virtual qDatContainer *NewData();
	virtual void Setup( void );

};

extern qIdcPhouseSchedList qIdcPhouseSchedList_;

#endif

