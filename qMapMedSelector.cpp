//#include <zapp.hpp>
#include "mapPoint.h"
#include "QmapController.h"
#include "qMercatorTransform.h"
#include "qMapMedSelector.h"
#include "qMapMedAnnouncements.h"
#include "qMapMedTypes.h"
#include "log.h"
#include "mapConstants.h"
#include "qGraphUpdateMed.h"
#include "qMapMedGeography.h"
#include <qpen.h>
#include <qpainter.h>

qMapMedSelector::qMapMedSelector(qMapPane *mp, QmapController *mc) 
	:	qMapSubMediator(mp, "The detector"), radius_(10), mapCon_(mc), oldPos_(mapPoint(0.0,0.0)), newPos_(mapPoint(0.0,0.0)),
		oldObj_(0), newObj_(0), oldGeoObj_(0), newGeoObj_(0)
{
	defaultPen_ = new QPen(blue,1,Qt::SolidLine);
	highlightPen_ = new QPen(red,1,SolidLine);
	defaultBrush_ = new QBrush(white);
	RegisterEvents();

}

qMapMedSelector::~qMapMedSelector(){
	delete defaultPen_;
	delete highlightPen_;
	delete defaultBrush_;
	DetachEvents();

}

int qMapMedSelector::isA(){
	return MMT_SELECTOR;
}

void qMapMedSelector::RegisterEvents(){
	connect(mapPane_, SIGNAL(evLButtonPressed(QPoint )), this, SLOT(evLButtonPressed(QPoint )));
	connect(mapPane_, SIGNAL(evMButtonPressed(QPoint )), this, SLOT(evMButtonPressed(QPoint )));
	connect(mapPane_, SIGNAL(evRButtonPressed(QPoint )), this, SLOT(evRButtonPressed(QPoint )));
	connect( mapPane_, SIGNAL(evMouseMoved(const QPoint)), this, SLOT( evMouseMoved(const QPoint) ) );
}

void qMapMedSelector::DetachEvents(){
	
}


void qMapMedSelector::evLButtonPressed(QPoint p ){
	QmapObject *mo;
	if(!lock_){
		lock_=TRUE;
		if(mo = intersectsIcon(p)){
//			log_msg(1, "MM_LButtonPressed_Icon");
			mapPane_->sendAnnouncement(MM_LButtonPressed_Icon, mo);	
		}
		else if(mo = intersectsGeography(p)){
		//	log_msg(1, "MM_LButtonPressed_Geography");
			mapPane_->sendAnnouncement(MM_LButtonPressed_Geography, mo);
		 }
		 else{ 						   	   
		//	log_msg(1, "MM_LButtonPressed_NoOp");
			mapPane_->sendAnnouncement(MM_LButtonPressed_NoOp, NULL); 
		 }
		lock_=FALSE;
	}
}

void qMapMedSelector::evMButtonPressed( QPoint p){
	QmapObject *mo;
//	log_msg(1, "middle buton Pressed");
	if(!lock_){
		lock_=TRUE;
		if(mo = intersectsIcon(p))
			mapPane_->sendAnnouncement(MM_MButtonPressed_Icon, mo);	
		else if(mo = intersectsGeography(p))
			mapPane_->sendAnnouncement(MM_MButtonPressed_Geography, mo);
		else 						   	   
			mapPane_->sendAnnouncement(MM_LButtonPressed_NoOp, NULL); 
		lock_=FALSE;
	}
}

void qMapMedSelector::evRButtonPressed(QPoint p ){
	QmapObject *mo;

	if(!lock_){
		lock_=TRUE;

		if(mo = intersectsIcon(p))
			mapPane_->sendAnnouncement(MM_RButtonPressed_Icon, mo);
		else if(mo = intersectsGeography(p))
			mapPane_->sendAnnouncement(MM_RButtonPressed_Geography, mo);
		else 						   
			mapPane_->sendAnnouncement(MM_RButtonPressed_NoOp, NULL); 

		lock_=FALSE;
	}
}

void qMapMedSelector::evMouseMoved(const QPoint p){
 	QmapObject *mo;
	if(!lock_){
		lock = TRUE;
        // Check if we are pointing at a dam or a release.
		if(mo = intersectsIcon(p)){
			if(mo->objType() == DAM){
				if(oldGeoObj_ != mo){
					if (oldGeoObj_)
						oldGeoObj_->Selected( 0 );
					mo->Selected( 1 );
					newGeoObj_ = mo;
					mapPane_->sendAnnouncement(MM_MouseMoved_Geography, newGeoObj_);
					qGraphUpdateMed::instance()->updateGraphs((char*)mo->getLabel().ascii(), mo->objType());
					updateGeo();
				}
			}
			else {
				if(oldObj_ != mo){
					if (oldObj_)
						oldObj_->Selected( 0 );
					mo->Selected( 1 );
					newObj_ = mo;
					mapPane_->sendAnnouncement(MM_MouseMoved_Icon, newObj_); 
					qGraphUpdateMed::instance()->updateGraphs((char*)mo->getLabel().ascii(), mo->objType());
					updateIcon();  
				}
			}
		}
        // If not, check if we are pointing at a reach or a headwater.
		else if(mo = intersectsGeography(p)) {
			if(oldGeoObj_ != mo){
				if (oldGeoObj_)
					oldGeoObj_->Selected( 0 );
				mo->Selected( 1 );
				newGeoObj_ = mo;
				mapPane_->sendAnnouncement(MM_MouseMoved_Geography, newGeoObj_);
				qGraphUpdateMed::instance()->updateGraphs((char*)mo->getLabel().ascii(), mo->objType());
				updateGeo();
			}
		}
        // If not, clear all previous selections.
        else {
            newObj_ = newGeoObj_ = NULL;
            if (oldGeoObj_) {
                oldGeoObj_->Selected( 0 );
                updateGeo();
				mapPane_->sendAnnouncement(MM_MouseMoved_Geography, NULL);
                oldGeoObj_ = NULL;
            }
            if (oldObj_) {
                oldObj_->Selected( 0 );
                updateIcon();
				mapPane_->sendAnnouncement(MM_MouseMoved_Icon, NULL); 
                oldObj_ = NULL;
            }
        }
		lock = FALSE;
	}
}

void qMapMedSelector::updateGeo(){
	if(oldGeoObj_ && (oldGeoObj_ != newGeoObj_)){
		if(oldGeoObj_->objType() == DAM){ 
			evDrawIcon(oldGeoObj_);					
		}
		else {
			evDrawGeo(oldGeoObj_);
		}
	}
	if(newGeoObj_){
		if(newGeoObj_->objType() == DAM){ 
			evDrawIcon(newGeoObj_);					
		}
		else {
			evDrawGeo(newGeoObj_);
		}
	}
	oldGeoObj_ = newGeoObj_;

}

void qMapMedSelector::updateIcon(){
	if(oldObj_ && (oldObj_ != newObj_)){
		evDrawIcon(oldObj_);
	}
	if(newObj_){
		evDrawIcon(newObj_);
	}
	oldObj_ = newObj_;

}

void qMapMedSelector::evDrawGeo(QmapObject *mapObj){
	QRect r;
	qMercatorTransform *zpt; 

	if (mapPane_->isPrintMode())
		return;
	//Create new painter to reflect highlighted items
	if(!lock_){
		lock_=1;
		QPainter* p = mapPane_->currPainter;
		mapPane_->initiateDraw();
		
		r = mapPane_->drawArea;
		
		
		if(r.right() == 0 || r.bottom() == 0) return;
			zpt = (qMercatorTransform *)(mapPane_->getTransform());
			
		if(mapObj){
			p->setPen(qMapMedGeography::getPen(mapObj));
			p->setBrush(qMapMedGeography::getPen(mapObj).color());
			qMapMedGeography::drawMapObject( mapObj, p, zpt );
		}

		mapPane_->endDraw();
		lock_ = 0;
	}

}

void qMapMedSelector::evDrawIcon(QmapObject *mapObj){

	QRect r;
	QPoint tempos;
	qMercatorTransform *zpt; 

	if (mapPane_->isPrintMode())
		return;
	//Create new painter to reflect highlighted items

	QPainter* p = mapPane_->currPainter;
	

	if(!lock_){
		lock_=1;
	
		r = mapPane_->drawArea;
		if(r.right() == 0 || r.bottom() == 0) return;
		zpt = (qMercatorTransform *)(mapPane_->getTransform());
		tempos = (*zpt)(*(mapPoint *)(mapObj->at(0)));
		mapPane_->update(QRect(tempos-QPoint(radius_,radius_),tempos+QPoint(radius_,radius_)));

		lock_=0;
	}
	
}

QmapObject *qMapMedSelector::intersectsIcon(const QPoint &pos){

	int intersect=0;
	int zoneradius = radius_;
	QmapObject *mo = NULL;
    mapPoint  *mpo;
	QPtrListIterator<QmapObject> *itr;
	qMercatorTransform *zpt; 

	zpt = (qMercatorTransform *)(mapPane_->getTransform());
	itr = mapCon_->getIconIterator();

	if (itr == NULL || itr->count() <= 0) return NULL;

	QString theString ("");
	while(!intersect && (mo = itr->current()) != 0){
				
		++(*itr);
		mpo=mo->at(0);
		if(mo->intersects((*zpt)(pos.x()-zoneradius,pos.y()-zoneradius),
						(*zpt)(pos.x()+zoneradius,pos.y()+zoneradius))){
			intersect = 1;
		}
	
	
	}

	if(itr)delete itr;
	if(intersect)
		return mo;
	return NULL;

}


QmapObject *qMapMedSelector::intersectsGeography(const QPoint &pos){

	int intersect = 0;
	double minDistance = 360.0, distance;
	int zoneradius = radius_+5;
	QmapObject *mo;
	QmapObject *minObj = NULL;
	mapPoint *mpo, mPos(0.0,0.0);
	QPtrListIterator<QmapObject> *itr;
	QPtrListIterator<mapPoint> *pItr;
	QPtrList<QmapObject> tList;
	qMercatorTransform *zpt; 
	QPoint po;

	zpt = (qMercatorTransform *)(mapPane_->getTransform());
	itr = mapCon_->getIterator();
	mPos = (*zpt)(pos.x(),pos.y());
	while((mo = itr->current()) != 0 ){
	
		++(*itr);
		if(mo->intersects((*zpt)(pos.x()-zoneradius,pos.y()-zoneradius),
					(*zpt)(pos.x()+zoneradius,pos.y()+zoneradius))){
			tList.append(mo);
			minObj = mo;
			intersect++;
		}
	}

	if(itr)
		delete itr;
	if(intersect == 1)
		return minObj;
	if(intersect){
		for(unsigned int i=0;i<tList.count();i++){
			mo=(QmapObject *)tList.at(i);
			pItr=mo->getIterator();
			while((mpo = pItr->current()) != 0){
				++(*pItr);
				distance = mPos.distance(*mpo);
				if(distance < 0.0)
					distance = -distance;
				if(distance < minDistance){
					minDistance = distance;
					minObj = mo;
				}
			}
			if(pItr) delete pItr;
		}
		return minObj;
	}
	return NULL;

}

