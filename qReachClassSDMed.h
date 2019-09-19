#ifndef _qReachClassSDMed_h_
#define _qReachClassSDMed_h_

//#include <help/zhelp.hpp>
#include "qIdcSelectBase.h"
#include "qSelectDialogMedBase.h"


class qReachClassSDMed : public qSelectDialogMedBase {
    
  public:
    
    qReachClassSDMed(qIdcSelectBase *idc, 
							qSelectDialogBase *dialog,
                            const char *title);
    virtual ~qReachClassSDMed();

	/* How to get the message from the global "purge all IDCs" event. */
  
	virtual void cbPurge(void);
	
};


#endif





