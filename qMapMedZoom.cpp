//#include <zapp.hpp>
#include "qMapMediator.h"
#include "qMercatorTransform.h"
#include "qMapMedZoom.h"
#include "qMapMedTypes.h"
#include "log.h"

qMapMedZoom::qMapMedZoom(qMapPane *mp, qMapMediator *mm) 
	: qMapSubMediator(mp, "Zoom Mediator"), mapMediator_(mm) {
	RegisterEvents();
}

qMapMedZoom::~qMapMedZoom(){

}

int qMapMedZoom::isA(){
	return MMT_ZOOM;
}
	
void qMapMedZoom::RegisterEvents(){
	connect(mapPane_, SIGNAL(evLButtonAreaSelection(const QRect)), this, SLOT(evLButtonAreaSelection(const QRect)) );
	connect(mapPane_, SIGNAL(evRButtonAreaSelection(const QRect)), this, SLOT(evRButtonAreaSelection(const QRect)) );
}

void qMapMedZoom::DetachEvents(){
}

void qMapMedZoom::evRButtonAreaSelection( const QRect r){
	evAreaSelection(r);
}

void qMapMedZoom::evLButtonAreaSelection( const QRect r){
	evAreaSelection(r);
}

void qMapMedZoom::evAreaSelection(const QRect &zr){

	QRect r;
	int xshift=0, yshift=0;
	qMercatorTransform *zpt;

	if(!lock_){
		lock_=1;
		r = mapPane_->rect();
		if(r.right() == 0 || r.bottom() == 0) return;
			
		zpt = (qMercatorTransform *)(mapPane_->getTransform());
		double oldAspect = double(r.right()-r.left())/double(r.bottom()-r.top());
		double newAspect = double(zr.right()-zr.left())/double(zr.bottom()-zr.top());

			if(oldAspect > newAspect){
			xshift = int((((oldAspect/newAspect)-1.0)*double(zr.right()-zr.left()))/2.0);
		}
		if(oldAspect < newAspect){
			yshift = int((((newAspect/oldAspect)-1.0)*double(zr.bottom()-zr.top()))/2.0);
		}
		mapMediator_->pushCoordset(
					(new mapCordset(zpt->revX(zr.left()-xshift,zr.top()-yshift), 
									zpt->revY(zr.top()-yshift), 
									zpt->revX(zr.right()+xshift,zr.bottom()+yshift), 
									zpt->revY(zr.bottom()+yshift))));
		
		//Force Map Pane Redraw
		mapPane_->repaint();

        mapPane_->update();  // prevents race condition with qMapMedSelector

	
		lock_=0;
	}

}


