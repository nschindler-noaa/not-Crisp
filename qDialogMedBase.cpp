
#include "qCheckBoxGroup.h"
#include "qEditSliderBase.h"
#include "qEditSliderDouble.h"
#include "qEditSliderLong.h"
#include "qDialogMedBase.h"
#include "Qcrisp.h"
#include <qmessagebox.h>
#include "qHelpManager.h"

qDialogMedBase::qDialogMedBase(qInterfaceDataControl *idc,
							   qDialogBase *dialog) : QObject(dialog)  {
	dialog_ = dialog;
	idc_ = idc;
	lock_ = 0;
	delta_ = 0;
	cbColumn_ = 1;
	activeTracking_ = 0;
}

qDialogMedBase::~qDialogMedBase(){

}


void qDialogMedBase::RegisterEvents(void){
	connect(dialog_, SIGNAL(evApply_()), this, SLOT(cbApply()));
	connect(dialog_, SIGNAL(evApplyAll_()), this, SLOT(cbApplyAll()));
	connect(dialog_, SIGNAL(evCancel_(int)), this, SLOT(cbCancel(int)));
	connect(dialog_, SIGNAL(evOK_()), this, SLOT(cbOK()));
	connect(dialog_, SIGNAL(evReset_()), this, SLOT(cbReset()));
	connect(dialog_, SIGNAL(evResetAll_()), this, SLOT(cbResetAll()));
	connect(dialog_, SIGNAL(evHelp_()), this, SLOT(cbHelp()));
	connect(dialog_, SIGNAL(evControlRegistration_(qItem*)), this, SLOT(cbControlRegistration(qItem*)));
	connect(dialog_, SIGNAL(evCmbStatic1_(int)), this, SLOT(cbStaticComboBox1(int)));
	connect(dialog_, SIGNAL(evCmbStatic2_(int)), this, SLOT(cbStaticComboBox2(int)));
	connect(dialog_, SIGNAL(evCmbStatic3_(int)), this, SLOT(cbStaticComboBox3(int)));
	connect(idc_, SIGNAL(evInvalidate_()), this, SLOT(cbInvalidate()));
/*
	dialog_->evApply()->attach(this, &qDialogMedBase::_cbApply);
	dialog_->evApplyAll()->attach(this, &qDialogMedBase::_cbApplyAll);
    dialog_->evCancel()->attach(this, &qDialogMedBase::_cbCancel);
    dialog_->evOK()->attach(this, &qDialogMedBase::_cbOK);
	dialog_->evReset()->attach(this, &qDialogMedBase::_cbReset);
	dialog_->evResetAll()->attach(this, &qDialogMedBase::_cbResetAll);
	dialog_->evHelp()->attach(this, &qDialogMedBase::_cbHelp);
	dialog_->evControlRegistration()->attach(this, &qDialogMedBase::_cbControlRegistration);
	dialog_->evCmbStatic1()->attach(this, &qDialogMedBase::_cbStaticComboBox1);
	dialog_->evCmbStatic2()->attach(this, &qDialogMedBase::_cbStaticComboBox2);
	dialog_->evCmbStatic3()->attach(this, &qDialogMedBase::_cbStaticComboBox3);
	idc_->evInvalidate()->attach(this, &qDialogMedBase::_cbInvalidate);
*/
}
void qDialogMedBase::unRegisterEvents(void){
	/*
	dialog_->evApply()->detach(this, &qDialogMedBase::_cbApply);
	dialog_->evApplyAll()->detach(this, &qDialogMedBase::_cbApplyAll);
    dialog_->evCancel()->detach(this, &qDialogMedBase::_cbCancel);
    dialog_->evOK()->detach(this, &qDialogMedBase::_cbOK);
	dialog_->evReset()->detach(this, &qDialogMedBase::_cbReset);
	dialog_->evResetAll()->detach(this, &qDialogMedBase::_cbResetAll);
	dialog_->evHelp()->detach(this, &qDialogMedBase::_cbHelp);
	dialog_->evControlRegistration()->detach(this, &qDialogMedBase::_cbControlRegistration);
	dialog_->evCmbStatic1()->detach(this, &qDialogMedBase::_cbStaticComboBox1);
	dialog_->evCmbStatic2()->detach(this, &qDialogMedBase::_cbStaticComboBox2);
	dialog_->evCmbStatic3()->detach(this, &qDialogMedBase::_cbStaticComboBox3);
	idc_->evInvalidate()->detach(this, &qDialogMedBase::_cbInvalidate);
*/
}

void qDialogMedBase::cbCheckAllModified( bool checked, int id,int box){
	int i;
	
	if(!lock_){	
		lock_=1;
		QPtrListIterator<qItem> *itr;
		qCheckBoxGroup *cbg;
		qItem *item;
		itr = dialog_->getItemIterator();
		if (itr == NULL)
			goto unlock;
		while(itr->current()){
			item = itr->current();
			if(item->groupId() == Q_CHECKBOXALL){
				cbg = (qCheckBoxGroup *)(item->w());
				if(cbg->isEnabled())cbg->setItem(box,checked);
			}
			++(*itr);
		}
		for(i=0;(size_t)i<dialog_->numItems();i++)
			dialog_->qDialogBase::setCheckBox(box,checked,i);

		delete itr;
unlock:
		lock_=0;
	}
}

void qDialogMedBase::cbCheckBoxModified( bool checked, int id,int box){
	if(!lock_){	
		lock_=1;
		//Find out which tab is the current one and set the marker for this data as checked;
		dialog_->setCheckBox(box,checked,id);
		lock_=0;
	}
}

void qDialogMedBase::cbDeltaModified( bool checked, int id,int box){
	if(!lock_){	
		lock_=1;
		delta_ = checked;
		//rwItemIterator *itr;
		QPtrListIterator<qItem> * itr;
		qEditSliderBase *es;
		qItem *item;
		itr = dialog_->getItemIterator();
		if (itr == NULL)
			goto unlock;
		while(itr->current() != 0 ){
			item = (*itr).current();
			if(item->groupId() == Q_CHECKBOXDELTA){
				es = (qEditSliderBase *)(item->w());
				es->setDelta(checked);
			}
			++(*itr);
		}

		delete itr;
unlock:
		lock_=0;
	}
}
void qDialogMedBase::cbControlRegistration(qItem *zi){

	qCheckBoxGroup *cbg;
	qEditSliderBase *esb;
	qEditSliderLong *esl;
	qEditSliderDouble *esd;

	switch(zi->id()){
		case Q_CHECKBOXALL:
			cbg = (qCheckBoxGroup *)zi->w();
			//cbg->cbgRegisterEvent()->attach(this, &qDialogMedBase::_cbCheckAllModified);
			connect(cbg, SIGNAL(command( bool , int ,int )), this , SLOT(cbCheckAllModified( bool , int ,int )));
			break;
		case Q_CHECKBOXDELTA:
			cbg = (qCheckBoxGroup *)zi->w();
			//cbg->cbgRegisterEvent()->attach(this, &qDialogMedBase::_cbDeltaModified);
			connect(cbg, SIGNAL(command( bool , int ,int )), this , SLOT(cbDeltaModified(bool , int ,int)));
			break;
	}
	//We're assuming that if the group type is Q_CHECKBOXDELTA, you're using a qEditSlider. If you use some 
	//other base class than qEditSliderBase you'll need to override this control registration 
	//function in your derived mediator.
	if(zi->groupId() == Q_CHECKBOXDELTA){  
		esb = (qEditSliderBase *)zi->w();
		switch(esb->editSliderType()){
			case Q_DOUBLEEDITSLIDER:
				esd = (qEditSliderDouble *)zi->w();
				//esd->evDoubleChanged()->attach(this, &qDialogMedBase::_cbEditSliderDoubleModified);
				connect(esd, SIGNAL(evDoubleChanged(double, int)), this, SLOT(cbEditSliderDoubleModified(double, int)));
				esd->setActiveUpdates(activeTracking_);
				break;
			case Q_LONGEDITSLIDER:
				esl = (qEditSliderLong *)zi->w();
				//esl->evLongChanged()->attach(this, &qDialogMedBase::_cbEditSliderLongModified);
				connect(esl, SIGNAL(evLongChanged(long, int)), this, SLOT(cbEditSliderLongModified(long, int)));
				esl->setActiveUpdates(activeTracking_);
				break;
		}
		return;
	}
	if(zi->groupId() == Q_CHECKBOXALL){
		cbg = (qCheckBoxGroup *)zi->w();
		//cbg->cbgRegisterEvent()->attach(this, &qDialogMedBase::_cbCheckBoxModified);
		connect(cbg, SIGNAL(cbgRegisterEvent(bool , int ,int)), this, SLOT(cbCheckBoxModified(bool , int ,int)));
	}
}

void qDialogMedBase::cbHelp(){
    if(!lock_){
		lock_=1;
		helpManager->showHelp( QString("%1.html").arg(idc_->getHelpPage()) );
	
		lock_=0;
	}
}

void qDialogMedBase::cbApply(){
	if(!lock_){
		lock_=1;
		idc_->ApplyCache();
		lock_=0;
        idc_->invalidate();
	}
}

void qDialogMedBase::cbApplyAll(){
	if(!lock_){
		lock_=1;
		idc_->ApplyCache();
		lock_=0;
        idc_->invalidate();
	}
}

void qDialogMedBase::cbOK(){
	if(!lock_){
		lock_=1;
		
		idc_->ApplyCache();
		idc_->invalidate();
		lock_=0;
	}

	/* This is the only notification we will ever get that our underlying
	 *  window object is going away. So deleting the mediator machinery
	 *  is unconditional. */
	//unRegisterEvents();
	emit evRemove_(this);
	delete this;
}

void qDialogMedBase::cbCancel(int i ){
	if(!lock_){
		lock_=1;
		idc_->ClearCache();
		lock_=0;
	}

	/* This is the only notification we will ever get that our underlying
	 *  window object is going away. So deleting the mediator machinery
	 *  is unconditional. */
	//unRegisterEvents();
	emit evRemove_(this);
	delete this;
}

void qDialogMedBase::cbReset(){
	if(!lock_){
		lock_=1;
		idc_->ClearCache();
		listUpdate();
		lock_=0;
	}
}

void qDialogMedBase::cbResetAll(){
	if(!lock_){
		lock_=1;
		idc_->ClearCache();
		listUpdate();
		lock_=0;
	}
}

void qDialogMedBase::cbStaticComboBox1( int i){
	if(!lock_){
		lock_=1;
	    listUpdate();
		lock_=0;
	}
}

void qDialogMedBase::cbStaticComboBox2(int i){
	if(!lock_){
		lock_=1;
	    listUpdate();
		lock_=0;
	}
}

void qDialogMedBase::cbStaticComboBox3( int i){
	
	if(!lock_){
		lock_=1;
	    listUpdate();
		
		lock_=0;
	}
}

void qDialogMedBase::cbInvalidate(){
	if(!lock_){
		lock_=1;
	    listUpdate();
		lock_=0;
	}
}


