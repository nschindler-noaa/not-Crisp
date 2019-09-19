#include "qDefines.h"
#include "qGlobalEvents.h"
#include "resource.h"
#include "run_mode.h"
#include "qRowColumnBox.h"
#include "qEditSliderLong.h"
#include "qEditSliderDouble.h"
#include "qZabelSlider.h"
#include "qDialogBase.h"
#include <qpixmap.h>
#include <qmessagebox.h>
#include "log.h"

//pixmap for dirty image
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



qDialogBase::qDialogBase(QWidget *w, WFlags  dw, const char *nt)
                    : QMainWindow(w,nt,  dw), dirtyPix((const char **) image0_data) {
	consCommon();
}

void qDialogBase::consCommon( void ) {
	
	cmbStatic_[0]=NULL; 
	cmbStatic_[1]=NULL;
	cmbStatic_[2] = NULL;
	cbgDelta_ = NULL;
	bpDirty1_ = NULL;
	wasCloseNotified_ = 0;
	rc_ = NULL;
	outerVertLayout = new QVBoxLayout ( this, 5, -1, "Outer Vertical Layout" );
	vertLayout = new QVBoxLayout (outerVertLayout , 0, "Main Vertical Layout" );
	
	outerVertLayout->setStretchFactor(vertLayout, 1);

	horizLayout[0] = new QHBoxLayout (vertLayout, 5, "Top Horizontal Layout" );	
	
	 
	if(parent()->isWidgetType())
		setIcon( *((QWidget*)parent())->icon()    );
		
	connect( qEvents::Instance() ,SIGNAL(GERunStatusChanged_()), this , SLOT(cbRunStatusChanged()));    
}

qDialogBase::~qDialogBase(){ 

	/* Make sure that anyone interested in hearing of our death, does
	 *  hear of it. */
	if (!wasCloseNotified_)
		emit evCancel_(1);
	
	/* Sizers not attached to windows must be deleted explicitly. */

    if (rc_ != NULL)
        delete rc_;	
}

/* Receive notification that run status has changed.
 */

void qDialogBase::cbRunStatusChanged( void ){
	if (run_mode.running) {
		pbOK_->setEnabled(false);
		pbApply_->setEnabled(false);
		pbApplyAll_->setEnabled(false);
	}
	else {
		pbOK_->setEnabled(true);
		pbApply_->setEnabled(true);
		pbApplyAll_->setEnabled(true);
	}
}

/* These routines show/hide the Delta checkbox and the dirty indicator */
void qDialogBase::showDelta( bool iShow ) {
	if (cbgDelta_ != NULL)
		cbgDelta_->setShown(  iShow );
}

void qDialogBase::showDirty( bool iShow ) {
	if (bpDirty1_ != NULL) {
		bpDirty1_->setShown( iShow );
		bpDirty2_->setShown( iShow );
		bpDirty3_->setShown( iShow );
		bpDirty4_->setShown( iShow );
	}
}


void qDialogBase::closeEvent ( QCloseEvent * e ){
	/* Underlying window system has killed our window - handle like
	 *  a user "Cancel" action.
	 */
	if (!wasCloseNotified_) {
		emit evCancel_(1);
		wasCloseNotified_ = 1;
	}
	
	QMainWindow::closeEvent(e );  /* Allow kill. */

		
}

void qDialogBase::addComboBoxText(const char *text, unsigned box){
	cmbStatic_[box]->insertItem(text);
}

void qDialogBase::addComboBox(unsigned box){
	switch (box) {
	case 0:
		cmbStatic_[0] = new QComboBox(this, "IDC_COMBOBOX1");
		horizLayout[0]->addWidget(cmbStatic_[0], Qt::AlignLeft);
		horizLayout[0]->setStretchFactor ( cmbStatic_[0], 0 );
		cmbStatic_[0]->setFocusPolicy (  QWidget::StrongFocus );
		cmbStatic_[0]->show();
		//make the signal connection
		connect (cmbStatic_[0], SIGNAL(activated(int)),this, SLOT( cbCmdStatic1()));
		break;
	case 1:
		cmbStatic_[1] = new QComboBox(this, "IDC_COMBOBOX2"); //= new zComboBoxStatic(this, new zPercentSizer(ZGRAV_RIGHT,100,45,cbgContainer_),WS_VSCROLL | WS_CHILD | WS_TABSTOP | CBS_DROPDOWN,NULL,IDC_COMBOBOX2);
		horizLayout[0]->addWidget(cmbStatic_[1], Qt::AlignLeft);
		horizLayout[0]->setStretchFactor ( cmbStatic_[1], 0 );
		horizLayout[0]->addStretch(0); //add a strechable space at the end of the layout
		cmbStatic_[1]->setFocusPolicy (  QWidget::StrongFocus );
		cmbStatic_[1]->show();
		connect (cmbStatic_[1], SIGNAL(activated(int)),this, SLOT( cbCmdStatic2()));
		break;
	case 2:
		cmbStatic_[2] = new QComboBox(this, "IDC_COMBOBOX3");
		//cmbStatic_[2]->setFixedSize(QSize( 120,23 ));
		horizLayout[0]->addWidget(cmbStatic_[2], Qt::AlignLeft);
		cmbStatic_[2]->setFocusPolicy (  QWidget::StrongFocus );
		cmbStatic_[2]->show();
		connect (cmbStatic_[2], SIGNAL(activated(int)),this, SLOT( cbCmdStatic3()));
		break;
	}
}

void qDialogBase::showComboBox( bool iShow, unsigned box ) {
	if (cmbStatic_[box] != NULL)
		cmbStatic_[box]->setShown( iShow );
}
	
int qDialogBase::selectComboBox(int i, unsigned box){ 
	if((i!=-1) && (!cmbStatic_[box]->count() || (i >= cmbStatic_[box]->count()))) 
		return 0;
	if(i!=-1)
		cmbStatic_[box]->setCurrentItem(i);
	return cmbStatic_[box]->currentItem();
}

QString qDialogBase::getComboBoxText(int box){ 
	return cmbStatic_[box]->currentText();
}

int qDialogBase::buildRowColumnControl(){
		
	rc_ = new qRowColumnBox(this);
	
	return 1;

}
	
int qDialogBase::drawDialog(unsigned topMargin){
	
	if(topMargin)topMargin_ = topMargin;
	
	QRect parentRect, parentIntRect;
	parentRect = frameGeometry(); 
	parentIntRect = geometry () ;

	/* First, create the bitmaps which will indicate data dirtyness.
	 *  We happen to know that the bitmaps are 10x10.
	 */
		
	
	QHBoxLayout * horLay = new QHBoxLayout(outerVertLayout, 0,  "Top dirty layout");
	outerVertLayout->removeItem(horLay);
	outerVertLayout->insertLayout(0,horLay,0);
	
	bpDirty1_ = new QLabel("dot 1", this, "Dirty Dot 1");	
	bpDirty1_->setScaledContents(TRUE);
	bpDirty1_->setPixmap ( dirtyPix );
	bpDirty1_->setFixedSize( QSize( 8, 8 ) );	
	horLay->addWidget(bpDirty1_);
	horLay->addItem( 
			new QSpacerItem ( 8,8, QSizePolicy::Expanding, QSizePolicy::Minimum) );
	horLay->addStrut(8);
	bpDirty2_= new QLabel("dot 2", this, "Dirty Dot 2");
	bpDirty2_->setPixmap ( dirtyPix );	
	bpDirty2_->setScaledContents(TRUE);
	bpDirty2_->setFixedSize( QSize( 8, 8 ) );	
	horLay->addWidget(bpDirty2_);
	
	horLay = new QHBoxLayout(outerVertLayout, 0,  "bottom dirty layout");
	outerVertLayout->removeItem(horLay);
	outerVertLayout->insertLayout(2,horLay,0);
	
	bpDirty3_ = new QLabel("dot3",this, "Dirty Dot 3");
	bpDirty3_->setPixmap ( dirtyPix );
	bpDirty3_->setScaledContents(TRUE);
	bpDirty3_->setFixedSize( QSize( 8, 8 ) );	
	horLay->addWidget(bpDirty3_);
	horLay->addItem( 
			new QSpacerItem ( 8,8, QSizePolicy::Expanding, QSizePolicy::Minimum) );
	horLay->addStrut(8);
	bpDirty4_ = new QLabel("dot4",this, "Dirty Dot 4");
	bpDirty4_->setPixmap ( dirtyPix );
	bpDirty4_->setScaledContents(TRUE);
	bpDirty4_->setFixedSize( QSize( 8, 8 ) );	
	horLay->addWidget(bpDirty4_);
	
	

	/* Then, create the rest of the dialog.
	 */
	
	buildRowColumnControl();
	buildDefaultButtons();
	cbRunStatusChanged();  // enable/disable OK, Apply, ApplyAll
	show();
	return 1;
}

void qDialogBase::drawStandardLayout(int sliderType){
	char *deltaValue[1] = {"Delta"};
	qEditSliderDouble *esd;
	qEditSliderLong *esl;
	qZabelSlider *zsl;
	qCheckBoxGroup *cbg;
	
	QLabel *st;

					
	int cols = 1;
	if(cbgBoxName_.count())
		cols++;

	QGridLayout * qgrid = new QGridLayout(vertLayout, numItems_ , cols, 3, "Gridlayout");
	//If the slider is a zabel, insert space for the label item
	if(sliderType == Q_ZABELSLIDER){
		qgrid->expand(qgrid->numRows() , qgrid->numCols()+1); //add a column for the zabel
		qgrid->setColStretch(1,1); //allow the second column to strech
	}
	vertLayout->removeItem(qgrid); //remove, before we reinsert it	
	vertLayout->insertLayout(1,qgrid,1); //now insert the entire grid, with a stretch factor
	vertLayout->insertStretch (2, 2 ); //add stretch at the end of the vertical layout before the btns

	cbgDelta_ = cbg = 
		new qCheckBoxGroup(this, Q_CHECKBOXDELTA, deltaValue,1);
		qgrid->addWidget(cbg, 0, qgrid->numCols()-1);	
	    emit evControlRegistration_(rc_->insert(cbg,0,0,Q_CHECKBOXDELTA));
	if(cbgBoxName_.count()){
		cbg = new qCheckBoxGroup(this, Q_CHECKBOXALL, &cbgBoxName_);
		qgrid->addWidget(cbg,0, qgrid->numCols()-1);
		emit evControlRegistration_(rc_->insert(cbg,0,1,Q_CHECKBOXALL));
	}


	/* Item lines. Each line has a slider and a check box group. */
	for(int i=0;i<(int)numItems_;i++){
		//	sizers_.append( 
		//	invis = new zPercentSizer(ZGRAV_TOP, (float)(90.0 / (numItems_+1)),
		//								100, rc_->sizer()) );
		if(sliderType == Q_DOUBLEEDITSLIDER){
			esd = new qEditSliderDouble(this," ",i,0.0,2.0);
			qgrid->addWidget(esd, i+1, 0);
			emit evControlRegistration_(rc_->insert(esd,i+1,0,i,Q_CHECKBOXDELTA));
		}
		else if(sliderType == Q_LONGEDITSLIDER){
			esl = new qEditSliderLong(this," ",i,0,2);
			qgrid->addWidget(esl, i+1, 0);
			emit evControlRegistration_(rc_->insert(esl,i+1,0,i,Q_CHECKBOXDELTA));
			
		}
		else if(sliderType == Q_ZABELSLIDER){
			st = new QLabel( this);
			st->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
			qgrid->addWidget(st, i+1, 0);
			emit evControlRegistration_(rc_->insert(st,i+1,0,i,Q_STATICTEXT));
			zsl = new qZabelSlider(this,0,"ZABEL",i);
			qgrid->addWidget(zsl, i+1, 1);
			zsl->setMaximumHeight(15);
			zsl->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
			emit evControlRegistration_(rc_->insert(zsl,i+1,0,i,Q_CHECKBOXDELTA));			
			zsl->show();
		}

		if(cbgBoxName_.count()){
			cbg = new qCheckBoxGroup(this,i, &cbgBoxName_);
			cbg->setBaseSize(25,20);
			cbg->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
			qgrid->addWidget(cbg, i+1 , 2);
			emit evControlRegistration_(rc_->insert(cbg,i+1,1,i,Q_CHECKBOXALL));
		}
	}
	
}

void qDialogBase::buildDefaultButtons(){


    /* Reserve some space on top of the unused space. I know, this doesn't
     *  really belong here. An accident of history.
     */
		
	horizLayout[1] = new QHBoxLayout(vertLayout, 0, "bottom buttons");
		
	pbOK_ = new QPushButton("OK", this);
	pbCancel_ = new QPushButton( "Cancel", this);
	pbApply_ = new QPushButton( "Apply", this);
	pbReset_ = new QPushButton("Reset", this);
	pbApplyAll_ = new QPushButton("Apply All", this);
	pbResetAll_ = new QPushButton( "Reset All", this);
	pbHelp_ = new QPushButton( "Help", this);	
		
	horizLayout[1]->addStretch ( 1 );
	horizLayout[1]->addWidget(pbHelp_);
	horizLayout[1]->addWidget(pbResetAll_);
	horizLayout[1]->addWidget(pbApplyAll_);
	horizLayout[1]->addWidget(pbReset_);
	horizLayout[1]->addWidget(pbApply_);
	horizLayout[1]->addWidget(pbCancel_);
	horizLayout[1]->addWidget(pbOK_);

	pbHelp_->show();
	pbApplyAll_->show();
	pbReset_->show();
	pbApply_->show();
	pbOK_->show();
	pbCancel_->show();
	pbResetAll_->show();

	connect(pbOK_, SIGNAL(clicked()), this, SLOT(cbOK()));
	connect(pbApplyAll_, SIGNAL(clicked()), this, SLOT(cbApplyAll()));
	connect(pbResetAll_, SIGNAL(clicked()), this, SLOT(cbResetAll()));
	connect(pbApply_, SIGNAL(clicked()), this, SLOT(cbApply()));
	connect(pbReset_, SIGNAL(clicked()), this, SLOT(cbReset()));
	connect(pbHelp_, SIGNAL(clicked()), this, SLOT(cbHelp()));
	connect(pbCancel_, SIGNAL(clicked()), this, SLOT(cbCancel()));

    cbRunStatusChanged();
}
void qDialogBase::addBoxName(const char *tag){
	cbgBoxName_.append(tag);	
	cbgTags_.resize(numItems_*cbgBoxName_.count());
	for(unsigned int l=0;l<cbgTags_.count();l++)
		cbgTags_[l]=0;
}	

void qDialogBase::setCheckBox(unsigned box, unsigned checked, unsigned id){
	unsigned int item = id+box*numItems_;
	if(item < cbgTags_.count())
		cbgTags_[item] = checked;
}

int qDialogBase::isBoxChecked(unsigned item, unsigned box){
	if((item+box*numItems_) < cbgTags_.count())
		return cbgTags_[item+box*numItems_];
	return -1;
}

void qDialogBase::cbCancel(){
	emit evCancel_(1);
	wasCloseNotified_ = 1; /* Prevent multi-notification. */
	close (true);
	return ;
}
void qDialogBase::cbOK(){
	emit evOK_();
	wasCloseNotified_ = 1;   /* Prevent multi-notification. */
	close (true);
	return ;
}
void qDialogBase::cbApply(){
	emit evApply_();
	return ;
}
void qDialogBase::cbReset(){
	emit evReset_();
	return ;
}
void qDialogBase::cbApplyAll(){
	emit evApplyAll_();
	return ;
}
void qDialogBase::cbResetAll(){
	emit evResetAll_();
	return ;
}
void qDialogBase::cbHelp(){
	emit evHelp_();
	return ;
}

void qDialogBase::cbCmdStatic1(){
   	if(cmbStatic_[0])
		emit evCmbStatic1_(cmbStatic_[0]->currentItem());
	//cmbStatic_[0]->repaint();
	return;
}

void qDialogBase::cbCmdStatic2(){
	
    if(cmbStatic_[1])
		emit evCmbStatic2_(cmbStatic_[1]->currentItem());
	//cmbStatic_[1]->repaint();
	return ;
}

void qDialogBase::cbCmdStatic3(){
	
	if(cmbStatic_[2]){
		emit evCmbStatic3_(cmbStatic_[2]->currentItem());
		
	}
	//cmbStatic_[2]->repaint();
	return ;
}

