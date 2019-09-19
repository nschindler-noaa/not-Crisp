#include "qGraphMediator.h"
#include "qGraphDialog.h"
#include "qGraphDataTokenBuilder.h"
#include "qGlobalEvents.h"
#include "Qcrisp.h"
#include "Qmain.h"
#include "qGraphUpdateMed.h"
#include <qcolor.h>
#include "qHelpManager.h"


qGraphMediator::qGraphMediator(qGraphDialog *gd, qGraphDataToken *dt)
    : _gd(gd), dataToken_(dt), _isData(0), _lock(0),_ref(0){
	
	_gp = _gd->getGraphPane();
    _gu = qGraphUpdateMed::instance();
	_gp->setDataToken(dt);
	_gd->setDataToken( dt );
	gd->scaleUpdated();

    RegisterEvents();
	DrawLabels();	
	_gp->repaint();
	
}
 
qGraphMediator::~qGraphMediator(){
	graphSubMedList_.setAutoDelete(TRUE);
	graphSubMedList_.clear();
	//delete dataToken_; automaticaly deleted
}

void qGraphMediator::insertSubMediator(qGraphSubMediator *msm){
	graphSubMedList_.append(msm);
}

void qGraphMediator::removeSubMediator(int medId){
	int j;
	for(size_t i=0;i<graphSubMedList_.count();i++){
		j = graphSubMedList_.at(i)->isA();
		if(medId == j){
			delete graphSubMedList_.take(i);
			i=0;
		}
	}
}

void
qGraphMediator::RegisterEvents(void){
	
	connect(qEvents::Instance(), SIGNAL(GEUpdateAllDataTokens_()), this , SLOT(cbReload()));
	
	if(!connect(_gu, SIGNAL(evGraphUpdate(char *,int)), this, SLOT(cbGraphUpdate(char *,int)))){
			log_msg(0, "Didn't connect updater");
	}

	connect(_gd, SIGNAL(evRemove_()), this, SLOT(cbRemove()));
	connect(_gd, SIGNAL(evSave_()), this, SLOT(cbSave()));
	connect(_gd, SIGNAL(evHelp_()), this, SLOT(cbHelp()));
}
    

void
qGraphMediator::DetachEvents(void){  
    
}

/*
 * The help event is easy to handle.
 */
void qGraphMediator::cbHelp( void ) {
	 helpManager->showHelp(QString("%1.html").arg(dataToken_->getHelpPage()) );
}

/* This event is signaled when the model has been run, or for some other reason
 *  it is necessary to reload all graph data tokens.
 */
void qGraphMediator::cbReload( void ){
	QString cstr;

	cstr = dataToken_->getSourceStr();
	dataToken_->getSourceStr() = "";

	// Clear out saved "feature" name in data token, tell it to refill with
	//  that same "feature" name. Invalidate and repaint. (Should be same code
	//  as for cbGraphUpdate(...) below.

	if (!dataToken_->blockUpdate()) {
		if (!dataToken_->Fill( cstr ))
			return;  // should never happen
	}
	_gd->scaleUpdated();
	_gd->setTitleLabel(dataToken_->getGraphTitle().ascii());	
	_gd->setCaption(dataToken_->getGraphTitle());
	_gp->repaint();

}

void qGraphMediator::cbGraphUpdate(char *sourceName, int type){
	
    if(_gd->isActive() && dataToken_ && (dataToken_->getEventTrigger() == type)){
		
		if(!strcmp(sourceName,dataToken_->getSourceStr().ascii()))
			return;
		if (!dataToken_->Fill( sourceName ))
			return;  // should never happen
		_gd->scaleUpdated();
		_gd->setTitleLabel(dataToken_->getGraphTitle().ascii());	
		_gd->setCaption(dataToken_->getGraphTitle());
		_gp->repaint();
	}
}

void qGraphMediator::cbRemove(){
	_gp->setDataToken( NULL );
	_gd->setDataToken( NULL );
		emit evRemove(this);
	delete this;
}

/* These routines are called when someone pushes the "Save" button
 *  on the graph dialog. The desired operation is to save the data
 *  in a file in ascii format. The data token does the actual work
 *  so that derived classes thereof can be clever. */
void qGraphMediator::cbSave(){
	if (dataToken_)
		dataToken_->fileSave( _gd );
}


void qGraphMediator::DrawLabels(void){

	QColor *color;
	for(unsigned int i=0;i<dataToken_->legendArray_.count();i++){
			switch(i){
				case 0:
					color = new QColor("red");
					break;
				case 1:
					color = new QColor("blue");
					break;
				case 2:
					color = new QColor("blue");
					break;
				case 3:
					color = new QColor("black");
					break;
				case 4:
					color = new QColor("green");
					break;
				default:
					color = new QColor("red");
					break;
			}
			_gd->addLegendLabel(dataToken_->legendArray_[i].ascii(), color);
	}
	_gd->setTitleLabel(dataToken_->getGraphTitle().ascii());
	

}
