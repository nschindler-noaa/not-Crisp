
#include "qCheckBoxGroup.h"
#include "qReachClassCBDMed.h"
#include "qGlobalEvents.h"

/* Constructor listens to data-file-read event as well as others.
 */
qReachClassCBDMed::qReachClassCBDMed(qIdcListBase *idc, 
						qCheckBoxDialogBase *dialog, const char *title)
	: qCheckBoxDialogMedBase( idc, dialog, title )
{

	if( !connect( qEvents::Instance(), SIGNAL(GEPurgeAllIDCs_()  ), this, SLOT(cbPurge()) ) ){
		log_msg(0,"problem connecting signal in qReachClassCBDMed");	
	}
}

/* Destructor un-registers from that event. */
qReachClassCBDMed::~qReachClassCBDMed()
{	
}

/* How to get the message from the global "purge all IDCs" event.
 */
void qReachClassCBDMed::cbPurge( void ){
    dialog_->cbCancel();
}

/* This virtual routine is called when a box is checked or unchecked. It handles
 *  any interactions with other check boxes. Each row is treated as
 *  a radio box group. The first row is full of "all" buttons.
 */
int qReachClassCBDMed::doBoxInteractions( int row, int column, bool check ) {
	qDatContainer *dat;
	int i, j, nc, nr;

	// If the user unchecks a box, default to the first column for the default reach class
	if (row != 0 && !check) {
		if (column != 0) {
			column = 0;
			check = 1;
		}
		else return 1; // Unchecking the first column makes no sense, so don't allow it
	}

	nr = idcListBase_->getNumRow();
	nc = idcListBase_->getNumColumn();
	dat = idcListBase_->GetData();

	for (j = 0; j < nc; j++)
        dat->setDouble( row * nc + j, j == column ? 1 : 0 );

    if (row == 0) {
    	/* Checked box is in the "all" row. */
		for (i = 1; i < nr; i++)
        	for (j = 0; j < nc; j++)
			    dat->setDouble( i * nc + j, j == column ? 1 : 0 );
    }

	return 0;
}

