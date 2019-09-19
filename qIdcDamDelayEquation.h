#ifndef _qIdcDamDelayEquation
#define _qIdcDamDelayEquation

//#include <rw/tvordvec.h>
//#include <rw/cstring.h>

#include "qIdcEquationBase.h"
#include "qDatContainer.h"


class qIdcDamDelayEquation : public qIdcEquationBase {  
	
public:

	qIdcDamDelayEquation(int iDialogID); 
	virtual int FillData(qDatContainer *dat);
	virtual void ApplyData(qDatContainer *dat);
	virtual const char *GetCaption(void){ return "Passage Delay"; }
};

/* Global, static object; single instantiation for all dams & species.
 */
extern qIdcDamDelayEquation qIdcDamDelayEquation_;

#endif

