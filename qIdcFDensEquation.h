#ifndef _qIdcFDensEquation_
#define _qIdcFDensEquation_

//#include <rw/tvordvec.h>
//#include <rw/cstring.h>

#include "qIdcEquationBase.h"
#include "qDatContainer.h"


class qIdcFDensEquation : public qIdcEquationBase {  

public:

	qIdcFDensEquation(int iDialogID); 
	virtual int FillData(qDatContainer *dat);
	virtual void ApplyData(qDatContainer *dat);
	virtual const char *GetCaption(void){ return "Population Density vs. Depth"; }
	virtual int selByDam( void ) { return 0; };

};

/* Global, static object; single instantiation for all dams & species.
 */
extern qIdcFDensEquation qIdcFDensEquation_;

#endif



