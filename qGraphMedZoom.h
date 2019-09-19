#ifndef _qGraphMedZoom_h_
#define _qGraphMedZoom_h_

#include "qGraphSubMediator.h"

class qGraphDialog;

class qGraphMedZoom : public qGraphSubMediator {

	Q_OBJECT

	protected:

		virtual void RegisterEvents();
		virtual void DetachEvents();
		qGraphDialog *GraphDialog_;

	public:

		qGraphMedZoom(qGraphPane *gp, qGraphDialog *gd)
										: qGraphSubMediator(gp) {
			GraphDialog_ = gd;
			RegisterEvents();
		}

		virtual ~qGraphMedZoom(){
			DetachEvents();
		}

	
	protected slots:
		virtual void evRButtonPressed( QPoint p);
		virtual void evRButtonAreaSelection( QRect r);
		virtual int isA();
};


#endif


