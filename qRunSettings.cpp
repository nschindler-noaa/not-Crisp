/* qRunSettings.cpp
 * This class defines a modal dialog which edits the runtime settings.
 *  The dialog is based on the IDD_RUNSETTINGS resource template.
 *
 * This class edits the global variable "struct run *current".
 *
 */

#include "qRunSettings.h"
#include "qGlobalEvents.h"
#include "Qcrisp.h"
#include "resource.h"
#include "crisp.h"
#include "run_mode.h"
#include <qmessagebox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpixmap.h>
#include "qHelpManager.h"

static const char* const image0_data[] = { 
"10 10 2 1",
". c #c0c0c0",
"# c #f40000",
"....##....",
"..######..",
".########.",
".########.",
"##########",
"##########",
".########.",
".########.",
"..######..",
"....##...."};


extern qRunSettings *theRunSettings_;   // defined in c1main.cpp

qRunSettings::qRunSettings( QWidget *w )
	: QMainWindow ( w, "Run Settings Window", WType_TopLevel | WDestructiveClose  ),
	dirtyPM( (const char**) image0_data )
{
	//Main Vertical Layout :
	QVBoxLayout *VertLayout = new QVBoxLayout(this,  10,-1, "Main vertical layout");
	
	setCaption("Runtime Settings");

	/* "Dirty dot" edit state indicators.
     */
	bpDirty1_ = new QLabel(this, "Dirty Dot 1");	
	bpDirty1_->setPixmap(dirtyPM);
	bpDirty1_->setFixedSize(8,8); 
	bpDirty1_->setScaledContents(TRUE);
	bpDirty1_->show();

	bpDirty2_ = new QLabel(this, "Dirty Dot 2");	
	bpDirty2_->setPixmap(dirtyPM);
	bpDirty2_->setFixedSize(8,8); 
	bpDirty2_->setScaledContents(TRUE);
	bpDirty2_->show();
	
	bpDirty3_ = new QLabel(this, "Dirty Dot 3");	
	bpDirty3_->setPixmap(dirtyPM);
	bpDirty3_->setFixedSize(8,8); 
	bpDirty3_->setScaledContents(TRUE);
	bpDirty3_->show();
	
	bpDirty4_ = new QLabel(this, "Dirty Dot 4");	
	bpDirty4_->setPixmap(dirtyPM);
	bpDirty4_->setFixedSize(8,8); 
	bpDirty4_->setScaledContents(TRUE);
	bpDirty4_->show();

    //Insert the top dirty Indicators in the layout
	QHBoxLayout* dirtyLayout = new QHBoxLayout(VertLayout, 5, "dirty Layout Manager");
	dirtyLayout->addWidget(bpDirty1_,Qt::AlignLeft);
	dirtyLayout->addStretch(1);
	dirtyLayout->addWidget(bpDirty2_,Qt::AlignRight);
	dirtyLayout->addStrut (8);
	//VertLayout->addLayout(dirtyLayout);

	QHBoxLayout * upperLayout = new QHBoxLayout(VertLayout, 10 , "upperLayout"  );
	//VertLayout->addLayout(upperLayout);

	/* Get the child window controls hooked up.
	 */
	QVBoxLayout * upperLeftLayout = new QVBoxLayout(upperLayout, 5, "upperLeftLayout");
	VarSup_ = new QCheckBox( "variance suppression", this, "ID_VARSUP" );
	upperLeftLayout->addWidget(VarSup_);
	PredVol_ = new QCheckBox( "predator density / volume interaction", this, "ID_PREDVOL" );
	upperLeftLayout->addWidget(PredVol_);
	AgeDep_ = new QCheckBox( "age dependent fge", this, "ID_AGEDEP" );
	upperLeftLayout->addWidget(AgeDep_);

	//upperLayout->addLayout(upperLeftLayout);

	buttonGroup = new QButtonGroup(1, Qt::Horizontal, "Mortality Model", this );
	buttonGroup->setExclusive ( true );
	upperLeftLayout->addWidget(buttonGroup);
	
    PredGasMort_ = new QRadioButton( "Gas and Pred Mortality", buttonGroup, "IDC_RADIOBUTTON1" ); 
    SimpleMort_ = new QRadioButton( "Simple Mortality", buttonGroup, "IDC_RADIOBUTTON3" ); 

	QVBoxLayout * upperRightLayout = new QVBoxLayout(upperLayout, -1, "upperRightLayout");

	pbHelp_ = new QPushButton( "Help", this, "IDHELP" );
	upperRightLayout->addWidget(pbHelp_ );		
	pbOK_ = new QPushButton( "OK", this, "IDOK" );
	upperRightLayout->addWidget(pbOK_ );
	pbCancel = new QPushButton( "Cancel", this, "IDC_CANCEL" );
	upperRightLayout->addWidget(pbCancel );
	pbApply_ = new QPushButton( "Apply", this, "IDC_APPLY" );
	upperRightLayout->addWidget(pbApply_ );
	pbReset = new QPushButton( "Reset", this, "IDC_RESET" );
	upperRightLayout->addWidget(pbReset );

	//upperLayout->addLayout(upperRightLayout);

	//GERunStatusChanged_.attach( this, &ZasRunSettings::_cbRunStatusChanged );
	if( !connect( qEvents::Instance(), SIGNAL(GERunStatusChanged_()),this,SLOT(cbRunStatusChanged()))){
		log_msg(0, "Couldn't connect qRunSettings to the c1event manager");
	}

	connect(VarSup_,SIGNAL(clicked ()), this, SLOT(checkBoxChecked()));
	connect(PredVol_, SIGNAL(clicked ()), this, SLOT(checkBoxChecked()));
	connect(AgeDep_, SIGNAL(clicked ()), this, SLOT(checkBoxChecked()));
	connect(pbReset, SIGNAL(clicked ()), this, SLOT(resetPressed()));
	connect(pbCancel, SIGNAL(clicked ()), this, SLOT(cancelPressed()));
	connect(PredGasMort_, SIGNAL(clicked ()), this, SLOT(PredGasMortChecked()));
	connect(SimpleMort_, SIGNAL(clicked ()), this, SLOT(SimpleMortChecked()));
	connect(pbOK_, SIGNAL(clicked ()), this, SLOT(okPressed()));
	connect(pbApply_, SIGNAL(clicked ()), this, SLOT(applyPressed()));
	connect(pbHelp_, SIGNAL(clicked()), this, SLOT(helpPressed()));

	//Insert the bottom dirty Indicators in the layout
	QHBoxLayout* dirtyLayout2 = new QHBoxLayout(VertLayout, 5, "dirty Layout Manager 2");
	dirtyLayout2->addWidget(bpDirty3_,Qt::AlignLeft);
	dirtyLayout2->addStretch(1);
	dirtyLayout2->addWidget(bpDirty4_,Qt::AlignRight);
	dirtyLayout2->addStrut (8);
	//VertLayout->addLayout(dirtyLayout2);


    cbRunStatusChanged();

    /* Initial state of the flags we are editing. 
	 */
    getSettings();

	if(parent()->isWidgetType() && ((QWidget*)parent())->icon())
		setIcon(*((QWidget*)parent())->icon() );
	show();
}

qRunSettings::~qRunSettings()
{
    theRunSettings_ = NULL;
}


void qRunSettings::cbRunStatusChanged( void ){
	if (run_mode.running) {
		pbOK_->setEnabled(false);
		pbApply_->setEnabled(false);
	}
	else {
		pbOK_->setEnabled(true);
		pbApply_->setEnabled(true);
	}
}

/* These routines move settings between the dialog and the underlying
 *  data structures.
 */
void qRunSettings::getSettings( void ) {
	/* Initial state of the flags we are editing. 
	 */
	VarSup_->setChecked ( (current->flags & VAR_SUPPRESS_FLAG) != 0 );
	PredVol_->setChecked ( (current->flags & PRED_VOL_FLAG) != 0 );
	AgeDep_->setChecked ( (current->flags & AGE_DEP_FGE_FLAG) != 0 );
    PredGasMort_->setChecked ( current->mortality_class == 0 );
    SimpleMort_->setChecked ( current->mortality_class == 1 );
    showDirty( 0 );
}

void qRunSettings::applySettings( void ) {
    /* Apply UI state to underlying data structures.
     */
    if (PredGasMort_->isOn ())
        current->mortality_class = 0;
    else if (SimpleMort_->isOn () )
        current->mortality_class = 1;

	current->flags &= ~ALL_RUN_FLAGS;
	if (VarSup_->isOn () )
		current->flags |= VAR_SUPPRESS_FLAG;
	if (PredVol_->isOn () )
		current->flags |= PRED_VOL_FLAG;
	if (AgeDep_->isOn () )
		current->flags |= AGE_DEP_FGE_FLAG;

	GOutputsValid_[1] = 0;   /* passage invalid. */
	//fire off the updatre all tokens event
	qEvents::Instance()->emitGEUpdateAllDataTokens_();

    showDirty( 0 );
}

/* This routine checks for a proper mortality class definition.
 */
int qRunSettings::checkMortClass( void ) {
     if (!(PredGasMort_->isOn () || SimpleMort_->isOn ())) {
		 QMessageBox::information( this,  "COMPASS Runtime Settings",
				 "You must select a mortality class." ) ;
        return 0;
     }
     return 1;
}

void qRunSettings::showDirty( int i ) {
    int iShow;
	if (bpDirty1_ != NULL) {
        iShow = i ? true : false;
		bpDirty1_->setShown( iShow );
		bpDirty2_->setShown( iShow );
		bpDirty3_->setShown( iShow );
		bpDirty4_->setShown( iShow );
	}
}


void qRunSettings::checkBoxChecked(){
	showDirty( 1 );
	return ;
}


void qRunSettings::PredGasMortChecked(){
      showDirty( 1 );
  
}

void qRunSettings::SimpleMortChecked(){
        showDirty( 1 );
 }

void qRunSettings::okPressed(){
	if (checkMortClass()) {
            applySettings();
            close();
        }
}

void qRunSettings::applyPressed(){
	if (checkMortClass())
         applySettings();
}


void qRunSettings::resetPressed(){
	getSettings();
}
void qRunSettings::cancelPressed(){
	close();
}

void qRunSettings::helpPressed(){
	helpManager->showHelp( QString("76.html") );
}
