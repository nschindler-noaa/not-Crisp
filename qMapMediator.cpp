
#include "qMapMediator.h"
#include "qMapSubMediator.h"
#include "QmapController.h"
#include "qMapPane.h"
#include "qMercatorTransform.h"
#include "mapPoint.h"

qMapMediator::qMapMediator(qMapPane *mp, QmapController *mc) : mp_(mp), mc_(mc), lock_(0) {
	defaultCord_ = NULL;
    registerEvents();
	mp_->show();
	mapSubMedList_.setAutoDelete ( true );
	mapStack_.setAutoDelete ( true );

}

qMapMediator::~qMapMediator(){
	if (defaultCord_ != NULL)
		delete defaultCord_;
	mapSubMedList_.clear();
	mapStack_.clear();
}

void qMapMediator::registerEvents(void){
	connect(mp_, SIGNAL(evConfigure()), this, SLOT(evConfigure()));
	connect(mp_, SIGNAL(evDisplay()), this, SLOT(evDisplay()));
	connect(mp_, SIGNAL(evDraw()), this, SLOT(redrawMap()));
}

void qMapMediator::insertSubMediator(qMapSubMediator *msm){

	mapSubMedList_.append(msm);
}

void qMapMediator::removeSubMediator(int medId){
	int j;
	for(size_t i=0;i<mapSubMedList_.count();i++){
		j = mapSubMedList_.at(i)->isA();
		if(medId == j){
			//delete mapSubMedList_.remove(i);
			mapSubMedList_.remove(i);
			i=0;
		}
	}
	
}

void qMapMediator::pushCoordset(mapCordset *co){ 

	mapStack_.push(co); 

}

mapCordset *qMapMediator::popCoordset(){ 

	return mapStack_.pop(); 
}


qPointTransform *qMapMediator::getTransform(QRect &r){
	qMercatorTransform *trans;
	if(mapStack_.top())
		trans = new qMercatorTransform(r,mapStack_.top());
	else	  
		trans = new qMercatorTransform(r,defaultCord_);
	mp_->pushTransform(trans);
	return trans;

}


void qMapMediator::evConfigure(){
	mr_ = mp_->rect() ;
	getTransform(mr_);
}

void qMapMediator::evDisplay(){
	
}

void qMapMediator::redrawMap(){
	
}


