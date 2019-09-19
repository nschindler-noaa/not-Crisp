//Abstract base class for Graph sub-mediators. 
//
//I designed the Graph system using these sub-mediators so that you could
//add functionality to the Graph (graphs, features, you name it) without 
//having to embed your code into the existing Graph system.  Now all you
//have to do is inherit from this qGraphSubMediator base class register some
//callbacks with the qGraphPane and do your stuff.
//This base class automatically registers with the evRefresh event of the
//qGraphPane as this is a common event.. override the RegisterEvents()
//and DetachEvents() functions in your derived class to change this.
//For an example of a VERY simple sub-mediator look at the qGraphMedGridLines
//class.

#ifndef _qGraphSubMediator_
#define _qGraphSubMediator_

#include "qGraphPane.h"
#include "log.h"

class qGraphSubMediator : public QObject{

	Q_OBJECT

  protected:
	
	qGraphPane *GraphPane_;		
	int lock_;

  public:

	qGraphSubMediator(qGraphPane *mp)  : QObject(mp) , GraphPane_(mp), lock_(0){
	}

	virtual ~qGraphSubMediator(){		
	}

	void SetGraphPane(qGraphPane *mp){ 
		GraphPane_ = mp;
	}

	virtual int isA()=0; 

  protected:
	
	virtual void RegisterEvents(){
		connect(GraphPane_, SIGNAL(evDraw()), this, SLOT(evDraw()));	
	}

	virtual void DetachEvents(){	
	}

  protected slots:
	// override this function in your derived class
	virtual void evDraw(){};  


	
};



#endif


