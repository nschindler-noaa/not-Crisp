#ifndef _qDamOutputCBDMedBase_h_
#define _qDamOutputCBDMedBase_h_


#include "qIdcListBase.h"
#include "qCheckBoxDialogMedBase.h"


class qDamOutputCBDMedBase : public qCheckBoxDialogMedBase {
    
  protected:

  public:
    
    qDamOutputCBDMedBase(qIdcListBase *idc, 
							qCheckBoxDialogBase *dialog, const char *title)
		: qCheckBoxDialogMedBase( idc, dialog, title ) {};
    virtual ~qDamOutputCBDMedBase() {};
	

	/* This virtual routine is called when a box is checked or unchecked. 
	 *  It handles any interactions with other check boxes. The default, 
	 *  base class behavior assumes that each row is a radio button 
	 *  group. Derive a class to override this behavior.
	 *  Return 1 if the user's choice should be ignored
	 */
	virtual int doBoxInteractions( int row, int column, bool check );

};


#endif





