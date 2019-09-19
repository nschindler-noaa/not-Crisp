
#include "resource.h"
#include "qGlobalEvents.h"
#include "run_mode.h"
#include "qScheduleDialogBase.h"
#include <qcombobox.h>
#include <qlistbox.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qpushbutton.h>

qScheduleDialogBase::qScheduleDialogBase(QWidget *w) 
: ScheduleBase(w, "schedule dialog base", FALSE, (Qt::WidgetFlags)WDestructiveClose) {
	consCommon();
}

void qScheduleDialogBase::consCommon( void ) {
	

	wasCloseNotified_ = 0;

	pbLeftArrow = pushButton1;

	cmbStatic_[0] = cmbStatic_0;
	cmbStatic_[1] = cmbStatic_1;
	cmbStatic_[1]->setShown( FALSE );  /* hidden by default */

	listBox_[0] = listBox_0;
	listBox_[1] = listBox_1;
	listBox_[0]->setSelectionMode ( (QListBox::SelectionMode )QListBox::Single);
	listBox_[1]->setSelectionMode ( (QListBox::SelectionMode ) QListBox::Single);
	listBox_[0]->setShown(TRUE);
	listBox_[1]->setShown(FALSE);
	

	listLabel_[0] = listLabel_0;
	listLabel_[1] = listLabel_1;
	listLabel_[0]->setShown(TRUE);
	listLabel_[1]->setShown(FALSE);

	editLine_[0] = editLine_0;
	editLine_[1] = editLine_1;
	editLine_[2] = editLine_2;
	editLine_[3] = editLine_3;

	editLine_[0]->setShown(TRUE);
	editLine_[1]->setShown(TRUE);
	editLine_[2]->setShown(TRUE);
	editLine_[3]->setShown(FALSE);

	editLabel_[0] = editLabel_0;
	editLabel_[1] = editLabel_1;
	editLabel_[2] = editLabel_2;
	editLabel_[3] = editLabel_3;
	
	editLabel_[0]->setShown(TRUE);
	editLabel_[1]->setShown(TRUE);
	editLabel_[2]->setShown(TRUE);
	editLabel_[3]->setShown(FALSE);
	
	checkBox1_->setShown(FALSE);

	/* Set up combobox notification events.
	 */

	connect(pbCancel_, SIGNAL(clicked()), SLOT(cbCancel()));
	connect(pbOK_, SIGNAL(clicked()), SLOT(cbOK()));
	connect(cmbStatic_[0], SIGNAL(activated(int)), SIGNAL(evCmbStatic1_(int)));
	connect(cmbStatic_[1], SIGNAL(activated(int)), SIGNAL(evCmbStatic2_(int)));
	connect(pbApply_, SIGNAL(clicked()), SIGNAL(evApply_()));
	connect(pbApplyAll_, SIGNAL(clicked()), SIGNAL(evApplyAll_()));
	connect(pbReset_, SIGNAL(clicked()), SIGNAL(evReset_()));
	connect(pbResetAll_, SIGNAL(clicked()), SIGNAL(evResetAll_()));
	connect(pbHelp_, SIGNAL(clicked()), SIGNAL(evHelp_()));
	connect(listBox_[0], SIGNAL(selected(int)), SIGNAL(evList1_(int)));
	connect(listBox_[1], SIGNAL(selected(int)), SIGNAL(evList2_(int)));
	connect(pbLeftArrow, SIGNAL(clicked()), SIGNAL(evLeftarrow_()));
	connect(qEvents::Instance(), SIGNAL(GERunStatusChanged_()), SLOT(cbRunStatusChanged()));


	if(parent()->isWidgetType() && ((QWidget*)parent())->icon())
		setIcon(*((QWidget*)parent())->icon() );
	show();
	cbRunStatusChanged();
}

qScheduleDialogBase::~qScheduleDialogBase(){ 
	/* Clean up combobox notification events.
	 */

	/* Make sure that anyone interested in hearing of our death, does
	 *  hear of it. */
	if (!wasCloseNotified_)
		emit evCancel_();
	
}

/* Receive notification that run status has changed.
 */
/*
void qScheduleDialogBase::_cbRunStatusChanged(void *msg){
    qScheduleDialogBase *m = (qScheduleDialogBase *)msg; 
    m->cbRunStatusChanged();
}
*/

void qScheduleDialogBase::cbRunStatusChanged( void ){
	if (run_mode.running) {
		pbOK_->setEnabled(FALSE);
		pbApply_->setEnabled(FALSE);
		pbApplyAll_->setEnabled(FALSE);
	}
	else {
		pbOK_->setEnabled(TRUE);
		pbApply_->setEnabled(TRUE);
		pbApplyAll_->setEnabled(TRUE);
	}
}

void qScheduleDialogBase::showDirty( bool iShow ) {
	/* Show the dirty indicators at the corners of the window.
	 */
	if (bpDirty1_ != NULL) {
		bpDirty1_->setShown( iShow );
		bpDirty2_->setShown( iShow );
		bpDirty3_->setShown( iShow );
		bpDirty4_->setShown( iShow );
	}
}


void qScheduleDialogBase::closeEvent ( QCloseEvent * e ){
	/* Underlying window system has killed our window - handle like
	 *  a user "Cancel" action.
	 */
	if (!wasCloseNotified_) {
		emit evCancel_();
		wasCloseNotified_ = 1;
	}
	QDialog::closeEvent(e); //send to parent
	return ;  /* Allow kill. */
}



void qScheduleDialogBase::selectComboBox( int iSel, unsigned box ) {
	cmbStatic_[box]->setCurrentItem( iSel );
}
void qScheduleDialogBase::selectComboBox( const QString& str, unsigned box ) {
	cmbStatic_[box]->setCurrentText( str );
}

void qScheduleDialogBase::addComboBoxText(const char *text, unsigned box){
	cmbStatic_[box]->insertItem(text);
}

QString qScheduleDialogBase::getComboBoxText(unsigned box){ 
	return cmbStatic_[box]->currentText();
}

void qScheduleDialogBase::showComboBox( bool iShow, unsigned box ) {
	cmbStatic_[box]->setShown( iShow );
}

void qScheduleDialogBase::addListBoxText( const char *text, unsigned box ) {
	listBox_[box]->insertItem( text );
}

QString qScheduleDialogBase::getListBoxText( unsigned box ) {
	return listBox_[box]->currentText(  );
}

void qScheduleDialogBase::selectListBox( int iSel, unsigned box ) {
	listBox_[box]->setCurrentItem( iSel );
}

void qScheduleDialogBase::clearListBox( unsigned box ) {
	listBox_[box]->clear( );
}

void qScheduleDialogBase::setListBoxLabel( const char *text, unsigned box ) {
	listLabel_[box]->setText( text );
}

void qScheduleDialogBase::showListBox( bool iShow, unsigned box ) {
	listBox_[box]->setShown( iShow );
	listLabel_[box]->setShown( iShow );
}

void qScheduleDialogBase::setEditBoxText( const char *text, unsigned box ) {
	editLine_[box]->setText( text );
}

QString qScheduleDialogBase::getEditBoxText( unsigned box ) {
	return editLine_[box]->text( );
}

void qScheduleDialogBase::setEditBoxLabel( const char *text, unsigned box ) {
	editLabel_[box]->setText( text );
}

void qScheduleDialogBase::showEditBox( bool iShow, unsigned box ) {
	editLine_[box]->setShown( iShow );
	editLabel_[box]->setShown( iShow );
}

int qScheduleDialogBase::isBoxChecked( void ) {
	return checkBox1_->isChecked();
}

void qScheduleDialogBase::setCheckBox( bool checked ) {
	checkBox1_->setChecked( checked );
}

void qScheduleDialogBase::setCheckBoxText( const char *text ) {
	checkBox1_->setText( text );
}

void qScheduleDialogBase::showCheckBox( bool iShow ) {
	checkBox1_->setShown( iShow );
}

void qScheduleDialogBase::showTitleText( bool iShow ) {
	titleText_->setShown( iShow );
}

void qScheduleDialogBase::setTitleText( const char *text ) {
	titleText_->setText( text );
}


void qScheduleDialogBase::cbCancel(){
	emit evCancel_();
	wasCloseNotified_ = 1; /* Prevent multi-notification. */
	close();

}

void qScheduleDialogBase::cbOK(){
	emit evOK_();
	wasCloseNotified_ = 1;   /* Prevent multi-notification. */
	close();

}

