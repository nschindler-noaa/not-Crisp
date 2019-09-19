//#include <zapp.hpp>
#include "qReachClassSDMed.h"
#include "qGlobalEvents.h"

/* Constructor listens to data-file-read event as well as others.
 */
qReachClassSDMed::qReachClassSDMed(qIdcSelectBase *idc, 
						qSelectDialogBase *dialog, const char *title)
	: qSelectDialogMedBase( idc, dialog, title )
{
	connect(qEvents::Instance(), SIGNAL(GEPurgeAllIDCs_()), SLOT(cbPurge()));
	//GEPurgeAllIDCs_.attach(this, &qReachClassSDMed::_cbPurge);
}

/* Destructor un-registers from that event. */
qReachClassSDMed::~qReachClassSDMed()
{
	//GEPurgeAllIDCs_.detach(this, &qReachClassSDMed::_cbPurge);
    unRegisterEvents();
}

/* How to get the message from the global "purge all IDCs" event.
 */
void qReachClassSDMed::cbPurge(void  ){
    dialog_->cbCancel();
}


