#ifndef _qIdcGasDistEquation_
#define _qIdcGasDistEquation_

//#include <rw/tvordvec.h>
//#include <rw/cstring.h>

#include "qIdcEquationBase.h"
#include "qDatContainer.h"


class qIdcGasDistEquation : public qIdcEquationBase {  

public:

	qIdcGasDistEquation(int iDialogID); 
	virtual int FillData(qDatContainer *dat);
	virtual void ApplyData(qDatContainer *dat);
	virtual const char *GetCaption(void){ return "Gas Distribution"; }

	virtual int selBySpecies( void ) { return 0; };
	virtual int selByFeature( void ) { return 0; };
	virtual int selByDam( void ) { return 0; };
	virtual int selByReach( void ) { return 1; };

};

/* Global, static object; single instantiation for all reaches.
 */
extern qIdcGasDistEquation qIdcGasDistEquation_;

#endif


