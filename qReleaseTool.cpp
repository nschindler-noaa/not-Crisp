/* qReleaseTool.h
 * This class defines a modal dialog which edits release structures
 *  via comboboxes, scrollbars, and edit boxes. An editable graph is also
 *  used. The dialog is based on the IDD_RELEASETOOL resource template.
 *
 * This is a special-purpose dialog which will probably not be subclassed.
 */

#include "qReleaseTool.h"
#include "resource.h"
#include "qGlobalEvents.h"
#include "run_mode.h"
#include "release.h"
#include "realtime.h"
#include "qReleaseEGDT.h"
#include "qReleaseEGM.h"
#include "qGlobalEvents.h"
#include "mapConstants.h"
#include "qUtils.h"
#include "qMapPane.h"
#include "Qcrisp.h"
#include <stdlib.h>
#include <qlistbox.h>
#include <qmessagebox.h>
#include <qcheckbox.h>
#include "qHelpManager.h"


extern qReleaseTool * theReleaseTool_ ;  // pathological data connections
extern qMapPane * theMapPane_ ;

/* These help us remember what we were editing so that we can try to get
 *  back to it.
 */
QString qReleaseTool::csSiteSave_, 
			qReleaseTool::csSpeciesSave_, 
			qReleaseTool::csStockSave_;

qReleaseTool::qReleaseTool( QWidget *w, const char *feature )
	: ReleaseToolBase(NULL,"Relase Tool", FALSE, Qt::WDestructiveClose | WType_TopLevel | WStyle_SysMenu | WStyle_Minimize | WStyle_Dialog)
		
{
	consCommon();
	selectRelease( feature );
}

qReleaseTool::qReleaseTool( QWidget *w, int iExRel )
	: ReleaseToolBase(NULL,"Relase Tool", FALSE, Qt::WDestructiveClose | WType_TopLevel | WStyle_SysMenu | WStyle_Minimize | WStyle_Dialog)
{
	consCommon();
	selectRelease( iExRel );
}

qReleaseTool::qReleaseTool( QWidget *w )
	: ReleaseToolBase(NULL,"Release Tool", FALSE, Qt::WDestructiveClose | WType_TopLevel | WStyle_SysMenu | WStyle_Minimize | WStyle_Dialog)
{

	consCommon();
	
	QListBox * lb;
	QListBoxItem * lbi;

	setExactSelection( cmbReleaseSite_, csSiteSave_.ascii() );
	//cmbSpecies_->selection( csSpeciesSave_ );
	lb = cmbSpecies_->listBox();
	lbi = lb->findItem(csSpeciesSave_);
	cmbSpecies_->setCurrentItem(lb->index(lbi));
	//lb->setSelected(lbi, true);

	lb = cmbStock_->listBox();
	lbi = lb->findItem(csStockSave_);
	cmbStock_->setCurrentItem(lb->index(lbi));
	//lb->setSelected (lbi, true);
	//cmbStock_->selection( csStockSave_ );
	cbComboSelChange();
	
}

/* Common code called by all constructors.
 */
void qReleaseTool::consCommon( void ) {
	
	//setFixedSize(size());

	localList_ = NULL;
	lock_ = 0;
	stNew_->setShown(FALSE);
	
	scrLength_->setMinValue (0);
	scrLength_->setMaxValue (300);
	scrSmoltStart_->setMinValue (0);
	scrSmoltStart_->setMaxValue (365);
	scrSmoltStop_->setMinValue (0);
	scrSmoltStop_->setMaxValue (365);
	scrReleaseStart_->setMinValue (0);
	scrReleaseStart_->setMaxValue (365);
	/* Get the child window controls hooked up. */

	connect(bCreate_, SIGNAL(clicked()), this, SLOT(cbCreate()));

	connect(bDelete_, SIGNAL(clicked()), this, SLOT(cbDelete()));
	/* NOTE - no notification function for bMouseUpdate_ */
	connect(bOK_, SIGNAL(clicked()), this , SLOT(cbOK()));
	connect(bCancel_, SIGNAL(clicked()), this , SLOT(cbCancel()));
	connect(bApply_, SIGNAL(clicked()), this , SLOT(cbApply()));
	connect(bReset_, SIGNAL(clicked()), this , SLOT(cbReset()));
	connect(bApplyAll_, SIGNAL(clicked()), this , SLOT(cbApplyAll()));
	connect(bResetAll_, SIGNAL(clicked()), this , SLOT(cbResetAll()));
	connect(bHelp_, SIGNAL(clicked()), this , SLOT(cbHelp()));
    
	 /* Fill up the combo boxes whose contents will not change. Set
	 *  default selections.
	 */
	struct release_site *rel;
	for (rel = release_site; rel != NULL; rel = rel->next)
		cmbReleaseSite_->insertItem( rel->name, cmbReleaseSite_->count() );
	cmbReleaseSite_->setCurrentItem( 0 );
	int i;
	for (i = 0; i < num_specs; i++)
		cmbSpecies_->insertItem( specs[i].name, cmbSpecies_->count() );
	cmbSpecies_->setCurrentItem( 0 );
	for (i = 0; i < stocks.num; i++)
		cmbStock_->insertItem( stocks.s[i]->name, cmbStock_->count() );
	cmbStock_->insertItem( "Generic", cmbStock_->count() );
	cmbStock_->setCurrentItem( 0 );

	/* Attach event handlers for the child window controls.
	 */
	
	connect(cmbReleaseSite_, SIGNAL(activated(int)), this, SLOT(cbComboSelChanged() ));
	
	connect(cmbSpecies_, SIGNAL(activated(int)), this, SLOT(cbComboSelChanged() ));
	connect(cmbStock_, SIGNAL(activated(int)), this, SLOT(cbComboSelChanged() ));
	connect(cmbExistingReleases_, SIGNAL(activated(int)), this, SLOT(cbComboSelExisting() ));

	focusLostEventFilter * fef;
	connect(edSmoltStart_, SIGNAL(lostFocus()), this, SLOT(cbSStartKillFocus()));
	connect(edSmoltStart_, SIGNAL(returnPressed()), this, SLOT(SStartEditChanged()));
	fef = new focusLostEventFilter(this);
	edSmoltStart_->installEventFilter(fef);  //install the filter to revieve focus lost events
	connect(fef, SIGNAL(focusGained()), this, SLOT(cbSStartSetFocus()));  //connect the filter to the slot
  
	connect(edSmoltStop_, SIGNAL(lostFocus()), this, SLOT(cbSStopKillFocus()));
	connect(edSmoltStop_, SIGNAL(returnPressed()), this, SLOT(RStopEditChanged()));
	fef = new focusLostEventFilter(this);
	edSmoltStop_->installEventFilter(fef);  //install the filter to revieve focus lost events
	connect(fef, SIGNAL(focusGained()), this, SLOT(cbSStopSetFocus()));  //connect the filter to the slot
   
	connect(edReleaseStart_, SIGNAL(lostFocus()), this, SLOT(cbRStartKillFocus()));
	connect(edReleaseStart_, SIGNAL(returnPressed()), this, SLOT(RStartEditChanged()));
	fef = new focusLostEventFilter(this);
	edReleaseStart_->installEventFilter(fef);  //install the filter to revieve focus lost events
	connect(fef, SIGNAL(focusGained()), this, SLOT(cbRStartSetFocus()));  //connect the filter to the slot
   
	connect(edLength_, SIGNAL(lostFocus()), this, SLOT(cbLengthKillFocus()));
	connect(edLength_, SIGNAL(returnPressed()), this, SLOT(LengthEditChanged()));
	fef = new focusLostEventFilter(this);
	edLength_->installEventFilter(fef);  //install the filter to revieve focus lost events
	connect(fef, SIGNAL(focusGained()), this, SLOT(cbLengthSetFocus()));  //connect the filter to the slot

	connect(scrSmoltStart_, SIGNAL(valueChanged(int)), this, SLOT(cbSStartScrollerChanged(int)));
	connect(scrSmoltStop_, SIGNAL(valueChanged(int)), this, SLOT(cbSStopScrollerChanged(int)));
	connect(scrReleaseStart_, SIGNAL(valueChanged(int)), this, SLOT(cbRStartScrollerChanged(int)));
	connect(scrLength_, SIGNAL(valueChanged(int)), this, SLOT(cbLengthScrollerChanged(int)));

	cmbExistingReleases_->insertItem( "", 0);
	cmbExistingReleases_->setCurrentItem ( 0 );   //set the first release to the current release
	cbComboSelExisting();  // make the first one active

	AttachEvents();
	CreateLocalList();

	/* Create editable graph for the release schedules.
	 */
	
	egdt_ = new qReleaseEGDT( this, "Release Schedule" );
	gd_ = new qGraphDialog(NULL , HIDE_DONE | SHOW_DATE | SHOW_BOTTOM,
			                    WType_TopLevel | WStyle_SysMenu | WStyle_Minimize | WStyle_Dialog ,egdt_->getGraphTitle().ascii());
	gd_->show();
	show();
	gd_->move(x() + 25, y() + 25);
	egm_ = new qReleaseEGM(gd_, egdt_, this);

	updateDialog();
	
	gd_->raise (); //make the grpah window visible
	cbRunStatusChanged();

	

	isRunning_ = 1;
	
}

/* These routines select a given release site, or existing release. They can
 *  be called by the constructors or by the main menu system.
 */
void qReleaseTool::selectRelease( const char *feature ) {
	QString csRel;
	struct release *rel;

	/* See if we can find an existing release with the proper release site.
	 */
	lock_ = 1;
	setExactSelection( cmbReleaseSite_, feature );	
	csRel = cmbReleaseSite_->currentText();	
	lock_ = 0;

	current_ = NULL;
	for (rel = localList_; rel != NULL; rel = rel->next) {
		if (!csRel.compare( QString(rel->site->name) )) {
			// Found first matching release in local list.
			current_ = rel;
			break;
		}
	}

	updateDialog();
}

void qReleaseTool::selectRelease( int iExRel ) {
	struct release *rel;

	for (rel = localList_; rel != NULL && iExRel > 0; iExRel--, rel = rel->next)
		continue;
	if (rel != NULL) {
		current_ = rel;
		updateDialog();
	}
}

qReleaseTool::~qReleaseTool()
{
	/* Notify all and sundry that we died. */
	DetachEvents();
	emit evRemove();
	DestroyLocalList();
	theReleaseTool_ = NULL;

}

void qReleaseTool::saveLocation( void ) {
	csSiteSave_ = cmbReleaseSite_->currentText();	
	csSpeciesSave_ = cmbSpecies_->currentText();	
	csStockSave_ = cmbStock_->currentText();
}

/* Notification functions for controls in dialog.
 */
void qReleaseTool::cbOK() {
	if (isRunning_ == 1) {
		isRunning_ = 0;
		saveLocation();
		cbApplyAll(  );
		setShown(FALSE); 
		//close();
	}
	return; // event handled.
}

void qReleaseTool::cbCancel() {
	if (isRunning_ == 1) {
		isRunning_ = 0;
		saveLocation();
		cbResetAll(  );
		setShown(FALSE);	
	}
	return; // event handled.
}

void qReleaseTool::setShown(bool show){
	ReleaseToolBase::setShown(show);	
	if(show == TRUE){
		//Reinitialize the graph windows
		egdt_ = new qReleaseEGDT( this, "Release Schedule" );
		gd_ = new qGraphDialog(NULL , HIDE_DONE | SHOW_DATE | SHOW_BOTTOM,
			                    Qt::WType_TopLevel  ,egdt_->getGraphTitle().ascii());
		egm_ = new qReleaseEGM(gd_, egdt_, this);

		isRunning_ = 1;
	}
	gd_->setShown(show);

}

void qReleaseTool::cbApply() {
	if (current_ != NULL) {
		egdt_->SetData();
		ApplyLocalList( current_ );
		emit evInvalidate();
		
	}
	return ; // event handled.
}

void qReleaseTool::cbReset() {
	if (current_ != NULL) {
		ResetLocalList( current_ );
		emit evInvalidate();
	}
	return ; // event handled.
}

void qReleaseTool::cbApplyAll() {
	egdt_->SetData();
	ApplyLocalList( NULL );
	emit evInvalidate();
	return; // event handled.
}

void qReleaseTool::cbResetAll() {
	ResetLocalList( NULL );
	emit evInvalidate();
	return; // event handled.
}

void qReleaseTool::cbHelp() {
	helpManager->showHelp( QString("72.html") );
	return; // event handled.
}

/* "Create new release" button pushed.
 */
void qReleaseTool::cbCreate() {
	QString csRel, csSpec, csStock;
	struct release *rel;
	struct release_site *relst;
	int i;
	/* Create a new release with the proper release site,
	 *  species, and stock.
	 */
	csRel = cmbReleaseSite_->currentText();	
	csSpec = cmbSpecies_->currentText();	
	csStock = cmbStock_->currentText();	
	
	rel = construct_release();

	for (relst = release_site; relst != NULL; relst = relst->next)
		if (!csRel.compare( relst->name )) {
			rel->site = relst;
			break;
		}
	if (relst == NULL) {
		//delete new zMessage( this, "Could not find release site",
		//						"Release Tool" );
		QMessageBox::information(this, "Could not find release site","Release Tool", QMessageBox::Ok);
		free( rel );
		return ;
	}

	for (i = 0; i < num_specs; i++)
		if (!csSpec.compare( specs[i].name ) ) {
			rel->species = &(specs[i]);
			break;
		}
	if (i == num_specs) {
		//delete new zMessage( this, "Could not find species",
		//						"Release Tool" );
		QMessageBox::information(this, "Could not find species","Release Tool", QMessageBox::Ok);
		free( rel );
		return ;
	}

	for (i = 0; i < stocks.num; i++)
		if (!csStock.compare( stocks.s[i]->name ) ) {
			rel->stock = stocks.s[i];
			break;
		}
	if (i == stocks.num)
		rel->stock = &(rel->species->stock);

	if (current_ != NULL) {
		rel->next = current_->next;
		rel->prev = current_;
		current_->next = rel;
	}
	else {
		rel->next = localList_;
		rel->prev = NULL;
		localList_ = rel;
	}
	if (rel->next != NULL)
		rel->next->prev = rel;
	current_ = rel;
	updateDialog();

	return ; // event handled.
}

/* "Delete this release" button pushed. NOTE that local list is never
 *  completely empty.
 */
void qReleaseTool::cbDelete() {
	struct release *rel = NULL;

	if (current_->next != NULL) {
		current_->next->prev = current_->prev;
		rel = current_->next;
	}
	if (current_->prev != NULL) {
		current_->prev->next = current_->next;
		rel = current_->prev;
	}
	if (current_ == localList_)
		localList_ = current_->next;
	free( current_ );
	current_ = rel;

	updateDialog();

	return ; // event handled.
}

/* Selection event from release site, species, or stock combobox.
 */
void qReleaseTool::cbComboSelChange() {
	QString csRel, csSpec, csStock;
	struct release *rel;

	/* See if we can find an existing release with the proper release site,
	 *  species, and stock.
	 */
	csRel = cmbReleaseSite_->currentText();	
	csSpec = cmbSpecies_->currentText();	
	csStock = cmbStock_->currentText();	

	current_ = NULL;
	for (rel = localList_; rel != NULL; rel = rel->next) {
		if (!csRel.compare( rel->site->name ) &&
			!csSpec.compare( rel->species->name ) &&
			(!csStock.compare( rel->stock->name ) ||
			!csStock.compare( "Generic" ) ) ) {
			// Found first matching release in local list.
			current_ = rel;
			break;
		}
	}

	updateDialog();

	return ; // event handled.
}

/* Selection event from list of existing releases.
 */
void qReleaseTool::cbComboSelExisting() {
	int i;
	i = cmbExistingReleases_->currentItem();
	if (i >= 0)
		selectRelease( i );

	return ; // event handled.
}

/* Helper routine to set the date field to reflect the contents of some
 *  edit field.
 */
void qReleaseTool::setDateToField( QLineEdit *pzel ) {
    if (!lock_) {
        if (pzel != NULL) {
	        QString cstr;
	        int i;
            cstr = pzel->text();
		    i = atol( cstr.ascii() );
            formatDate( i, &cstr );
		    stDate_->setText( cstr );
        }
        else
            stDate_->setText( "" );
	}
}

/* Edit field has been changed (seperate routines for each one).
 */
void qReleaseTool::cbSStartSetFocus() {
    setDateToField( edSmoltStart_ );
	return;
}

void qReleaseTool::cbSStopSetFocus() {
    setDateToField( edSmoltStop_ );
	return;
}

void qReleaseTool::cbRStartSetFocus() {
    setDateToField( edReleaseStart_ );
	return;
}

void qReleaseTool::cbLengthSetFocus() {
    setDateToField( NULL );
	return;
}

void qReleaseTool::cbSStartKillFocus() {
	SStartEditChanged();
	return;
}

void qReleaseTool::cbSStopKillFocus() {
	SStopEditChanged();
	return;
}

void qReleaseTool::cbRStartKillFocus() {
	RStartEditChanged();
	return;
}

void qReleaseTool::cbLengthKillFocus() {
	LengthEditChanged();
	return;
}


void qReleaseTool::SStartEditChanged( void ) {
	QString cstr;
	int i;

	if (!lock_) {
		cstr = edSmoltStart_->text();
		i = atol( cstr.ascii() );
		if (current_ != NULL && current_->smolt_onset != i) {
			lock_ = 1;
			scrSmoltStart_->setValue(i);
			current_->smolt_onset = i;
			current_->dirty_flag = 1;
			lock_ = 0;
			updateDialog();
		}


		/* NOTE this must come after updateDialog(). */
        setDateToField( edSmoltStart_ );
	}
}

void qReleaseTool::SStopEditChanged( void ) {
	QString cstr;
	int i;

	if (!lock_) {
		cstr = edSmoltStop_->text();
		i = atol( cstr.ascii() );
		if (current_ != NULL && current_->smolt_finish != i) {
			lock_ = 1;
			scrSmoltStop_->setValue(i);
			current_->smolt_finish = i;
			current_->dirty_flag = 1;
			lock_ = 0;
			updateDialog();
		}


		/* NOTE this must come after updateDialog(). */
        setDateToField( edSmoltStop_ );
	}
}

void qReleaseTool::RStartEditChanged( void ) {
	QString cstr;
	int i;

	if (!lock_) {
		cstr = edReleaseStart_->text();
		i = atol( cstr.ascii() );
		if (current_ != NULL && current_->start_date != i) {
			lock_ = 1;
			scrReleaseStart_->setValue(i);
			current_->start_date = i;
			current_->dirty_flag = 1;
			lock_ = 0;
			updateDialog();
		}


		/* NOTE this must come after updateDialog(). */
        setDateToField( edReleaseStart_ );
	}
}

void qReleaseTool::LengthEditChanged( void ) {
	QString cstr;
	int i;

	if (!lock_) {
		cstr = edLength_->text();
		i = atol( cstr.ascii() );
		if (current_ != NULL && current_->length != i) {
			lock_ = 1;
			scrLength_->setValue(i);
			current_->length = i;
			current_->dirty_flag = 1;
			lock_ = 0;
			updateDialog();
		}


		/* NOTE this must come after updateDialog(). */
        setDateToField( NULL );
	}
}

/* Scrollbar has changed (a different routine for each one).
 */
void qReleaseTool::cbSStartScrollerChanged( int pos ) {
	char cBuf[10];
	int i;

	if (!lock_) {
		lock_ = 1;
		sprintf( cBuf, "%d", i = scrSmoltStart_->value() );
		edSmoltStart_->setText( cBuf );
		if (current_ != NULL) {
			current_->smolt_onset = i;
			current_->dirty_flag = 1;
		}

		lock_ = 0;
		updateDialog();

		/* NOTE this must come after updateDialog(). */
        setDateToField( edSmoltStart_ );
	}

	return ; // event handled.
}


void qReleaseTool::cbSStopScrollerChanged( int pos ) {
	char cBuf[10];
	int i;

	if (!lock_) {
		lock_ = TRUE;
		sprintf( cBuf, "%d", i = scrSmoltStop_->value() );
		edSmoltStop_->setText( cBuf );
		if (current_ != NULL) {
			current_->smolt_finish = i;
			current_->dirty_flag = 1;
		}

		lock_ = FALSE;
		updateDialog();

		/* NOTE this must come after updateDialog(). */
        setDateToField( edSmoltStop_ );
	}

	return ; // event handled.
}


void qReleaseTool::cbRStartScrollerChanged( int pos ) {
	char cBuf[10];
	QString cstr;
	int i;

	if (!lock_) {
		lock_ = 1;
		sprintf( cBuf, "%d", i = scrReleaseStart_->value() );
		edReleaseStart_->setText( cBuf );
		if (current_ != NULL) {
			current_->start_date = i;
			current_->dirty_flag = 1;
		}

		lock_ = 0;
		updateDialog();

		/* NOTE this must come after updateDialog(). */
        setDateToField( edReleaseStart_ );
	}

	return ; // event handled.
}



void qReleaseTool::cbLengthScrollerChanged( int pos ) {
	char cBuf[10];
	QString cstr;
	int i;

	if (!lock_) {
		lock_ = 1;
		sprintf( cBuf, "%d", i = scrLength_->value() );
		edLength_->setText( cBuf );
		if (current_ != NULL) {
			current_->length = i;
			current_->dirty_flag = 1;
		}

		lock_ = 0;
		updateDialog();

		/* NOTE this must come after updateDialog(). */
        setDateToField( NULL );
	}

	return ; // event handled.
}


/* Attach/detach with the data token's invalidate event and get notified
 *  when data token has changed. */
void qReleaseTool::AttachEvents( void ) {

	connect(qEvents::Instance(), SIGNAL(GEPurgeAllIDCs_()), this, SLOT(cbPurgeAll()));
	/* Yes, we are attaching to our own event - there is method in it. */
	connect(this, SIGNAL(evInvalidate()), this, SLOT(cbInvalidate()));
	connect(qGraphUpdateMed::instance(), SIGNAL(evGraphUpdate( char *, int)), this, SLOT(cbGraphUpdate(char *, int)));
	connect(qEvents::Instance() , SIGNAL(GERunStatusChanged_()), this, SLOT(cbRunStatusChanged()));

}

void qReleaseTool::DetachEvents( void ) {

}

/* Receive notification that run status has changed.
 */


void qReleaseTool::cbRunStatusChanged( void ){
	if (run_mode.running) {
		bOK_->setEnabled(FALSE);
		bApply_->setEnabled(FALSE);
		bApplyAll_->setEnabled(FALSE);
	}
	else {
		bOK_->setEnabled(TRUE);
		if (current_ != NULL)
            bApply_->setEnabled(TRUE);
        else
            bApply_->setEnabled(FALSE);
		bApplyAll_->setEnabled(TRUE);
	}
}

void qReleaseTool::cbInvalidate(){
	updateDialog();
}


void qReleaseTool::cbPurgeAll(){
	DestroyLocalList();
	CreateLocalList();
	updateDialog();
	emit evInvalidate();
	
}


/* Routine to format the name string for a release.
 */
void qReleaseTool::formatName( struct release *rel, QString *pstr ) {
	int i;
	struct release *r;
	

	*pstr = rel->site->name;
	*pstr += " (";
	*pstr += rel->species->name;
	*pstr += ", ";
	*pstr += rel->stock == &(rel->species->stock) 
							? "Generic" : rel->stock->name;
	*pstr += ")";

	for (i = 0, r = rel->prev; r != NULL; r = r->prev)
		if (r->site == rel->site && r->species == rel->species
											&& r->stock == rel->stock)
			i++;
		else
			break;

	if (i > 0) {
		*pstr += " ";
		*pstr->append( (char)('A' + i++) );
	}
}

/* Routine to fill the dialog fields from the current list element.
 */
void qReleaseTool::updateDialog( void ) {
	int i;
	char cBuf[10];
	struct release *rel;
	QString cs, cstr, cstrCur, sstr;

	if (lock_)
		return;

	lock_ = 1;
    scrSmoltStart_->setValue( i = current_ ? current_->smolt_onset : 70 );
	sprintf( cBuf, "%d", i );
	edSmoltStart_->setText( cBuf );
	scrSmoltStop_->setValue( i = current_ ? current_->smolt_finish : 70 );
	sprintf( cBuf, "%d", i );
	edSmoltStop_->setText( cBuf );
	scrReleaseStart_->setValue( i = current_ ? current_->start_date : 80 );
	sprintf( cBuf, "%d", i );
	edReleaseStart_->setText( cBuf );
	scrLength_->setValue( i = current_ ? (int)current_->length : 0 );
	sprintf( cBuf, "%d", i );
	edLength_->setText( cBuf );

	if (current_ != NULL ) {
		setExactSelection( cmbReleaseSite_, current_->site->name );
		QListBox * lb;
		QListBoxItem * lbi;		
		lb = cmbSpecies_->listBox();
		lbi = lb->findItem( current_->species->name );
		cmbSpecies_->setCurrentItem(lb->index(lbi));
	
		lb = cmbStock_->listBox();
		lbi = lb->findItem( current_->stock->name );
		cmbStock_->setCurrentItem(lb->index(lbi));
		//lb->setSelected (lbi, true);

	}

	/* Put all existing releases in the existing releases combobox.
	 */

	//if(current_ == NULL)
	//	log_msg(0, "current is null");

	cmbExistingReleases_->clear();
	i = cmbReleaseSite_->currentItem();
	cs = cmbReleaseSite_->text( i );
	
	for (rel = localList_; rel != NULL; rel = rel->next) {
		formatName( rel, &cstr );
		cmbExistingReleases_->insertItem( cstr, cmbExistingReleases_->count() );
		if (current_ == rel && current_ != NULL)
			cstrCur = cstr;
	}

	if (cstrCur.length() != 0){

		setExactSelection( cmbExistingReleases_, cstrCur.ascii() );
		//log_msg(0, "set exact cmbRelase Exisitng in update dialog");
		//log_msg(0, (char*)cstrCur.ascii() );
	}
	

	if (current_ != NULL && current_->dirty_flag)
		i = TRUE;
	else
		i = FALSE;
	bpDirty1_->setShown( i );
	bpDirty2_->setShown( i );
	bpDirty3_->setShown( i );
	bpDirty4_->setShown( i );

	/* Some controls are enabled or disabled depending on what we are
	 *  editing.
	 */
	if (current_ == NULL) {
		/* No current edit object.
		 */
		stNew_->setShown( FALSE );
		stExisting_->setShown( FALSE );
		bDelete_->setEnabled(FALSE);
		edSmoltStart_->setEnabled(FALSE);
		edSmoltStop_->setEnabled(FALSE);
		edReleaseStart_->setEnabled(FALSE);
		edLength_->setEnabled(FALSE);
		scrSmoltStart_->setEnabled(FALSE);
		scrSmoltStop_->setEnabled(FALSE);
		scrReleaseStart_->setEnabled(FALSE);
		scrLength_->setEnabled(FALSE);
		bApply_->setEnabled(FALSE);
		bReset_->setEnabled(FALSE);
	}
	else {
		/* We have a current edit object.
		 */
		bDelete_->setEnabled(TRUE);
        if (run_mode.running)
            bApply_->setEnabled(FALSE);
        else
		    bApply_->setEnabled(TRUE);
		edSmoltStart_->setEnabled(TRUE);
		edSmoltStop_->setEnabled(TRUE);
		edReleaseStart_->setEnabled(TRUE);
		edLength_->setEnabled(TRUE);
		scrSmoltStart_->setEnabled(TRUE);
		scrSmoltStop_->setEnabled(TRUE);
		scrReleaseStart_->setEnabled(TRUE);
		scrLength_->setEnabled(TRUE);

		if (current_->temp != NULL) {
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

	/* Put a caption on the window frame.
	 */
	if (current_ != NULL)
		formatName( current_, &cstr );
	else {
		cstr = "Release Tool: ";
		sstr = "Release Schedule: ";
		if ((i = cmbReleaseSite_->currentItem()) >= 0){
			cstr += cmbReleaseSite_->text(i);
			sstr += cmbReleaseSite_->text(i);
		}
		cstr += " (";
		sstr += " (";
		if ((i = cmbSpecies_->currentItem()) >= 0){
			cstr += cmbSpecies_->text(i) + ", ";
			sstr += cmbSpecies_->text(i) + ", ";
		}
		if ((i = cmbStock_->currentItem()) >= 0){
			cstr += cmbStock_->text(i);
			sstr += cmbStock_->text(i);
		}
		cstr += ")";
		sstr += ")";
	}
	setCaption( cstr );
	egdt_->setGraphTitle( sstr );
	gd_->setTitleLabel(egdt_->getGraphTitle().ascii());	
	stDate_->setText( "" );

	/* Tell the graph to get a new list of stuff to show.
	 */
	egdt_->Fill( );
	egdt_->cbInvalidate(  );
	lock_ = 0;
}

/* Routines to manage the local list of release structures.
 */
void qReleaseTool::DestroyLocalList( void ) {
	struct release *rel, *fwd;

	rel = localList_;
	while (rel) {
		fwd = rel->next;
#ifdef REALTIME
		if (rel->rt)
			destroy_rtinfo(&rel->rt);
#endif
	    free(rel);
		rel = fwd;
	}
	localList_ = current_ = NULL;
}

void qReleaseTool::CreateLocalList( void ) {
	struct release *rel, *newrel, *end;

	DestroyLocalList(); // Just to be sure.
	end = NULL;
	for (rel = current->release_list; rel != NULL; rel = rel->next) {
		newrel = construct_release();
		bcopy ( rel, newrel, sizeof(struct release) );
#ifdef REALTIME
		newrel->rt = NULL;
#endif
		copy_release( rel, newrel );
		newrel->next = NULL;
		newrel->temp = (char *)rel;  // point to corresponding list element.
		newrel->dirty_flag = 0;
        newrel->rls_seg = NULL;  // don't need these for editing.
		if (end == NULL) {
			end = localList_ = current_ = newrel;
			newrel->prev = NULL;
		}
		else {
			end->next = newrel;
			newrel->prev = end;
			end = newrel;
		}
	}
}

void qReleaseTool::CreateRlsSegList( struct release *rel ) {
	/* Create a list of rls_seg structures for this release structure.
	 *  Stolen from activate_release() in release.c
	 */
    struct rls_seg *rls_seg;
    struct river_segment *river_seg;

    rls_seg = NULL;
    for (river_seg = rel->site->seg; river_seg; river_seg = river_seg->down) {
		if (rls_seg) {
			if (! (rls_seg->down =
			   (struct rls_seg *)malloc(sizeof(struct rls_seg)))) {
				return;
			}
			bzero((char *)rls_seg->down, sizeof(struct rls_seg));
			rls_seg->down->up = rls_seg;
			rls_seg = rls_seg->down;
		} else {
			if (! (rls_seg =
			   (struct rls_seg *)malloc(sizeof(struct rls_seg)))) {
				return;
			}		
			bzero((char *)rls_seg, sizeof(struct rls_seg));
			rls_seg->up = NULL;
			rel->rls_seg = rls_seg;
		}
		rls_seg->data = NULL;
		rls_seg->down = NULL;
		rls_seg->river_segment = river_seg;
    }
}

void qReleaseTool::ApplyLocalList( struct release *applyMe ) {
	struct release *rel, *fwd, *back, *ex, *n, *p, *hold;
	struct rls_seg *rls;
#ifdef REALTIME
	RtInfo *srt;
#endif	    

	if (applyMe) {
		// Apply single element.
		if (applyMe->temp != NULL) {
			// single element has corresponding element in underlying list.
			rel = (struct release *)(applyMe->temp);
			n = rel->next;
			p = rel->prev;
#ifdef REALTIME
			srt = rel->rt;
#endif	    
			rls = rel->rls_seg;
			bcopy( applyMe, rel, sizeof( struct release ) );
			rel->next = n;
			rel->prev = p;
#ifdef REALTIME
			rel->rt = srt;
#endif	    
			rel->rls_seg = rls;
			rel->temp = NULL;
		}
		else {
			// Single element is new.
			rel = construct_release();
			copy_release( applyMe, rel );
			applyMe->temp = (char *)rel;
			CreateRlsSegList( rel );

			// Search in local list for place to put this element.
			for (back = applyMe->prev; back != NULL; back = back->prev)
				if (back->temp != NULL)
					break;
			if (back != NULL) {
				// put new record after "back->temp" in underlying list
				ex = (struct release *)(back->temp);
				rel->prev = ex;
				rel->next = ex->next;
				ex->next = rel;
				if (rel->next != NULL)
					rel->next->prev = rel;
			}
			else {
				for (fwd = applyMe->next; fwd != NULL; fwd = fwd->next)
					if (fwd->temp != NULL)
						break;
				if (fwd != NULL) {
					// put new record before "fwd->temp" in underlying list.
					ex = (struct release *)(fwd->temp);
					rel->next = ex;
					rel->prev = ex->prev;
					ex->prev = rel;
					if (rel->prev != NULL)
						rel->prev->next = rel;
					else
						current->release_list = rel;
				}
				else {
					// put new record at front of underlying list.
					rel->prev = NULL;
					rel->next = current->release_list;
					current->release_list = rel;
					if (rel->next != NULL)
						rel->next->prev = rel;
				}
			}
		}
		applyMe->dirty_flag = 0;
	}
	else {
		// Apply entire list. Delete underlying list, move local list to
		//  underlying list, create a new local list. Try to remember where
		//  we are.
		while (current->release_list)  {
			delete_release( current->release_list );
		}
		hold = current_;
		current->release_list = localList_;
		for (rel = localList_; rel != NULL; rel = rel->next)
			CreateRlsSegList( rel );
		localList_ = NULL;
		CreateLocalList();
		for (rel = localList_; rel != NULL; rel = rel->next)
			if ((char *)hold == rel->temp) {
				current_ = rel;
				break;
			}
	}
	GOutputsValid_[1] = 0;   /* passage invalid */
	updateDialog();
	theMapPane_->repaint();
	qEvents::Instance()->emitGEUpdateAllDataTokens_();
	//GEUpdateAllDataTokens_.notify();
}


void qReleaseTool::ResetLocalList( struct release *resetMe ) {
	struct release *n, *p;
	char *t;
	if (resetMe != NULL) {
		// Reset single element.
		if (resetMe->temp != NULL) {
			// Single element corresponds to an underlying list element.
			n = resetMe->next;
			p = resetMe->prev;
			t = resetMe->temp;
			bcopy ( resetMe->temp, resetMe, sizeof(struct release) );
			resetMe->next = n;
			resetMe->prev = p;
			resetMe->temp = t;
			resetMe->dirty_flag = 0;
#ifdef REALTIME
			resetMe->rt = NULL;
#endif
		}
		else {
			// Single element is new.
			if (localList_ == resetMe)
				localList_ = resetMe->next;

			if (resetMe->next != NULL) {
				if (current_ == resetMe)
					current_ = resetMe->next;
				resetMe->next->prev = resetMe->prev;
			}
			if (resetMe->prev != NULL) {
				if (current_ == resetMe)
					current_ = resetMe->prev;
				resetMe->prev->next = resetMe->next;
			}
			if (current_ == resetMe)
				current_ = NULL;
			
			free( resetMe );
		}
	}
	else {
		// Reset entire list.
		CreateLocalList();
	}
	updateDialog();
}

/* Handle live updating (map update mediator forwards mouse events to
 *  to interested parties.
 */

void qReleaseTool::cbGraphUpdate( char *sourceName,  int type){

	if (type == RELEASE && bMouseUpdate_->isOn()) {
		/* Find the first existing release by that name and select it.
		 *  If none such exist, select that site anyways 
		 */
		for (current_ = localList_; current_ != NULL; current_ = current_->next){
			if (!strcmp( sourceName, current_->site->name )) {
				updateDialog();
				return;
			}

		}
		setExactSelection( cmbReleaseSite_, sourceName ) ;
		updateDialog();
	}
}
