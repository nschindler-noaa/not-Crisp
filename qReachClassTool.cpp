/* qReachClassTool.cpp
 * This class defines a modal dialog which edits the number and names
 *  of reach classes. It is based on the IDD_REACHCLASSNAMES 
 *  resource template.
 *
 * This class edits the global variables "num_reach_classes" and
 *  "reach_class_names[10]".
 *
 */
#include "qReachClassTool.h"
#include "qGlobalEvents.h"
#include "Qcrisp.h"
#include "resource.h"
#include "crisp.h"
#include "run_mode.h"
#include "init.h"
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qobject.h>
#include <stdlib.h>
#include <qvalidator.h>
#include "qHelpManager.h"


extern qReachClassTool *theReachClassTool_;   // defined in c1main.cpp

qReachClassTool::qReachClassTool( QWidget *w )
	: ReachClassNameBase ( w )
{

	/* Get the child window controls hooked up.
	 */
    radioButton_[0] = radio1;
    radioButton_[1] = radio2;
    radioButton_[2] = radio3;
    radioButton_[3] = radio4;
    radioButton_[4] = radio5;
    radioButton_[5] = radio6;
    radioButton_[6] = radio7;
    radioButton_[7] = radio8;
    radioButton_[8] = radio9;
    radioButton_[9] = radio10;

	bGroup = new QButtonGroup();
	bGroup->setExclusive(true);
	bGroup->insert(radio1, 1);
	bGroup->insert(radio2, 2);
	bGroup->insert(radio3, 3);
	bGroup->insert(radio4, 4);
	bGroup->insert(radio5, 5);
	bGroup->insert(radio6, 6);
	bGroup->insert(radio7, 7);
	bGroup->insert(radio8, 8);
	bGroup->insert(radio9, 9);
	bGroup->insert(radio10, 10);

	connect( bGroup, SIGNAL(clicked(int)), this, SLOT(radioClicked(int)));

	//Connect main control buttons
	connect(OKButton, SIGNAL(clicked()), this, SLOT(cbOK()));
	connect(ResetButton, SIGNAL(clicked()), this, SLOT(cbReset()));
	connect(CancelButton, SIGNAL(clicked()), this, SLOT(cbCancel()));
	connect(HelpButton, SIGNAL(clicked()), this, SLOT(cbHelp()));
	connect(ApplyButton, SIGNAL(clicked()), this, SLOT(cbApply()));

	editLine_[0] = line1;
	editLine_[1] = line2;
	editLine_[2] = line3;
	editLine_[3] = line4;
	editLine_[4] = line5;
	editLine_[5] = line6;
	editLine_[6] = line7;
	editLine_[7] = line8;
	editLine_[8] = line9;
	editLine_[9] = line10;

	for (int i=0; i < 10; i++) {
		editLine_[i]->setValidator(new QRegExpValidator(QString("^[a-zA-Z0-9_]*$"), editLine_[i]));
	}

	connect(qEvents::Instance(), SIGNAL(GERunStatusChanged_() ), this , SLOT(cbRunStatusChanged()));

    cbRunStatusChanged();

    /* Initial state of the flags we are editing. 
	 */
    getSettings();
	
	show();
}

qReachClassTool::~qReachClassTool()
{
	//GERunStatusChanged_.detach( this, &qReachClassTool::_cbRunStatusChanged );
    theReachClassTool_ = NULL;
	delete bGroup; //may need to do this, since its not a child of this dialog?
}

/* Receive notification that run status has changed.
 */
void qReachClassTool::cbRunStatusChanged( void ){
	if (run_mode.running) {
		OKButton->setEnabled(false);
		ApplyButton->setEnabled(false);
	}
	else {
		OKButton->setEnabled(true);
		ApplyButton->setEnabled(true);
	}
}

/* These routines move settings between the dialog and the underlying
 *  data structures.
 */
void qReachClassTool::getSettings( void ) {
	/* Initial state of the flags we are editing. 
	 */
    int i;
    for (i = 0; i < 10; i++) {
        radioButton_[i]->setChecked( i+1 == num_reach_classes );
        if (i < num_reach_classes) {
            editLine_[i]->setEnabled(true);
            editLine_[i]->setText( reach_class_names[i] );
        }
        else {
            editLine_[i]->setText( "" );
            editLine_[i]->setEnabled(false);
        }
    }

    showDirty( 0 );
}

void qReachClassTool::applySettings( void ) {
    int i;
	int nrc = 0;

    /* Apply UI state to underlying data structures.
     */
    for (i = 0; i < 10; i++)
        if (radioButton_[i]->isOn()) {
            nrc = i+1;
            break;
        }

    set_num_reach_classes( nrc );

    for (i = 0; i < num_reach_classes; i++) {
        free( reach_class_names[i] );
        reach_class_names[i]
            = (char *)malloc( editLine_[i]->text().length() + 1 );
        strcpy( reach_class_names[i], editLine_[i]->text().ascii() );
    }


	GOutputsValid_[1] = 0;   /* passage invalid. */
	//GEUpdateAllDataTokens_.notify();
	qEvents::Instance()->emitGEUpdateAllDataTokens_();
	qEvents::Instance()->emitGEPurgeAllIDCs_();
    //GEPurgeAllIDCs_.notify();

    showDirty( 0 );
}

/* This routine checks for a proper mortality class definition.
 */
int qReachClassTool::checkClassNames( void ) {
    int i;
	int nrc = 0;

    for (i = 0; i < 10; i++)
        if (radioButton_[i]->isOn()) {
            nrc = i+1;
            break;
        }

    for (i = 0; i < nrc; i++) {
         if (editLine_[i]->text().length() == 0) {
			 QMessageBox::information( this, "COMPASS Reach Class Names",
                    "You must give a name for all active reach classes");
            return 0;
         }
    }

    for (i = 0; i < nrc; i++) {
         if (editLine_[i]->text().contains( " " ) ||
             editLine_[i]->text().contains( "\t" )) {
            	 QMessageBox::information( this, "COMPASS Reach Class Names",
                 "Reach class names may not contain whitespace");                  
                 return 0;
         }
    }

    return 1;
}

void qReachClassTool::showDirty( int i ) {
    int iShow;
	if (bpDirty1_ != NULL) {
        iShow = i ? true : false;
		bpDirty1_->setShown( iShow );
		bpDirty2_->setShown( iShow );
		bpDirty3_->setShown( iShow );
		bpDirty4_->setShown( iShow );
	}
}

void qReachClassTool::radioClicked(int id){

	int i;
	for (i = 0; i < 10; i++) {
		if(i <= id-1)
			editLine_[i]->setEnabled(true);
		else
            editLine_[i]->setEnabled(false);		
     }
	showDirty( 1 );
}


void qReachClassTool::cbOK(){
	applySettings();
    close();
}

void qReachClassTool::cbReset(){
	getSettings();
}

void qReachClassTool::cbCancel(){
	close();
}

void qReachClassTool::cbHelp(){
	
	helpManager->showHelp(QString("407.html") );	

}

void qReachClassTool::cbApply(){
	if (checkClassNames())
          applySettings();
}

