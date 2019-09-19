//qMapMedGridLines
//
//Great, you want to know how to write you own map sub mediator.. 
//good place to start.  If you look below you'll see that we've provided
//our own evDraw() function.. The base class ZatMapSubMediator has the _evDraw()
//static function which calls our grid line evDraw through the wonder of
//virtual functions. The evDraw function gets called when the screen REALLY
//wants to be redrawn.  The ZatMapPane handles all the double buffering 
//and coordination of the redraw so all you have to do is write evDraw. 

#ifndef _qMapMedGridLines_h_
#define _qMapMedGridLines_h_

#include "qMapSubMediator.h"


class qMapMedGridLines : public qMapSubMediator {

	protected:

		virtual void evDraw();

	public:
		
		qMapMedGridLines(qMapPane *mp, char* name) : qMapSubMediator(mp, name) {
			//Using the RegisterEvents() of the base class as
			//it'll hook us up with a refresh event.
			RegisterEvents();
		}
		virtual ~qMapMedGridLines(){
			//DetachEvents();
		}
		//All the submediators have to have a isA function so that
		//mediators can be killed. The ZatMapMediator class holds
		//the list of pointers to all the sub mediators and provides
		//a removeSubMediator function so that you can remove them as needed.
		//GridLines is an excellent example of this.  The map gets
		//a ZasMapMedGridLines object when the app starts up, but when
		//the user presses the grid lines button in the tool bar we actually
		//kill this object with a call to removeSubMediator(ZMMT_GRIDLINES).
		//Pressing that button again, makes a new ZasMapMedGridLines object 
		//and passes it into the mapMediator with 
		//ZatMapMediator->insertSubMeditator.  Now you can create multiple 
		//maps with various features based simply on what submediators you pass
		//to it. 
	  	virtual int isA();
};


#endif

