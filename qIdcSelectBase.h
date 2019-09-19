/* idcSelectBase.h   Base class for interface data controllers which control
 *  a set of lists of values, selected by a single index (they may be
 *  logically organized into rows and columns, in which case all of the first
 *  row is stored, followed by all of the second row, etc - the column
 *  subscript varies fastest, as it were). The list is selected by two strings
 *  ("feature" and "species", although in principle they could be anything).
 */

#ifndef _idcSelectBase_
#define _idcSelectBase_

#include "qInterfaceDataControl.h"
//#include <rw/tvordvec.h>
//#include <rw/cstring.h>
#include "qDatContainer.h"
#include "qUtils.h"

class qIdcSelectBase : public qInterfaceDataControl {  

protected:

	int iDialogID_;
	QValueVector<QString> listRowLabel_;
	QValueVector<QString> listColumnLabel_;
	QValueVector<QString> listFeature_;
	QValueVector<QString> listSpecies_;

public:

	qIdcSelectBase(int iDialogID) : iDialogID_(iDialogID) {}

	virtual int ModifyCache(
		const char *feature,
		const char *species,
		const int index,		// Position in the datContainer array
		const double dValue,    // The value returned 
		const int gFeature,		// if !=0, apply to all features.
		const int gSpecies );	// if !=0, apply to all species.

	virtual qDatContainer *GetData( const char *feature, const char *species );

	virtual qDatContainer *NewData();
	virtual int FillData(qDatContainer *dat) = 0;

	/* Assign various labels to self by consulting database. 
	 */
	virtual void Setup( void );

	void SetDialogID(int id){ iDialogID_ = id; }
	int  GetDialogID(void){ return iDialogID_; }

	/* Interface to the various lists maintained by the base class. 
	 */
	void insertRowLabel(const char *str){ listRowLabel_.push_back(str); }
	void insertColumnLabel(const char *str){ listColumnLabel_.push_back(str); }
	const char *getRowLabel( int i ) { return listRowLabel_[i].ascii(); }
	const char *getColumnLabel( int i ) { return listColumnLabel_[i].ascii(); }
	void insertFeature(const char *str){ listFeature_.push_back(str); }
	void insertSpecies(const char *str){ listSpecies_.push_back(str); }
	const char *getFeature( int i ) { return listFeature_[i].ascii(); }
	const char *getSpecies( int i ) { return listSpecies_[i].ascii(); }
	int getNumRow( void ) { return (int)(listRowLabel_.count()); }
	int getNumColumn( void ) { return (int)(listColumnLabel_.count()); }
	int getNumFeature( void ) { return (int)(listFeature_.count()); }
	int getNumSpecies( void ) { return (int)(listSpecies_.count()); }
    void clearLists( void ) { listRowLabel_.clear(); listColumnLabel_.clear();
                               listFeature_.clear(); listSpecies_.clear(); }

	/* derived classes define their preferred scroll bar range.
	 */
	virtual double minVal( void ) = 0;
	virtual double maxVal( void ) = 0;
};

#endif


