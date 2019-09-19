#ifndef _qIdcSpillEquation_
#define _qIdcSpillEquation_



#include "qIdcEquationBase.h"
#include "qDatContainer.h"


class qIdcSpillEquation : public qIdcEquationBase {  

public:

	qIdcSpillEquation(int iDialogID); 
	virtual int FillData(qDatContainer *dat);
	virtual void ApplyData(qDatContainer *dat);
	virtual const char *GetCaption(void){ return "Spill Efficiency"; }

};

/* Global, static object; single instantiation for all dams & species.
 */
extern qIdcSpillEquation qIdcSpillEquation_;

#endif


