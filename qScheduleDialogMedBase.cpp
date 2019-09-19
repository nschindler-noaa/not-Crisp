/* qScheduleDialogMedBase.cpp
 *  This is the base class for mediators which handle a qScheduleDialogBase
 *  and an idcScheduleBase - to edit PERIOD_LIST objects from the underlying
 *  database.
 */

#include <stdlib.h>
#include "qScheduleDialogMedBase.h"
#include "crisp_defs.h"
#include "Qcrisp.h"
#include <qmessagebox.h>
#include "qHelpManager.h"

qScheduleDialogMedBase::qScheduleDialogMedBase(qIdcScheduleBase *idc, 
												qScheduleDialogBase *dialog,
												const char *title, const char *feature) {

	int i;
	qDatContainer *dat;

	dialog_ = dialog;
	idcScheduleBase_ = idc;
	lock_ = 0;
	dialog_->setCaption( title );

	/* This prods the IDC to initialize itself if needs be. This cannot be
	 *  done by the IDC constructor because static objects are constructed
	 *  before the database is read in.
	 */
	idcScheduleBase_->Setup();

	/* Now initialize the dialog.
	 */
	for (i = 0; i < idcScheduleBase_->getNumFeature(); i++)
		dialog_->addComboBoxText( idcScheduleBase_->getFeature(i), 0 );
	/* If this box is for a specific feature, then don't allow the user to select another one */
	if (feature) {
		dialog_->selectComboBox( feature, 0 );
		dialog_->showComboBox(FALSE, 0);
	}
	else dialog_->selectComboBox( 0, 0 );
	for (i = 0; i < idcScheduleBase_->getNumSpecies(); i++)
		dialog_->addComboBoxText( idcScheduleBase_->getSpecies(i), 1 );
	dialog_->selectComboBox( 0, 1 );

	/* Show or hide controls as appropriate.
	 */
	if(dat = idcScheduleBase_->GetData( dialog_->getComboBoxText(0).ascii(),
									dialog_->getComboBoxText(1).ascii() )) {
		if (dat->lengthIntVec() == DAYS_IN_SEASON * 24) {
			dialog_->showListBox( TRUE, 1 );
			dialog_->showEditBox( TRUE, 3 );
			dialog_->showCheckBox( TRUE );
		}
	}

	updateDialog( 0, 0 );
	RegisterEvents();
}

qScheduleDialogMedBase::~qScheduleDialogMedBase() {
	unRegisterEvents();
}


void qScheduleDialogMedBase::RegisterEvents(void){
	connect(dialog_, SIGNAL(evApply_()), SLOT(cbApply()));
	connect(dialog_, SIGNAL(evApplyAll_()), SLOT(cbApplyAll()));
	connect(dialog_, SIGNAL(evCancel_()), SLOT(cbCancel()));
	connect(dialog_, SIGNAL(evOK_()), SLOT(cbOK()));
	connect(dialog_, SIGNAL(evReset_()), SLOT(cbReset()));
	connect(dialog_, SIGNAL(evResetAll_()), SLOT(cbResetAll()));
	connect(dialog_, SIGNAL(evHelp_()), SLOT(cbHelp()));
	connect(dialog_, SIGNAL(evLeftarrow_()), SLOT(cbLeftarrow()));

	connect(dialog_, SIGNAL(evCmbStatic1_(int)), SLOT(cbComboSelected(int)));
	connect(dialog_, SIGNAL(evCmbStatic2_(int)), SLOT(cbComboSelected(int)));
	connect(dialog_, SIGNAL(evList1_(int)), SLOT(cbList1Selected(int)));
	connect(dialog_, SIGNAL(evList2_(int)), SLOT(cbList2Selected(int)));

	connect(idcScheduleBase_, SIGNAL(evInvalidate_()), SLOT(cbInvalidate()));

	//idcScheduleBase_->evInvalidate()->attach(this, &qScheduleDialogMedBase::_cbInvalidate);
}

void qScheduleDialogMedBase::unRegisterEvents(void){
}

/* Update dialog from current state of IDC.
 */
void qScheduleDialogMedBase::updateDialog( int l1, int l2 ) {
	qDatContainer *dat;
	int i, j, k, iSel, iLine;
	int diff = 0;
	char cBuf[50];

	/* First, see if we can really get a data container from the IDC.
	 */
	if(dat = idcScheduleBase_->GetData( dialog_->getComboBoxText(0).ascii(),
									dialog_->getComboBoxText(1).ascii() )) {
		/* Common initialiqions.
		 */
		dialog_->clearListBox( 0 );
		dialog_->setEditBoxText( "", 0 );
		dialog_->setEditBoxText( "", 1 );
		dialog_->setEditBoxText( "", 2 );
		iSel = -1;
		iLine = -1;
									
		/* Are we a working on a single-level or two-level list?
		 */
		if (dat->lengthDoubleVec() == DAYS_IN_SEASON) {
			/* Single level list - run down double vector run-coding values
			 *  and shovel them into the first listbox.
			 */
			for (i = 0; i < DAYS_IN_SEASON; i = j) {
				for (j = i+1, diff = 0; j <= DAYS_IN_SEASON; j++)
					if (dat->getDouble(i) != dat->getDouble(j)) {
						diff = 1;
						break;
					}
				sprintf( cBuf, "%d-%d:%g", i, j-2+diff, dat->getDouble(i) );
				dialog_->addListBoxText( cBuf, 0 );
				iLine++;
				if (i <= l1 && l1 <= j-2+diff) {
					/* If we have found the day we are trying to keep selected,
					 *  make a note of where it ended up in the list, and
					 *  copy the appropriate text into the edit fields.
					 */
					iSel = iLine;
				}
			}
			if (iSel != -1) {
				dialog_->selectListBox( iSel, 0 );
				cbList1Selected( iSel );
			}
		}
		else if (dat->lengthIntVec() == DAYS_IN_SEASON * 24) {
			/* Two level list. Run down the list figuring out the runs of
			 *  equivalent days and shovel them into the first listbox. Then
			 *  use the "list box 2 selected" code to handle the hours
			 *  in the selected day.
			 */
			dialog_->clearListBox( 1 );
			dialog_->setEditBoxText( "", 3 );
			for (i = 0; i < DAYS_IN_SEASON; i = j) {
				for (j = i+1; j <= DAYS_IN_SEASON; j++) {
					for (k = diff = 0; k < 24; k++)
						if (dat->getInt(i*24+k) != dat->getInt(j*24+k)) {
							diff = 1;
							break;
						}
					if (diff)
						break;
				}
				sprintf( cBuf, "%d-%d", i, j-2 + diff );
				dialog_->addListBoxText( cBuf, 0 );
				iLine++;
				if (i <= l1 && l1 <= j-2+diff) {
					/* If we have found the day we are trying to keep selected,
					 *  make a note of where it ended up in the list, and
					 *  copy the appropriate text into the edit fields.
					 */
					iSel = iLine;
				}
			}
			if (iSel != -1) {
				dialog_->selectListBox( iSel, 0 );
				cbList1Selected( iSel );
			}
		}
		else {
			/* Should never happen */
		}

		/* Update the dirtiness indicators.
		 */
		if (dat->isDirty())
			dialog_->showDirty();
		else
			dialog_->showDirty( FALSE );
	}
}

void qScheduleDialogMedBase::cbHelp(){
    if(!lock_){
		lock_=1;
		helpManager->showHelp( QString("%1.html").arg(idcScheduleBase_->getHelpPage()) );
		lock_=0;
	}
}

void qScheduleDialogMedBase::cbApply(){
	qDatContainer *dat;

	if(!lock_){
		lock_=1;
		if(dat = idcScheduleBase_->GetData( dialog_->getComboBoxText(0).ascii(),
										dialog_->getComboBoxText(1).ascii() )) {
			idcScheduleBase_->ApplyData( dat );
			idcScheduleBase_->invalidate();
			dialog_->showDirty( FALSE );
		}
		lock_=0;
	}
}

void qScheduleDialogMedBase::cbApplyAll(){
	if(!lock_){
		lock_=1;
		idcScheduleBase_->ApplyCache();
		idcScheduleBase_->invalidate();
		dialog_->showDirty( FALSE );
		lock_=0;
	}
}

void qScheduleDialogMedBase::cbOK(){
	if(!lock_){
		lock_=1;
		idcScheduleBase_->ApplyCache();
		idcScheduleBase_->invalidate();
		lock_=0;
	}

	/* This is the only notification we will ever get that our underlying
	 *  window object is going away. So deleting the mediator machinery
	 *  is unconditional. */
	//unRegisterEvents();
	emit evRemove_();
	delete this;
}

void qScheduleDialogMedBase::cbCancel(){
	if(!lock_){
		lock_=1;
		idcScheduleBase_->ClearCache();
		lock_=0;
	}

	/* This is the only notification we will ever get that our underlying
	 *  window object is going away. So deleting the mediator machinery
	 *  is unconditional. */
	//unRegisterEvents();
	emit evRemove_();
	delete this;
}

void qScheduleDialogMedBase::cbReset(){
	qDatContainer *dat;
	
	if(!lock_){
		lock_=1;
		if(dat = idcScheduleBase_->GetData( dialog_->getComboBoxText(0).ascii(),
									dialog_->getComboBoxText(1).ascii() )) {
			idcScheduleBase_->ClearCache( dat );
			updateDialog( 0, 0 );
		}
		lock_=0;
	}
}

void qScheduleDialogMedBase::cbResetAll(){
	if(!lock_){
		lock_=1;
		idcScheduleBase_->ClearCache();
		updateDialog( 0, 0 );
		lock_=0;
	}
}

void qScheduleDialogMedBase::cbComboSelected(int i){
	updateDialog( 0, 0 );
}

void qScheduleDialogMedBase::cbList1Selected( int iSelect){
	qDatContainer *dat;
	QString cstr, substr;
	int i, j, day, iSel;
	char cBuf[10];

	if(dat = idcScheduleBase_->GetData( dialog_->getComboBoxText(0).ascii(),
									dialog_->getComboBoxText(1).ascii() )) {
		/* A day range has been selected - copy it into the edit fields.
		 *  If this is a two level list box, also update the hour ranges
		 *  into the second list box and select that box as appropriate.
		 */
		cstr = dialog_->getListBoxText( 0 );
		i = cstr.find( '-' );
		substr = cstr.left( i );
		dialog_->setEditBoxText( substr.ascii(), 0 );
		if (dat->lengthDoubleVec() > 0) {
			/* Single level list - set value text and be done.
			 */
			j = cstr.find( ':' );
			substr = cstr.mid( i+1, j-(i+1) );
			dialog_->setEditBoxText( substr.ascii(), 1 );
			substr = cstr.mid( j+1, cstr.length()-(j+1) );
			dialog_->setEditBoxText( substr.ascii(), 2 );
		}
		else {
			/* Two level list - setup second listbox and do a selection
			 *  therein.
			 */
			day = atoi( cstr.mid( 0, i ).ascii() );
			substr = cstr.mid( i+1, cstr.length()-(i+1) );
			dialog_->setEditBoxText( substr.ascii(), 1 );
			dialog_->setEditBoxText( "", 2 );
			dialog_->setEditBoxText( "", 3 );
			dialog_->setCheckBox( 0 );
			dialog_->clearListBox( 1 );
			iSel = 0;
			for (i = 0; i < 24; i++) {
				if (dat->getInt( day*24+i )) {
					for (j = i; j < 24; j++) {
						if (j == 23 || !(dat->getInt( day*24+j+1 ))) {
							sprintf( cBuf, "%d-%d", i, j+1 );
							dialog_->addListBoxText( cBuf, 1 );
							i = j;
							iSel = 1;
							break;
						}
					}
				}
			}
			if (iSel) {
				dialog_->selectListBox( 0, 1 );
				cbList2Selected( 0 );
			}
		}
	}
}

void qScheduleDialogMedBase::cbList2Selected(int iSel){
	QString cstr, substr;
	int i;

	/* Must be a two level list - get the values in the selection into the
	 *  edit fields.
	 */
	dialog_->setCheckBox( 1 );
	cstr = dialog_->getListBoxText( 1 );
	i = cstr.find( '-' );
	substr = cstr.mid( 0, i );
	dialog_->setEditBoxText( substr.ascii(), 2 );
	substr = cstr.mid( i+1, cstr.length()-(i+1) );
	dialog_->setEditBoxText( substr.ascii(), 3 );
}

void qScheduleDialogMedBase::cbLeftarrow(){
	qDatContainer *dat;
	QString cstr;
	int startday, stopday, starthour, stophour, i, j, check;
	double value;

	/* First get start and stop days.
	 */
	lock_ = 1;
	cstr = dialog_->getEditBoxText( 0 );
	startday = atoi( cstr.ascii() );
	cstr = dialog_->getEditBoxText( 1 );
	stopday = atoi( cstr.ascii() );
	/* TODO better error detection & handling. */
	if (startday < 0)
		QMessageBox::warning(dialog_, "Schedule Tool", "Start day must be non-negative",
					QMessageBox::NoButton, QMessageBox::Ok);
	//	delete new zMessage( dialog_, "Start day must be non-negative",
		//	"Schedule Tool" );
	else if (stopday < 0)
		QMessageBox::warning(dialog_, "Schedule Tool", "Stop day must be non-negative",
					QMessageBox::NoButton, QMessageBox::Ok);
		//delete new zMessage( dialog_, "Stop day must be non-negative",
		//	"Schedule Tool" );
	else if (startday >= DAYS_IN_SEASON)
		QMessageBox::warning(dialog_, "Schedule Tool", "Start day is beyond end of season",
					QMessageBox::NoButton, QMessageBox::Ok);
		//delete new zMessage( dialog_, "Start day is beyond end of season",
		//	"Schedule Tool" );
	else if (stopday >= DAYS_IN_SEASON)
		QMessageBox::warning(dialog_, "Schedule Tool", "Stop day is beyond end of season",
					QMessageBox::NoButton, QMessageBox::Ok);
		//delete new zMessage( dialog_, "Stop day is beyond end of season",
		//	"Schedule Tool" );
	else if (startday > stopday)
		QMessageBox::warning(dialog_, "Schedule Tool", "Start day cannot be after stop day",
					QMessageBox::NoButton, QMessageBox::Ok);
		//delete new zMessage( dialog_, "Start day cannot be after stop day",
		//	"Schedule Tool" );
	else {
		/* OK to continue processing. */
		if (dat = idcScheduleBase_->GetData( dialog_->getComboBoxText(0).ascii(),
										dialog_->getComboBoxText(1).ascii() )) {
			if (dat->lengthDoubleVec() == DAYS_IN_SEASON) {
				cstr = dialog_->getEditBoxText( 2 );
				value = atof( cstr.ascii() );
				int volume = dialog_->isBoxChecked();

				if (value < 0.0) {
					QMessageBox::warning(dialog_, "Schedule Tool", "Flow fraction/volume must be greater than or equal to zero",
						QMessageBox::NoButton, QMessageBox::Ok);
				}
				else if (value > 1.0 && !volume) {
					QMessageBox::warning(dialog_, "Schedule Tool", "Flow fraction must less than or equal to one",
						QMessageBox::NoButton, QMessageBox::Ok);
				}
				else {
					for (i = startday; i <= stopday; i++)
						dat->setDouble( i, value, 0 );

					if (dat->lengthIntVec() == DAYS_IN_SEASON) {
						/* This means that we have a checkbox for each day (e.g. to input
						 * flow volumes instead of flow fractions) */
						for (i = startday; i <= stopday; i++)
							dat->setInt(i, volume);
					}

					idcScheduleBase_->invalidate();
					updateDialog( startday, 0 );
				}
			}
			else if (dat->lengthIntVec() == DAYS_IN_SEASON * 24) {
				/* First get start and stop hours.
				 */
				cstr = dialog_->getEditBoxText( 2 );
				starthour = atoi( cstr.ascii() );
				cstr = dialog_->getEditBoxText( 3 );
				stophour = atoi( cstr.ascii() );
				/* TODO better error detection & handling. */
				if (starthour < 0)
					QMessageBox::warning(dialog_, "Schedule Tool", "Start hour must be non-negative",
						QMessageBox::NoButton, QMessageBox::Ok);
					//delete new zMessage( dialog_, "Start hour must be non-negative",
					//	"Schedule Tool" );
				else if (stophour < 0)
					QMessageBox::warning(dialog_, "Schedule Tool", "Stop hour must be non-negative",
						QMessageBox::NoButton, QMessageBox::Ok);
					//delete new zMessage( dialog_, "Stop hour must be non-negative",
					//	"Schedule Tool" );
				else if (starthour > 24)
					QMessageBox::warning(dialog_, "Schedule Tool", "Start hour is greater than 24",
						QMessageBox::NoButton, QMessageBox::Ok);
					//delete new zMessage( dialog_, "Start hour is greater than 24",
						//"Schedule Tool" );
				else if (stophour > 24)
					QMessageBox::warning(dialog_, "Schedule Tool", "Stop hour is greater than 24",
						QMessageBox::NoButton, QMessageBox::Ok);
					//delete new zMessage( dialog_, "Stop hour is greater than 24",
						//"Schedule Tool" );
				else if (starthour >= stophour)
					QMessageBox::warning(dialog_, "Schedule Tool", "Stop hour must be after start hour",
						QMessageBox::NoButton, QMessageBox::Ok);
					//delete new zMessage( dialog_, "Stop hour must be after start hour",
					//	"Schedule Tool" );
				else {
					/* OK to process. */
					check = dialog_->isBoxChecked();

					for (i = startday; i <= stopday; i++)
						for (j = starthour; j < stophour; j++)
							dat->setInt( i*24+j, check );
					dat->setDirty();
					idcScheduleBase_->invalidate();
					updateDialog( startday, 0 );
				}
			}
			else {
				/* Should never get here. */
			}
		}
	}
	lock_ = 0;
}

void qScheduleDialogMedBase::cbInvalidate(){
	if(!lock_){
		lock_=1;
	    updateDialog( 0, 0 );
		lock_=0;
	}
}

