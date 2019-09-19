
#include "qCheckBoxGroup.h"
#include "qEditSliderBase.h"
#include "qZabelSlider.h"
#include "qSelectDialogMedBase.h"
#include "Qcrisp.h"
#include <qptrlist.h>
#include "qHelpManager.h"

qSelectDialogMedBase::qSelectDialogMedBase(qIdcSelectBase *idc, 
												qSelectDialogBase *dialog,
												const char *title) {

	int i;
	dialog_ = dialog;
	idcSelectBase_ = idc;
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
	idcSelectBase_->Setup();

	/* Now initialize the dialog.
	 */
	dialog_->setNumRows( idcSelectBase_->getNumRow() );
	dialog_->setNumColumns( idcSelectBase_->getNumColumn() );
	for (i = 0; i < idcSelectBase_->getNumFeature(); i++)
		dialog_->addComboBoxText( idcSelectBase_->getFeature(i), 0 );
	dialog_->selectComboBox( 0, 0 );
	if (dialog_->numCbgCols() == 1)
		dialog_->hideComboBox( 1 );
	for(i = 0; i < idcSelectBase_->getNumSpecies(); i++)
		dialog_->addComboBoxText( idcSelectBase_->getSpecies(i), 1 );
	dialog_->selectComboBox( 0, 1 );
    
    // If we have one "feature" in combobox 0 and a bunch of "species"
    //  in combobox 1, we are in a different situation: rows are assumed
    //  to be river features.
    rowsAreFeatures_ = idcSelectBase_->getNumFeature() <= 1 &&
                            dialog_->numCbgCols() > 1 &&
                            idcSelectBase_->getNumSpecies() > 1;
    if (rowsAreFeatures_)
        dialog_->hideComboBox( 0 );

    if (idcSelectBase_->getNumFeature() < 2)
        dialog_->hideComboBox( 0 );
    if (idcSelectBase_->getNumSpecies() < 2)
        dialog_->hideComboBox( 1 );

    RegisterEvents();
	dialog_->drawDialog( 0 );
	listUpdate();
}

qSelectDialogMedBase::~qSelectDialogMedBase() {
	unRegisterEvents();
}


void qSelectDialogMedBase::RegisterEvents(){
	connect(dialog_, SIGNAL(evApply(int)), this, SLOT(cbApply(int)));
	connect(dialog_, SIGNAL(evApplyAll(int)), this, SLOT(cbApplyAll(int)));
	connect(dialog_, SIGNAL(evCancel(int)), this, SLOT(cbCancel(int)));
	connect(dialog_, SIGNAL(evOK(int)), this, SLOT(cbOK(int)));
	connect(dialog_, SIGNAL(evReset(int)), this, SLOT(cbReset(int)));
	connect(dialog_, SIGNAL(evResetAll(int)), this, SLOT(cbResetAll(int)));
	connect(dialog_, SIGNAL(evHelp(int)), this, SLOT(cbHelp(int)));
	connect(dialog_, SIGNAL(evControlRegistration(qItem*)), this, SLOT(cbControlRegistration(qItem*)));
	connect(dialog_, SIGNAL(evTabSelected(int)), this, SLOT(cbTabSelected(int)));
	connect(dialog_, SIGNAL(evCmbStatic1(int)), this, SLOT(cbComboSelected(int)));
	connect(dialog_, SIGNAL(evCmbStatic2(int)), this, SLOT(cbComboSelected(int)));
	connect(idcSelectBase_, SIGNAL(evInvalidate_()), this, SLOT(cbInvalidate()));


}

void qSelectDialogMedBase::unRegisterEvents(void){

}

void qSelectDialogMedBase::cbCheckAllModified( bool checked,  int id,  int box){
	int i;
	

	if(!lock_){	
		//log_msg(0, "Check box all called");
		lock_=1;
		QPtrListIterator <qItem> *itr;
		qCheckBoxGroup *cbg;
		qItem *item;
		itr = dialog_->getItemIterator();
		if (itr == NULL)
			goto unlock;
		while(itr->current() !=  0){
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
		for(i=0;(size_t)i<dialog_->numRows();i++)
			dialog_->setCheckBox(box,checked,i);

		delete itr;
unlock:
		lock_=0;
	}
}

void qSelectDialogMedBase::cbCheckBoxModified( bool checked, int id, int box){
	if(!lock_){	
		lock_=1;
		//Find out which tab is the current one and set the marker for this data as checked;
		dialog_->setCheckBox(box,checked,id + dialog_->currentIndex()
											* dialog_->numRowsPerTab());
		lock_=0;
	}
}

void qSelectDialogMedBase::cbDeltaModified( bool checked,  int id, int box){
	if(!lock_){	
		lock_=1;
		delta_ = checked;
		QPtrListIterator<qItem> *itr;
		qEditSliderBase *es;
		qItem *item;
		itr = dialog_->getItemIterator();
		if (itr == NULL)
			goto unlock;
		while(itr->current() !=  0){
			item = itr->current();
			++(*itr);
			if(item->groupId() == Q_CHECKBOXDELTA){
				es = (qEditSliderBase *)(item->w());
				es->setDelta(checked);
 
			}
		}

		delete itr;
unlock:
		lock_=0;
	}
}
void qSelectDialogMedBase::cbControlRegistration(qItem *zi){

	qCheckBoxGroup *cbg;
	qZabelSlider *zsl;
	QLabel *st;

	switch(zi->id()){
		case Q_CHECKBOXALL:
			cbg = (qCheckBoxGroup *)zi->w();
			connect(cbg, SIGNAL(command(bool , int , int )), this , SLOT(cbCheckAllModified(bool, int, int)));
			
			//cbg->cbgRegisterEvent()->attach(this, &qSelectDialogMedBase::_cbCheckAllModified);
			break;
		case Q_CHECKBOXDELTA:
			cbg = (qCheckBoxGroup *)zi->w();
			connect(cbg, SIGNAL(command(bool state, int id, int box )), this , SLOT(cbDeltaModified(bool, int, int)));
			//cbg->cbgRegisterEvent()->attach(this, &qSelectDialogMedBase::_cbDeltaModified);
			break;
	}
	//We're assuming that if the group type is Q_CHECKBOXDELTA, you're using a qZabelSlider. If you use some 
	//other base class than qZabelSlider you'll need to override this control registration 
	//function in your derived mediator.
	if(zi->groupId() == Q_CHECKBOXDELTA){  
		zsl = (qZabelSlider *)zi->w();
		connect(zsl, SIGNAL(evDoubleChanged_(double, int)), this , SLOT(cbEditSliderDoubleModified(double, int)));
		zsl->setRange( idcSelectBase_->minVal(), idcSelectBase_->maxVal() );
		zsl->show();
	}
	else if(zi->groupId() == Q_STATICTEXT){  
		st = (QLabel *)zi->w();
		st->setText(idcSelectBase_->getRowLabel(zi->id()));
		st->show();
	}
	else if (zi->groupId() == Q_LABELTEXT) {
		st = (QLabel *)zi->w();
		st->setText( idcSelectBase_->getColumnLabel( zi->id() ) );
		st->show();
	}

	if(zi->groupId() == Q_CHECKBOXALL){
		cbg = (qCheckBoxGroup *)zi->w();
		connect(cbg, SIGNAL(command(bool , int , int )), this , SLOT(cbCheckBoxModified(bool, int, int)));
		
	}

	lock_=0;
	return;
}

void qSelectDialogMedBase::listUpdate(){
	
	QPtrListIterator <qItem>*itr;
	qZabelSlider *es; 
	qCheckBoxGroup *cbg;
	qItem *item;
	qDatContainer *dat;
	QLabel *st;
	int id, check;

	if (luLock_ == 0) {
		luLock_ = 1;

		if(!(itr = dialog_->getItemIterator()))goto unlock;
		if(!(dat = idcSelectBase_->GetData( dialog_->getComboBoxText(0).ascii(),
			                                dialog_->getComboBoxText(1).ascii() ))){
			while(itr->current() !=  0){
				item = itr->current(); 
				++(*itr);
				if(item->groupId() == Q_CHECKBOXDELTA){
					es = (qZabelSlider *)(item->w());
					es->setShown(FALSE);
				}
				else if(item->groupId() == Q_CHECKBOXALL){
					cbg = (qCheckBoxGroup *)(item->w());
					cbg->setShown(FALSE);
				}
				else if (item->groupId() == Q_STATICTEXT || 
						  item->groupId() == Q_LABELTEXT) {
					st = (QLabel *)(item->w());
					st->setShown( FALSE );
				}
			}
			goto cleanup;
		}

		while(itr->current() !=  0){
			item = itr->current(); 
			++(*itr);
			if(item->groupId() == Q_CHECKBOXDELTA){
				es = (qZabelSlider *)(item->w());
				id = item->id() + dialog_->currentIndex() 
									* dialog_->numRowsPerTab() 
									* dialog_->numColumns();
				if (id >= idcSelectBase_->getNumRow()
									* idcSelectBase_->getNumColumn())
					es->setShown( FALSE );
				else {
					es->setSliderValueNN(dat->getDouble(id));
					es->show();
				}
			}
			else if(item->groupId() == Q_CHECKBOXALL){
				cbg = (qCheckBoxGroup *)(item->w());
				id = item->id() + dialog_->currentIndex()
											* dialog_->numRowsPerTab();
				if (id >= idcSelectBase_->getNumRow())
					cbg->setShown( FALSE );
				else {
					check = dialog_->isBoxChecked( 0, id );
					cbg->setItem( 0, check );
					if (dialog_->numCbgCols() > 1) {
						check = dialog_->isBoxChecked( 1, id );
						cbg->setItem( 1, check );
					}
					cbg->show();
				}
			}
			else if (item->groupId() == Q_STATICTEXT) {
				st = (QLabel *)(item->w());
				id = item->id() + dialog_->currentIndex()
											* dialog_->numRowsPerTab();
				if (id >= idcSelectBase_->getNumRow())
					st->setShown( FALSE );
				else {
					st->setText(idcSelectBase_->getRowLabel(id));
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
			dialog_->showDirty( FALSE );

cleanup:
		delete itr;
unlock:
		luLock_ = 0;
	}

}

void qSelectDialogMedBase::cbEditSliderDoubleModified( double delta,
													   int i) {
	int id, idd, j;

	id = i + dialog_->currentIndex() * dialog_->numRowsPerTab() 
								* dialog_->numColumns();
	idd = i / dialog_->numColumns()
				+ dialog_->currentIndex() * dialog_->numRowsPerTab();

    // If rows are representing river features, we are in a different
    //  situation than if they are not.
    if (rowsAreFeatures_) {
        // Modify cache for current row, maybe ganged over species.
	    idcSelectBase_->ModifyCache( dialog_->getComboBoxText(0).ascii(),
			                         dialog_->getComboBoxText(1).ascii(),
								     id, delta, 0,
								     dialog_->isBoxChecked( 1, idd ) );

        // If "F" box checked, loop over all rows and modify those whose
        //  "F" boxes are also checked.
        if (dialog_->isBoxChecked( 0, idd )) {
		    for (j = 0; j < idcSelectBase_->getNumRow(); j++)
			    if (j != idd && dialog_->isBoxChecked( 0, j )) {
	                idcSelectBase_->ModifyCache(
                             dialog_->getComboBoxText(0).ascii(),
			                 dialog_->getComboBoxText(1).ascii(),
						     (id % dialog_->numColumns())
                                       + (j * dialog_->numColumns()),
                             delta, 0,
						     dialog_->isBoxChecked( 1, j ) );
                }
        }
    }
    else if (idcSelectBase_->getNumSpecies() < 2) {
        /* A special case for things known per species per reach class
         *  (only uses the "feature" combobox, the "species" combobox
         *  is hidden.
         */

        // Modify cache for current row, maybe ganged over features.
	    idcSelectBase_->ModifyCache( dialog_->getComboBoxText(0).ascii(),
			                         dialog_->getComboBoxText(1).ascii(),
								     id, delta,
                                     dialog_->isBoxChecked( 0, idd ), 0 );

        // Perhaps gang over rows of dialog. 
        if (dialog_->isBoxChecked( 1, idd )) {
		    for (j = 0; j < idcSelectBase_->getNumRow(); j++)
			    if (j != idd && dialog_->isBoxChecked( 1, j )) {
	                idcSelectBase_->ModifyCache(
                             dialog_->getComboBoxText(0).ascii(),
			                 dialog_->getComboBoxText(1).ascii(),
						     (id % dialog_->numColumns())
                                       + (j * dialog_->numColumns()),
                             delta,
                             dialog_->isBoxChecked( 0, j ), 0 );
                }
        }
    }
    else {
	    /* Modify cache unconditionally - signals the Invalidate event and causes
	     *  repainting.
	     */
	    idcSelectBase_->ModifyCache( dialog_->getComboBoxText(0).ascii(),
			                         dialog_->getComboBoxText(1).ascii(),
								     id, delta, 
								     dialog_->isBoxChecked( 0, idd ),
								     dialog_->isBoxChecked( 1, idd ) );
    }
}

void qSelectDialogMedBase::cbHelp(int i){
    if(!lock_){
		lock_=1;
		helpManager->showHelp( QString("%1.html").arg(idcSelectBase_->getHelpPage()) );
		lock_=0;
	}
}

void qSelectDialogMedBase::cbApply(int i){
	qDatContainer *dat;
	
	if(!lock_){
		if(dat = idcSelectBase_->GetData( dialog_->getComboBoxText(0).ascii(),
										dialog_->getComboBoxText(1).ascii() )) {
    		lock_=1;
			idcSelectBase_->ApplyData( dat );
    		lock_=0;
			idcSelectBase_->invalidate();
		}
	}
}

void qSelectDialogMedBase::cbApplyAll(int i){
	if(!lock_){
		lock_=1;
		idcSelectBase_->ApplyCache();
		lock_=0;
		idcSelectBase_->invalidate();
	}
}

void qSelectDialogMedBase::cbOK(int i){
	if(!lock_){
		lock_=1;
		idcSelectBase_->ApplyCache();
		idcSelectBase_->invalidate();
		lock_=0;
	}

	/* This is the only notification we will ever get that our underlying
	 *  window object is going away. So deleting the mediator machinery
	 *  is unconditional. */
	unRegisterEvents();
	emit evRemove();
	delete this;
}

void qSelectDialogMedBase::cbCancel(int i){
	if(!lock_){
		lock_=1;
		idcSelectBase_->ClearCache();
		lock_=0;
	}

	/* This is the only notification we will ever get that our underlying
	 *  window object is going away. So deleting the mediator machinery
	 *  is unconditional. */
	unRegisterEvents();
	emit evRemove();
	delete this;
}

void qSelectDialogMedBase::cbReset(int i){
	qDatContainer *dat;
	
	if(!lock_){
		lock_=1;
		if(dat = idcSelectBase_->GetData( dialog_->getComboBoxText(0).ascii(),
									dialog_->getComboBoxText(1).ascii() )) {
			idcSelectBase_->ClearCache( dat );
			listUpdate();
		}
		lock_=0;
	}
}

void qSelectDialogMedBase::cbResetAll(int i){
	if(!lock_){
		lock_=1;
		idcSelectBase_->ClearCache();
		listUpdate();
		lock_=0;
	}
}

void qSelectDialogMedBase::cbTabSelected(int i){
	if(!lock_){
		lock_=1;
		listUpdate();
		lock_=0;
	}
}

void qSelectDialogMedBase::cbComboSelected(int i){
	if(!lock_){
		lock_=1;
		listUpdate();
		lock_=0;
	}
}

void qSelectDialogMedBase::cbInvalidate(){
	if(!lock_){
		lock_=1;
	    listUpdate();
		lock_=0;
	}
}

void qSelectDialogMedBase::cbPurge(){

	//Do nothing, subclasses override

}
