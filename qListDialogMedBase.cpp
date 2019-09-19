#include "qCheckBoxGroup.h"
#include "qEditSliderBase.h"
#include "qZabelSlider.h"
#include "qListDialogMedBase.h"
#include "Qcrisp.h"
#include "qHelpManager.h"

qListDialogMedBase::qListDialogMedBase(qIdcListBase *idc, 
												qListDialogBase *dialog,
												const char *title) {

	dialog_ = dialog;
	idcListBase_ = idc;
	lock_ = 0;
	luLock_ = 0;
	delta_ = 0;
	dialog_->setCaption( title );

	/* TODO what do these two things do and what should they be set to? */
	cbColumn_ = 1;
	activeTracking_ = 0;

	/* This prods the IDC to initialize itself if needs be. This cannot be
	 *  done by the IDC constructor because static objects are constructed
	 *  before the database is read in.
	 */
	idcListBase_->Setup();

	/* Now initialize the dialog.
	 */
	dialog_->setNumRows( idcListBase_->getNumRow() );
	dialog_->setNumColumns( idcListBase_->getNumColumn() );
	RegisterEvents();
	dialog_->drawDialog( 0 );	
	listUpdate();
}

qListDialogMedBase::~qListDialogMedBase() {
//	unRegisterEvents();
}


void qListDialogMedBase::RegisterEvents(void){

	connect(dialog_, SIGNAL(evApply_()), this, SLOT(cbApply()));
	connect(dialog_, SIGNAL(evCancel_()), this ,SLOT(cbCancel()));
	connect(dialog_, SIGNAL(evOK_()), this, SLOT(cbOK()));
	connect(dialog_, SIGNAL(evReset_()), this ,SLOT(cbReset()));
	connect(dialog_, SIGNAL(evHelp_()), this, SLOT(cbHelp()));
	connect(dialog_, SIGNAL(evControlRegistration_(qItem*)), this,SLOT(cbControlRegistration(qItem*)));
	connect(dialog_, SIGNAL(evTabSelected_(int)), this, SLOT(cbTabSelected(int)));
	connect(idcListBase_, SIGNAL(evInvalidate_()), this, SLOT(cbInvalidate()));

}

void qListDialogMedBase::cbCheckAllModified(bool checked, int id, int box){
	unsigned int i;
	
	if(!lock_){	
		lock_=1;
		QPtrListIterator<qItem> *itr;
		qCheckBoxGroup *cbg;
		qItem *item;
		itr = dialog_->getItemIterator();
		if (itr == NULL)
			goto unlock;
		while((*itr).current() != 0){
			item = itr->current();
			if(item->id() == Q_CHECKBOXALL){
				cbg = (qCheckBoxGroup *)(item->w());
				cbg->setItem(box,checked);
			}
			if(item->groupId() == Q_CHECKBOXALL){
				cbg = (qCheckBoxGroup *)(item->w());
				if(cbg->isEnabled())cbg->setItem(box,checked);
			}
			++(*itr);
		}
		for( i=0;i < dialog_->numRows();i++)
			dialog_->setCheckBox(checked,i);

		delete itr;
unlock:
		lock_=0;
	}
}

void qListDialogMedBase::cbCheckBoxModified(bool checked, int id, int box){
	if(!lock_){	
		lock_=1;
		//Find out which tab is the current one and set the marker for this data as checked;
		dialog_->setCheckBox(checked,id + dialog_->currentIndex()
											* dialog_->numRowsPerTab());
		lock_=0;
	}
}

void qListDialogMedBase::cbDeltaModified(bool checked, int id, int box){
	if(!lock_){	
		lock_=1;
		delta_ = checked;
		QPtrListIterator<qItem> *itr;
		qEditSliderBase *es;
		qItem *item;
		itr = dialog_->getItemIterator();
		if (itr == NULL)
			goto unlock;
		while(itr->current() != 0 ){
			item = itr->current();
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
void qListDialogMedBase::cbControlRegistration(qItem *zi){

	qCheckBoxGroup *cbg;
	qZabelSlider *zsl;
	QLabel *st;

	switch(zi->id()){
		case Q_CHECKBOXALL:
			cbg = (qCheckBoxGroup *)zi->w();
			connect(cbg, SIGNAL(command(bool , int , int )), this, SLOT(cbCheckAllModified(bool,int,int)));
			break;
		case Q_CHECKBOXDELTA:
			cbg = (qCheckBoxGroup *)zi->w();
			connect(cbg, SIGNAL(command(bool , int , int )), this, SLOT(cbDeltaModified(bool,int,int)));
			break;
	}
	//We're assuming that if the group type is Q_CHECKBOXDELTA, you're using a qZabelSlider. If you use some 
	//other base class than qZabelSlider you'll need to override this control registration 
	//function in your derived mediator.
	if(zi->groupId() == Q_CHECKBOXDELTA){  
		zsl = (qZabelSlider *)zi->w();
		connect(zsl, SIGNAL(evDoubleChanged_(double, int)), this, SLOT(cbEditSliderDoubleModified(double, int)));
		zsl->setRange( idcListBase_->minVal(), idcListBase_->maxVal() );
		zsl->show();
	}
	else if(zi->groupId() == Q_STATICTEXT){  
		st = (QLabel *)zi->w();
		st->setText(idcListBase_->getRowLabel(zi->id()));
		st->show();
	}
	else if (zi->groupId() == Q_LABELTEXT) {
		st = (QLabel *)zi->w();
		st->setText( idcListBase_->getColumnLabel( zi->id() ) );
		st->show();
	}

	if(zi->groupId() == Q_CHECKBOXALL){
		cbg = (qCheckBoxGroup *)zi->w();
		connect(cbg, SIGNAL(command(bool,int,int )), this, SLOT(cbCheckBoxModified(bool,int,int)));
	}
	lock_=0;
	return;
}

void qListDialogMedBase::listUpdate(){
	
	QPtrListIterator<qItem> *itr;
	qZabelSlider *es; 
	qCheckBoxGroup *cbg;
	qItem *item;
	qDatContainer *dat;
	QLabel *st;
	int id;
	bool check;

	if (luLock_ == 0) {
		luLock_ = 1;

		if(!(itr = dialog_->getItemIterator()))goto unlock;
		if(!(dat = idcListBase_->GetData())){
			while((*itr).current() != 0){
				item = (*itr).current(); 
				if(item->groupId() == Q_CHECKBOXDELTA){
					es = (qZabelSlider *)(item->w());
					es->setShown(false);
				}
				else if(item->groupId() == Q_CHECKBOXALL){
					cbg = (qCheckBoxGroup *)(item->w());
					cbg->setShown(false);
				}
				else if (item->groupId() == Q_STATICTEXT || 
						  item->groupId() == Q_LABELTEXT) {
					st = (QLabel *)(item->w());
					st->setShown( false );
				}
				++(*itr);
			}
			goto cleanup;
		}

		while((*itr).current() != 0){
			item = (*itr).current() ; 
			++(*itr);
			if(item->groupId() == Q_CHECKBOXDELTA){
				es = (qZabelSlider *)(item->w());
				id = item->id() + dialog_->currentIndex() 
									* dialog_->numRowsPerTab() 
									* dialog_->numColumns();
				if (id >= idcListBase_->getNumRow()
									* idcListBase_->getNumColumn())
					es->setShown( false );
				else {
                    if (idcListBase_->minValu(id)
                        != idcListBase_->maxValu(id))
		                es->setRange( idcListBase_->minValu(id),
                                      idcListBase_->maxValu(id) );
					es->setShown( true);
					es->setSliderValueNN(dat->getDouble(id));
					es->show();
					
				}
			}
			else if(item->groupId() == Q_CHECKBOXALL){
				cbg = (qCheckBoxGroup *)(item->w());
				id = item->id() + dialog_->currentIndex()
											* dialog_->numRowsPerTab();
				if (id >= idcListBase_->getNumRow())
					cbg->setShown( false );
				else {
					check = dialog_->isBoxChecked( id );
					cbg->setItem( 0, check );
					cbg->setShown( true);
					cbg->show();
				}
			}
			else if (item->groupId() == Q_STATICTEXT) {
				st = (QLabel *)(item->w());
				id = item->id() + dialog_->currentIndex()
											* dialog_->numRowsPerTab();
				if (id >= idcListBase_->getNumRow())
					st->setShown( false );
				else {
					st->setText(idcListBase_->getRowLabel(id));
					st->setShown( true);
					st->show( );
				}
			}
			else if (item->groupId() == Q_LABELTEXT) {
				// we believe that column labels never change.
				st = (QLabel *)(item->w());
				st->show( );
			}
			
		}

		if (dat->isDirty())
			dialog_->showDirty();
		else
			dialog_->showDirty( false );

cleanup:
		delete itr;
unlock:
		luLock_ = 0;
	}
}

void qListDialogMedBase::cbEditSliderDoubleModified( double delta, int i) {

	qDatContainer *dat = idcListBase_->GetData();
	int id, j;

	id = i + dialog_->currentIndex() * dialog_->numRowsPerTab() 
								* dialog_->numColumns();

	// First check on ganged update, if so, do it directly to data container.
	if (dialog_->isBoxChecked( id / dialog_->numColumns() ) ) {
		for (j = 0; j < idcListBase_->getNumRow(); j++)
			if (dialog_->isBoxChecked( j )) {
				dat->setDouble( j * dialog_->numColumns()
											+ id % dialog_->numColumns(),
								delta );
			}
	}

	/* Modify cache unconditionally - signals the Invalidate event and causes
	 *  repainting.
	 */
	idcListBase_->ModifyCache( id, delta );
}

void qListDialogMedBase::cbHelp(){
    if(!lock_){
		lock_=1;
		helpManager->showHelp( QString("%1.html").arg(idcListBase_->getHelpPage()) );
		lock_=0;
	}
}

void qListDialogMedBase::cbApply(){
	if(!lock_){
		lock_=1;
		idcListBase_->ApplyCache();
		lock_=0;
		idcListBase_->invalidate();
	}
}

void qListDialogMedBase::cbOK(){
	if(!lock_){
		lock_=1;
		idcListBase_->ApplyCache();
		idcListBase_->invalidate();
		lock_=0;
	}

	/* This is the only notification we will ever get that our underlying
	 *  window object is going away. So deleting the mediator machinery
	 *  is unconditional. */
	emit evRemove_(this);
	delete this;
}

void qListDialogMedBase::cbCancel(){
	if(!lock_){
		lock_=1;
		idcListBase_->ClearCache();
		lock_=0;
	}

	/* This is the only notification we will ever get that our underlying
	 *  window object is going away. So deleting the mediator machinery
	 *  is unconditional. */
//	unRegisterEvents();
	emit evRemove_(this);
	delete this;
}

void qListDialogMedBase::cbReset(){
	if(!lock_){
		lock_=1;
		idcListBase_->ClearCache();
		listUpdate();
		lock_=0;
	}
}

void qListDialogMedBase::cbTabSelected(int i){
	if(!lock_){
		lock_=1;
		listUpdate();
		lock_=0;
	}
}

void qListDialogMedBase::cbInvalidate(){
	if(!lock_){
		lock_=1;
	    listUpdate();
		lock_=0;
	}
}



