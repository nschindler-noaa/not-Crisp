#ifndef _qIdcNSatEquation_
#define _qIdcNSatEquation_

//#include <rw/tvordvec.h>
//#include <rw/cstring.h>

#include "qIdcEquationBase.h"
#include "qDatContainer.h"


class qIdcNSatEquation : public qIdcEquationBase {  

public:

	qIdcNSatEquation(int iDialogID); 
	virtual int FillData(qDatContainer *dat);
	virtual void ApplyData(qDatContainer *dat);
	virtual const char *GetCaption(void){ return "TDG Saturation > 100% (Day)"; }
	virtual int selBySpecies( void ) { return 0; };

};

/* Global, static object; single instantiation for all dams & species.
 */
extern qIdcNSatEquation qIdcNSatEquation_;

/* *************** Same thing but at night. ***************** */

class qIdcNightNSatEquation : public qIdcEquationBase {  

public:

	qIdcNightNSatEquation(int iDialogID); 
	virtual int FillData(qDatContainer *dat);
	virtual void ApplyData(qDatContainer *dat);
	virtual const char *GetCaption(void){ return "TDG Saturation > 100% (Night)"; }
	virtual int selBySpecies( void ) { return 0; };

};

/* Global, static object; single instantiation for all dams & species.
 */
extern qIdcNightNSatEquation qIdcNightNSatEquation_;


/* *************** Same thing but backup for new eqns. ***************** */

class qIdcBackupNSatEquation : public qIdcEquationBase {  

public:

	qIdcBackupNSatEquation(int iDialogID); 
	virtual int FillData(qDatContainer *dat);
	virtual void ApplyData(qDatContainer *dat);
	virtual const char *GetCaption(void){ return "TDG Saturation > 100% (Backup)"; }
	virtual int selBySpecies( void ) { return 0; };

};

/* Global, static object; single instantiation for all dams & species.
 */
extern qIdcBackupNSatEquation qIdcBackupNSatEquation_;

#endif


