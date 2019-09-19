#ifndef _qReachOutputCBDMedBase_h_
#define _qReachOutputCBDMedBase_h_

//#include <help/zhelp.hpp>
#include "qIdcListBase.h"
#include "qCheckBoxDialogMedBase.h"
//#include "smart_ptr.h"

class qReachOutputCBDMedBase : public qCheckBoxDialogMedBase {
    
  protected:

  public:
    
    qReachOutputCBDMedBase(qIdcListBase *idc, 
							qCheckBoxDialogBase *dialog, const char *title)
		: qCheckBoxDialogMedBase( idc, dialog, title ) {};
    virtual ~qReachOutputCBDMedBase() {};
/*
	inline int ref(){ return ++ref_; }
	inline int unref(){ return --ref_; }

	int operator==(const qReachOutputCBDMedBase &ref){ 
		if(&ref == this)
			return 1;
		return 0;
	}
*/
	/* This virtual routine is called when a box is checked or unchecked. 
	 *  It handles any interactions with other check boxes. The default, 
	 *  base class behavior assumes that each row is a radio button 
	 *  group. Derive a class to override this behavior.
	 *  Return 1 if the user's choice should be ignored
	 */
	virtual int doBoxInteractions( int row, int column, bool check );

};

//typedef utSmartPtr<qReachOutputCBDMedBase> spqReachOutputCBDMedBase;

#endif





