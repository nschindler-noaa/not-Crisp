#ifndef _qMapMedUnZoom_h_
#define _qMapMedUnZoom_h_

#include "qMapSubMediator.h"

class qMapMediator;

class qMapMedUnZoom : public qMapSubMediator {

	Q_OBJECT

		qMapMediator *mapMediator_;

	protected:
				
		virtual void RegisterEvents();
		virtual void DetachEvents();

	public:

		qMapMedUnZoom::qMapMedUnZoom(qMapPane *mp, qMapMediator *mm);
		virtual ~qMapMedUnZoom();

		virtual int isA();
		
	protected slots:
		virtual void evAnnounce(const int type, void *data);
};


#endif


