#ifndef _qMapMediator_
#define _qMapMediator_

#include <math.h>
#include <qrect.h>
#include <qptrstack.h>
#include <qptrlist.h>
#include <qobject.h>
#include "mapPoint.h"
#include "qMapSubMediator.h"

class QmapController;
class mapCordset;
class qMapPane;
class qPointTransform;


class qMapMediator : public QObject{ 

	Q_OBJECT

 protected:

    qMapPane *mp_;

    QRect mr_;

    QPtrStack <mapCordset> mapStack_;
	QPtrList <qMapSubMediator> mapSubMedList_;

    mapCordset *defaultCord_;
    QmapController *mc_;

    int lock_;
    
    virtual void registerEvents(void);
	//Builds a new transform and inserts it into the mapPane.
	qPointTransform *getTransform(QRect &r);

  public:
	  
    qMapMediator(qMapPane *mp, QmapController *mc);
	virtual ~qMapMediator();

	//Coordinate Set methods
    virtual void pushCoordset(mapCordset *co);
    virtual mapCordset *popCoordset();
	virtual QPtrStack <mapCordset> getCordStack(){ return mapStack_; }

	//Sub Mediator methods
    virtual void insertSubMediator(qMapSubMediator *msm);
    virtual void removeSubMediator(int medId);
		
	//Events
	//virtual void evConfigure();

	//virtual void evDisplay();

protected slots:
	void evConfigure();
	void evDisplay();
	void redrawMap();
    
};


#endif

