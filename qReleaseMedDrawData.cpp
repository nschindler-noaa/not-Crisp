//#include <zapp.hpp>
#include "qEditGraphDataToken.h"
#include "qReleaseMedDrawData.h"
#include "qGraphDialog.h"
#include "qGraphMedTypes.h"



int qReleaseMedDrawData::isA(){
	return GMT_DRAWDATA;
}

	
void qReleaseMedDrawData::RegisterEvents(){
	//GraphPane_->evMouseMovedLButtonPressed()->attach(this, &qReleaseMedDrawData::_evMouseMovedLButtonPressed);
	//GraphPane_->evLButtonPressed()->attach(this, &qReleaseMedDrawData::_evLButtonPressed);
	connect(GraphPane_, SIGNAL(evMouseMovedLButtonPressed()), this, SLOT(evMouseMovedLButtonPressed()));
	connect(GraphPane_, SIGNAL(evLButtonPressed()), this, SLOT(evLButtonPressed()));
	

}
/*
void qReleaseMedDrawData::DetachEvents(){
	//GraphPane_->evMouseMovedLButtonPressed()->detach(this, &qReleaseMedDrawData::_evMouseMovedLButtonPressed);
	//GraphPane_->evLButtonPressed()->detach(this, &qReleaseMedDrawData::_evLButtonPressed);
}

void qReleaseMedDrawData::_evMouseMovedLButtonPressed(void *msg, const zPoint pos){
	qReleaseMedDrawData *gmp = (qReleaseMedDrawData *)msg;
	gmp->evMouseMovedLButtonPressed(pos);
}
*/
void qReleaseMedDrawData::evMouseMovedLButtonPressed(QPoint pos){
	if (regdt_->isWritable())
		qGraphMedDrawData::evMouseMovedLButtonPressed( pos );
}
/*
void qReleaseMedDrawData::_evLButtonPressed(void *msg, const zPoint pos){
	qReleaseMedDrawData *gmp = (qReleaseMedDrawData *)msg;
	gmp->evLButtonPressed(pos);
}
*/

void qReleaseMedDrawData::evLButtonPressed(QPoint pos){
	if (regdt_->isWritable())
		qGraphMedDrawData::evLButtonPressed( pos );
}

