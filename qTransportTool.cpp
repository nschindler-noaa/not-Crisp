/* qTransportTool.h
 * This class defines a modal dialog which edits transport structures
 *  via comboboxes, scrollbars, and edit boxes. The dialog is based on 
 *  the IDD_TRANSPORTTOOL resource template.
 *
 * This is a special-purpose dialog which will probably not be subclassed.
 */

#include "qTransportTool.h"
#include "resource.h"
#include "run_mode.h"
#include "qGlobalEvents.h"
#include "qSelectDialogMedBase.h"
#include "qUtils.h"
#include "qIdcSelect.h"
#include "qMapPane.h"
#include "Qmain.h"
#include "Qcrisp.h"
#include <stdlib.h>
#include <qlineedit.h>
#include <qscrollbar.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qmessagebox.h>
#include <qlistbox.h>
#include "qHelpManager.h"


extern qTransportTool *theTransportTool_;  // pathological data connections
extern qMapPane *theMapPane_;
/* Horrible KLUDGE
 */

static QString csOnDay( "On day" ), csIfCount( "If daily count above" );

qTransportTool::qTransportTool( QWidget *w )
: TransportBase ( NULL, "Transport Tool", FALSE, Qt::WDestructiveClose | WType_TopLevel | WStyle_SysMenu | WStyle_Minimize | WStyle_Dialog	 )
{
	consCommon();
}

/* Common code called by all constructors.
 */
void qTransportTool::consCommon( void ) {
	

	lock_ = 1;
	
	connect(cmbDam_, SIGNAL( activated(int)), SLOT(cbComboDamChange()));
	connect(cmbReach_, SIGNAL( activated(int)), SLOT(cbComboReachChange()));
	connect(cmbExisting_, SIGNAL( activated(int)), SLOT(cbComboExistingChange()));
	connect(cmbStart_, SIGNAL( activated(int)), SLOT(cbComboStartChange()));
	connect(cmbRestart_, SIGNAL( activated(int)), SLOT(cbComboRestartChange()));
	connect(cmbSpecies_, SIGNAL( activated(int)), SLOT(cbComboSpeciesChange()));
	focusGainedEventFilter * fef;
	connect(edStartTrans_, SIGNAL(lostFocus()), this, SLOT(cbStartTransKillFocus()));
	connect(edStartTrans_, SIGNAL(returnPressed()), this, SLOT(cbStartTransReturn()));
	fef = new focusGainedEventFilter(this);
	edStartTrans_->installEventFilter(fef);  //install the filter to revieve focus lost events
	connect(fef, SIGNAL(focusGained()), this, SLOT(cbStartTransSetFocus()));  //connect the filter to the slot

	connect(edRestartTrans_, SIGNAL(lostFocus()), this, SLOT(cbRestartTransKillFocus()));
	connect(edRestartTrans_, SIGNAL(returnPressed()), this, SLOT(cbRestartTransReturn()));
	fef = new focusGainedEventFilter(this);
	edRestartTrans_->installEventFilter(fef);  //install the filter to revieve focus lost events
	connect(fef, SIGNAL(focusGained()), this, SLOT(cbRestartTransSetFocus()));  //connect the filter to the slot

	connect(edStopDay_, SIGNAL(lostFocus()), this, SLOT(cbStopDayKillFocus()));
	connect(edStopDay_, SIGNAL(returnPressed()), this, SLOT(cbStopDayReturn()));
	fef = new focusGainedEventFilter(this);
	edStopDay_->installEventFilter(fef);  //install the filter to revieve focus lost events
	connect(fef, SIGNAL(focusGained()), this, SLOT(cbStopDaySetFocus()));  //connect the filter to the slot

	connect(edStopCount_, SIGNAL(lostFocus()), this, SLOT(cbStopCountKillFocus()));
	connect(edStopCount_, SIGNAL(returnPressed()), this, SLOT(cbStopCountReturn()));
	connect(edStopNDay_, SIGNAL(lostFocus()), this, SLOT(cbStopNDayKillFocus()));
	connect(edStopNDay_, SIGNAL(returnPressed()), this, SLOT(cbStopNDayReturn()));

	connect(edSepFlow_, SIGNAL(lostFocus()), this, SLOT(cbSepFlowKillFocus()));
	connect(edSepFlow_, SIGNAL(returnPressed()), this, SLOT(cbSepFlowReturn()));

	connect(edTermCount_, SIGNAL(lostFocus()), this, SLOT(cbTermCountKillFocus()));
	connect(edTermCount_, SIGNAL(returnPressed()), this, SLOT(cbTermCountReturn()));
	connect(edTransSpeed_, SIGNAL(lostFocus()), this, SLOT(cbTransSpeedKillFocus()));
	connect(edTransSpeed_, SIGNAL(returnPressed()), this, SLOT(cbTransSpeedReturn()));
	scrStopDay_->setMinValue ( 0 );
	scrStopDay_->setMaxValue ( 365 );
	scrStopCount_->setMinValue ( 0 );
	scrStopCount_->setMaxValue ( 2000 );
	scrStopNDay_->setMinValue ( 0 );
	scrStopNDay_->setMaxValue ( 365 );
	scrSepFlow_->setMinValue ( 0 );
	scrSepFlow_->setMaxValue ( 250 );
	scrTermCount_->setMinValue ( 0 );
	scrTermCount_->setMaxValue ( 100 );
	scrTransSpeed_->setMinValue ( 0 );
	scrTransSpeed_->setMaxValue ( 500 );
	connect(scrStartTrans_, SIGNAL(valueChanged(int)), SLOT(cbStartTransScrollerChanged(int)));
	connect(scrRestartTrans_, SIGNAL(valueChanged(int)), SLOT(cbRestartTransScrollerChanged(int)));
	connect(scrStopDay_, SIGNAL(valueChanged(int)), SLOT(cbStopDayScrollerChanged(int)));
	connect(scrStopCount_, SIGNAL(valueChanged(int)), SLOT(cbStopCountScrollerChanged(int)));
	connect(scrStopNDay_, SIGNAL(valueChanged(int)), SLOT(cbStopNDayScrollerChanged(int)));
	connect(scrSepFlow_, SIGNAL(valueChanged(int)), SLOT(cbSepFlowScrollerChanged(int)));
	connect(scrTermCount_, SIGNAL(valueChanged(int)), SLOT(cbTermCountScrollerChanged(int)));
	connect(scrTransSpeed_, SIGNAL(valueChanged(int)), SLOT(cbTransSpeedScrollerChanged(int)));
	connect(bCreate_, SIGNAL(clicked()), SLOT(cbCreate()));
	connect(bDelete_, SIGNAL(clicked()), SLOT(cbDelete()));
	connect(bFractions_, SIGNAL(clicked()), SLOT(cbFractions()));
	connect(bOK_, SIGNAL(clicked()), SLOT(cbOK()));
	connect(bCancel_, SIGNAL(clicked()), SLOT(cbCancel()));
	connect(bApply_, SIGNAL(clicked()), SLOT(cbApply()));
	connect(bReset_, SIGNAL(clicked()), SLOT(cbReset()));
	connect(bApplyAll_, SIGNAL(clicked()), SLOT(cbApplyAll()));
	connect(bResetAll_, SIGNAL(clicked()), SLOT(cbResetAll()));

	connect(bHelp_, SIGNAL(clicked()), SLOT(cbHelp()));



	QButtonGroup *bg  = new QButtonGroup(this);
	bg->setShown(FALSE);
	bg->insert ( rbNever_ );
	bg->insert ( rbOnce_ );
	bg->insert ( rbAsNec_ );

	connect(rbNever_, SIGNAL(clicked()), SLOT(cbRbNever()));
	connect(rbOnce_, SIGNAL(clicked()), SLOT(cbRbOnce()));
	connect(rbAsNec_, SIGNAL(clicked()), SLOT(cbRbAsNec()));

	rbNever_->setChecked( FALSE );
	rbAsNec_->setChecked( FALSE );
	rbOnce_->setChecked(TRUE );

	/* The dirty indicators are little red circles at the corners of the
	 *  main dialog window.
	 */	

	/* Fill up the combo boxes whose contents will not change.
	 */
	int i;
	for (i = 0; i < num_specs; i++)
		cmbSpecies_->insertItem( specs[i].name, cmbSpecies_->count() );
	for (i = 0; i < num_dams; i++)
		cmbDam_->insertItem( dams[i]->seg_name, cmbDam_->count() );
	struct river_segment *seg;
	for (seg = river_mouth; seg != NULL; seg = seg->next)
		if (seg->seg_type == REACH_SEG) 
			cmbReach_->insertItem( seg->seg_name, cmbReach_->count() );
	cmbStart_->insertItem( csOnDay, cmbStart_->count() );
	cmbStart_->insertItem( csIfCount, cmbStart_->count() );
	cmbRestart_->insertItem( csOnDay, cmbRestart_->count() );
	cmbRestart_->insertItem( csIfCount, cmbRestart_->count() );

	AttachEvents();
	CreateLocalList();

	lock_ = 0;
	updateDialog();
	show();
	cbRunStatusChanged();

	isRunning_ = 1;
}

/* Show or hide "dirty" annunciators.
 */
 void qTransportTool::showDirty( int dirty ) {
	int i;
	if (dirty)
		i = TRUE;
	else
		i = FALSE;
	bpDirty1_->setShown( i );
	bpDirty2_->setShown( i );
	bpDirty3_->setShown( i );
	bpDirty4_->setShown( i );

}

qTransportTool::~qTransportTool()
{
	/* Notify all and sundry that we died. */
	DetachEvents();
	DestroyLocalList();
	theTransportTool_ = NULL;

}

/* Receive notification that run status has changed.
 */
void qTransportTool::cbRunStatusChanged( void ){
	if (run_mode.running) {
		bOK_->setEnabled(FALSE);
		bApply_->setEnabled(FALSE);
		bApplyAll_->setEnabled(FALSE);
	}
	else {
		bOK_->setEnabled(TRUE);
		bApply_->setEnabled(TRUE);
		bApplyAll_->setEnabled(TRUE);
	}
}

/* Notification functions for controls in dialog.
 */
void qTransportTool::cbOK() {
	if (isRunning_) {
		isRunning_ = 0;
		cbApplyAll(  );
		close();
	}
	return; // event handled.
}

void qTransportTool::cbCancel() {
	if (isRunning_) {
		isRunning_ = 0;
		cbResetAll(  );
		close();
	}
	return; // event handled.
}

void qTransportTool::cbApply() {
	if (current_ != NULL)
		ApplyLocalList( current_ );
	return ; // event handled.
}

void qTransportTool::cbReset() {
	if (current_ != NULL)
		ResetLocalList( current_ );
	return; // event handled.
}

void qTransportTool::cbApplyAll() {
	ApplyLocalList( NULL );
	return; // event handled.
}

void qTransportTool::cbResetAll() {
	ResetLocalList( NULL );
	return; // event handled.
}

void qTransportTool::cbHelp() {

	helpManager->showHelp( QString("73.html") );
	return ; // event handled.
}

/* "Create new transport" button pushed.
 */
void qTransportTool::cbCreate() {
	int iDam, iReach, iStart, iRestart, iSpec, i;
	struct translist *trl;
	struct transport *tr;

	/* Create a new transport with the proper starting dam, ending reach,
	 *  and other values. We assume we could only have gotten here if the
	 *  given dam does not have a transport yet,
	 */
	iDam = cmbDam_->currentItem();	
	if (iDam < 0) {
		QMessageBox::warning(this, "Transport Dam - selection error",
                                "Transport Tool",  QMessageBox::NoButton,  QMessageBox::Ok);
		return;
	}
	iReach = cmbReach_->currentItem();
	if (iReach < 0) {
		QMessageBox::warning(this, "Release Reach - selection error",
                                "Transport Tool",  QMessageBox::NoButton,  QMessageBox::Ok);
	
		return;
	}
	iStart = cmbStart_->currentItem();
	if (iStart < 0) {
		QMessageBox::warning(this, "Start Transportation - selection error",
                         "Transport Tool",  QMessageBox::NoButton,  QMessageBox::Ok);
	
		return;
	}
	iRestart = cmbRestart_->currentItem();
	if (iRestart < 0) {
		QMessageBox::warning(this, "Restart Transportation - selection error",
                         "Transport Tool",  QMessageBox::NoButton,  QMessageBox::Ok);
	
		return;
	}
	iSpec = cmbSpecies_->currentItem();
	if (iSpec < 0) {
		QMessageBox::warning(this, "Separation Termination species - selection error",
                     "Transport Tool",  QMessageBox::NoButton,  QMessageBox::Ok);
	
		
		return;
	}

	/* Create and fill in transport record.
	 */
	tr = (struct transport *)malloc( sizeof( struct transport ) );
	trl = (struct translist *)malloc( sizeof( struct translist ) );
	trl->transport = tr;
	trl->dirty = 1;
	localList_.append( trl );
	current_ = trl;

	tr->start_count = tr->start_date = 0;
	i = scrStartTrans_->value();
	tr->start_by_date = !iStart;
	if (tr->start_by_date)
		tr->start_date = i;
	else 
		tr->start_count = i;

	if (rbNever_->isOn())
		tr->max_restarts = 0;
	else if (rbOnce_->isOn())
		tr->max_restarts = 1;
	else
		tr->max_restarts = -1;

	tr->restart_date = tr->restart_count = 0;
	i = scrRestartTrans_->value();
	tr->restart_by_date = !iRestart;
	if (tr->restart_by_date)
		tr->restart_date = i;
	else
		tr->restart_count = i;

	tr->end_date = scrStopDay_->value();
	tr->end_count = scrStopCount_->value();
	tr->num_low_days = scrStopNDay_->value();
	tr->hfl_passed_perc = (float)(scrTermCount_->value() / 100.0);
	tr->hi_flow = scrSepFlow_->value();
	tr->rate = scrTransSpeed_->value();

	tr->source_seg = dams[iDam];
	tr->target_seg = getReach();

	tr->hfl_spec = &(specs[iSpec]);

	updateDialog();

	return; // event handled.
}

/* Get the river segment selected by "cmbReach_", or NULL if any problems.
 */
struct river_segment *qTransportTool::getReach( void ) {
	struct river_segment *seg;
	int iReach = cmbReach_->currentItem();

	if (iReach < 0)
		return NULL;

	for (seg = river_mouth; seg != NULL; seg = seg->next)
		if (seg->seg_type == REACH_SEG) {
			if (iReach == 0)
				return seg;
			iReach--;
		}
	return NULL;
}

/* "Delete this transport" button pushed.
 */
void qTransportTool::cbDelete() {
	int i;

	if (current_ != NULL) {
		for (i = 0; i < (int)(localList_.count()); i++) {
			if (localList_.at(i) == current_) {
				localList_.remove( i );
				free( current_->transport );
				free( current_ );
				current_ = NULL;
				updateDialog();
				break;
			}
		}
	}

	return; // event handled.
}

/* Fire up the "Sepearation Success Fractions" dialog.
 */
void qTransportTool::cbFractions() {
	new qSelectDialogMedBase( &qIdcSeparationProb_,
				new qSelectDialogBase( this,  1 ),
				"fraction returned to river when separating" );
	return;
}

/* Selection events from the radio buttons.
 */
void qTransportTool::cbRbOnce() {
	
	if (current_ != NULL && !lock_) {
		current_->dirty = 1;
		current_->transport->max_restarts = 1;
		showDirty( TRUE );
	}
	return;
}

void qTransportTool::cbRbNever() {

	if (current_ != NULL && !lock_) {
		current_->dirty = 1;
		current_->transport->max_restarts = 0;
		showDirty( TRUE );
	}
	return;
}

void qTransportTool::cbRbAsNec() {

	if (current_ != NULL && !lock_) {
		current_->dirty = 1;
		current_->transport->max_restarts = -1;
		showDirty( TRUE );
	}
	return;
}

/* Selection event from list of existing releases.
 */
void qTransportTool::cbComboExistingChange() {
	int i;

	if (lock_)
		return;

	i = cmbExisting_->currentItem();
	if (i >= 0) {
		current_ = localList_.at(i);
		updateDialog();
	}

	return; // event handled.
}

/* Selection event from list of dams. Seek existing transport record for this
 *  dam if any.
 */
void qTransportTool::cbComboDamChange() {
	int iDam, i;

	if (lock_)
		return;

	iDam = cmbDam_->currentItem();
	if (iDam >= 0) {
		for (i = 0; i < (int)(localList_.count()); i++) {
			current_ = localList_.at(i);
			if (current_->transport->source_seg == dams[iDam])
				break;
		}
		if (i == (int)(localList_.count()))
			current_ = NULL;
		updateDialog();
	}
	return;
}

/* Selection event from list of reaches. Change reach pointer in existing
 *  record, else ignore.
 */
void qTransportTool::cbComboReachChange() {

	if (lock_)
		return;

	if (current_ != NULL) {
		current_->dirty = 1;
		current_->transport->target_seg = getReach();
		showDirty( 1 );
	}
	return;
}

void qTransportTool::cbComboStartChange() {
	int i;

	if (lock_)
		return;

	if (current_ != NULL) {
		i = cmbStart_->currentItem();
		if (i >= 0) {
			current_->dirty = 1;
			current_->transport->start_by_date = 1 - i;
			updateDialog();
		}
	}
	return;
}

void qTransportTool::cbComboRestartChange() {
	int i;

	if (lock_)
		return;

	if (current_ != NULL) {
		i = cmbRestart_->currentItem();
		if (i >= 0) {
			current_->dirty = 1;
			current_->transport->restart_by_date = 1 - i;
			updateDialog();
		}
	}
	return;
}

void qTransportTool::cbComboSpeciesChange() {
	int i;

	if (lock_)
		return;

	if (current_ != NULL) {
		i = cmbSpecies_->currentItem();
		if (i >= 0) {
			current_->dirty = 1;
			current_->transport->hfl_spec = &(specs[i]);
			showDirty( 1 );
		}
	}
	return;
}

/* Edit field and/or scrollbar change notification routines. Each control
 *  has a separate routine.
 */

/* Helper routine to set the date field to reflect the contents of some
 *  edit field.
 */
void qTransportTool::setDateToField( QLineEdit *pzel ) {
	QString cstr;
	int i;

	cstr = pzel->text();
	i = atol( cstr.ascii() );
    formatDate( i, &cstr );
	stDate_->setText( cstr );
}

/* General helper routines. */
int qTransportTool::editChanged( QLineEdit *ed, QScrollBar *scr,
                                    int doDate ) {
	int i, j, curval;
	QString cstr;
	char cBuf[10];

	lock_ = 1;

	/* Copy from edit control to scrollbar. Then copy back to edit field
	 *  (this prevents value from going out-of-range).
	 */
	cstr = ed->text();
	i = atol( cstr.ascii() );
	curval = scr->value();
	if (i != curval) {
		scr->setValue( i );
		j = scr->value();
		if (j != i) {
			sprintf( cBuf, "%d", j );
			ed->setText( cBuf );
			i = j;
		}

		/* Set dirty indicator if necessary.
		 */
		if (current_ != NULL) {
			current_->dirty = 1;
			showDirty( 1 );
		}
	}

	/* Set date indicator if we are editing days of a year.
	 */
	if (doDate && scr->maxValue() == 365)
        setDateToField( ed );
	else
		stDate_->setText( "" );

	/* Unlock and return value from edit control.
	 */
	lock_ = 0;
	return i;
}

int qTransportTool::scrollChanged( QLineEdit *ed, QScrollBar *scr,
                                       int doDate ) {
	int i;
	char cBuf[10];
	QString cstr;

	lock_ = 1;

	/* Copy from scrollbar to edit control.
	 */
	sprintf( cBuf, "%d", i = scr->value() );
	ed->setText( cBuf );

	/* Set dirty indicator if necessary.
	 */
	if (current_ != NULL) {
		current_->dirty = 1;
		showDirty( 1 );
	}

	/* Set date indicator if we are editing days of a year.
	 */
	if (doDate && scr->maxValue() == 365)
        setDateToField( ed );
	else
		stDate_->setText( "" );

	/* Unlock and return value from scrollbar.
	 */
	lock_ = 0;
	return i;
}

/* StartTrans */
void qTransportTool::cbStartTransKillFocus() {
	int i;

	if (lock_) 
		return;

	i = editChanged( edStartTrans_, scrStartTrans_, 1 );
	if (current_ != NULL) {
		if (current_->transport->start_by_date)
			current_->transport->start_date = i;
		else
			current_->transport->start_count = i;
	}

	return; // event handled.
}

void qTransportTool::cbStartTransSetFocus() {
    if (!lock_)
        if (current_ != NULL)
            if (current_->transport->start_by_date)
                setDateToField( edStartTrans_ );
    return ;
}

/*
void qTransportTool::_cbStartTransReturn( zEvH *obj ) {
	qTransportTool *ztt = (qTransportTool *)obj;
	ztt->cbStartTransKillFocus( NULL );
}
*/

void qTransportTool::cbStartTransScrollerChanged( int pos ) {
	int i;

	if (lock_)
		return;

	i = scrollChanged( edStartTrans_, scrStartTrans_, 1 );
	if (current_ != NULL) {
		if (current_->transport->start_by_date)
			current_->transport->start_date = i;
		else
			current_->transport->start_count = i;
	}

	return; // event handled.
}
/*
void qTransportTool::_cbStartTransScrollerChanged(void *msg, const int i){
    qTransportTool *zrt = (qTransportTool *)msg; 
    zrt->cbStartTransScrollerChanged(i);
}
*/
/* RestartTrans */
void qTransportTool::cbRestartTransKillFocus() {
	int i;

	if (lock_) 
		return;

	i = editChanged( edRestartTrans_, scrRestartTrans_, 1 );
	if (current_ != NULL) {
		if (current_->transport->restart_by_date)
			current_->transport->restart_date = i;
		else
			current_->transport->restart_count = i;
	}

	return; // event handled.
}

void qTransportTool::cbRestartTransSetFocus() {
    if (!lock_)
        if (current_ != NULL)
            if (current_->transport->restart_by_date)
                setDateToField( edRestartTrans_ );
    return ;
}
/*
void qTransportTool::_cbRestartTransReturn( zEvH *obj ) {
	qTransportTool *ztt = (qTransportTool *)obj;
	ztt->cbRestartTransKillFocus( NULL );
}
*/

void qTransportTool::cbRestartTransScrollerChanged( int pos ) {
	int i;

	if (lock_)
		return;

	i = scrollChanged( edRestartTrans_, scrRestartTrans_, 1 );
	if (current_ != NULL) {
		if (current_->transport->restart_by_date)
			current_->transport->restart_date = i;
		else
			current_->transport->restart_count = i;
	}

	return; // event handled.
}
/*
void qTransportTool::_cbRestartTransScrollerChanged(void *msg, const int i){
    qTransportTool *zrt = (qTransportTool *)msg; 
    zrt->cbRestartTransScrollerChanged(i);
}
*/
/* StopDay */
void qTransportTool::cbStopDayKillFocus() {
	int i;

	if (lock_) 
		return;

	i = editChanged( edStopDay_, scrStopDay_, 1 );
	if (current_ != NULL)
		current_->transport->end_date = i;

	return; // event handled.
}

void qTransportTool::cbStopDaySetFocus() {
    if (!lock_)
        setDateToField( edStopDay_ );
    return ;
}
/*
void qTransportTool::_cbStopDayReturn( zEvH *obj ) {
	qTransportTool *ztt = (qTransportTool *)obj;
	ztt->cbStopDayKillFocus( NULL );
}
*/
void qTransportTool::cbStopDayScrollerChanged( int pos ) {
	int i;

	if (lock_)
		return;

	i = scrollChanged( edStopDay_, scrStopDay_, 1 );
	if (current_ != NULL)
		current_->transport->end_date = i;

	return; // event handled.
}
/*
void qTransportTool::_cbStopDayScrollerChanged(void *msg, const int i){
    qTransportTool *zrt = (qTransportTool *)msg; 
    zrt->cbStopDayScrollerChanged(i);
}
*/

/* StopCount */
void qTransportTool::cbStopCountKillFocus() {
	int i;

	if (lock_) 
		return;

	i = editChanged( edStopCount_, scrStopCount_, 0 );
	if (current_ != NULL)
		current_->transport->end_count = i;

	return; // event handled.
}
/*
void qTransportTool::_cbStopCountReturn( zEvH *obj ) {
	qTransportTool *ztt = (qTransportTool *)obj;
	ztt->cbStopCountKillFocus( NULL );
}
*/
void qTransportTool::cbStopCountScrollerChanged( int pos ) {
	int i;

	if (lock_)
		return;

	i = scrollChanged( edStopCount_, scrStopCount_, 0 );
	if (current_ != NULL)
		current_->transport->end_count = i;

	return; // event handled.
}
/*
void qTransportTool::_cbStopCountScrollerChanged(void *msg, const int i){
    qTransportTool *zrt = (qTransportTool *)msg; 
    zrt->cbStopCountScrollerChanged(i);
}
*/

/* StopNDay */
void qTransportTool::cbStopNDayKillFocus() {
	int i;

	if (lock_) 
		return;

	i = editChanged( edStopNDay_, scrStopNDay_, 0 );
	if (current_ != NULL)
		current_->transport->num_low_days = i;

	return; // event handled.
}
/*
void qTransportTool::_cbStopNDayReturn( zEvH *obj ) {
	qTransportTool *ztt = (qTransportTool *)obj;
	ztt->cbStopNDayKillFocus( NULL );
}
*/
void qTransportTool::cbStopNDayScrollerChanged( int pos ) {
	int i;

	if (lock_)
		return;

	i = scrollChanged( edStopNDay_, scrStopNDay_, 0 );
	if (current_ != NULL)
		current_->transport->num_low_days = i;

	return; // event handled.
}
/*
void qTransportTool::_cbStopNDayScrollerChanged(void *msg, const int i){
    qTransportTool *zrt = (qTransportTool *)msg; 
    zrt->cbStopNDayScrollerChanged(i);
}
*/
/* SepFlow */
void qTransportTool::cbSepFlowKillFocus() {
	int i;

	if (lock_) 
		return;

	i = editChanged( edSepFlow_, scrSepFlow_, 0 );
	if (current_ != NULL)
		current_->transport->hi_flow = i;

	return; // event handled.
}
/*
void qTransportTool::_cbSepFlowReturn( zEvH *obj ) {
	qTransportTool *ztt = (qTransportTool *)obj;
	ztt->cbSepFlowKillFocus( NULL );
}
*/
void qTransportTool::cbSepFlowScrollerChanged( int pos ) {
	int i;

	if (lock_)
		return;

	i = scrollChanged( edSepFlow_, scrSepFlow_, 0 );
	if (current_ != NULL)
		current_->transport->hi_flow = i;

	return; // event handled.
}
/*
void qTransportTool::_cbSepFlowScrollerChanged(void *msg, const int i){
    qTransportTool *zrt = (qTransportTool *)msg; 
    zrt->cbSepFlowScrollerChanged(i);
}
*/
/* TermCount */
void qTransportTool::cbTermCountKillFocus() {
	int i;

	if (lock_) 
		return;

	i = editChanged( edTermCount_, scrTermCount_, 0 );
	if (current_ != NULL)
		current_->transport->hfl_passed_perc = (float)(i / 100.0);

	return; // event handled.
}
/*
void qTransportTool::_cbTermCountReturn( zEvH *obj ) {
	qTransportTool *ztt = (qTransportTool *)obj;
	ztt->cbTermCountKillFocus( NULL );
}
*/
void qTransportTool::cbTermCountScrollerChanged( int pos ) {
	int i;

	if (lock_)
		return;

	i = scrollChanged( edTermCount_, scrTermCount_, 0 );
	if (current_ != NULL)
		current_->transport->hfl_passed_perc = (float)(i / 100.0);

	return; // event handled.
}
/*
void qTransportTool::_cbTermCountScrollerChanged(void *msg, const int i){
    qTransportTool *zrt = (qTransportTool *)msg; 
    zrt->cbTermCountScrollerChanged(i);
}
*/
/* TransSpeed */
void qTransportTool::cbTransSpeedKillFocus() {
	int i;

	if (lock_) 
		return;

	i = editChanged( edTransSpeed_, scrTransSpeed_, 0 );
	if (current_ != NULL)
		current_->transport->rate = i;

	return; // event handled.
}
/*
void qTransportTool::_cbTransSpeedReturn( zEvH *obj ) {
	qTransportTool *ztt = (qTransportTool *)obj;
	ztt->cbTransSpeedKillFocus( NULL );
}
*/
void qTransportTool::cbTransSpeedScrollerChanged( int pos ) {
	int i;

	if (lock_)
		return;

	i = scrollChanged( edTransSpeed_, scrTransSpeed_, 0 );
	if (current_ != NULL)
		current_->transport->rate = i;

	return; // event handled.
}
/*
void qTransportTool::_cbTransSpeedScrollerChanged(void *msg, const int i){
    qTransportTool *zrt = (qTransportTool *)msg; 
    zrt->cbTransSpeedScrollerChanged(i);
}
*/
/* Attach/detach with the data token's invalidate event and get notified
 *  when data token has changed. */
void qTransportTool::AttachEvents( void ) {
	connect(qEvents::Instance(), SIGNAL(GERunStatusChanged_()), SLOT(cbRunStatusChanged()));
	//GERunStatusChanged_.attach( this, &qTransportTool::_cbRunStatusChanged );
//	GEPurgeAllIDCs_.attach( this, &qTransportTool::_cbPurgeAll );
}

void qTransportTool::DetachEvents( void ) {
	//GERunStatusChanged_.detach( this, &qTransportTool::_cbRunStatusChanged );
//	GEPurgeAllIDCs_.detach( this, &qTransportTool::_cbPurgeAll );
}

void qTransportTool::cbPurgeAll(){
	DestroyLocalList();
	CreateLocalList();
	updateDialog();
}
/*
void qTransportTool::_cbPurgeAll(void *msg){
    qTransportTool *zrt = (qTransportTool *)msg;
    zrt->cbPurgeAll();
}
*/
/* Routine to fill the dialog fields from the current list element.
 */
void qTransportTool::updateDialog( void ) {
	int i, j, k;
	char cBuf[10];
	QString cstr, cstrCur;

	if (lock_)
		return;
	lock_ = 1;

	/* bring "start transportation" fields into agreement with current_
	 */
	if (current_ == NULL) {
		i = 15;
		j = 0;
	}
	else if (current_->transport->start_by_date) {
		i = current_->transport->start_date;
		j = 0;
	}
	else {
		i = current_->transport->start_count;
		j = 1;
	}
	stStartRange_->setText( j ? "2000" : "365" );
	scrStartTrans_->setMinValue(0);
	scrStartTrans_->setMaxValue(  j ? 2000 : 365 );
	QListBox *lb = cmbStart_->listBox();
	int ind = lb->index(lb->findItem ( j ? csIfCount : csOnDay, Qt::ExactMatch));
	if(ind >= 0) cmbStart_->setCurrentItem(ind);	
    scrStartTrans_->setValue( i );
	sprintf( cBuf, "%d", i );
	edStartTrans_->setText( cBuf );

	/* Bring "restart transportation" fields in agreement with current_
	 */
	//rbAsNec_->check( FALSE );
	//rbOnce_->check( FALSE );
	//rbNever_->check( FALSE );
	if (current_ == NULL) {
		i = 500;
		j = 1;
		rbOnce_-> setChecked( TRUE );
	}
	else {
		if (current_->transport->max_restarts == -1)
			rbAsNec_-> setChecked( TRUE );
		else if (current_->transport->max_restarts == 0)
			rbNever_-> setChecked( TRUE );
		else 
			rbOnce_-> setChecked( TRUE );

		if (current_->transport->restart_by_date) {
			i = current_->transport->restart_date;
			j = 0;
		}
		else {
			i = current_->transport->restart_count;
			j = 1;
		}
	}
	stRestartRange_->setText( j ? "2000" : "365" );
	scrRestartTrans_->setMinValue(0);
	scrRestartTrans_->setMaxValue( j ? 2000 : 365 );
	//cmbRestart_->selection( j ? csIfCount : csOnDay );
	lb = cmbRestart_->listBox();
	ind = lb->index(lb->findItem ( j ? csIfCount : csOnDay, Qt::ExactMatch));
	if(ind >= 0) cmbRestart_->setCurrentItem(ind);	

    scrRestartTrans_->setValue( i );
	sprintf( cBuf, "%d", i );
	edRestartTrans_->setText( cBuf );

	/* Put "stop transportation" stuff into line with current_
	 */
	if (current_ == NULL) {
		i = 200;
		j = 500;
		k = 5;
	}
	else {
		i = current_->transport->end_date;
		j = current_->transport->end_count;
		k = current_->transport->num_low_days;
	}
    scrStopDay_->setValue( i );
	sprintf( cBuf, "%d", i );
	edStopDay_->setText( cBuf );
    scrStopCount_->setValue( j );
	sprintf( cBuf, "%d", j );
	edStopCount_->setText( cBuf );
    scrStopNDay_->setValue( k );
	sprintf( cBuf, "%d", k );
	edStopNDay_->setText( cBuf );

	/* Put separation stuff in line with current_.
	 */
	if (current_ == NULL) {
		i = 125;
		j = 80;
	}
	else {
		i = (int)(current_->transport->hi_flow);
		j = (int)(current_->transport->hfl_passed_perc * 100.0);
	}
    scrSepFlow_->setValue( i );
	sprintf( cBuf, "%d", i );
	edSepFlow_->setText( cBuf );
    scrTermCount_->setValue( j );
	sprintf( cBuf, "%d", j );
	edTermCount_->setText( cBuf );

	/* Put transport speed in line with current_.
	 */
	if (current_ == NULL)
		i = 100;
	else
		i = (int)current_->transport->rate;
    scrTransSpeed_->setValue( i );
	sprintf( cBuf, "%d", i );
	edTransSpeed_->setText( cBuf );

	/* Set selections in remaining comboboxes.
	 */
	if (current_ != NULL) {
		//cmbDam_->selection( current_->transport->source_seg->seg_name );
		lb = cmbDam_->listBox();
		ind = lb->index(lb->findItem ( current_->transport->source_seg->seg_name, Qt::ExactMatch));
		if(ind >= 0) cmbDam_->setCurrentItem(ind);
		///cmbReach_->selection( current_->transport->target_seg->seg_name );
		lb = cmbReach_->listBox();
		ind = lb->index(lb->findItem ( current_->transport->target_seg->seg_name, Qt::ExactMatch));
		if(ind >= 0) cmbReach_->setCurrentItem(ind);
		//cmbSpecies_->selection( current_->transport->hfl_spec->name );
		lb = cmbSpecies_->listBox();
		ind = lb->index(lb->findItem (  current_->transport->hfl_spec->name, Qt::ExactMatch));
		if(ind >= 0) cmbSpecies_->setCurrentItem(ind);

	}

	/* Put existing transports in the combobox for that
	 */
	cmbExisting_->clear();
	for (i = 0; i < (int)(localList_.count()); i++) {
		cmbExisting_->insertItem( localList_.at(i)->transport->source_seg->seg_name,
								cmbExisting_->count() );
	}
	if (current_ != NULL){
		lb = cmbExisting_->listBox();
		ind = lb->index(lb->findItem (current_->transport->source_seg->seg_name, Qt::ExactMatch));
		if(ind >= 0) cmbExisting_->setCurrentItem(ind);
		//cmbExisting_->selection( current_->transport->source_seg->seg_name );

	}

	/* Show or hide "dirty" annunciators.
	 */
	showDirty( current_ != NULL && current_->dirty != 0 );

	/* Some controls are enabled or disabled depending on what we are
	 *  editing.
	 */
	if (current_ == NULL) {
		/* No current edit object.
		 */
		stNew_->setShown( FALSE );
		stExisting_->setShown( FALSE );
		bDelete_->setEnabled(FALSE);
		bCreate_->setEnabled(TRUE);
		edStartTrans_->setEnabled(FALSE);
		edRestartTrans_->setEnabled(FALSE);
		edStopDay_->setEnabled(FALSE);
		edStopCount_->setEnabled(FALSE);
		edStopNDay_->setEnabled(FALSE);
		edSepFlow_->setEnabled(FALSE);
		edTermCount_->setEnabled(FALSE);
		edTransSpeed_->setEnabled(FALSE);
		scrStartTrans_->setEnabled(FALSE);
		scrRestartTrans_->setEnabled(FALSE);
		scrStopDay_->setEnabled(FALSE);
		scrStopCount_->setEnabled(FALSE);
		scrStopNDay_->setEnabled(FALSE);
		scrSepFlow_->setEnabled(FALSE);
		scrTermCount_->setEnabled(FALSE);
		scrTransSpeed_->setEnabled(FALSE);
		bApply_->setEnabled(FALSE);
		bReset_->setEnabled(FALSE);
		rbNever_->setEnabled(FALSE);
		rbOnce_->setEnabled(FALSE);
		rbAsNec_->setEnabled(FALSE);
	}
	else {
		/* We have a current edit object.
		 */
		bDelete_->setEnabled(TRUE);
		bApply_->setEnabled(TRUE);
		bCreate_->setEnabled(FALSE);
		edStartTrans_->setEnabled(TRUE);
		edRestartTrans_->setEnabled(TRUE);
		edStopDay_->setEnabled(TRUE);
		edStopCount_->setEnabled(TRUE);
		edStopNDay_->setEnabled(TRUE);
		edSepFlow_->setEnabled(TRUE);
		edTermCount_->setEnabled(TRUE);
		edTransSpeed_->setEnabled(TRUE);
		scrStartTrans_->setEnabled(TRUE);
		scrRestartTrans_->setEnabled(TRUE);
		scrStopDay_->setEnabled(TRUE);
		scrStopCount_->setEnabled(TRUE);
		scrStopNDay_->setEnabled(TRUE);
		scrSepFlow_->setEnabled(TRUE);
		scrTermCount_->setEnabled(TRUE);
		scrTransSpeed_->setEnabled(TRUE);
		rbNever_->setEnabled(TRUE);
		rbOnce_->setEnabled(TRUE);
		rbAsNec_->setEnabled(TRUE);

		if (current_->transport->source_seg->current->dam.transport != NULL) {
			stNew_->setShown( FALSE );
			stExisting_->setShown( TRUE );
			bReset_->setEnabled(TRUE);
		}
		else {
			stNew_->setShown( TRUE );
			stExisting_->setShown( FALSE );
			bReset_->setEnabled(FALSE);
		}
	}

	/* Clear the date tracking field.
	 */
	stDate_->setText( "" );

	lock_ = 0;

}

/* Routines to manage the local list of release structures.
 */
void qTransportTool::DestroyLocalList( void ) {
	int i;

	for (i = 0; i < (int)(localList_.count()); i++) {
		free( localList_.at(i)->transport );
		free( localList_.at(i) );
	}
	localList_.clear();
	current_ = NULL;
}

void qTransportTool::CreateLocalList( void ) {
	int i;
	struct translist *trl;
	struct transport *tr;

	DestroyLocalList();
	for (i = 0; i < num_dams; i++) {
		if (dams[i]->current->dam.transport != NULL) {
			tr = (struct transport *)malloc( sizeof( struct transport ) );
			trl = (struct translist *)malloc( sizeof( struct translist ) );
			trl->transport = tr;
			trl->dirty = 0;
			bcopy( dams[i]->current->dam.transport, tr, 
								sizeof( struct transport ) );
			if (current_ == NULL)
				current_ = trl;
			localList_.append( trl );
		}
	}
}

void qTransportTool::ApplyLocalList( struct translist *applyMe ) {
	int i;

	if (applyMe) {
		// Apply single element.
		if (applyMe->transport->source_seg->current->dam.transport == NULL) {
			applyMe->transport->source_seg->current->dam.transport =
				(struct transport *)malloc( sizeof( struct transport ) );
		}
		bcopy( applyMe->transport,
					applyMe->transport->source_seg->current->dam.transport,
					sizeof( struct transport ) );
		applyMe->dirty = 0;
	}
	else {
		// Apply entire list recursively.
		lock_ = 1;
		for (i = 0; i < num_dams; i++) {
			if (dams[i]->current->dam.transport != NULL) {
				free( dams[i]->current->dam.transport );
				dams[i]->current->dam.transport = NULL;
			}
		}
		for (i = 0; i < (int)(localList_.count()); i++) {
			ApplyLocalList( localList_.at(i) );
		}
		lock_ = 0;
	}
	GOutputsValid_[1] = 0;   /* passage invalid */
	updateDialog();
	theMapPane_->repaint();
	qEvents::Instance()->emitGEUpdateAllDataTokens_();
}

void qTransportTool::ResetLocalList( struct translist *resetMe ) {
	QString cstr;
	int i;
	
	if (resetMe != NULL) {
		// Reset single element.
		if (resetMe->transport->source_seg->current->dam.transport != NULL) {
			// Copy values from existing transport being edited.
			bcopy( resetMe->transport->source_seg->current->dam.transport,
					resetMe->transport, sizeof( struct transport ) );
		}
		else {
			// Set default values for a new transport.
			resetMe->transport->start_by_date = 1;
			resetMe->transport->start_date = 15;
			resetMe->transport->start_count = 0;
			resetMe->transport->max_restarts = 1;
			resetMe->transport->restart_by_date = 0;
			resetMe->transport->restart_count = 500;
			resetMe->transport->restart_date = 0;
			resetMe->transport->end_date = 200;
			resetMe->transport->end_count = 500;
			resetMe->transport->num_low_days = 5;
			resetMe->transport->hfl_passed_perc = 0.80;
			resetMe->transport->hi_flow = 125;
			resetMe->transport->rate = 100;
			resetMe->transport->hfl_spec = &(specs[0]);
		}
		resetMe->dirty = 0;
	}
	else {
		// Reset entire list.
		if (current_ != NULL)
			cstr = current_->transport->source_seg->seg_name;
		CreateLocalList();
		for (i = 0; i < (int)(localList_.count()); i++) {
			if (!cstr.compare( localList_.at(i)->transport->source_seg->seg_name )) {
				current_ = localList_.at(i);
				break;
			}
		}
	}
	updateDialog();
}

