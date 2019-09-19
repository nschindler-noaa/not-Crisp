
#ifndef __qfocusEventFilter_H__
#define __qfocusEventFilter_H__

#include <qobject.h>
#include <qevent.h>


//For getting focus lost events

class focusGainedEventFilter : public QObject {

	Q_OBJECT
public:
	focusGainedEventFilter( QObject * w ) : QObject(w){}

	protected:
        bool eventFilter( QObject *o, QEvent *e ){
			if(e->type() == QEvent::FocusIn)
				emit focusGained();	
						
			return FALSE;  //pass on event to object

		}

signals:
		void focusGained();		

};

#endif
