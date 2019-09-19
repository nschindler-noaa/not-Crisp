//#include <zapp.hpp>
#include "qMapMediator.h"
#include "qMapMedUnZoom.h"
#include "qMapMedAnnouncements.h"
#include "qMapMedTypes.h"
#include "mapPoint.h"

qMapMedUnZoom::qMapMedUnZoom(qMapPane *mp, qMapMediator *mm) 
	: qMapSubMediator(mp, "Unzoom Mediator"), mapMediator_(mm) {
	
	RegisterEvents();
}

qMapMedUnZoom::~qMapMedUnZoom(){
}

int qMapMedUnZoom::isA(){
	return MMT_UNZOOM;
}
	
void qMapMedUnZoom::RegisterEvents(){
	connect(mapPane_, SIGNAL(evAnnounce(const int, void* )), this, SLOT(evAnnounce(const int, void *)) );
}

void qMapMedUnZoom::DetachEvents(){

}

void qMapMedUnZoom::evAnnounce(const int type, void *data){
	QRect r;

	if(!lock_){
		lock_=TRUE;
		if(type == MM_LButtonPressed_NoOp || type == MM_RButtonPressed_NoOp){
			//mapPane_->canvas()->getVisible(r);
			r = mapPane_->rect();
			if(r.right() == 0 || r.bottom() == 0) return;

			//mapPane_->canvas()->lock();

			mapCordset *pmc = mapMediator_->popCoordset();
			if (pmc != NULL)
				delete pmc;
			//mapPane_->size(NULL);
            //mapPane_->update();  // prevents race condition with ZasMapMedDetector
			mapPane_->repaint();

			//mapPane_->canvas()->unlock();
		}
		lock_=FALSE;
	}

}


