//Abstract base class for map sub-mediators. 
//
//I designed the map system using these sub-mediators so that you could
//add functionality to the map (graphs, features, you name it) without 
//having to embed your code into the existing map system.  Now all you
//have to do is inherit from this ZatMapSubMediator base class register some
//callbacks with the ZatMapPane and do your stuff.
//This base class automatically registers with the evRefresh event of the
//ZatMapPane as this is a common event.. override the RegisterEvents()
//and DetachEvents() functions in your derived class to change this.
//For an example of a VERY simple sub-mediator look at the ZasMapMedGridLines
//class.

#ifndef _qMapSubMediator_
#define _qMapSubMediator_

#include "qMapPane.h"
#include <qobject.h>

class qMapSubMediator : public QObject {

  Q_OBJECT

  protected:
	
	qMapPane *mapPane_;
	int lock_;

  public:

	qMapSubMediator(qMapPane *mp, char* name) : QObject(mp,name){
		  mapPane_ = mp; 
		  lock_ = 0;
	}
	virtual ~qMapSubMediator(){	}

	void SetMapPane(qMapPane *mp){ 
		mapPane_ = mp;
	}

	virtual int isA(){return -1;} 

  protected:
	
	virtual void RegisterEvents(){
		connect( mapPane_, SIGNAL( evRefresh() ), this, SLOT( evDraw() ));		
	}

	//no need to disconnnect
	virtual void DetachEvents(){

	}

  protected slots:

	// override this function in your derived class
	virtual void evDraw(){}; 
	

};



#endif

