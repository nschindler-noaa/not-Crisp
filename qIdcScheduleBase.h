/* idcScheduleBase.h   Base class for interface data controllers which control
 *  a set of lists of values, selected by a single index. The list of values
 *  derives from a PERIOD_LIST structure in the underlying database. There
 *  are two types of possible period lists - 366 double values, one per day,
 *  or 366*24 boolean values, one per hour. The first type is a single-level
 *  list and the second type is a two-level list. The way to tell is by the
 *  length of the data arrays in the data container.
 *  The list is selected by two strings ("feature" and "species", although 
 *  in principle they could be anything).
 */

#ifndef _qIdcScheduleBase_
#define _qIdcScheduleBase_

#include "qInterfaceDataControl.h"
//#include <rw/tvordvec.h>
//#include <rw/cstring.h>
#include "period.h"
#include "qDatContainer.h"

class qIdcScheduleBase : public qInterfaceDataControl {  

protected:

	int iDialogID_;
	QValueVector<QString> listFeature_;
	QValueVector<QString> listSpecies_;

public:

	qIdcScheduleBase(int iDialogID) : iDialogID_(iDialogID) {}

	/* single level list of 366 values */
	virtual int ModifyCache(
		const char *feature,
		const char *species,
		const int index,		// Position in the datContainer array
		const double dValue,    // The value returned 
		const int gFeature,		// if !=0, apply to all features.
		const int gSpecies );	// if !=0, apply to all species.

	/* two level list of 366*24 values */
	virtual int ModifyCache(
		const char *feature,
		const char *species,
		const int index,		// Position in the datContainer array
		const int iValue,		// The value returned (0 or 1). 
		const int gFeature,		// if !=0, apply to all features.
		const int gSpecies );	// if !=0, apply to all species.

	virtual qDatContainer *GetData( const char *feature, const char *species );

	virtual qDatContainer *NewData();
	virtual int FillData(qDatContainer *dat) = 0;

	/* Translations to/from PERIOD_LIST's in underlying database.
	 */
	void getFromList( qDatContainer *dat, PERIOD_LIST *pl );
	virtual void putToList( qDatContainer *dat, PERIOD_LIST **ppl );

	/* Assign various labels to self by consulting database. 
	 */
	virtual void Setup( void );

	void SetDialogID(int id){ iDialogID_ = id; }
	int  GetDialogID(void){ return iDialogID_; }

	/* Interface to the various lists maintained by the base class. 
	 */
	void insertFeature(const char *str){ listFeature_.append(str); }
	void insertSpecies(const char *str){ listSpecies_.append(str); }
	const char *getFeature( int i ) { return listFeature_[i].ascii(); }
	const char *getSpecies( int i ) { return listSpecies_[i].ascii(); }
	int getNumFeature( void ) { return (int)(listFeature_.count()); }
	int getNumSpecies( void ) { return (int)(listSpecies_.count()); }

};

#endif


