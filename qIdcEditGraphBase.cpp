/*
 * qIdcEditGraphBase.cpp
 * Base class for interface data controllers for editable graphs
 *  (generally based on 366 data values).
 */

#include "qIdcEditGraphBase.h"
#include "crisp_defs.h"
#include "log.h"
#include  <stdlib.h>
/* 
 * A datContainer for the given data is created or retrieved.
 *  Be sure to check for NULL returned.
 */
qDatContainer *qIdcEditGraphBase::GetData(const char *id){

	qDatContainer *target;
	int index;
	target = NewData();
	if(!target)
		return NULL;

	target->setString(0,id);
//	char * buf  = new char[40];
//	itoa(bt.count(), buf, 10);

//	delete buf;

		/* If we already have a datContainer for this id, return it. */
	index = bt.find(target);


	if(index != -1){
		
		delete target;
		qDatContainer* dc = bt.at(index);
		return dc;
	}

	/* Else go get the data, and add it to the cache, and return it. */
	if(FillData(target)){
		bt.inSort(target);
		return target;
	}

	/* Failure, somehow. */
	delete target;
	return NULL;
}

/*
 * Called to modify cached data. A single char string
 *  identifies the datContainer. "data" is stored at "index".
 *  Returns 1 for success, 0 for failure.
 */
int qIdcEditGraphBase::ModifyCache(
	const char *id,         //Name of the dam, reach, etc
	const int index,		//position in the datContainer array
	const double data    //The value to be inserted. 
){
	
	qDatContainer *target =NULL; 
	qDatContainer *dat = NULL;

	if(!(dat = GetData(id))) 
		return 0;
	dat->setDouble(index,data);
	return 1;
}

	/*
	 * Allocates a new data container of the type needed. In this
	 *  case, one string for ID, no int vector, 366 double values.
	 */
qDatContainer *qIdcEditGraphBase::NewData() {
	return new qDatContainer( DAYS_IN_SEASON, 1, 0 );
}

