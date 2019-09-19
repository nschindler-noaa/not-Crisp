/* qIdcListBase.h   Base class for interface data controllers which control
 *  a single list of values, selected by a single index (they may be
 *  logically organized into rows and columns, in which case all of the first
 *  row is stored, followed by all of the second row, etc - the column
 *  subscript varies fastest, as it were).
 */

#ifndef _qIdcListBase_
#define _qIdcListBase_

#include "qInterfaceDataControl.h"
//#include <rw/tvordvec.h>
//#include <rw/cstring.h>
#include "qDatContainer.h"
#include "qUtils.h"

struct river_segment;

class qIdcListBase : public qInterfaceDataControl {  

protected:

	int iDialogID_;
	//RWTValOrderedVector<RWCString> listRowLabel_;
	//RWTValOrderedVector<RWCString> listColumnLabel_;
	QValueVector<QString> listRowLabel_;
	QValueVector<QString> listColumnLabel_;


public:

	qIdcListBase(int iDialogID) : iDialogID_(iDialogID) {}

	virtual int ModifyCache(
		const int index,		// Position in the qDatContainer array
		const double dValue );    // The value returned 

	virtual qDatContainer *GetData();

	virtual qDatContainer *NewData();
	virtual int FillData(qDatContainer *dat) = 0;

	/* Assign row and column labels to self by consulting database. */
	virtual void Setup( void ) = 0;

	void SetDialogID(int id){ iDialogID_ = id; }
	int  GetDialogID(void){ return iDialogID_; }

	void insertRowLabel(const char *str){ listRowLabel_.append(str); }
	void insertColumnLabel(const char *str){ listColumnLabel_.append(str); }
	const char *getRowLabel( int i ) { return listRowLabel_[i].ascii(); }
	const char *getColumnLabel( int i ) { return listColumnLabel_[i].ascii(); }
	int getNumRow( void ) { return (int)(listRowLabel_.count()); }
	int getNumColumn( void ) { return (int)(listColumnLabel_.count()); }
    void clearLabels( void ) {listRowLabel_.clear(); listColumnLabel_.clear();}

	/* derived classes define their preferred scroll bar range. There
     *  is special provision to make the range different for different
     *  data items. If minVal(i) == maxVal(i), that indicates that this
     *  special provision is not being used. The single index "i" is
     *  ?? the same as the index into the dataContainer ??
	 */
	virtual double minVal( void ) = 0;
	virtual double maxVal( void ) = 0;
    virtual double minValu( int i ) { return 0.0; };
    virtual double maxValu( int i ) { return 0.0; };
};

#endif



