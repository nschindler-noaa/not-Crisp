
#include "qDefines.h"
#include "qGlobalEvents.h"
#include "run_mode.h"
#include "resource.h"
#include "qRowColumnBox.h"
#include "qCheckBoxDialogBase.h"
#include "qCheckBoxGroup.h"
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


qCheckBoxDialogBase::qCheckBoxDialogBase(QWidget *w, WFlags dw, const char *nt)
                    : QFrame(NULL,nt, dw | WType_TopLevel | WStyle_SysMenu | WStyle_Minimize | WStyle_Dialog | WDestructiveClose),
						image0( (const char **) image0_data ) {
	consCommon();
}

void qCheckBoxDialogBase::consCommon( void ) {
	
	rc_ = NULL;
	tc_ = NULL;
	bpDirty1_ = NULL;
	wasCloseNotified_ = 0;
	numRowsPerTab_ = 10;
	numTabs_ = 0;
	currentIndex_ = 0;
	numRows_ = 0;
	numColumns_ = 1;

	VertLayout = new QVBoxLayout(this);  //main layout holder

	if(!connect(qEvents::Instance(), SIGNAL(GERunStatusChanged_() ), this, SLOT(cbRunStatusChanged()))){
		log_msg(0, "!Error connecting GERunstatusCHanged in qCheckDialog Base!");
	}
}

qCheckBoxDialogBase::~qCheckBoxDialogBase(){ 


	/* Make sure that anyone interested in hearing of our death, does
	 *  hear of it. */
	if (!wasCloseNotified_)
		emit evCancel();


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

void qCheckBoxDialogBase::cbRunStatusChanged(  ){
	if (run_mode.running) {
		pbOK_->setEnabled(false);
		pbApply_->setEnabled(false);
	}
	else {
		pbOK_->setEnabled(true);
		pbApply_->setEnabled(true);
	}
}

/* This routine shows/hides the dirty indicators */
void qCheckBoxDialogBase::showDirty( bool iShow ) {
	if (bpDirty1_ != NULL) {
		bpDirty1_->setShown( iShow );
		bpDirty2_->setShown( iShow );
		bpDirty3_->setShown( iShow );
		bpDirty4_->setShown( iShow );
	}

}


void qCheckBoxDialogBase::closeEvent( QCloseEvent *ev ) {
	/* Underlying window system has killed our window - handle like
	 *  a user "Cancel" action.
	 */
	if (!wasCloseNotified_) {
		emit evCancel();
		wasCloseNotified_ = 1;
	}
	ev->accept();
	QFrame::close(ev);

	return ;  /* Allow kill. */
}

int qCheckBoxDialogBase::buildTabControl(){

	char cBuf[2];
	cBuf[0] = 'A';
	cBuf[1] = 0;
	tc_ = new qImpTabCtrl(this);
	VertLayout->addWidget(tc_);
	
	//tc_->evPageChange()->attach(this, &qCheckBoxDialogBase::_cbTabSelected);
	//generate error is 
	if(!connect(tc_, SIGNAL(evPageChange_(int , int)), this , SLOT(cbTabSelected(int,int) )) ){
		log_msg(0, "CHECK BOX SIGNAL CONNECTION ERROR");	
	}

	for(unsigned i=0; i < (unsigned)numTabs_; i++) {
		appendTab(i,cBuf);
		cBuf[0]++;
	}
	tc_->setCurrentPage(0);
	tc_->show();
	return 1;
}

void qCheckBoxDialogBase::appendTab(int tabIndex, const char *tabName){
	
	if(tabIndex < (int)numTabs_){
		QWidget * qw = new QWidget(tc_, QString(tabName));
		QGridLayout * qgl = new QGridLayout(qw, numColumns_, numRows_, 5);		
		tc_->appendTab(qw, QString(tabName));
	}
}

int qCheckBoxDialogBase::buildRowColumnControl(){
	
	rc_ = new qRowColumnBox( this );
	
	return 1;
}

int qCheckBoxDialogBase::drawDialog(unsigned topMargin){
	/* First, create the bitmaps which will indicate data dirtyness.
	 *  We happen to know that the bitmaps are 10x10.
	 */
	
	bpDirty1_ = new QLabel(this, "Dirty Dot 1");	
	bpDirty1_->setFixedSize(8,8); 
	bpDirty1_->setPixmap ( image0 );
	bpDirty1_->setScaledContents( TRUE );
	bpDirty1_->show();

	bpDirty2_= new QLabel(this, "Dirty Dot 2");
	bpDirty2_->setPixmap ( image0 );
	bpDirty2_->setScaledContents( TRUE );
	bpDirty2_->setFixedSize(8,8); 
	bpDirty2_->show();

	bpDirty3_ = new QLabel(this, "Dirty Dot 3");
	bpDirty3_->setPixmap ( image0 );
	bpDirty3_->setScaledContents( TRUE );
	bpDirty3_->setFixedSize(8,8); 
	bpDirty3_->show();

	bpDirty4_ = new QLabel(this, "Dirty Dot 4");
	bpDirty4_->setPixmap ( image0 );
	bpDirty4_->setScaledContents( TRUE );
	bpDirty4_->setFixedSize(8,8); 
	bpDirty4_->show();

	//Insert the top dirty Indicators in the layout
	QHBoxLayout* dirtyLayout = new QHBoxLayout(VertLayout, 0, "dirty Layout Manager");
	dirtyLayout->addWidget(bpDirty1_,Qt::AlignLeft);
	dirtyLayout->addStretch(1);
	dirtyLayout->addWidget(bpDirty2_,Qt::AlignRight);
	dirtyLayout->addStrut (8);
	
	
	/* Then, create the rest of the dialog.
	 */
	numTabs_ = (numRows_ + numRowsPerTab_ - 1) / numRowsPerTab_;
	if (numTabs_ == 1)
		numRowsPerTab_ = numRows_;
	if (numTabs_ > 1)
		buildTabControl( );
	buildRowColumnControl();
	//Make all of the checkboxes here
	drawStandardLayout();

	/*
	if (numRowsPerTab_ < 10) {
		QRect r;
		getExterior( r );
		move( r.left(), r.top(), r.width(), 
				(int)(r.height() * (1.0 - 0.06 * (10 - numRowsPerTab_))) );
	}
*/
	buildDefaultButtons();  //lastly build these buttons
	//Now the bottom dirty dots
	QHBoxLayout* dirtyLayout2 = new QHBoxLayout(VertLayout, 0, "dirty Layout Manager 2 ");
	dirtyLayout2->addWidget(bpDirty3_,Qt::AlignLeft);
	dirtyLayout2->addStretch(1);
	dirtyLayout2->addWidget(bpDirty4_,Qt::AlignRight);
	dirtyLayout2->addStrut ( 8);
	//VertLayout->addLayout(dirtyLayout2);

	showDirty(false);
	cbRunStatusChanged();
	resize(0,0);
	show();
	return 1;
}

void qCheckBoxDialogBase::drawStandardLayout(){
	
	QLabel *st;
	qCheckBoxGroup *cbg;
	int i, j;
	QValueVector<QString> cbgBoxName;  
	QWidget *q;
	QGridLayout* qgl;	

	for (i = 0; i < (int)numColumns_; i++)
		cbgBoxName.append( "  " );

	/* Column heading line. Like row labels, column headings are initially
	 *  blank text fields which will be filled in by the mediator.
	 */
	for (j = 0; j < (int)numTabs_; j++) {
		if (numTabs_ == 1) {
			q  = this; //add widgets directly to this dialog
			
			//Add 
			qgl = new QGridLayout(VertLayout, numColumns_, numRows_, 5 );
		}
		else { // More than 1 tab
			q  = tc_->page(j); //add widgets to the current page
			qgl = (QGridLayout*)q->layout();
		}
		//make a horizontal layout to contain the labels
		QHBoxLayout * hlabel  = new QHBoxLayout();


		for (i = 0; i < (int)numColumns_; i++) {
			st = new QLabel( "col label" , q );
			hlabel->addWidget(st);			
			st->show();
			emit evControlRegistration(rc_->insert(st,0,i+1,i,Q_LABELTEXT));
		}
		
		qgl->addLayout(hlabel, 0, 1);

		/* Item lines. Each line has a row label, one or more sliders and 
		*  a single check box.
		*/
	
		for(i = 0;i < (int)numRowsPerTab_;i++){
			/* Controls size to take up horizontal space dynamically.
			 * add to q */

			st = new QLabel("row label",q);
			qgl->addWidget(st, i+1, 0);
			emit evControlRegistration(rc_->insert(st,i+1, 0, i, Q_STATICTEXT));
			st->show();

			cbg = new qCheckBoxGroup(q,	i, &cbgBoxName );
			cbg->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred );
			qgl->addWidget(cbg, i+1, 1 );
			cbg->show();
			/* TODO: this was i+1 in the multipage one for some reason
			 * duplicated below. Just tested the Output Settings on both
			 * paged and not paged and it seems good... was this an error? */
//				emit evControlRegistration(rc_->insert(cbg, i+1, 1, i, Q_CHECKBOXGROUP));
			emit evControlRegistration(rc_->insert(cbg,i, 1, i , Q_CHECKBOXGROUP));
		}
	}
}

void qCheckBoxDialogBase::buildDefaultButtons(){


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

	horizLayout->addStretch(1); //push butttons to the right
	horizLayout->addWidget( pbHelp_ );
	horizLayout->addWidget( pbReset_ );
	horizLayout->addWidget( pbApply_ );
	horizLayout->addWidget( pbCancel_ );
	horizLayout->addWidget( pbOK_ );	

	connect(pbOK_, SIGNAL(clicked()), this , SLOT(cbOK()));
	connect(pbCancel_, SIGNAL(clicked()), this , SLOT(cbCancel()));
	connect(pbApply_, SIGNAL(clicked()), this , SLOT(cbApply()));
	connect(pbReset_, SIGNAL(clicked()), this , SLOT(cbReset()));
	connect(pbHelp_, SIGNAL(clicked()), this , SLOT(cbHelp()));
	//Now show the buttons
	pbOK_->show();
	pbApply_->show();
	pbReset_->show();
	pbHelp_->show();
	pbCancel_->show();	
}

void qCheckBoxDialogBase::cbCancel(){
	emit evCancel();
	wasCloseNotified_ = 1; /* Prevent multi-notification. */
	close(); //shutdown();
	return ;
}
void qCheckBoxDialogBase::cbOK(){
	emit evOK();
	wasCloseNotified_ = 1;   /* Prevent multi-notification. */
	close(); //shutdown();
	return;
}
void qCheckBoxDialogBase::cbApply(){
	emit evApply();
	return;
}
void qCheckBoxDialogBase::cbReset(){
	emit evReset();
	return;
}
void qCheckBoxDialogBase::cbHelp(){
	emit evHelp();
	return;
}


void qCheckBoxDialogBase::cbTabSelected(int newIndex, int oldIndex){
	currentIndex_ = newIndex;
	emit evTabSelected(newIndex);
}
