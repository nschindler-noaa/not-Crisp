#ifndef _qReachClassCBDMed_h_
#define _qReachClassCBDMed_h_

//#include <help/zhelp.hpp>
#include "qIdcListBase.h"
#include "qCheckBoxDialogMedBase.h"
//#include "smart_ptr.h"

class qReachClassCBDMed : public qCheckBoxDialogMedBase {
	
	Q_OBJECT
    
  protected:

  public:
    
    qReachClassCBDMed(qIdcListBase *idc, 
							qCheckBoxDialogBase *dialog,
                            const char *title);
    virtual ~qReachClassCBDMed();

	//inline int ref(){ return ++ref_; }
	//inline int unref(){ return --ref_; }

	////int operator==(const qReachClassCBDMed &ref){ 
	//	if(&ref == this)
	//		return 1;
    //		return 0;
	//}
		/* This virtual routine is called when a box is checked or unchecked. 
	 *  It handles any interactions with other check boxes.
     *  Each row is treated as a radio button group
	 *  Return 1 if the user's choice should be ignored
	 */
	virtual int doBoxInteractions( int row, int column, bool check );

	/* How to get the message from the global "purge all IDCs" event. */
  protected slots:
	virtual void cbPurge( void );
	//static void _cbPurge( void *msg );



};

//typedef utSmartPtr<qReachClassCBDMed> spqReachClassCBDMed;

#endif





