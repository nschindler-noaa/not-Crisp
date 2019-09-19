//

#ifndef _qGraphMedDrawText_h_
#define _qGraphMedDrawText_h_

#include "qGraphSubMediator.h"


class qGraphMedDrawText : public qGraphSubMediator {

	protected:

		virtual void evDraw();

	public:

		qGraphMedDrawText(qGraphPane *mp) : qGraphSubMediator(mp) {
			//Using the RegisterEvents() of the base class as
			//it'll hook us up with the ZasGraphPane evDraw event.
			RegisterEvents();
		}

		virtual ~qGraphMedDrawText(){
			DetachEvents();
		}
	  	virtual int isA();
};


#endif


