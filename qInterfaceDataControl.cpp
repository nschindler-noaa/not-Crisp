#include "qInterfaceDataControl.h"
#include "qGlobalEvents.h"
#include <qptrlist.h>
#include "log.h"
/*
 * This is the abstract base class for interface data controllers. IDCs
 *  access data of a given type from the underlying data base. IDCs are
 *  static objects (one per type of data) which may be accessed by several
 *  mediators at once. IDCs maintain caches of edited data; a seperate
 *  operation is needed to commit the cached data to the underlying data base,
 *  or conversely to discard the cached data without changing the underlying
 *  data base. In addition, other objects can be notified when the cached
 *  data changes (this allows multiple dialogs to display the same data, with
 *  live updating).
 */



//applies all datacaches to databes
void qInterfaceDataControl::ApplyCache( void ){

	
	qDatContainer *dat; 
	QPtrListIterator <qDatContainer> itr =  QPtrListIterator <qDatContainer>(bt);
	while((dat = itr.current()) != 0 ){
		ApplyData(dat);
		++(itr);
	}
}

//applies given data container to database
void qInterfaceDataControl::ApplyData( qDatContainer *dat ){
	// Base class action merely notes that output side data is no longer valid
	int i;
    for (i = 0; i < nGOutputsValid_; i++)
        GOutputsValid_[i] = 0;
	qEvents::Instance()->emitGEUpdateAllDataTokens_();
}

//clears out all datacontainers in the cache
void qInterfaceDataControl::ClearCache( void ){ 
	qDatContainer *dat; 
	QPtrListIterator <qDatContainer> itr =  QPtrListIterator <qDatContainer>(bt);
	while((dat = itr.current()) != 0 ){
		delete dat;
		++(itr);
	}
	bt.clear(); //clear entire list
    invalidate();
}



void qInterfaceDataControl::ClearCache( qDatContainer *dat ){
	// NOTE "dat" will be a pointer to something in the collection.
	//  We must delete it to prevent leakage.
	bt.remove(dat); //clear will delete this item from list
	delete dat;
	invalidate();
}

//notifies all data containers to purge themselves (clear cache)
qInterfaceDataControl::qInterfaceDataControl( void ) {
	helpPage_ = 1;
	lock = 1; //set unlocked
	connect(qEvents::Instance(), SIGNAL(GEPurgeAllIDCs_()), this, SLOT(cbPurge()));

	//GEPurgeAllIDCs_.attach(this, &qInterfaceDataControl::_cbPurge);
};


qInterfaceDataControl::~qInterfaceDataControl() {
	qDatContainer *dat; 
	QPtrListIterator <qDatContainer> itr =  QPtrListIterator <qDatContainer>(bt);
	while((dat = itr.current()) != 0 ){
		delete dat;
		++(itr);
	}
	bt.clear(); //clear entire list
	//GEPurgeAllIDCs_.detach(this, &qInterfaceDataControl::_cbPurge);
};

/* How to get the message from the global "purge all IDCs" event.
 */
void qInterfaceDataControl::cbPurge( void ){
	ClearCache();
}
/*
void qInterfaceDataControl::_cbPurge(void *msg){
    qInterfaceDataControl *idc = (qInterfaceDataControl *)msg; 
    idc->cbPurge();
}
*/

/* New stochastic parameter support utility functions */
int qInterfaceDataControl::FillBrokenStickData(qDatContainer *dat, int offset, float *variable) {
	variable_parameter *vp = get_variable_parameter(variable);

	if (vp && vp->type == DIST_BROKEN_STICK) {
		dat->setDouble( offset, vp->parameters.broken_stick.mean );
		dat->setDouble( offset+1, vp->parameters.broken_stick.low );
		dat->setDouble( offset+2, vp->parameters.broken_stick.high );
	}
	else {
		dat->setDouble( offset, *variable );
		dat->setDouble( offset+1, 0.0 );
		dat->setDouble( offset+2, 0.0 );
	}

	return 1;
}

void qInterfaceDataControl::ApplyBrokenStickData(qDatContainer *dat, int offset, float *variable, variable_parameter **dist) {
	variable_parameter *vp = get_variable_parameter(variable);

	const float mean = (float)(dat->getDouble( offset ));
	const float low = (float)(dat->getDouble( offset+1 ));
	const float high = (float)(dat->getDouble( offset+2 ));

	/* If low = high = 0.0, assume that the parameter is meant to be constant.
	 * Eventually, a check box in the GUI or something should make this more
	 * clear, but we're trying to preserve the GUI as much as possible at
	 * the moment. */
	if (low == 0 && high == 0) {
		if (vp) {
			remove_variable_parameter(vp);
			vp = NULL;
		}

		/* Assign to the mean */
		*variable = mean;
	}
	else {
		if (vp) {
			if (vp->type != DIST_BROKEN_STICK) {
				vp->type = DIST_BROKEN_STICK;
			}
			vp->parameters.broken_stick.mean =	mean;
			vp->parameters.broken_stick.low =	low;
			vp->parameters.broken_stick.high =	high;
		}
		else {
			vp = add_broken_stick_parameter(variable, mean, low, high);
		}
	}

	/* Assign distribution pointer if necessary */
	if (dist) *dist = vp;
}
