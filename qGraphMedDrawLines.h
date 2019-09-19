//qGraphMedGridLines
//

#ifndef _qGraphMedDrawLines_h_
#define _qGraphMedDrawLines_h_

#include "qGraphSubMediator.h"


class qGraphMedDrawLines : public qGraphSubMediator {



	protected:

		virtual void evDraw();

	public:

		qGraphMedDrawLines(qGraphPane *mp) : qGraphSubMediator(mp) {
			//Using the RegisterEvents() of the base class as
			//it'll hook us up with the ZasGraphPane evDraw event.
			RegisterEvents();
		}

		virtual ~qGraphMedDrawLines(){
			//DetachEvents();
		}

		virtual QPen getPen(int num);		
		virtual QBrush getBrush(int num);		

	  	virtual int isA();
};


#endif


