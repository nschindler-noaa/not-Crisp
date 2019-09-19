/* qEditGraphMediator.cpp
 *  Main mediator for graphs which contain a single, editable line of data.
 *  Editing is by left-button drag-draw or by opening a "Schedule Tool"
 *  dialog.
 */

#include "qEditGraphMediator.h"
#include "qGlobalEvents.h"
#include "qScheduleTool.h"
#include <qwidget.h>

/* Constructor registers base and derived class events. Also maintains
 *  a pointer to the qEditGraphDataToken (derived-class) object. Destructor
 *  detaches from events. NOTE that we do not need to handle base-class
 *  events here.
 */
qEditGraphMediator::qEditGraphMediator(qGraphDialog *gd, 
										   qEditGraphDataToken *edt) :
					qGraphMediator( gd, edt ) {
	edt_ = edt;
	sched_ = NULL;
	RegisterEvents();
}

qEditGraphMediator::~qEditGraphMediator() {
	DetachEvents();
	if (sched_ != NULL)
		delete sched_;
}

void
qEditGraphMediator::RegisterEvents(void){
	connect(_gd, SIGNAL(evOK_()), this, SLOT(cbOK()));
	connect(_gd, SIGNAL(evCancel_()), this, SLOT(cbCancel()));
	connect(_gd, SIGNAL(evApply_()), this, SLOT(cbApply()));
	connect(_gd, SIGNAL(evReset_()), this, SLOT(cbReset()));
	connect(_gd, SIGNAL(evApplyAll_()), this, SLOT(cbApplyAll()));
	connect(_gd, SIGNAL(evResetAll_()), this, SLOT(cbResetAll()));
	connect(_gd, SIGNAL(evSchedule_()), this, SLOT(cbSchedule()));
	connect(edt_, SIGNAL(evInvalidate_()), this, SLOT(cbInvalidate()));
	disconnect(qEvents::Instance(), SIGNAL(GEUpdateAllDataTokens_()), this , SLOT(cbReload()));


/*   _gd->evOK()->attach(this, &qEditGraphMediator::_cbOK);
   _gd->evCancel()->attach(this, &qEditGraphMediator::_cbCancel);
   _gd->evApply()->attach(this, &qEditGraphMediator::_cbApply);
   _gd->evReset()->attach(this, &qEditGraphMediator::_cbReset);
   _gd->evApplyAll()->attach(this, &qEditGraphMediator::_cbApplyAll);
   _gd->evResetAll()->attach(this, &qEditGraphMediator::_cbResetAll);
   _gd->evSchedule()->attach(this, &qEditGraphMediator::_cbSchedule);

   edt_->evInvalidate()->attach(this, &qEditGraphMediator::_cbInvalidate);

   // De-attach the "update all GDTs" event - our EGDT invalidation
   //  handles this instead.
	GEUpdateAllDataTokens_.detach(this, &qGraphMediator::_cbReload);
	*/
}

void
qEditGraphMediator::DetachEvents(void){
	/*
   _gd->evOK()->detach(this, &qEditGraphMediator::_cbOK);
   _gd->evCancel()->detach(this, &qEditGraphMediator::_cbCancel);
   _gd->evApply()->detach(this, &qEditGraphMediator::_cbApply);
   _gd->evReset()->detach(this, &qEditGraphMediator::_cbReset);
   _gd->evApplyAll()->detach(this, &qEditGraphMediator::_cbApplyAll);
   _gd->evResetAll()->detach(this, &qEditGraphMediator::_cbResetAll);
   _gd->evSchedule()->attach(this, &qEditGraphMediator::_cbSchedule);

   edt_->evInvalidate()->detach(this, &qEditGraphMediator::_cbInvalidate);

   if (sched_ != NULL)
		sched_->evRemove()->detach( this, &qEditGraphMediator::_cbSchedToolRemove );
*/		
}


/* Event methods: get notified of button presses in the dialog box, or
 *  of invalidation events from the underlying InterfaceDataController.
 */

void qEditGraphMediator::cbOK(){
	/* Apply current chunk of data from IDC cache. */
	edt_->Apply();
}
/*
void qEditGraphMediator::_cbOK(void *msg){
    qEditGraphMediator *zeg = (qEditGraphMediator *)msg;
    zeg->cbOK();
}
*/
void qEditGraphMediator::cbCancel(){
	cbReset();  /* Discard current chunk of data from IDC cache. */
}
/*
void qEditGraphMediator::_cbCancel(void *msg){
    qEditGraphMediator *zeg = (qEditGraphMediator *)msg;
    zeg->cbCancel();
}
*/
void qEditGraphMediator::cbApply(){
	edt_->Apply();
}
/*
void qEditGraphMediator::_cbApply(void *msg){
    qEditGraphMediator *zeg = (qEditGraphMediator *)msg;
    zeg->cbApply();
}
*/
void qEditGraphMediator::cbReset(){
	edt_->Reset();
}
/*
void qEditGraphMediator::_cbReset(void *msg){
    qEditGraphMediator *zeg = (qEditGraphMediator *)msg;
    zeg->cbReset();
}
*/
void qEditGraphMediator::cbApplyAll(){
	edt_->ApplyAll();
}
/*
void qEditGraphMediator::_cbApplyAll(void *msg){
    qEditGraphMediator *zeg = (qEditGraphMediator *)msg;
    zeg->cbApplyAll();
}
*/
void qEditGraphMediator::cbResetAll(){
	edt_->ResetAll();
}
/*
void qEditGraphMediator::_cbResetAll(void *msg){
    qEditGraphMediator *zeg = (qEditGraphMediator *)msg;
    zeg->cbResetAll();
}
*/
void qEditGraphMediator::cbSchedule(){
	if (sched_ != NULL)
		sched_->show();
	else {
		sched_ = new qScheduleTool( (QWidget*)_gd->parent(), edt_ );
		connect(sched_, SIGNAL(evRemove()), SLOT(cbSchedToolRemove()));
		//sched_->evRemove()->attach( this,
		//	               &qEditGraphMediator::_cbSchedToolRemove );
	}
}
/*
void qEditGraphMediator::_cbSchedule(void *msg){
    qEditGraphMediator *zeg = (qEditGraphMediator *)msg;
    zeg->cbSchedule();
}
*/
void qEditGraphMediator::cbInvalidate(){
	_gp->repaint();
	_gd->scaleUpdated();
}
/*
void qEditGraphMediator::_cbInvalidate(void *msg){
    qEditGraphMediator *zeg = (qEditGraphMediator *)msg;
    zeg->cbInvalidate();
}
*/
void qEditGraphMediator::cbSchedToolRemove(){
	sched_ = NULL;
}
/*
void qEditGraphMediator::_cbSchedToolRemove(void *msg){
    qEditGraphMediator *zeg = (qEditGraphMediator *)msg;
    zeg->cbSchedToolRemove();
}
*/
