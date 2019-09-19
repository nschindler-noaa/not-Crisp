/* qScheduleTool.cpp
 * This class defines a modeless dialog which edits qGraphDataToken-s
 *  via a listbox and edit boxes. The dialog is based on the 
 *  IDD_SCHEDULETOOL resource template. The data token is shared with
 *  a qEditGraphMediator, so some extra co-ordination is needed there.
 *
 * This class edits a "qEditGraphDataToken" object. Updating the object
 *  into the underlying data structure is also handled.
 *
 * NOTE: this class only edits the first "line" of data in the proferred
 *         object.
 */

#include "qScheduleTool.h"
#include "qGlobalEvents.h"
#include "resource.h"
#include "run_mode.h"
#include "Qcrisp.h"
#include <stdlib.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qlistbox.h>
#include <qmessagebox.h>
#include <qlineedit.h>
#include "qHelpManager.h"


qScheduleTool::qScheduleTool( QWidget *w, qEditGraphDataToken *edt )
: ScheduleToolBase(w, "Schedule tool" , FALSE, Qt::WDestructiveClose)
{
	QRect r;

	edt_ = edt;

	FillFromToken();
	AttachEvents();

	setCaption("Schedule Tool");

	show();
	cbRunStatusChanged();
	
}

qScheduleTool::~qScheduleTool()
{
	
	/* Notify all and sundry that we died. */
	DetachEvents();
	emit evRemove();
	
}

/* Attach/detach with the data token's invalidate event and get notified
 *  when data token has changed. */
void qScheduleTool::AttachEvents( void ) {
	

	connect(pbOK_, SIGNAL(clicked()), SLOT(cbOK()));
	connect(pbCancel_, SIGNAL(clicked()), SLOT(cbCancel()));
	connect(pbApply_, SIGNAL(clicked()), SLOT(cbApply()));
	connect(pbReset_, SIGNAL(clicked()), SLOT(cbReset()));
	connect(pbResetAll_, SIGNAL(clicked()), SLOT(cbResetAll()));
	connect(pbApplyAll_, SIGNAL(clicked()), SLOT(cbApplyAll()));
	connect(pbHelp_, SIGNAL(clicked()), SLOT(cbHelp()));
	connect(pbLeftArrow_, SIGNAL(clicked()), SLOT(cbLeftArrow()));
	connect(ListBox_, SIGNAL(selectionChanged()), SLOT(cbList1()));
	connect(edt_, SIGNAL(evFilled_()), SLOT(cbFilled()));
	connect(edt_, SIGNAL(evInvalidate_()), SLOT(cbInvalidate()));
	connect(qEvents::Instance(), SIGNAL(GERunStatusChanged_()), SLOT(cbRunStatusChanged()));

}

void qScheduleTool::DetachEvents( void ) {

}

/* Receive notification that run status has changed.

*/

void qScheduleTool::cbRunStatusChanged( void ){

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

void qScheduleTool::cbInvalidate(){
	FillFromToken();
}

void qScheduleTool::cbFilled(){
	FillFromToken();
}

/* Fill dialog fields from data token. */
void qScheduleTool::Format( char *cb, int start, int stop, double value ) {
	sprintf( cb, "%d-%d: %g", start, stop, value );
}

void qScheduleTool::FillFromToken( void ) {
	
	int i,l,start;
	char cBuf[50];
	double value;

	/* Show the dirty indicators at the corners of the window.*/
	 
	if (edt_->isDirty())
		i = TRUE;
	else
		i = FALSE;
	if (bpDirty1_ != NULL) {
		bpDirty1_->setShown( i );
		bpDirty2_->setShown( i );
		bpDirty3_->setShown( i );
		bpDirty4_->setShown( i );
	}

	/* Clear listbox and set up to run down data array.*/ 
	ListBox_->clear();
	l = edt_->getNumElementsPerLine();
	value = edt_->dataArray_[0];
	start = 0;

	/* Run down data array, detecting runs of identical data and
	 *  formatting each run as a line in the listbox.*/
	 
	ListBox_->setShown( FALSE );
	for (i = 1; i < l; i++) {
			//log_msg(0, (char*)QString::number(edt_->dataArray_[i]).ascii());
		if (edt_->dataArray_[i] != value) {
			Format( cBuf, start, i-1, value );
			ListBox_->insertItem( cBuf );
			start = i;
			value = edt_->dataArray_[i];
		}
	}
	Format( cBuf, start, l-1, value );
	ListBox_->insertItem( cBuf );
	ListBox_->setShown(TRUE );

	/* Update title and label text. **/
	TitleText_->setText( edt_->getGraphTitle() );
	LabelText_->setText( edt_->legendArray_[0].ascii() );
	
}


void  qScheduleTool::cbOK(){
	close();
}
	
void  qScheduleTool::cbCancel(){
	edt_->Reset();
	close();
}

void  qScheduleTool::cbApply(){
	edt_->Apply();
}
	
	
void  qScheduleTool::cbReset(){
	edt_->Reset();
}
	
void  qScheduleTool::cbApplyAll(){
	edt_->ApplyAll();
}
	
void  qScheduleTool::cbResetAll(){
	edt_->ResetAll();
}

void  qScheduleTool::cbHelp(){
	helpManager->showHelp( QString("95.html") );
}

void  qScheduleTool::cbLeftArrow(){
		/*
		 * Left arrow button: Get values from Start, Stop, and Value
		 *  edit fields. Apply new data and range of values directly
		 *  to data token and mark it dirty.
		 */
		//log_msg(0,"Left arrow pressed");
		QString str;
		int newStart, newStop;
		int i,j;
		int day;
		double newValue;
		str = StartDay_->text();
		newStart = atoi( str.ascii() );
		str = StopDay_->text();
		newStop = atoi( str.ascii() );
		str = Value_->text();
		newValue = atof( str.ascii() );
		/* TODO better error detection & handling. */
		if (newValue < 0.0 && edt_->getGraphDataRange() == GR_POSITIVE)
			//delete new zMessage( this, "Value must be non-negative",
			//	"Schedule Tool" );
			QMessageBox::warning(this, "Value must be non-negative","Schedule Tool",
					QMessageBox::NoButton,QMessageBox::Ok);
		else if (newValue > 0.0 && edt_->getGraphDataRange() == GR_NEGATIVE)
			//delete new zMessage( this, "Value must be non-positive",
			//	"Schedule Tool" );
			QMessageBox::warning(this, "Value must be non-positive","Schedule Tool",
				QMessageBox::NoButton,QMessageBox::Ok);
		else if (newStart < 0)
			//delete new zMessage( this, "Start must be non-negative",
			//	"Schedule Tool" );
			QMessageBox::warning(this, "Start must be non-negative","Schedule Tool",
					QMessageBox::NoButton,QMessageBox::Ok);
		else if (newStop < 0)
			//delete new zMessage( this, "Stop must be non-negative",
			//	"Schedule Tool" );
			QMessageBox::warning(this, "Stop must be non-negative","Schedule Tool",
				QMessageBox::NoButton,QMessageBox::Ok);
		else if (newStart >= edt_->getNumElementsPerLine())
			//delete new zMessage( this, "Start is beyond end of graph",
			//	"Schedule Tool" );
			QMessageBox::warning(this, "Start is beyond end of graph","Schedule Tool",
				QMessageBox::NoButton,QMessageBox::Ok);
		else if (newStop >= edt_->getNumElementsPerLine())
			//delete new zMessage( this, "Stop is beyond end of graph",
			//	"Schedule Tool" );
			QMessageBox::warning(this,"Stop is beyond end of graph","Schedule Tool",
				QMessageBox::NoButton,QMessageBox::Ok);
		else if (newStart > newStop)
			//delete new zMessage( this, "Start cannot be after Stop",
			//	"Schedule Tool" );
			QMessageBox::warning(this, "Start cannot be after Stop","Schedule Tool",
				QMessageBox::NoButton,QMessageBox::Ok);
		else {
			/* Values from edit fields are superficially plausible.
			 *  Update the data token. Try to preserve scrolling of
			 *  listbox.
			 */
			for (i = newStart; i <= newStop; i++){
				//log_msg(0, (char*)QString::number(edt_->dataArray_[i]).ascii());

				edt_->dataArray_[i] = newValue;

			}
			edt_->SetData();
			for (i = 1; (unsigned)i < ListBox_->count(); i++) {
				/* Get first date in this list box item. 
				 */
				str = ListBox_->text(  i );
				j = str.find( '-' );
				str = str.left( j );
				day = atoi( str.ascii() );
				if (day >= newStart) {
					/* Try to scroll that item to the middle of the box,
					 *   or so.
					 */
					j = i + 8;
					if ((unsigned)j >= ListBox_->count())
						j = ListBox_->count() - 1;

					ListBox_->setSelected ( j , TRUE);
					ListBox_->setSelected (i, TRUE );
					
					break;
				}
			}
		}

		return ;

}


void qScheduleTool::cbList1(){
	QString str;
	int i , j;
	/* We respond to listbox selection events here. */
	if (str = ListBox_->currentText()) {
		i = str.find( '-' );
		StartDay_->setText( str.mid( 0, i ) );
		j = str.find( ':' );
		StopDay_->setText( str.mid( i+1, j-(i+1) ) );
		i = str.find( ' ' );
		Value_->setText( str.mid( i+1, str.length()-(i+1) ) );
	}
	return ;

}
	
