//qMapMedStatusBarUpdator
//
//This is a hip little map mediator.. it takes a zStatusline object
//and updates it when the mouse moves over features, so it
//subscribes to evAnnounce and it also hooks up with evMouseMoved
//to update the status bar with the latest latitude and longitude
//coordinate.  It's legitimate to hook up with evMouseMoved in this
//case as it does something on every call and it's fast, that is, it's not
//searching a database or something, it just converts the screen x,y, to 
//a lat lon with the tranform object and tosses it into the status line.

#ifndef _qMapMedStatusBarUpdator_h_
#define _qMapMedStatusBarUpdator_h_

#include "qMapSubMediator.h"
#include "QCStatusBar.h"

//class zStatusLine;

class qMapMedStatusBarUpdator : public qMapSubMediator {

	Q_OBJECT	

	protected:
	
		QCStatusBar *statusBar;

		int radius_;

		virtual void RegisterEvents();
		virtual void DetachEvents();


	public:

		qMapMedStatusBarUpdator(qMapPane *mp, QCStatusBar *sb, char* name);
		virtual ~qMapMedStatusBarUpdator();

		virtual int isA();
		
	protected slots:
		void evMouseMoved(const QPoint );
		void evAnnounce(const int , void *);

};



#endif


