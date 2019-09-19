#ifndef _qGraphMedDrawAxisXI_h_
#define _qGraphMedDrawAxisXI_h_

#include "qGraphSubMediator.h"


class qGraphMedDrawAxisXI : public qGraphSubMediator {

	protected:

		virtual void evDraw();

	public:

		qGraphMedDrawAxisXI(qGraphPane *mp) : qGraphSubMediator(mp) {
			//Using the RegisterEvents() of the base class as
			//it'll hook us up with the ZasGraphPane evDraw event.
			RegisterEvents();
		}

		virtual ~qGraphMedDrawAxisXI(){
			DetachEvents();
		}
	  	virtual int isA();
};


#endif

