//#include <zapp.hpp>
//#include <rw/collect.h>
//#include "ZatGraphUpdateMed.h"

#include "qMapMedDialogBuilder.h"
#include "qMapMedAnnouncements.h"
#include "qMapMedTypes.h"
#include "qMouseTool.h"
#include "MouseToolDefines.h"
#include "QmapObject.h"
#include "mapConstants.h"
#include "log.h"


qMapMedDialogBuilder::qMapMedDialogBuilder(qMapPane *mp, qMouseTool *mt, char* name) 
	: qMapSubMediator(mp, name), mouseTool_(mt), geoObject1_(0), geoObject2_(0) {
	RegisterEvents();
}

qMapMedDialogBuilder::~qMapMedDialogBuilder(){

}

int qMapMedDialogBuilder::isA(){
	return MMT_DIALOGBUILDER;
}

void qMapMedDialogBuilder::RegisterEvents(){

	connect(mapPane_, SIGNAL(evAnnounce(const int, void *)), this, SLOT(evAnnounce(const int,void *)));
}

void qMapMedDialogBuilder::DetachEvents(){

}

void qMapMedDialogBuilder::evAnnounce(const int type, void *data){
	QmapObject *mo;
		
	if(!lock_){
		lock_=1;
		switch(type){
			case MM_MouseMoved_Icon:
                if (data == NULL)
                    break;
				mo = (QmapObject *)data;
				if(mo->objType() == DAM)
					geoObject2_ = mo;
				else
					geoObject1_ = mo;
				break;
			case MM_MouseMoved_Geography:
                if (data == NULL)
                    break;
				geoObject2_ = (QmapObject *)data;
				break;
			case MM_LButtonPressed_Geography:
				if(geoObject2_->objType() == HEADWATER)
					mouseTool_->dialogBuildRequest(IDC_LBUTTON_ONHEADWATER, geoObject1_, geoObject2_);				
				else
					mouseTool_->dialogBuildRequest(IDC_LBUTTON_ONREACH, geoObject1_, geoObject2_);				
				break;
			case MM_RButtonPressed_Geography:
				if (geoObject2_->objType() == HEADWATER)
					mouseTool_->dialogBuildRequest(IDC_RBUTTON_ONHEADWATER, geoObject1_, geoObject2_);				
				else
					mouseTool_->dialogBuildRequest(IDC_RBUTTON_ONREACH, geoObject1_, geoObject2_);				
				break;
			case MM_MButtonPressed_Geography:
				if (geoObject2_->objType() == HEADWATER)
					mouseTool_->dialogBuildRequest(IDC_MBUTTON_ONHEADWATER, geoObject1_, geoObject2_);				
				else
					mouseTool_->dialogBuildRequest(IDC_MBUTTON_ONREACH, geoObject1_, geoObject2_);				
				break;
			case MM_LButtonPressed_Icon:
				mo = (QmapObject *)data;
				if(mo->objType() == DAM)
					mouseTool_->dialogBuildRequest(IDC_LBUTTON_ONDAM, geoObject1_, geoObject2_);				
				else 
					mouseTool_->dialogBuildRequest(IDC_LBUTTON_ONRELEASE, geoObject1_, geoObject2_);				
				break;
			case MM_RButtonPressed_Icon:
				mo = (QmapObject *)data;
				if(mo->objType() == DAM)
					mouseTool_->dialogBuildRequest(IDC_RBUTTON_ONDAM, geoObject1_, geoObject2_);				
				else 
					mouseTool_->dialogBuildRequest(IDC_RBUTTON_ONRELEASE, geoObject1_, geoObject2_);				
				break;
			case MM_MButtonPressed_Icon:
				mo = (QmapObject *)data;
				if(mo->objType() == DAM)
					mouseTool_->dialogBuildRequest(IDC_MBUTTON_ONDAM, geoObject1_, geoObject2_);				
				else 
					mouseTool_->dialogBuildRequest(IDC_MBUTTON_ONRELEASE, geoObject1_, geoObject2_);				
				break;
			case MM_LButtonPressed_NoOp:
			case MM_RButtonPressed_NoOp:
			case MM_MButtonPressed_NoOp:
				mouseTool_->dialogBuildRequest(type, NULL, NULL);
				break;
			default:
				break;
		}
		lock_=0;
	}

}
