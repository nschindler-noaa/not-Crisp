
#include "qReachClassEquationMed.h"
#include "qGlobalEvents.h"

/* Constructor listens to data-file-read event as well as others.
 */
qReachClassEquationMed::qReachClassEquationMed(qIdcEquationBase *idc, 
						qEquationDialogBase *dialog, const char *title)
	: qDialogMedEquationBase( idc, dialog, title )
{
	connect(qEvents::Instance(), SIGNAL(GEPurgeAllIDCs_()),this, SLOT(cbPurge()));
	//GEPurgeAllIDCs_.attach(this, &qReachClassEquationMed::_cbPurge);
}

/* Destructor un-registers from that event. */
qReachClassEquationMed::~qReachClassEquationMed()
{
	//GEPurgeAllIDCs_.detach(this, &qReachClassEquationMed::_cbPurge);
    unRegisterEvents();
}

/* How to get the message from the global "purge all IDCs" event.
 */
void qReachClassEquationMed::cbPurge( void ){
    dialog_->callCancel();
}
/*
void qReachClassEquationMed::_cbPurge(void *msg){
    qReachClassEquationMed *me = (qReachClassEquationMed *)msg; 
    me->cbPurge();
}
*/
