/* qReleaseEGM.cpp
 *  Main mediator for graphs which edit the release schedule in a "release"
 *  structure. Derived from qEditGraphMediator with, hopefully, minimal
 *  changes.
 *  Editing is by left-button drag-draw or by opening a "Schedule Tool"
 *  dialog.
 */

#include "qReleaseEGM.h"
#include "qReleaseTool.h"
#include "qGraphMedDrawLines.h"
#include "qReleaseMedDrawData.h"
#include "qGraphMedZoom.h"
#include "qGraphMedPosition.h"
#include "qGraphMedDrawAxisYD.h"
#include "qGraphMedDrawAxisXI.h"

/* Constructor registers base and derived class events. Also maintains
 *  a pointer to the qEditGraphDataToken (derived-class) object. Destructor
 *  detaches from events. NOTE that we do not need to handle base-class
 *  events here.
 */
qReleaseEGM::qReleaseEGM(qGraphDialog *gd, qReleaseEGDT *relEGDT,
								qReleaseTool *relTool) :
					qEditGraphMediator( gd, relEGDT ) {

	qGraphPane *gp;

	relEGDT_ = relEGDT;
	sched_ = NULL;
	relTool_ = relTool;	
	gdClosing_ = 0;

	/* Set up my own sub-mediators for a change.
	 */
	gp = gd->getGraphPane();
	gp->drawMethod( CITYSCAPE );
	insertSubMediator(new qGraphMedDrawLines(gp));
	insertSubMediator(new qGraphMedDrawAxisYD(gp));
	insertSubMediator(new qGraphMedDrawAxisXI(gp));
	insertSubMediator(new qGraphMedZoom(gp, gd));
	insertSubMediator(new qGraphMedPosition(gp,gd));
	insertSubMediator(new qReleaseMedDrawData(gp,relEGDT));

	RegisterEvents();
}

qReleaseEGM::~qReleaseEGM() {
//	DetachEvents();
	if (sched_ != NULL)
		delete sched_;
}

void
qReleaseEGM::RegisterEvents(void){
	/*
   _gd->evOK()->attach(this, &qReleaseEGM::_cbOK);
   _gd->evCancel()->attach(this, &qReleaseEGM::_cbCancel);
   _gd->evApply()->attach(this, &qReleaseEGM::_cbApply);
   _gd->evReset()->attach(this, &qReleaseEGM::_cbReset);
   _gd->evApplyAll()->attach(this, &qReleaseEGM::_cbApplyAll);
   _gd->evResetAll()->attach(this, &qReleaseEGM::_cbResetAll);
   _gd->evSchedule()->attach(this, &qReleaseEGM::_cbSchedule);

   relTool_->evRemove()->attach( this, &qReleaseEGM::_cbReltoolRemove );

   relEGDT_->evInvalidate()->attach(this, &qReleaseEGM::_cbInvalidate);
   relEGDT_->evFilled()->attach(this, &qReleaseEGM::_cbInvalidate);
   */
	connect( _gd, SIGNAL(evOK_()), this, SLOT(cbOK()));
	connect( _gd, SIGNAL(evCancel_()), this, SLOT(cbCancel()));
	connect( _gd, SIGNAL(evApply_()), this, SLOT(cbApply()));
	connect( _gd, SIGNAL(evReset_()), this, SLOT(cbReset()));
	connect( _gd, SIGNAL(evApplyAll_()), this, SLOT(cbApplyAll()));
	connect( _gd, SIGNAL(evResetAll_()), this, SLOT(cbResetAll()));
	connect( _gd, SIGNAL(evSchedule_()), this, SLOT(cbSchedule()));
	
	connect(relTool_, SIGNAL(evRemove()), this , SLOT(cbReltoolRemove()));

	connect(relEGDT_, SIGNAL(evInvalidate_()), this, SLOT(cbInvalidate()));
	connect(relEGDT_, SIGNAL(evFilled()),this, SLOT(cbInvalidate()));

}

void
qReleaseEGM::DetachEvents(void){
	/*
   _gd->evOK()->detach(this, &qReleaseEGM::_cbOK);
   _gd->evCancel()->detach(this, &qReleaseEGM::_cbCancel);
   _gd->evApply()->detach(this, &qReleaseEGM::_cbApply);
   _gd->evReset()->detach(this, &qReleaseEGM::_cbReset);
   _gd->evApplyAll()->detach(this, &qReleaseEGM::_cbApplyAll);
   _gd->evResetAll()->detach(this, &qReleaseEGM::_cbResetAll);
   _gd->evSchedule()->detach(this, &qReleaseEGM::_cbSchedule);

   relTool_->evRemove()->detach( this, &qReleaseEGM::_cbReltoolRemove );

   relEGDT_->evInvalidate()->detach(this, &qReleaseEGM::_cbInvalidate);
   relEGDT_->evFilled()->detach(this, &qReleaseEGM::_cbInvalidate);

   if (sched_ != NULL)
	   disconnect(sched_, SIGNAL(evRemove()), this, SLOT(cbSchedToolRemove()));
		//sched_->evRemove()->detach( this, &qReleaseEGM::_cbSchedToolRemove );
		*/
}


/* Event methods: get notified of button presses in the dialog box, or
 *  of invalidation events from the underlying InterfaceDataController.
 */

void qReleaseEGM::cbOK(){
	gdClosing_ = 1;
	relTool_->cbOK( );
}

void qReleaseEGM::cbCancel(){
	gdClosing_ = 1;
	relTool_->cbCancel(  );
}

void qReleaseEGM::cbReltoolRemove(){
	if (!gdClosing_) {
		gdClosing_ = 1;
		delete _gd;
	}
}


void qReleaseEGM::cbApply(){
	relTool_->cbApply(  );
}

void qReleaseEGM::cbReset(){
	relTool_->cbReset(  );
}


void qReleaseEGM::cbApplyAll(){
	relTool_->cbApplyAll(  );


}


void qReleaseEGM::cbResetAll(){
	relTool_->cbResetAll( );
}

void qReleaseEGM::cbSchedule(){
	if (sched_ != NULL)
		sched_->setShown(TRUE);
	else {
		sched_ = new qScheduleTool( _gd, edt_ );
		connect( sched_, SIGNAL( evRemove()), this, SLOT(cbSchedToolRemove()));
		//sched_->evRemove()->attach( this,
		//	               &qReleaseEGM::_cbSchedToolRemove );
	}
	
}

void qReleaseEGM::cbInvalidate(){
	_gp->repaint();
}

void qReleaseEGM::cbSchedToolRemove(){
	sched_ = NULL;
}


