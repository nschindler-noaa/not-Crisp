#ifndef _qIdcTransMortEquation_
#define _qIdcTransMortEquation_

//#include <rw/tvordvec.h>
//#include <rw/cstring.h>

#include "qIdcEquationBase.h"
#include "qDatContainer.h"


class qIdcTransMortEquation : public qIdcEquationBase {  

public:

	qIdcTransMortEquation(int iDialogID); 
	virtual int FillData(qDatContainer *dat);
	virtual void ApplyData(qDatContainer *dat);
	virtual const char *GetCaption(void){ return "Transportation Mortality"; }

};

/* Global, static object; single instantiation for all dams & species.
 */
extern qIdcTransMortEquation qIdcTransMortEquation_;

#endif


