//#include <zapp.hpp>
#include "qMapMedGridLines.h"
#include "qMercatorTransform.h"
#include "qMapMedTypes.h"
#include <qpainter.h>
#include "log.h"
#include <qstring.h>


int qMapMedGridLines::isA(){
	//These map sub mediators are held in a list by their base class
	//this allows us to figure out what kind they are for insertion
	//and deletion purposes.
	return MMT_GRIDLINES;
}


void qMapMedGridLines::evDraw(){
						 	
	QRect r;
	int topLat=0, bottomLat=0, leftLon=0, rightLon=0;
	QPainter *p =  mapPane_->currPainter;
	
	qMercatorTransform *zpt; 
	
	if(!lock_){
		//ALL MEDIATORS MUST USE LOCKS
		//Why?  Well if any of this callback code kicks off another
		//callback we might start an infinate loop without it, so 
		//lock your submediator before you start drawing.
		lock_=1; 
		
		//find out what's visible.. if r comes back as 0,0,0,0 we know
		//that the window doesn't exist yet and drawing on it would be bad.
		r = mapPane_->drawArea; 
		if(r.right() == 0 || r.bottom() == 0){
			lock_=0;
			return;
		}
	
		//That qMapPane holds the all mighty transform so lets grab the
		//latest one.
		zpt = (qMercatorTransform *)(mapPane_->getTransform());
		p->setBrush(Qt::DotLine );
		QPen q(Qt::gray.dark(130) ,1, Qt::DotLine);
		p->setPen( q );

		
		//Various calls to get the latitude and longitude coordinates
		//of the 4 sides of the screen. If you look at the leftLon
		//and rightLon settings you'll see I'm calling it with r.top
		//that's cause the distance across the screen in degrees is larger 
		//at the top of the screen with the mercator transform in the 
		//northern hemisphere.
		topLat = (int)zpt->revY(r.top());
		bottomLat = (int)zpt->revY(r.bottom());
		leftLon = (int)zpt->revX(r.left(),r.top());
		rightLon = (int)zpt->revX(r.right(),r.top());



		//Hey.. now we're drawing..
		int i;
		for(i = bottomLat;i <= topLat;i++){
				//I've added operators to the transform class so that you can
				//change between screen x,y and lat,lon easily.
				p->moveTo((*zpt)(zpt->revX(r.left(),double(i)),double(i)));
				p->lineTo((*zpt)(zpt->revX(r.right(),double(i)),double(i)));		
		}
		for(i = rightLon;i <= leftLon;i++){
				p->moveTo((*zpt)(double(i),zpt->revY(r.top())));
				p->lineTo((*zpt)(double(i),zpt->revY(r.bottom())));
		}
		
		//Unlocking this submediator so that it can be activated again.
		lock_=0;

	}
	
}
