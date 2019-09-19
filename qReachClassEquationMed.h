#ifndef _qReachClassEquationMed_h_
#define _qReachClassEquationMed_h_

//#include <help/zhelp.hpp>
#include "qIdcListBase.h"
#include "qDialogMedEquationBase.h"
//#include "smart_ptr.h"

class qReachClassEquationMed : public qDialogMedEquationBase {
   
	Q_OBJECT

  protected:

  public:
    
    qReachClassEquationMed(qIdcEquationBase *idc, 
							qEquationDialogBase *dialog,
                            const char *title);
    virtual ~qReachClassEquationMed();

	//inline int ref(){ return ++ref_; }
	//inline int unref(){ return --ref_; }

	//int operator==(const qReachClassEquationMed &ref){ 
	//	if(&ref == this)
	//		return 1;
	//	return 0;
	//}

	/* How to get the message from the global "purge all IDCs" event. */

  protected slots:
	virtual void cbPurge( void );
	//static void _cbPurge( void *msg );

};

//typedef utSmartPtr<qReachClassEquationMed> spqReachClassEquationMed;

#endif





