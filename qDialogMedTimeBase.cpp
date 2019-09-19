//#include <zapp.hpp>
#include "crisp.h"
#include "qCheckBoxGroup.h"
#include "qZabelSlider.h"
#include "qDialogMedTimeBase.h"
#include <qptrlist.h>

qDialogMedTimeBase::qDialogMedTimeBase(qIdcTimeBase *idc,
										   qDialogBase *dp, char *title) 
		: qDialogMedBase(idc,dp), df_(0), idcTimeBase_(idc) {
	dialog_->setCaption( title );
	RegisterEvents();

	dialog_->setNumItems(DAM_TIME_SLC);
	dialog_->addBoxName("T"); //Box name must be set AFTER the number of items has been set (numItems_).
	dialog_->addBoxName("D");
	dialog_->addBoxName("S");
	dialog_->drawDialog(qDefRowHeight);

	dialog_->drawStandardLayout(Q_ZABELSLIDER);

	dialog_->showDelta( false );

	dialog_->addComboBox(0);
	dialog_->addComboBox(1);
	
	addListItems();

	//Now set the dialog to an appropriate size:
	dialog_->resize(100,100);
	lock_=1;
	listUpdate();
	lock_=0;
	
	
}

void qDialogMedTimeBase::cbControlRegistration(qItem *zi){

	qCheckBoxGroup *cbg;
	qZabelSlider *zsl;
	QLabel *st;
	char buf[40];

	switch(zi->id()){
		case Q_CHECKBOXALL:
			cbg = (qCheckBoxGroup *)zi->w();
			connect(cbg,SIGNAL(command(bool,int,int)),this,SLOT(cbCheckAllModified(bool,int,int)));
			break;
		case Q_CHECKBOXDELTA:
			cbg = (qCheckBoxGroup *)zi->w();
			connect(cbg,SIGNAL(command(bool,int,int)),this,SLOT(cbDeltaModified(bool,int,int)));
			break;
	}
	if(zi->groupId() == Q_CHECKBOXDELTA){  
		zsl = (qZabelSlider *)zi->w();
		//zsl->evDoubleChanged()->attach(this, &qDialogMedBase::_cbEditSliderDoubleModified);
		connect(zsl, SIGNAL(evDoubleChanged_(double, int)), this, SLOT(cbEditSliderDoubleModified(double, int)));
		zsl->setRange( idcTimeBase_->minVal(), idcTimeBase_->maxVal() );
		return;
	}
	if(zi->groupId() == Q_STATICTEXT){  
		st = (QLabel *)zi->w();
		sprintf( buf, "%d:00-%d:00", zi->id()*(24/DAM_TIME_SLC),
                                    (zi->id()+1)*(24/DAM_TIME_SLC) );
		st->setText(buf);
		st->show();
		return;
	}
	if(zi->groupId() == Q_CHECKBOXALL){
		cbg = (qCheckBoxGroup *)zi->w();
		//cbg->cbgRegisterEvent()->attach(this, &qDialogMedBase::_cbCheckBoxModified);
		connect(cbg, SIGNAL( command(bool,int,int)),this,SLOT(cbCheckBoxModified(bool,int,int)));

	}
}

void qDialogMedTimeBase::cbEditSliderDoubleModified(double delta, int id){
	if(!lock_){	
		lock_=1;
		if(!dialog_->isBoxChecked(id,0)){
			idcTimeBase_->ModifyCache(
				id, 
				dialog_->isBoxChecked(id,1),
				dialog_->isBoxChecked(id,2), 
				delta, 
				dialog_->getComboBoxText(0),
				dialog_->getComboBoxText(1),
				delta_);
			dialog_->showDirty();
		}
		else{
			for(int i=0;(unsigned)i<dialog_->numItems();i++)
				if(dialog_->qDialogBase::isBoxChecked(i,0))
					idcTimeBase_->ModifyCache(
						i, 
						dialog_->qDialogBase::isBoxChecked(i,1),
						dialog_->qDialogBase::isBoxChecked(i,2), 
						delta, 
						dialog_->getComboBoxText(0),
						dialog_->getComboBoxText(1),
						delta_);
			listUpdate();
		}
		lock_=0;
	}
}


void qDialogMedTimeBase::addListItems(){
	if(!lock_){
		lock_=1;
	    dialog_->clearComboBox(0);
		dialog_->clearComboBox(1);
		int i = 0;
		if (!(idcTimeBase_->getNumFeature())) {
			for(i=0;i<num_specs;i++)
				idcTimeBase_->InsertSpecies(spec_names[i]);
			for(i=0;i<num_dams;i++)
				idcTimeBase_->InsertFeature(dams[i]->seg_name);
		}
		
		for(i=0;i<num_specs;i++)
			dialog_->addComboBoxText(spec_names[i],1);
		for(i=0;i<num_dams;i++)
			dialog_->addComboBoxText(dams[i]->seg_name,0);

		dialog_->selectComboBox(0,0);
		dialog_->selectComboBox(0,1);
		lock_=0;
	}
}

void qDialogMedTimeBase::listUpdate(){
	
	//rwItemIterator *itr;
	QPtrListIterator<qItem> *itr;
	qZabelSlider *es; 
	qCheckBoxGroup *cbg;
	qItem *item;
	qDatContainer *dat;

	if(!(itr = dialog_->getItemIterator()))return;
	if(dialog_->selectComboBox() < 0) goto cleanup;
	if(dialog_->selectComboBox(-1,1) < 0) goto cleanup;
	if(!(dat = idcTimeBase_->GetData(dialog_->getComboBoxText(0),dialog_->getComboBoxText(1)))){
		while((*itr).current() != 0){
			item = itr->current(); 
			if(item->groupId() == Q_CHECKBOXDELTA){
				es = (qZabelSlider *)(item->w());
				es->setShown(false);
			}
			else if(item->groupId() == Q_CHECKBOXALL){
				cbg = (qCheckBoxGroup *)(item->w());
				cbg->setShown(false);
			}
			++(*itr);
		}
		goto cleanup;
	}
	while((*itr).current() != 0){
		item = itr->current(); 
		if(item->groupId() == Q_CHECKBOXDELTA){
			es = (qZabelSlider *)(item->w());
			es->setShown(true);
			es->setSliderValue(dat->getDouble(item->id()));
		}
		else if(item->groupId() == Q_CHECKBOXALL){
			cbg = (qCheckBoxGroup *)(item->w());
			cbg->setShown(true);
			cbg->setItem(2,dialog_->isBoxChecked(item->id(),2));
			cbg->setItem(1,dialog_->isBoxChecked(item->id(),1));
			cbg->setItem(0,dialog_->isBoxChecked(item->id(),0));
		}
		++(*itr);
	}

	if (dat->isDirty())
		dialog_->showDirty();
	else
		dialog_->showDirty( false);

cleanup:
	delete itr;
}

void qDialogMedTimeBase::cbReset(){
	qDatContainer *dat;

	if(!lock_){
		lock_=1;

		if(dialog_->selectComboBox() < 0)
			return;
		if(dialog_->selectComboBox(-1,1) < 0)
			return;
		if(!(dat = idcTimeBase_->GetData(dialog_->getComboBoxText(0),
											 dialog_->getComboBoxText(1))))
			return;

		idcTimeBase_->ClearCache( dat );
		listUpdate();
		lock_=0;
	}
}

void qDialogMedTimeBase::cbApply(){
	qDatContainer *dat;

	if(!lock_){
		lock_=1;

		if(dialog_->selectComboBox() < 0)
			return;
		if(dialog_->selectComboBox(-1,1) < 0)
			return;
		if(!(dat = idcTimeBase_->GetData(dialog_->getComboBoxText(0),
											 dialog_->getComboBoxText(1))))
			return;

		idcTimeBase_->ApplyData( dat );
		lock_=0;
	    idcTimeBase_->invalidate();
	}
}

void qDialogMedTimeBase::cbApplyAll(){
	if(!lock_){
		lock_=1;
		idcTimeBase_->ApplyCache();
		lock_=0;
	    idcTimeBase_->invalidate();
	}
}
