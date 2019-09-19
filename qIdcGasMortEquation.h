#ifndef _qIdcGasMortEquation_
#define _qIdcGasMortEquation_

//#include <rw/tvordvec.h>
//#include <rw/cstring.h>

#include "qIdcEquationBase.h"
#include "qDatContainer.h"


class qIdcGasMortEquation : public qIdcEquationBase {  

public:

	qIdcGasMortEquation(int iDialogID); 
	virtual int FillData(qDatContainer *dat);
	virtual void ApplyData(qDatContainer *dat);
	virtual const char *GetCaption(void){ return "Gas Mortality"; }
	virtual int selByDam( void ) { return 0; };

};

/* Global, static object; single instantiation for all dams & species.
 */
extern qIdcGasMortEquation qIdcGasMortEquation_;

#endif


