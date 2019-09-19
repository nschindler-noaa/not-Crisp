/*
 * idcEditGraphBase.h
 * Base class for interface data controllers for editable graphs
 *  (generally based on 366 data values).
 */

#ifndef __QIDCEDITGRAPHBASE_H__
#define __QIDCEDITGRAPHBASE_H__

#include "qInterfaceDataControl.h"

class qIdcEditGraphBase : public qInterfaceDataControl {
public:
	/*
	 * Called to modify cached data. A single char string
	 *  identifies the datContainer. "data" is stored at "index".
	 *  Returns 1 for success, 0 for failure.
	 */
	virtual int ModifyCache( const char *id, const int index,
		                      const double data );

	/* 
	 * A datContainer for the given data is created or retrieved.
	 *  Be sure to check for NULL returned.
	 */
	virtual qDatContainer *GetData(const char *id);

	/*
	 * Allocates a new data container of the type needed. In this
	 *  case, one string for ID, no int vector, 366 double values.
	 */
	virtual qDatContainer *NewData();
};

#endif

