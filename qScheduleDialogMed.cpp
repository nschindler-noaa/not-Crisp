/* qScheduleDialogMed.cpp
 *  Derived classes for schedule dialog mediators.
 */

//#include <zapp.hpp>
#include "qScheduleDialogMed.h"
#include "Qcrisp.h"
#include "qIdcSchedule.h"
#include "resource.h"

qSDMPlannedSpillDay::qSDMPlannedSpillDay( QWidget *w, const char *feature )
	: qScheduleDialogMedBase( &qIdcPlannedSpillDay_, 
		new qScheduleDialogBase( w), "Planned Spill (Day)", feature ) {
	/* Base class has now set up dialog: put in label text.
	 */
	dialog_->setTitleText( "Planned Spill (Day)" );
	dialog_->setListBoxLabel( "Spill Days", 0 );
	dialog_->setEditBoxLabel( "Start Day", 0 );
	dialog_->setEditBoxLabel( "Stop Day", 1 );
	dialog_->setEditBoxLabel( "Spill", 2 );
	dialog_->setCheckBoxText( "Volume" );
	dialog_->showCheckBox( TRUE );
}

qSDMPlannedSpillNight::qSDMPlannedSpillNight( QWidget *w, const char *feature )
	: qScheduleDialogMedBase( &qIdcPlannedSpillNight_, 
		new qScheduleDialogBase( w), "Planned Spill (Night)", feature ) {
	/* Base class has now set up dialog: put in label text.
	 */
	dialog_->setTitleText( "Planned Spill (Night)" );
	dialog_->setListBoxLabel( "Spill Days", 0 );
	dialog_->setEditBoxLabel( "Start Day", 0 );
	dialog_->setEditBoxLabel( "Stop Day", 1 );
	dialog_->setEditBoxLabel( "Spill", 2 );
	dialog_->setCheckBoxText( "Volume" );
	dialog_->showCheckBox( TRUE );
}


qSDMPhouseSchedList::qSDMPhouseSchedList( QWidget *w )
	: qScheduleDialogMedBase( &qIdcPhouseSchedList_, 
		new qScheduleDialogBase( w), "Powerhouse Schedule" ) {
	/* Base class has now set up dialog: put in label text.
	 */
	dialog_->setTitleText( "Powerhouse Schedule" );
	dialog_->setListBoxLabel( "Day List", 0 );
	dialog_->setListBoxLabel( "Hour List", 1 );
	dialog_->setEditBoxLabel( "Start Day", 0 );
	dialog_->setEditBoxLabel( "Stop Day", 1 );
	dialog_->setEditBoxLabel( "Start Hour", 2 );
	dialog_->setEditBoxLabel( "Stop Hour", 3 );
	dialog_->showEditBox( TRUE, 3 );
	dialog_->setCheckBoxText( "On" );
	dialog_->showCheckBox( TRUE );
}

