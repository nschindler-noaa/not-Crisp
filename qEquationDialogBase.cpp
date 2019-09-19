
#include "qDefines.h"
#include "qRowColumnBox.h"
#include "qZabelSlider.h"
#include "qGraphPane.h"
#include "qEquationDialogBase.h"
#include "log.h"

qEquationDialogBase::qEquationDialogBase(QWidget *w,int dw, const char *nt)
					: qDialogBase(w,dw,nt) {
    dividerParam_ = -1;
	dividerWidget = NULL;
    spacers_[0] = NULL;
	stPositionX_ = NULL;
	stPositionY_ = NULL;
	stDateX_ = NULL;

}

qEquationDialogBase::~qEquationDialogBase(){ 

}

void qEquationDialogBase::setDividerParam( int i ) {
    /* This routine sets the divider between the two types of parameters
     *  and causes the space to open up properly.
     */
    if (dividerParam_ == i)
        return;

    dividerParam_ = i;

	if(dividerWidget == NULL ){
		dividerWidget = new QFrame(this);
		dividerWidget->setFrameStyle(QFrame::HLine | QFrame::Raised);
		dividerWidget->setLineWidth( 2 );		
	}

	//First remove it
	gridl->remove ( dividerWidget );
	
	//Now reinsert it, the odd rows hold spaces for divider widgets
	//Divider param is the row we want to insert it after
	if(dividerParam_ >= 0){		
		gridl->addMultiCellWidget(dividerWidget, dividerParam_*2+1, dividerParam_*2+1, 
										0,gridl->numCols() , 0);
		dividerWidget->setShown(TRUE);
	}
	else{
		dividerWidget->setShown(FALSE);
	}
    
}
	
int qEquationDialogBase::drawDialog(unsigned topMargin){
	
	if(topMargin)topMargin_ = topMargin;
	
	
	buildDefaultButtons();

	return 1;
}

void qEquationDialogBase::drawStandardLayout(int sliderType){

	qZabelSlider *zsl;

	QLabel *st;

	/* Put in the three edit fields that will show cursor position.
	 */

	QHBoxLayout* newRow = new QHBoxLayout(this, 5, -1, "coordinate text rows");
	vertLayout->insertLayout ( 1, newRow );
	stPositionX_ = new QLabel(this);
	newRow->addWidget(stPositionX_); //add to the new row

	stPositionY_ = new QLabel(this);
	
	newRow->addWidget(stPositionY_); //add to the new row

	stDateX_ = new QLabel( this);
	newRow->addWidget(stDateX_); //add to the new row

	stDateX_->setShown( false );  /* mediator must reveal this if necessary */


	/* Now put the slider controls at the bottom of the dialog. NOTE:
     *  dividerParam_ is ignored if it equals -1; otherwise, it is the
     *  slider after which to insert 1/2 a row height of empty space.
	 */
	buildRowColumnControl();
	if(!rc_)
		return;
	
	
	gridl = new QGridLayout ( this, 2*numItems_, 2, 4, -1, "Sliders Grid" );
	vertLayout->insertLayout ( 2, gridl );
	gridl->setColStretch(1,QSizePolicy::Horizontally);
	
	int i;
	//We make the grid twice as big as it needs to be
	//to make space for the horizontal spacer bars
	for(i = 0;(unsigned)i<numItems_;i++){
		
		st = new QLabel("", this);
		//Insert items in thier row*2, to allow spaces for dividers
		gridl->addWidget(st, i*2, 0,Qt::AlignLeft);
		st->show();
		
		emit evControlRegistration_(rc_->insert(st,i+1,0,i,Q_STATICTEXT));
		
		zsl = new qZabelSlider(this,0,"",i);
		gridl->addWidget(zsl,i*2,1);
		zsl->show();
	
		emit evControlRegistration_(rc_->insert(zsl,i+1,0,i,Q_CHECKBOXDELTA));
		
        /* These windows sit between the visible sliders. At most one
         *  is visible at any given time, looking like a horizontal line
         *  to divide the two classes of parameters.
         */
       
	}

	//Add the divider line to the grid of widgets
	if(dividerParam_ >= 0){	
		QFrame* dividerWidget = new QFrame(this);
		dividerWidget->setFrameStyle(QFrame::HLine | QFrame::Raised);
		dividerWidget->setLineWidth( 2 );
		gridl->addMultiCellWidget(dividerWidget, dividerParam_*2+1, dividerParam_*2+1, 
										0,gridl->numCols() , 0);
	}

	/* The graphics pane gets whatever was left over in the middle.
		The pane in inserted into a frame widget, to make a black border
	 */
	QFrame * frame = new QFrame(this);
	frame->setFrameStyle( QFrame::GroupBoxPanel | QFrame::Plain );
	frame->setLineWidth ( 1 );
	
	//this layout allows the frame to have its black border
	QHBoxLayout * boxLayout  = new QHBoxLayout(frame,1,1);
		
	//add to layout manager
	vertLayout->insertWidget(2, frame, 1);

	gp_ = new qGraphPane(frame);
	boxLayout->addWidget(gp_);

	//vertLayout->insertWidget ( 2, gp_,1 );
	resize(QSize(550,660));
	show();
}


int
qEquationDialogBase::setPositionYLabel(const char *pstr){
	stPositionY_->setText(pstr);
	stPositionY_->repaint();
	return 0;
}

int
qEquationDialogBase::setPositionXLabel(const char *pstr){
	stPositionX_->setText(pstr);
	stPositionX_->repaint();
	return 0;
}

int
qEquationDialogBase::setPositionDateLabel(const char *pstr){
	stDateX_->setText(pstr);

	stDateX_->repaint();
	return 0;
}

void
qEquationDialogBase::showPositionDateLabel(const bool iShow){
	stDateX_->setShown(iShow);
}

