#ifndef _qIdcPredatorProbability_
#define _qIdcPredatorProbability_

//#include <rw/tvordvec.h>
//#include <rw/cstring.h>

#include "qIdcTimeBase.h"
#include "qDatContainer.h"
#include "resource.h"


class qIdcPredatorProbability : public qIdcTimeBase {  

public:

	qIdcPredatorProbability(int iDialogID); 
	virtual qDatContainer *NewData();
	virtual int FillData(qDatContainer *dat);
	virtual void ApplyData(qDatContainer *dat);
	
	/* derived classes define their preferred scroll bar range.
	 */
	virtual double minVal( void ) { return 0.0; };
	virtual double maxVal( void ) { return 1.0; };
};

/* Single global, static object does all interfacing for this data.
 */
extern qIdcPredatorProbability qIdcPredatorProbability_;

#endif



