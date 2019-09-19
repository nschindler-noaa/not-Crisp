#ifndef _qGraphMedDrawData_h_
#define _qGraphMedDrawData_h_

#include "qGraphSubMediator.h"
#include "qEditGraphDataToken.h"

class qGraphDialog;
//class qEditGraphDataToken;

class qGraphMedDrawData : public qGraphSubMediator {

	Q_OBJECT

	protected:

		qEditGraphDataToken *edt_;

		double dXPercent_[2], dYPercent_[2];

		QPoint pSave_;  // for lclick-lclick drawing (rectangle drawing).

		void RegisterEvents();
		virtual void DetachEvents();
		
		virtual void SetXYPercent(const QRect &r, const QPoint &p1, const QPoint &p2);
		virtual void ModifyData(int i, int j);
	public:

		qGraphMedDrawData(qGraphPane *gp, qEditGraphDataToken *edt);

		virtual ~qGraphMedDrawData();
		
		virtual int isA();
		//static void _evMouseMovedRButtonPressed(void *ptr, const zPoint p);
	protected slots:
		virtual void evMouseMovedRButtonPressed(QPoint );
		//static void _evMouseMovedLButtonPressed(void *ptr, const zPoint p);
		virtual void evMouseMovedLButtonPressed(QPoint );
		//static void _evLButtonPressed(void *ptr, const QzPoint p);
		virtual void evLButtonPressed( QPoint );

		//static void _evRButtonAreaSelection(void *ptr, const QRect p);
		virtual void evRButtonAreaSelection( QRect );
		//static void _evLButtonAreaSelection(void *ptr, const QRect p);
		virtual void evLButtonAreaSelection( QRect );
		
		
};


#endif
