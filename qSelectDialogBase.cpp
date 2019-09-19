
#include "qDefines.h"
#include "resource.h"
#include "qGlobalEvents.h"
#include "run_mode.h"
#include "qRowColumnBox.h"
#include "qZabelSlider.h"
#include "qSelectDialogBase.h"
#include "log.h"

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

qSelectDialogBase::qSelectDialogBase(QWidget *w, int numCbgCols, int featuresAreDams ) 
: QDialog(w,"Select Dialog Base", FALSE, Qt::WType_Dialog | Qt::WDestructiveClose) ,
					image0( (const char **) image0_data ){
	numCbgCols_ = numCbgCols;
    featuresAreDams_ = featuresAreDams;
	consCommon();
}

void qSelectDialogBase::consCommon( void ) {
		
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
    cbgBoxName_.push_back( featuresAreDams_ ? "D" : "R" );
	if (numCbgCols_ > 1)
		cbgBoxName_.push_back( "S" );		
	
	VertLayout = new QVBoxLayout(this, 0, -1, "Vertical Layout Manager");
	resize(50,50);   ///shrink to the smallest possible size
	
	cmbStatic_[0] = new QComboBox(this);
	cmbStatic_[1] = new QComboBox(this);
		
	cmbStatic_[0]->setFocusPolicy(StrongFocus);
	cmbStatic_[1]->setFocusPolicy(StrongFocus);

	connect(cmbStatic_[0], SIGNAL(activated ( int )), this, SLOT(cbCmbStatic1()));
	connect(cmbStatic_[1], SIGNAL(activated ( int )), this, SLOT(cbCmbStatic2()));

	
	if(parent()->isWidgetType() && ((QWidget*)parent())->icon())
		setIcon(*((QWidget*)parent())->icon() );
	
	connect(qEvents::Instance(),  SIGNAL(GERunStatusChanged_()), this, SLOT(cbRunStatusChanged()));

}

qSelectDialogBase::~qSelectDialogBase(){ 

	/* Make sure that anyone interested in hearing of our death, does
	 *  hear of it. */
	if (!wasCloseNotified_)
		emit evCancel(1);
	//GERunStatusChanged_.detach( this, &qSelectDialogBase::_cbRunStatusChanged );

	/* Sizers not attached to windows must be deleted explicitly. */
	//sizers_.clearAndDestroy();

    /* NOTE: this must come before deletion of tc_, to avoid 
     *  double-deletion of child windows.
     */
    if (rc_ != NULL)
        delete rc_;

	/* This should be cleaned up automatically, but appearently is not. */
	if (tc_ != NULL) {
		//tc_->evPageChange()->detach(this, &qSelectDialogBase::_cbTabSelected);
		delete tc_;
	}
}

/* These routines show/hide the Delta checkbox and the dirty indicator */
void qSelectDialogBase::showDelta( bool iShow ) {
	if (cbgDelta_ != NULL)
		cbgDelta_->showCBG( 0, iShow );
}

void qSelectDialogBase::showDirty( bool iShow ) {
	if (bpDirty1_ != NULL) {
		bpDirty1_->setShown( iShow );
		bpDirty2_->setShown( iShow );
		bpDirty3_->setShown( iShow );
		bpDirty4_->setShown( iShow );
	}
}


void qSelectDialogBase::closeEvent( QCloseEvent * e ) {
	/* Underlying window system has killed our window - handle like
	 *  a user "Cancel" action.
	 */
	if (!wasCloseNotified_) {
		emit evCancel(1);
		wasCloseNotified_ = 1;
	}
	e->accept();
	QDialog::close(e);
	return;  /* Allow kill. */
}

int qSelectDialogBase::buildTabControl(){

	char cBuf[2];
	cBuf[0] = 'A';
	cBuf[1] = 0;
	tc_ = new qImpTabCtrl(this);
	VertLayout->addWidget(tc_);

	connect(tc_, SIGNAL(evPageChange_(int , int )), this, SLOT(cbTabSelected(int,int)));
	//tc_->evPageChange()->attach(this, &qSelectDialogBase::_cbTabSelected);
	for(size_t i=0;i<numTabs_;i++) {
		appendTab(i,cBuf);
		cBuf[0]++;
	}
	tc_->setCurrentPage(0);
	tc_->show();
	return 1;
}

//Make a widget to hold the tab's children, giving it a grid layout

void qSelectDialogBase::appendTab(int tabIndex, const char *tabName){
	if(tabIndex < (int)numTabs_){
		QWidget * qw = new QWidget(tc_, QString(tabName));		
		QGridLayout * qgl = new QGridLayout(qw, numColumns_, numRows_, 5);	
		tc_->appendTab(qw, QString(tabName) );
		qw->show();
	}
}

int qSelectDialogBase::buildRowColumnControl(){
	
	rc_ = new qRowColumnBox( this );
	
	return 1;
}

int qSelectDialogBase::drawDialog(unsigned topMargin){
	
	/* create the bitmaps which will indicate data dirtyness.
	 *  We happen to know that the bitmaps are 10x10.
	 */
	
	bpDirty1_ = new QLabel( this, "dirty indicator" );
    bpDirty1_->setFixedSize( QSize( 8, 8 ) );
	bpDirty1_->setBackgroundColor(red);
    bpDirty1_->setPixmap( image0 );
    bpDirty1_->setScaledContents( TRUE );
	
	bpDirty2_ = new QLabel( this, "dirty indicator");
    bpDirty2_->setFixedSize( QSize( 8, 8 ) );
    bpDirty2_->setPixmap( image0 );
    bpDirty2_->setScaledContents( TRUE );
	
	bpDirty3_ = new QLabel( this, "dirty indicator" );
    bpDirty3_->setFixedSize( QSize( 8, 8 ) );
    bpDirty3_->setPixmap( image0 );
    bpDirty3_->setScaledContents( TRUE );

	bpDirty4_ = new QLabel( this, "dirty indicator" );
    bpDirty4_->setFixedSize( QSize( 8, 8 ) );
    bpDirty4_->setPixmap( image0 );
    bpDirty4_->setScaledContents( TRUE );

	bpDirty1_->show();
	bpDirty2_->show();
	bpDirty3_->show();
	bpDirty4_->show();

	//Insert the dirty Indicators in the layout
	QHBoxLayout* dirtyLayout = new QHBoxLayout(VertLayout, 0, "dirty Layout Manager");
	dirtyLayout->addWidget(bpDirty1_,Qt::AlignLeft);
	dirtyLayout->addStretch(1);
	dirtyLayout->addWidget(bpDirty2_,Qt::AlignRight);
	dirtyLayout->addStrut (8);
	VertLayout->removeItem(dirtyLayout);
	VertLayout->insertLayout(0,dirtyLayout);

	//Add combo Boxes
	QHBoxLayout * cmbLayout = new QHBoxLayout(VertLayout, 0,  "cmb box layout manager");

	cmbLayout->addItem( new QSpacerItem( 400, 8, QSizePolicy::Maximum, QSizePolicy::Minimum ));
	cmbLayout->addWidget(cmbStatic_[0]);
	cmbLayout->addWidget(cmbStatic_[1]);
	cmbLayout->addItem( new QSpacerItem( 400, 8, QSizePolicy::Maximum, QSizePolicy::Minimum ));
	cmbStatic_[0]->setUpdatesEnabled( TRUE );

	buildRowColumnControl( );	

	numTabs_ = (numRows_ + numRowsPerTab_ - 1) / numRowsPerTab_;
	if (numTabs_ == 1)
		numRowsPerTab_ = numRows_;
	if (numTabs_ > 1)
		buildTabControl();

	drawStandardLayout();

	/* Then, create the rest of the dialog.
	 */
	buildDefaultButtons();

	//Bottom dirty indicators
	QHBoxLayout* dirtyLayout2 = new QHBoxLayout(VertLayout, 0, "dirty Layout Manager 2 ");
	dirtyLayout2->addWidget(bpDirty3_,Qt::AlignLeft);
	dirtyLayout2->addStretch(1);
	dirtyLayout2->addWidget(bpDirty4_,Qt::AlignRight);
	dirtyLayout2->addStrut ( 8);
	

	showDirty( FALSE );
	showDelta( FALSE );

	if (numRowsPerTab_ < 10) {
		QRect r;
		r = rect();
		resize( r.width(),(int)(r.height() * (1.0 - 0.06 * (10 - numRowsPerTab_))) );
	}
	
	show();	
	return 1;
}

void qSelectDialogBase::drawStandardLayout(){
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
	cbgTags_.resize( numRows_ * 2 );
	for(size_t l=0;l<cbgTags_.count();l++)
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

	//Make a button group to link all of the Check All buttons together
	if(numTabs_ ==1){
		q  = this; //add widgets directly to this widget
		qgl = new QGridLayout(VertLayout, numColumns_+1, numRows_, 10,  "Grid Layout for SelectDialogBase");
		 

		for (j = 0; j < (int)numColumns_; j++) {
			st = new QLabel(q );
			qgl->addWidget(st, 1, j+1, Qt::AlignHCenter);
			qgl->setColStretch ( j+1, 1 );
			st->show();
			emit evControlRegistration(rc_->insert(st,0,j+1,j,Q_LABELTEXT));
		}
		if (numRows_ > 1) {
			 cbg = new qCheckBoxGroup( q ,Q_CHECKBOXALL, &cbgBoxName_);
			qgl->addWidget(cbg, 0, qgl->numCols());
			//evControlRegistration_.notify(rc_->insert(cbg,0,1,Q_CHECKBOXALL));
			emit evControlRegistration(rc_->insert(cbg,0,1,Q_CHECKBOXALL));

		}		
		//cbgDelta_ = cbg = 
		//	new qCheckBoxGroup( q,Q_CHECKBOXDELTA, deltaValue, 1 );
		//qgl->addWidget(cbg, 0, qgl->numCols()-2);
		//emit evControlRegistration_(rc_->insert(cbg,0,0,Q_CHECKBOXDELTA));
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
				emit evControlRegistration(rc_->insert(st,page*numRowsPerTab_,j+1,j,Q_LABELTEXT));
			}
			
			if (numRows_ > 1) {
				cbg = new qCheckBoxGroup( q ,Q_CHECKBOXALL, &cbgBoxName_);
				qgl->addWidget(cbg, 0, qgl->numCols()-1);
				//evControlRegistration_.notify(rc_->insert(cbg,0,1,Q_CHECKBOXALL));
				emit evControlRegistration(rc_->insert(cbg,0,1,Q_CHECKBOXALL));

			}
			
			//cbgDelta_ = cbg = 
			//	new qCheckBoxGroup( q,Q_CHECKBOXDELTA, deltaValue, 1 );
			//qgl->addWidget(cbg, 0, qgl->numCols()-2);
			//emit evControlRegistration_(rc_->insert(cbg,0,0,Q_CHECKBOXDELTA));
		
		} //end for loop of pages
	
	} //end tabbed dialog


	if(numTabs_ == 1){
		for(i=0;i < (int)numRowsPerTab_  ;i++){
			//Set up the blankspacers for layouts with partialy empty grids
			if(i >= (int)numRows_){
				continue;		
			}
			/* Single check box for ganged updates. */
			if (numRows_ > 1) {
//				log_msg(0, "adding checkbox group for q_checkboxall");
				cbg = new qCheckBoxGroup(  q ,	i, &cbgBoxName_);
				qgl->addWidget(cbg, i+2, qgl->numCols()-1 );
				cbg->show();
				emit evControlRegistration( rc_->insert(cbg,i+1,numColumns_,
												i,Q_CHECKBOXALL) );
			}
			/* Rest of controls size to take up horizontal space dynamically.
			 */
			//invis2 = new zGravSizer( ZGRAV_MIDDLE, 0, invis );
			//sizers_.append( invis2 );
			st = new QLabel(q,"static label" + QString::number(i));
			qgl->addWidget(st, i+2, 0);  //insert into first position of the ith+2 row
			emit evControlRegistration(rc_->insert(st,i+1,0,
													i,Q_STATICTEXT));
			st->show();

			for (j = 0; j < (int)numColumns_; j++) {
				zsl = new qZabelSlider(
							 q ,0,"ZABEL", j + i * numColumns_  );	
				qgl->addWidget(zsl, i+2, j+1);  //insert into column j+1, to account for the text
				zsl->show();
				emit evControlRegistration(
								rc_->insert(zsl, i+1, j,
								 j + i * numColumns_, Q_CHECKBOXDELTA));
			}
			
		} //done buiding tab

	}
	else{
		//Do an iteration for each page
		for (page = 0; page < (int)numTabs_; page++) {
			q = tc_->page(page);
			qgl = (QGridLayout*) q->layout();

			for(i=0;i < (int)numRowsPerTab_  ;i++){
				
				//Set up the blankspacers for layouts with partialy empty grids
				if((i+page*numRowsPerTab_) >= numRows_){
					zsl = new qZabelSlider(
								 q ,0,"ZABEL", j + i * numColumns_  );	
					zsl->setShown(FALSE);   //this one won't be shown
					qgl->addWidget(zsl, i+2, j+1);  //insert into column j+1, to account for the text
					continue;
				}
				/* Single check box for ganged updates. */
			
				if (numRows_ > 1) {
					cbg = new qCheckBoxGroup( q ,	i, &cbgBoxName_);
					qgl->addWidget (cbg, i+2, qgl->numCols()-1 );
					cbg->show();
					emit evControlRegistration( rc_->insert(cbg,i+1,numColumns_,
													i,Q_CHECKBOXALL) );
				//	log_msg(0, (char*)( "Creating check box group with id = " + QString::number(i)).ascii());
				//	log_msg(0, (char*)( "and group id = " + QString::number(Q_CHECKBOXALL)).ascii());
				}
				/* Rest of controls size to take up horizontal space dynamically.
				 */
				///Label for the ROW
				st = new QLabel(q,"static label" + QString::number(i+page*numRowsPerTab_), Qt::WNoAutoErase);
				qgl->addWidget(st, i+2, 0);  //insert into first position of the ith+2 row
				emit evControlRegistration(rc_->insert(st,page*numRowsPerTab_+i+1,0,
														i,Q_STATICTEXT));
				st->show();
				//log_msg(0, "Text ID:");
				//log_msg(0, (char*)QString::number(i ).ascii() );

				for (j = 0; j < (int)numColumns_; j++) {
					zsl = new qZabelSlider(
								 q ,0,"ZABEL", j + i * numColumns_  );	
					
					qgl->addWidget(zsl, i+2, j+1);  //insert into column j+1, to account for the text
					emit evControlRegistration(
									rc_->insert(zsl, page*numRowsPerTab_+i+1, j+1,
									 j + i * numColumns_, Q_CHECKBOXDELTA));

				}
				
			} //done buiding tab
		} //done building pages
	}//Done with else
	
	//Done with layout

}

void qSelectDialogBase::buildDefaultButtons(){

	horizLayout = new QHBoxLayout(VertLayout);
	
			
	pbHelp_ = new QPushButton( "Help", this);
	pbResetAll_ = new QPushButton( "Reset All", this);
	pbApplyAll_ = new QPushButton( "Apply All", this);
	pbReset_ = new QPushButton(  "Reset", this);
	pbApply_ = new QPushButton(  "Apply", this);
	pbCancel_ = new QPushButton(  "Cancel", this);
	pbOK_ = new QPushButton(  "OK", this);
	
	horizLayout->addStretch(1);
	horizLayout->addWidget(pbHelp_);
	horizLayout->addWidget(pbResetAll_);
	horizLayout->addWidget(pbApplyAll_);	
	horizLayout->addWidget(pbReset_);
	horizLayout->addWidget(pbApply_);	
	horizLayout->addWidget(pbCancel_);	
	horizLayout->addWidget(pbOK_);

	connect( pbOK_, SIGNAL(clicked()), this, SLOT(cbOK()));
	connect( pbApply_, SIGNAL(clicked()), this, SLOT( cbApply()));
	connect( pbReset_, SIGNAL(clicked()), this, SLOT( cbReset()));
	connect( pbApplyAll_, SIGNAL(clicked()), this, SLOT( cbApplyAll()));
	connect( pbResetAll_, SIGNAL(clicked()), this, SLOT( cbResetAll()));
	connect( pbHelp_, SIGNAL(clicked()), this, SLOT( cbHelp()));
	connect( pbCancel_, SIGNAL(clicked()), this, SLOT( cbCancel()));
	
	pbOK_->show();
	pbApply_->show();
	pbReset_->show();
	pbApplyAll_->show();
	pbResetAll_->show();
	pbHelp_->show();
	pbCancel_->show();	

    cbRunStatusChanged();

}

/* Receive notification that run status has changed.
 */

void qSelectDialogBase::cbRunStatusChanged( void ){
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

void qSelectDialogBase::setCheckBox(unsigned box, unsigned checked,
									  unsigned id){
	if(id < numRows_)
		cbgTags_[id * 2 + box] = checked;
}

int qSelectDialogBase::isBoxChecked(unsigned box, unsigned id){
	if(box < numRows_)
		return cbgTags_[id * 2 + box];
	return 0;
}

void qSelectDialogBase::selectComboBox( int iSel, int box ) {
	cmbStatic_[box]->setCurrentItem( iSel );
}

void qSelectDialogBase::addComboBoxText(const char *text, unsigned box){
	cmbStatic_[box]->insertItem(text);
}

QString qSelectDialogBase::getComboBoxText(int box){ 
	return cmbStatic_[box]->currentText(); //text(cmbStatic_[box]->selection());
}

void qSelectDialogBase::hideComboBox( unsigned box ) {
	cmbStatic_[box]->setShown( FALSE );
}

void qSelectDialogBase::cbCancel(){
	emit evCancel(1);
	wasCloseNotified_ = 1; /* Prevent multi-notification. */
	close(true);
	return ;
}
void qSelectDialogBase::cbOK(){
	emit evOK(1);
	wasCloseNotified_ = 1;   /* Prevent multi-notification. */
	close(true);
	return ;
}
void qSelectDialogBase::cbApply(){
	emit evApply(1);
	return ;
}
void qSelectDialogBase::cbReset(){
	emit evReset(1);
	return ;
}
void qSelectDialogBase::cbApplyAll(){
	emit evApplyAll(1);
	return ;
}
void qSelectDialogBase::cbResetAll(){
	emit evResetAll(1);
	return ;
}
void qSelectDialogBase::cbHelp(){
	emit evHelp(1);
	return ;
}

void qSelectDialogBase::cbCmbStatic1(){

	//cmbStatic_[0]->repaint();
   	emit evCmbStatic1(cmbStatic_[0]->currentItem());
	cmbStatic_[0]->repaint();
	cmbStatic_[1]->repaint();
	

}

void qSelectDialogBase::cbCmbStatic2(){

	//cmbStatic_[1]->repaint();
	emit evCmbStatic2(cmbStatic_[1]->currentItem());
	cmbStatic_[0]->repaint();
	cmbStatic_[1]->repaint();
	
		
}

void qSelectDialogBase::cbTabSelected( int newIndex,  int oldIndex){
	//log_msg(0, "Tab selected");
	currentIndex_ = newIndex;
	emit evTabSelected(newIndex);
}

//Handle the layout changes dynamicly

void qSelectDialogBase::paintEvent( QPaintEvent * se)
{
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
	
	if(itr)
		delete itr;
		
	cmbStatic_[0]->repaint();
	cmbStatic_[1]->repaint();
	bpDirty1_->repaint();
	bpDirty2_->repaint();
	bpDirty3_->repaint();
	bpDirty4_->repaint();

	//QDialog::paintEvent(se);
	

}

