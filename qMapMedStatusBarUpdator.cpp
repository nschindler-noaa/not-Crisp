//#include <zapp.hpp>
//#include <intpack/zip.hpp>
#include "mapPoint.h"
#include "QmapObject.h"
#include "mapConstants.h"
#include "qMercatorTransform.h"
#include "qMapMedStatusBarUpdator.h"
#include "qMapMedAnnouncements.h"
#include "qMapMedTypes.h"
#include "log.h"

qMapMedStatusBarUpdator::qMapMedStatusBarUpdator(qMapPane *mp, QCStatusBar *sb, char* name)
	: qMapSubMediator(mp, name), statusBar(sb){
	RegisterEvents();
}

qMapMedStatusBarUpdator::~qMapMedStatusBarUpdator(){

}

int qMapMedStatusBarUpdator::isA(){
	return MMT_STATUSBARUPDATOR;
}

void qMapMedStatusBarUpdator::RegisterEvents(){
    connect(mapPane_, SIGNAL(evMouseMoved(const QPoint )), this, SLOT(evMouseMoved(const QPoint )));
	connect(mapPane_, SIGNAL(evAnnounce(const int , void *)), this, SLOT(evAnnounce(const int , void *)));

}

void qMapMedStatusBarUpdator::DetachEvents(){

}

void qMapMedStatusBarUpdator::evAnnounce(const int type, void *data){
	QmapObject *mo;
	char msg[60];
	if(!lock_){
		lock_=TRUE;
		switch(type){
			case MM_MouseMoved_Icon:
                if (data == NULL) {
                    msg[0] = '\0';
                }
                else {
				    mo = (QmapObject *)data;
				    if(mo->objType() == DAM)
					    sprintf(msg,"Dam: %s", mo->getLabel().ascii());
				    else if(mo->objType() == RELEASE) 
					    sprintf(msg,"Release: %s", mo->getLabel().ascii());
				    else 
					    sprintf(msg, "UNDEFINED: %s", mo->getLabel().ascii());
                }
				statusBar->setRelease(msg);
				break;
			case MM_MouseMoved_Geography:
                if (data == NULL) {
                    msg[0] = '\0';
                }
                else {
				    mo = (QmapObject *)data;
				    if(mo->objType() == REACH) 
					    sprintf(msg,"Reach: %s", mo->getLabel().ascii());
				    else if(mo->objType() == DAM)
					    sprintf(msg,"Dam: %s", mo->getLabel().ascii());
				    else if(mo->objType() == HEADWATER) 
					    sprintf(msg,"Headwater: %s", mo->getLabel().ascii());
				    else 
					    sprintf(msg, "UNDEFINED: %s", mo->getLabel().ascii());
                }				
                statusBar->setDamReach(msg);
				break;
			case MM_StatusBar_Message:
				statusBar->setDamReach(msg);				
				break;
		}
		lock_=FALSE;
	}
}


void qMapMedStatusBarUpdator::evMouseMoved(const QPoint p){
 	
	QRect r;
	char realPos[20];
	qMercatorTransform *zpt; 	

	if(!lock_){
		lock_=1;
		r = mapPane_->rect(); 
		if(r.right() == 0 || r.bottom() == 0) return;
		zpt = (qMercatorTransform *)(mapPane_->getTransform());
		QString str;
		QString str2;
		str.setNum(p.x());
		str2.setNum(p.y());
		str = "x:   " + str + "  y: " + str2;
		sprintf(realPos," LON: %.2f ",zpt->revX(p.x(),p.y()));
		statusBar->setLat(realPos);//IDSL_LATITUDE
		sprintf(realPos," LAT: %.2f ",zpt->revY(p.y()));
		statusBar->setLon(realPos);//IDSL_LONGITUDE
		lock_=0;
	}
	
}
