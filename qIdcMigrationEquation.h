#ifndef _qIdcMigrationEquation_
#define _qIdcMigrationEquation_

//#include <rw/tvordvec.h>
//#include <rw/cstring.h>

#include "qIdcEquationBase.h"
#include "qDatContainer.h"


class qIdcMigrationEquation : public qIdcEquationBase {  
public:

	qIdcMigrationEquation(int iDialogID); 
	virtual int FillData(qDatContainer *dat);
	virtual void ApplyData(qDatContainer *dat);
	virtual const char *GetCaption(void){ return "Migration Rate (mi/day)"; }
	virtual int selByDam( void ) { return 0; };
	virtual int selByStock( void ) { return 1; };
    virtual int selByReachClass( void ) { return 1; }
};

/* Global, static object; single instantiation for all dams & species.
 */
extern qIdcMigrationEquation qIdcMigrationEquation_;

#endif


