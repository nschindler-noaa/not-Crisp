
#ifndef _qGraphMedDrawAxisYD_h_
#define _qGraphMedDrawAxisYD_h_

#include "qGraphSubMediator.h"


class qGraphMedDrawAxisYD : public qGraphSubMediator {

	protected:

		virtual void evDraw();

	public:

		qGraphMedDrawAxisYD(qGraphPane *mp) : qGraphSubMediator(mp) {
			//Using the RegisterEvents() of the base class as
			//it'll hook us up with the ZasGraphPane evDraw event.
			RegisterEvents();
		}

		virtual ~qGraphMedDrawAxisYD(){
			DetachEvents();
		}
	  	virtual int isA();
};


#endif


