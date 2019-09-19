/* qIdcListBase.cpp   Base class for interface data controllers which control
 *  a single list of values, selected by a single index (they may be
 *  logically organized into rows and columns, in which case all of the first
 *  row is stored, followed by all of the second row, etc - the column
 *  subscript varies fastest, as it were).
 */

#include "qIdcListBase.h"
#include "crisp.h"

qDatContainer *qIdcListBase::NewData() {
	// Return a single array of double values.
	return new qDatContainer( getNumRow() * getNumColumn(), 2, 0 );
}

qDatContainer *qIdcListBase::GetData(){

	qDatContainer *target;
//	RWCollectable *rc;

	target = NewData();
	if(!target)
		return NULL;

	target->setString(0,"all");
	target->setString(1,"all");

	int index = bt.find(target);
	if(index != -1){
		delete target;
		return bt.at(index);
	}

	if(FillData(target)){
		bt.inSort(target);
		return target;
	}
	return NULL;
}

int qIdcListBase::ModifyCache(const int index,		//position in the qDatContainer array
	const double dValue)   // the value returned.
{
	qDatContainer *dat = NULL;

	if(!(dat = GetData())) 
		return 0;

	dat->setDouble(index,dValue,0);

	//evInvalidate_.notify();
	emit evInvalidate_();

	return 1;
}
