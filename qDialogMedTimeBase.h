//ZasDialogMedTimeBase
//
//Purpose:
//	This is the base class for "time dialog" mediators.  That is,
//  a dialog with zabel sliders 
//

#ifndef _qDialogMedTimeBase_h_
#define _qDialogMedTimeBase_h_

#include "qDialogMedBase.h"
#include "qDialogBase.h"
#include "qIdcTimeBase.h"

class qDialogMedTimeBase : public qDialogMedBase {
    

	int df_; // default Stock	
	qIdcTimeBase *idcTimeBase_;

  public:
    
    qDialogMedTimeBase(qIdcTimeBase *idc, qDialogBase *dp, char *title);
    virtual ~qDialogMedTimeBase(){}

    virtual void cbApply();
    virtual void cbApplyAll();
	virtual void cbReset();
	void listUpdate();
	void addListItems();
	virtual void cbControlRegistration(qItem *zi);
	virtual void cbEditSliderDoubleModified( double delta, int id);
    
};


#endif

