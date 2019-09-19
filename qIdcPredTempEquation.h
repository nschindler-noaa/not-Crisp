#ifndef _qIdcPredTempEquation_
#define _qIdcPredTempEquation_

//#include <rw/tvordvec.h>
//#include <rw/cstring.h>

#include "qIdcEquationBase.h"
#include "qDatContainer.h"


class qIdcPredTempEquation : public qIdcEquationBase {  
public:

	qIdcPredTempEquation(int iDialogID); 
	virtual int FillData(qDatContainer *dat);
	virtual void ApplyData(qDatContainer *dat);
	virtual const char *GetCaption(void){ return "Predator Temperature Response"; }
	virtual int selByDam( void ) { return 0; };
	virtual int selByStock( void ) { return 1; };
    virtual int selByReachClass( void ) { return 1; }
};

/* Global, static object; single instantiation for all dams & species.
 */
extern qIdcPredTempEquation qIdcPredTempEquation_;

#endif


