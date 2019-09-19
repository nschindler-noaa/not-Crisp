
#include "qDefines.h"
#include "qGlobalEvents.h"
#include "resource.h"
#include "run_mode.h"
#include "qRowColumnBox.h"
#include "qEditSliderLong.h"
#include "qEditSliderDouble.h"
#include "qZabelSlider.h"
#include "qListDialogBase.h"
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



qListDialogBase::qListDialogBase(QWidget *w, WFlags dw, const char *nt)
                    : QDialog(w,nt, dw),
					image0( (const char **) image0_data ){
	consCommon();
}

void qListDialogBase::consCommon( void ) {


	cbgDelta_ = NULL;
	rc_ = NULL;
	tc_ = NULL;
	bpDirty1_ = NULL;
	wasCloseNotified_ = 0;
	numRowsPerTab_ = 10;
	numTabs_ = 0;
	currentIndex_ = 0;
	numRows_ = 0;
	numColumns_ = 1;
	cbgBoxName_.append( "G" );
	VertLayout = new QVBoxLayout(this, 0, -1, "Vertical Layout Manager");
	resize(600,300);

	if(parent()->isWidgetType() && ((QWidget*)parent())->icon())
		setIcon(*((QWidget*)parent())->icon() );


}

/* Receive notification that run status has changed.
 */

void qListDialogBase::cbRunStatusChanged( void ){
	if (run_mode.running) {
		pbOK_->setEnabled(false);
		pbApply_->setEnabled(false);
	}
	else {
		pbOK_->setEnabled(true);
		pbApply_->setEnabled(true);
	}
}

qListDialogBase::~qListDialogBase(){ 

	/* Make sure that anyone interested in hearing of our death, does
	 *  hear of it. */
	if (!wasCloseNotified_)
		emit evCancel_();

    /* NOTE: this must come before deletion of tc_, to avoid 
     *  double-deletion of child windows.
     */
    if (rc_ != NULL)
        delete rc_;

	/* This should be cleaned up automatically, but appearently is not. */
	if (tc_ != NULL) {
		delete tc_;
	}
}

/* These routines show/hide the Delta checkbox and the dirty indicator */
void qListDialogBase::showDelta( bool iShow ) {
	if (cbgDelta_ != NULL)
		cbgDelta_->showCBG( 0, iShow );
}

void qListDialogBase::showDirty( bool iShow ) {
	if (bpDirty1_ != 0) {
		bpDirty1_->setShown(iShow);
		bpDirty2_->setShown(iShow);
		bpDirty3_->setShown(iShow);
		bpDirty4_->setShown(iShow);		
		

	}
}


void qListDialogBase::closeEvent ( QCloseEvent * e ){
	/* Underlying window system has killed our window - handle like
	 *  a user "Cancel" action.
	 */
	if (!wasCloseNotified_) {
		emit evCancel_();
		wasCloseNotified_ = 1;
	}
	e->accept();
	QDialog::close(e);
	return ;  /* Allow kill. */
}

int qListDialogBase::buildTabControl(){

	char cBuf[2];
	cBuf[0] = 'A';
	cBuf[1] = 0;
	tc_ = new qImpTabCtrl(this);
	VertLayout->addWidget(tc_);

	connect(tc_, SIGNAL(evPageChange_(int , int)), this , SLOT(cbTabSelected(int,int) ));
	for(unsigned int i=0;i<numTabs_;i++) {
		appendTab((int)i,cBuf);
		cBuf[0]++;
	}
	tc_->setCurrentPage(0);
	tc_->show();
	return 1;
}

void qListDialogBase::appendTab(int tabIndex, const char *tabName){
	if(tabIndex < (int)numTabs_){
		QWidget * qw = new QWidget(tc_, QString(tabName));
		QGridLayout * qgl = new QGridLayout(qw, numColumns_, numRows_, 5);		
		tc_->appendTab(qw, QString(tabName)  );
	}
}

int qListDialogBase::buildRowColumnControl(){
	rc_ = new qRowColumnBox( this );

	return 1;
}

int qListDialogBase::drawDialog(unsigned topMargin){
	/* First, create the bitmaps which will indicate data dirtyness.
	 *  We happen to know that the bitmaps are 10x10.
	 */

	numTabs_ = (numRows_ + numRowsPerTab_ - 1) / numRowsPerTab_;

	bpDirty1_ = new QLabel(this, "Dirty Dot 1");	
	bpDirty1_->setPixmap ( image0 );
	bpDirty1_->setFixedSize(8,8); 
	bpDirty1_->setScaledContents( TRUE );
	bpDirty1_->show();

	bpDirty2_= new QLabel(this, "Dirty Dot 2");
	bpDirty2_->setPixmap ( image0 );
	bpDirty2_->setFixedSize(8,8); 
	bpDirty2_->setScaledContents( TRUE );
	bpDirty2_->show();

	bpDirty3_ = new QLabel(this, "Dirty Dot 3");
	bpDirty3_->setPixmap ( image0 );
	bpDirty3_->setFixedSize(8,8); 
	bpDirty3_->setScaledContents( TRUE );
	bpDirty3_->show();

	bpDirty4_ = new QLabel(this, "Dirty Dot 4");
	bpDirty4_->setPixmap ( image0 );
	bpDirty4_->setFixedSize(8,8); 
	bpDirty4_->setScaledContents( TRUE );
	bpDirty4_->show();

	//Insert the dirty Indicators in the layout
	QHBoxLayout* dirtyLayout = new QHBoxLayout(VertLayout, 0, "dirty Layout Manager");
	dirtyLayout->addWidget(bpDirty1_,Qt::AlignLeft);
	dirtyLayout->addStretch(1);
	dirtyLayout->addWidget(bpDirty2_,Qt::AlignRight);
	dirtyLayout->addStrut (8);
	VertLayout->removeItem(dirtyLayout);
	VertLayout->insertLayout(0,dirtyLayout);
	
	if (numTabs_ == 1)
		numRowsPerTab_ = numRows_;
	if (numTabs_ > 1)
		buildTabControl();
	buildRowColumnControl();

	drawStandardLayout();

		/* Then, create the rest of the dialog.
	 */
	buildDefaultButtons();
	//Bottom row of default buttons
	QHBoxLayout* dirtyLayout2 = new QHBoxLayout(VertLayout, 0, "dirty Layout Manager 2 ");
	dirtyLayout2->addWidget(bpDirty3_,Qt::AlignLeft);
	dirtyLayout2->addStretch(1);
	dirtyLayout2->addWidget(bpDirty4_,Qt::AlignRight);
	dirtyLayout2->addStrut ( 8);
	

	showDirty( false );
	showDelta( false );
	//sizer()->update();
	if (numRowsPerTab_ < 10) {
		QRect r;
		r = rect();
		resize( r.width(),(int)(r.height() * (1.0 - 0.06 * (10 - numRowsPerTab_))) );
	}
	show();
	cbRunStatusChanged(); // enable/disable Apply button etc
	return 1;
}

void qListDialogBase::drawStandardLayout(){
	char *deltaValue[1] = {"Delta"};
	qZabelSlider *zsl;
	qCheckBoxGroup *cbg;
	QLabel *st;
	int i;
	int j = 0;
	int page;
	float textPercent, sliderPercent;

	QWidget *q = NULL;
	QGridLayout* qgl = NULL;	

	/* Ganged update box state is maintained in cbgTags_
	 */
	cbgTags_.resize( numRows_ );
	for(unsigned int l=0;l<cbgTags_.count();l++)
		cbgTags_[l]=0;

	/* Decide how wide the label text, sliders, and check box group will be.
	 */
	if (numColumns_ <= 1) {
		textPercent = 45.0;
		sliderPercent = 55.0;
	}
	else if (numColumns_ == 2) {
		textPercent = 35.0;
		sliderPercent = 32.5;
	}
	else /* if (numColumns_ >= 3) */ {
		textPercent = 25.0;
		sliderPercent = (float)(75.0 / numColumns_);
	}


	/* Top line. */

	//Make a button group to link all of the Check All buttons together
	if(numTabs_ ==1){
		q  = this; //add widgets directly to this widget
		qgl = new QGridLayout(VertLayout, numColumns_+1, numRows_, 10);
		//VertLayout->addLayout( qgl); 

		for (j = 0; j < (int)numColumns_; j++) {
			st = new QLabel(q );
			qgl->addWidget(st, 1, j+1, Qt::AlignHCenter);
			qgl->setColStretch ( j+1, 1 );
			st->show();
			emit evControlRegistration_(rc_->insert(st,0,j+1,j,Q_LABELTEXT));
		}
		if (numRows_ > 1) {
			cbg = new qCheckBoxGroup( q ,Q_CHECKBOXALL, &cbgBoxName_);
			qgl->addWidget(cbg, 0, qgl->numCols());
			emit evControlRegistration_(rc_->insert(cbg,0,1,Q_CHECKBOXALL));

		}		
		
	}
	else{ //a tabbed dialog
	//Do an iteration for each tab
	for (page = 0; page < (int)numTabs_; page++) {
		q = tc_->page(page);
		qgl = (QGridLayout*) q->layout();
		for (j = 0; j < (int)numColumns_; j++) {
			st = new QLabel(q );
			qgl->addWidget(st, 1, j+1, Qt::AlignHCenter);
			qgl->setColStretch ( j+1, 1 );
			st->show();
			emit evControlRegistration_(rc_->insert(st,page*numRowsPerTab_,j+1,j,Q_LABELTEXT));
		}
		if (numRows_ > 1) {
			cbg = new qCheckBoxGroup( q ,Q_CHECKBOXALL, &cbgBoxName_);
			qgl->addWidget(cbg, 0, qgl->numCols()-1);
			emit evControlRegistration_(rc_->insert(cbg,0,1,Q_CHECKBOXALL));

		}		
	
	
	}
	
	} //end labbed dialog


	if(numTabs_ ==1){
		for(i=0;i < (int)numRowsPerTab_  ;i++){
			//Set up the blankspacers for layouts with partialy empty grids
			if(i >= (int)numRows_){
				continue;		
			}
			/* Single check box for ganged updates. */
			if (numRows_ > 1) {

				cbg = new qCheckBoxGroup(  q ,	i, &cbgBoxName_);
				qgl->addWidget(cbg, i+2, qgl->numCols()-1 );
				cbg->show();
				emit evControlRegistration_( rc_->insert(cbg,i+1,numColumns_,
												i,Q_CHECKBOXALL) );
			}
			/* Rest of controls size to take up horizontal space dynamically.
			 */
			
			st = new QLabel(q,"");
			qgl->addWidget(st, i+2, 0);  //insert into first position of the ith+2 row
			emit evControlRegistration_(rc_->insert(st,i+1,0,
													i,Q_STATICTEXT));
			st->show();

			for (j = 0; j < (int)numColumns_; j++) {
				zsl = new qZabelSlider(
							 q ,0,"ZABEL", j + i * numColumns_  );	
				qgl->addWidget(zsl, i+2, j+1);  //insert into column j+1, to account for the text
				zsl->show();
				emit evControlRegistration_(
								rc_->insert(zsl, i+1, j,
								 j + i * numColumns_, Q_CHECKBOXDELTA));
			}
			
		} //done buiding tab

	}
	else{
		//Do an interaction for each page
		for (page = 0; page < (int)numTabs_; page++) {
			q = tc_->page(page);
			qgl = (QGridLayout*) q->layout();

			for(i=0;i < (int)numRowsPerTab_  ;i++){
				
				//Set up the blankspacers for layouts with partialy empty grids
				if((i+page*numRowsPerTab_) >= numRows_){
					zsl = new qZabelSlider(
								 q ,0,"ZABEL", j + i * numColumns_  );	
					qgl->addWidget(zsl, i+2, j+1);  //insert into column j+1, to account for the text
					continue;
				}
				/* Single check box for ganged updates. */
				if (numRows_ > 1) {
					cbg = new qCheckBoxGroup(  q ,	i, &cbgBoxName_);
					qgl->addWidget (cbg, i+2, qgl->numCols()-1 );
					cbg->show();
					emit evControlRegistration_( rc_->insert(cbg,page*numRowsPerTab_+i+1,numColumns_,
													i,Q_CHECKBOXALL) );
				}
				/* Rest of controls size to take up horizontal space dynamically.
				 */
			
				st = new QLabel(q,"");
				qgl->addWidget(st, i+2, 0);  //insert into first position of the ith+2 row
				emit evControlRegistration_(rc_->insert(st,page*numRowsPerTab_+i+1,0,
														i,Q_STATICTEXT));
				st->show();

				for (j = 0; j < (int)numColumns_; j++) {
					zsl = new qZabelSlider(
								 q ,0,"ZABEL", j + i * numColumns_  );	
					qgl->addWidget(zsl, i+2, j+1);  //insert into column j+1, to account for the text
					zsl->show();
					emit evControlRegistration_(
									rc_->insert(zsl, page*numRowsPerTab_+i+1, j,
									 j + i * numColumns_, Q_CHECKBOXDELTA));
				}
				
			} //done buiding tab
		} //done building pages
	}
		

}

void qListDialogBase::buildDefaultButtons(){
   
    /* NOTE buttons are created in "reverse" order to make keyboard
     *  interface to dialog box work more intuitively.
     */
	horizLayout = new QHBoxLayout(VertLayout);
	//VertLayout->addLayout(horizLayout);

	pbHelp_ = new QPushButton("Help", this);
	pbReset_ = new QPushButton("Reset",  this);
	pbApply_ = new QPushButton( "Apply",  this);
	pbCancel_ = new QPushButton( "Cancel",  this);
	pbOK_ = new QPushButton( "OK",  this);

	horizLayout->addStretch(1);
	horizLayout->addWidget(pbHelp_);
	horizLayout->addWidget(pbReset_);
	horizLayout->addWidget(pbApply_);
	horizLayout->addWidget(pbCancel_);
	horizLayout->addWidget(pbOK_);


	connect(pbOK_, SIGNAL(clicked()), this , SLOT(cbOK()));
	connect(pbCancel_, SIGNAL(clicked()), this , SLOT(cbCancel()));
	connect(pbApply_, SIGNAL(clicked()), this , SLOT(cbApply()));
	connect(pbReset_, SIGNAL(clicked()), this , SLOT(cbReset()));
	connect(pbHelp_, SIGNAL(clicked()), this , SLOT(cbHelp()));

	pbOK_->show();
	pbApply_->show();
	pbReset_->show();
	pbHelp_->show();
	pbCancel_->show();	


}

void qListDialogBase::setCheckBox(unsigned checked, unsigned id){
	if(id < cbgTags_.count())
		cbgTags_[id] = checked;
}

int qListDialogBase::isBoxChecked(unsigned box){
	if(box < cbgTags_.count())
		return cbgTags_[box];
	return -1;
}

void qListDialogBase::cbCancel(){
	emit evCancel_();
	wasCloseNotified_ = 1; /* Prevent multi-notification. */
	close(true);
	return ;
}
void qListDialogBase::cbOK(){
	emit evOK_();
	wasCloseNotified_ = 1;   /* Prevent multi-notification. */
	close(true);
	return ;
}
void qListDialogBase::cbApply(){
	emit evApply_();
	return ;
}
void qListDialogBase::cbReset(){
	emit evReset_();
	return ;
}
void qListDialogBase::cbHelp(){
	emit evHelp_();
	return ;
}
/*
int qListDialogBase::command(zCommandEvt *Ev){

	// Allow default command processing.
	return 0;
}
*/
void qListDialogBase::cbTabSelected( int newIndex, int oldIndex){
	currentIndex_ = newIndex;
	emit evTabSelected_(newIndex);
}
/*
void qListDialogBase::_cbTabSelected(void *vp, const int newIndex, 
									   const int oldIndex){

	qListDialogBase *zld = (qListDialogBase *)vp;
	zld->cbTabSelected(newIndex, oldIndex);

}
*/

void qListDialogBase::paintEvent ( QPaintEvent * pe){
	
	QPtrListIterator<qItem>* itr = getItemIterator();
	qItem* item; 
	QWidget * widget;
	//Repaint all widgets
	while(itr->current() != 0){
		item = itr->current();
		++(*itr);
		widget = item->w();
		widget->repaint();
	}
	//Repaint All buttons
	pbCancel_->repaint(), pbApply_->repaint(), pbOK_->repaint();
	pbHelp_->repaint(), pbReset_->repaint();


}

