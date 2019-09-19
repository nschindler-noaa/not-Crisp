#ifndef _qIdcTimeBase_
#define _qIdcTimeBase_

#include "qInterfaceDataControl.h"
//#include <rw/tvordvec.h>
//#include <rw/cstring.h>
#include "qDatContainer.h"
#include "qUtils.h"

struct river_segment;

class qIdcTimeBase : public qInterfaceDataControl {  

protected:

	int iDialogID_;
	//RWTValOrderedVector<RWCString> listFeature_;
	//RWTValOrderedVector<RWCString> listSpecies_;
	QValueVector<QString> listFeature_;
	QValueVector<QString> listSpecies_;

public:

	qIdcTimeBase(int iDialogID) : iDialogID_(iDialogID) {}

	virtual int ModifyCache(
		const int index,		// Position in the qDatContainer array
		const int flagFeature,	// (Ex: If the D check box is on, modify for all dams)
		const int flagSpecies,  // (Ex: If the S check box is on, modify for all species)
		const double dValue,    // The value returned 
		const char *strFeature, // Name of the dam
		const char *strSpecies, // Name of the species
		const int isDelta);		// If we're in Delta mode, dValue is the amount changed.

	virtual qDatContainer *GetData(const char *strFeature, const char *strSpecies);

	virtual qDatContainer *NewData() = 0;
	virtual int FillData(qDatContainer *dat) = 0;

	void SetDialogID(int id){ iDialogID_ = id; }
	int  GetDialogID(void){ return iDialogID_; }

	void InsertFeature(const char *str){ listFeature_.append(str); }
	void InsertSpecies(const char *str){ listSpecies_.append(str); }
	int getNumFeature( void ) { return (int)(listFeature_.count()); }

	/* derived classes define their preferred scroll bar range.
	 */
	virtual double minVal( void ) = 0;
	virtual double maxVal( void ) = 0;
};

#endif


