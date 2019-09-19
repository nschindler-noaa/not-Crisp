#ifndef _qIdcFGEEquation_
#define _qIdcFGEEquation_

//#include <rw/tvordvec.h>
//#include <rw/cstring.h>

#include "qIdcEquationBase.h"
#include "qDatContainer.h"


class qIdcFGEEquation : public qIdcEquationBase {  

	Q_OBJECT

protected:
    int eventFlag_;  // Allows attachment to events, without timing problems.

	/* This event tells us that values our equation display depends on may
     *  have changed.
	 */
	
	//static void _cbUpdateSelf( void *msg );

public:

	qIdcFGEEquation(int iDialogID); 
	virtual int FillData(qDatContainer *dat);
	virtual void ApplyData(qDatContainer *dat);
	virtual const char *GetCaption(void)
                { return "FGE Equation (night FGE shown)"; }
	virtual int selByDam( void ) { return 0; };
	virtual int selByPowerhouse( void ) { return 1; };

protected slots:
	virtual void cbUpdateSelf( );

};

/* Global, static object; single instantiation for all dams & species.
 */
extern qIdcFGEEquation qIdcFGEEquation_;

#endif


