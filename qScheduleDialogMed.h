/* qScheduleDialogMed.h
 *  Derived classes for schedule dialog mediators.
 */

#ifndef __qScheduleDialogMed_h__
#define __qScheduleDialogMed_h__

#include "qScheduleDialogMedBase.h"

class qSDMPlannedSpillDay : public qScheduleDialogMedBase {
public:
	qSDMPlannedSpillDay( QWidget *w = NULL, const char *feature = NULL );
};

class qSDMPlannedSpillNight : public qScheduleDialogMedBase {
public:
	qSDMPlannedSpillNight( QWidget *w = NULL, const char *feature = NULL );
};

class qSDMPhouseSchedList : public qScheduleDialogMedBase {
public:
	qSDMPhouseSchedList( QWidget *w = NULL );
};


#endif
