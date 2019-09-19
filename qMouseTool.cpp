//#include <zapp.hpp>

#include "qMouseTool.h"
#include "qMediatorBuilder.h"
#include "qMapMedTypes.h"
#include "qMapMedAnnouncements.h"
#include "qGraphDataTokenTypes.h"
#include "Qcrisp.h"
#include "resource.h"
#include "mapPoint.h"
#include "QmapObject.h"
#include "mapConstants.h"
#include <qstring.h>
#include "qHelpManager.h"


int	qMouseTool::id[NumCategories][NumButtons];
qMouseTool *qMouseTool::instance_ = NULL;

qMouseTool::qMouseTool(QWidget * parent) : Mouse_Tool_Base(parent) {
	w_ = parent;
	

	// Left button
	OnButton[0][0] = comboBox1_0; 
	OnButton[1][0] = comboBox2_0;
	OnButton[2][0] = comboBox3_0;
	OnButton[3][0] = comboBox4_0;  

	// Middle button						  
	OnButton[0][1] = comboBox1_2;
	OnButton[1][1] = comboBox2_2;
	OnButton[2][1] = comboBox3_2;
	OnButton[3][1] = comboBox4_2; 

	// Right button							  
	OnButton[0][2] = comboBox1_3;
	OnButton[1][2] = comboBox2_3;
	OnButton[2][2] = comboBox3_3;
	OnButton[3][2] = comboBox4_3;

	initializeComboBoxes();

	connect(pushButton1, SIGNAL(clicked ()), SLOT(cbDone()));
	connect(pushButton2, SIGNAL(clicked ()), SLOT(cbHelp()));

}

qMouseTool::~qMouseTool(){
	instance_ = NULL;
}


void qMouseTool::dialogBuildRequest(const int type, QmapObject *geoObject1, QmapObject *geoObject2){

	QString s = QString::number( lButton ); 
	switch(type){
		case MM_LButtonPressed_NoOp:
		case MM_MButtonPressed_NoOp:
		case MM_RButtonPressed_NoOp:
			break;

		case IDC_LBUTTON_ONDAM:
			qMediatorBuilder::instance(w_)->makeDialog(
				*IdCode[onDam][lButton].at(OnButton[onDam][lButton]->currentItem())
				,geoObject1, geoObject2);
			break;
		case IDC_MBUTTON_ONDAM:
			qMediatorBuilder::instance(w_)->makeDialog(
				*IdCode[onDam][mButton].at(OnButton[onDam][mButton]->currentItem())
				,geoObject1, geoObject2);
			break;
		case IDC_RBUTTON_ONDAM:
			qMediatorBuilder::instance(w_)->makeDialog(
				*IdCode[onDam][rButton].at(OnButton[onDam][rButton]->currentItem())
				,geoObject1, geoObject2);
			break;
		case IDC_LBUTTON_ONREACH:
			qMediatorBuilder::instance(w_)->makeDialog(
				*IdCode[onReach][lButton].at(OnButton[onReach][lButton]->currentItem())
				,geoObject1, geoObject2);
			break;
		case IDC_MBUTTON_ONREACH:
			qMediatorBuilder::instance(w_)->makeDialog(
				*IdCode[onReach][mButton].at(OnButton[onReach][mButton]->currentItem())
				,geoObject1, geoObject2);
			break;
		case IDC_RBUTTON_ONREACH:
			qMediatorBuilder::instance(w_)->makeDialog(
				*IdCode[onReach][rButton].at(OnButton[onReach][rButton]->currentItem())
				,geoObject1, geoObject2);
			break;
		case IDC_LBUTTON_ONRELEASE:
			qMediatorBuilder::instance(w_)->makeDialog(
				*IdCode[onRelease][lButton].at(OnButton[onRelease][lButton]->currentItem())
				,geoObject2, geoObject1);
			break;
		case IDC_MBUTTON_ONRELEASE:
			qMediatorBuilder::instance(w_)->makeDialog(
				*IdCode[onRelease][mButton].at(OnButton[onRelease][mButton]->currentItem())
				,geoObject2, geoObject1);
			break;
		case IDC_RBUTTON_ONRELEASE:
			qMediatorBuilder::instance(w_)->makeDialog(
				*IdCode[onRelease][rButton].at(OnButton[onRelease][rButton]->currentItem())
				,geoObject2, geoObject1);
			break;
		case IDC_LBUTTON_ONHEADWATER:
			qMediatorBuilder::instance(w_)->makeDialog(
				*IdCode[onHeadwater][lButton].at(OnButton[onHeadwater][lButton]->currentItem())
				,geoObject1, geoObject2);
			break;
		case IDC_MBUTTON_ONHEADWATER:
			qMediatorBuilder::instance(w_)->makeDialog(
				*IdCode[onHeadwater][mButton].at(OnButton[onHeadwater][mButton]->currentItem())
				,geoObject1, geoObject2);
			break;
		case IDC_RBUTTON_ONHEADWATER:
			qMediatorBuilder::instance(w_)->makeDialog(
				*IdCode[onHeadwater][rButton].at(OnButton[onHeadwater][rButton]->currentItem())
				,geoObject1, geoObject2);
			break;
		default:
			return;
	}
		
}



void qMouseTool::addEntry(const char *label, int idcode, int button, int category){

	OnButton[category][button]->insertItem(label);
	IdCode[category][button].append(idcode);	
}


void qMouseTool::cbHelp(){	
	helpManager->showHelp( QString("82.html") );	
}


qMouseTool *qMouseTool::instance(QWidget* parent){
	if(!instance_){
		for(int i=0;i<NumCategories;i++)
			for(int j=0;j<NumButtons;j++)
				id[i][j]=0;
		instance_ = new qMouseTool(parent);
	}
	return (qMouseTool *)instance_;
}

void qMouseTool::initializeComboBoxes(){

	//Dam Setup
	int i;
	for(i=0;i<NumButtons;i++){
		addEntry("No Operation",0,i,onDam);
		addEntry("Flow Rate",GDTT_DAM_Flow,i,onDam);
		addEntry("Spill Rate",GDTT_DAM_Spill,i,onDam);
		addEntry("TDG Saturation > 100%",GDTT_DAM_NSat,i,onDam);
		addEntry("Water Temperature",GDTT_DAM_WaterTemp,i,onDam);
		addEntry("Turbidity",GDTT_DAM_Turbidity,i,onDam);
		addEntry("Basin Volume",GDTT_DAM_BasinVolume,i,onDam);
		addEntry("Spill Equation",GDTT_DAMEQUATION_Spill,i,onDam);
		addEntry("Delay Equation",GDTT_DAMEQUATION_Delay,i,onDam);
		addEntry("Day TDG Sat Equation",GDTT_DAMEQUATION_NSat,i,onDam);
		addEntry("Night TDG Sat Equation",GDTT_DAMEQUATION_NightNSat,i,onDam);
		addEntry("Backup TDG Sat Equation",GDTT_DAMEQUATION_BackupNSat,i,onDam);
		addEntry("FGE Equation",GDTT_DAMEQUATION_FGE,i,onDam);
		addEntry("Transport Mortality Equation",GDTT_DAMEQUATION_Trans,i,onDam);
		addEntry("Passage Histogram", GDTT_PASSAGE_Dam, i, onDam );
#ifdef ROUTE_TRACKING
		addEntry("Bypass Histogram", GDTT_PASSAGE_Bypass, i, onDam );
		addEntry("Transport Histogram", GDTT_PASSAGE_Transport, i, onDam );
		addEntry("Spillway Histogram", GDTT_PASSAGE_Spillway, i, onDam );
		addEntry("Turbine Histogram", GDTT_PASSAGE_Turbine, i, onDam );
#endif
	}

	//Release Setup
	for(i=0;i<NumButtons;i++){
		addEntry("No Operation",0,i,onRelease);
		addEntry( "Release Tool", GDTT_RELEASE_Tool, i, onRelease );
	}

	//Reach Setup
	for(i=0;i<NumButtons;i++){
		addEntry("No Operation",0,i,onReach);
		addEntry("Flow Rate",GDTT_REACH_Flow,i,onReach);
		addEntry("Water Temperature",GDTT_REACH_WaterTemp,i,onReach);
		addEntry("Turbidity",GDTT_REACH_Turbidity,i,onReach);
		addEntry("TDG Saturation > 100%",GDTT_REACH_NSatOut,i,onReach);
		addEntry("Velocity",GDTT_REACH_Velocity,i,onReach);
		addEntry("Elevation Change",GDTT_REACH_ElevationChange,i,onReach);
		addEntry("Loss",GDTT_REACH_Loss,i,onReach);
		addEntry("Passage Histogram", GDTT_PASSAGE_Reach, i, onReach );
		addEntry("Length Histogram", GDTT_PASSAGE_ReachLength, i, onReach );
		addEntry("Gas Distribution", GDTT_REACH_GasDist, i, onReach );
	}

	//Headwater Setup
	for(i=0;i<NumButtons;i++){
		addEntry("No Operation",0,i,onHeadwater);
		addEntry("Headwater Flow",GDTT_HEADWATER_Flow,i,onHeadwater);
		addEntry("Headwater Temperature",GDTT_HEADWATER_WaterTemp,i,onHeadwater);
		addEntry("Headwater Turbidity",GDTT_HEADWATER_Turbidity,i,onHeadwater);
	}

	//Default Selections
	OnButton[onDam][0]->setCurrentItem(13);
	OnButton[onDam][1]->setCurrentItem(1);
	OnButton[onDam][2]->setCurrentItem(2);

	OnButton[onReach][0]->setCurrentItem(7);
	OnButton[onReach][1]->setCurrentItem(3);
	OnButton[onReach][2]->setCurrentItem(2);

	OnButton[onRelease][2]->setCurrentItem(1);

	OnButton[onHeadwater][1]->setCurrentItem(1);
	OnButton[onHeadwater][2]->setCurrentItem(2);

}

void qMouseTool::cbDone(){

	close();
}
