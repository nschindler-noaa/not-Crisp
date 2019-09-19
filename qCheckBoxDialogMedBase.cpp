#include "qCheckBoxGroup.h"
#include "qCheckBoxDialogMedBase.h"
#include "Qmain.h"
#include "Qcrisp.h"
#include "qHelpManager.h"

qCheckBoxDialogMedBase::qCheckBoxDialogMedBase(qIdcListBase *idc, 
												qCheckBoxDialogBase *dialog,
												const char *title) : QObject(dialog) {

	dialog_ = dialog;
	idcListBase_ = idc;
	lock_ = 0;
	luLock_ = 0;
	dialog_->setCaption( title );

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

qCheckBoxDialogMedBase::~qCheckBoxDialogMedBase() {
	
}


void qCheckBoxDialogMedBase::RegisterEvents(void){

	if( !(
	connect(dialog_, SIGNAL(evApply()), this, SLOT(cbApply())) 
	&& connect(dialog_, SIGNAL(evCancel()), this, SLOT(cbCancel()))
	&& connect(dialog_, SIGNAL(evOK()), this, SLOT(cbOK()))
	&& connect(dialog_, SIGNAL(evReset()), this, SLOT(cbReset()))
	&& connect(dialog_, SIGNAL(evHelp()), this, SLOT(cbHelp()))
	&& connect(dialog_, SIGNAL(evControlRegistration(qItem *)), this, SLOT(cbControlRegistration(qItem *)))
	&& connect(dialog_, SIGNAL(evTabSelected(int)), this, SLOT(cbTabSelected(int))) 
	&& connect(idcListBase_, SIGNAL(evInvalidate_()), this, SLOT(cbInvalidate())) ))
	{
		log_msg(0, "Error connecting evInvalidate() signals in CheckBoxDialogMedBase");
	}


}

void qCheckBoxDialogMedBase::unRegisterEvents(void){

}

/* This virtual routine is called when a box is checked or unchecked. It handles
 *  any interactions with other check boxes. The default, base class behavior
 *  assumes that each row is a radio button group. Derive a class to 
 *  override this behavior.
 */
int qCheckBoxDialogMedBase::doBoxInteractions( int row, int column, bool check ) {
	qDatContainer *dat;
	int base, j, nc;

	nc = idcListBase_->getNumColumn();
	int rows = idcListBase_->getNumRow();
	base = row * nc;
	dat = idcListBase_->GetData();
	
	
	if (!check) {
		/* Box has been unchecked - make sure one other box is checked.
		 */
		for (j = 0; j < nc; j++)
			if (j != column && dat->getDouble( base + j ) > 0.5)
				return 0;
		j = (column + 1) % nc;
		dat->setDouble( base + j, 1 );
	}
	else {
		/* Box has been checked - uncheck all other boxes in row.
		 */
		for (j = 0; j < nc; j++)
			if (j != column)
				dat->setDouble( base + j, 0 );
	}

	return 0;
}

void qCheckBoxDialogMedBase::cbCheckBoxModified(bool checked, int id, int box){
	int r,c;
	if(!lock_){	
		lock_=1;
		/* Modify the underlying IDC (which will update the dialog).
		 *  Before that, call a virtual function to implement any interactions
		 *  between buttons.
		 */
		r = id + dialog_->currentIndex() * dialog_->numRowsPerTab();
		c = box;
		int override = doBoxInteractions( r, c, checked );

		/* NOTE - remove lock first, so that dialog can be updated.
		 */
		lock_=0;
		// Undo the change by the user if requested
		if (!override) idcListBase_->ModifyCache(
						r * idcListBase_->getNumColumn() + c, checked );	
		else idcListBase_->ModifyCache(
						r * idcListBase_->getNumColumn() + c, !checked );
	}
}

void qCheckBoxDialogMedBase::cbControlRegistration(qItem *zi){

	qCheckBoxGroup *cbg;
	QLabel *st;

	if(zi->groupId() == Q_STATICTEXT){  
		st = (QLabel *)zi->w();
		st->setText(idcListBase_->getRowLabel(zi->id()));
		st->show();
	}
	else if (zi->groupId() == Q_LABELTEXT) {
		st = (QLabel *)zi->w();
		st->setText( idcListBase_->getColumnLabel( zi->id() ) );
		st->show();
	}

	if(zi->groupId() == Q_CHECKBOXGROUP){
		cbg = (qCheckBoxGroup *)zi->w();
		connect(cbg, SIGNAL(command(bool, int, int)), this, SLOT( cbCheckBoxModified(bool,int,int)));	
	}

	lock_=0;
	return;
}

void qCheckBoxDialogMedBase::listUpdate(){
	
	//rwItemIterator *itr;
	QPtrListIterator<qItem> *itr;
	qCheckBoxGroup *cbg;
	qItem *item;
	qDatContainer *dat;
	QLabel *st;
	int id, check, j;

	if (luLock_ == 0) {
		luLock_ = 1;

		if(!(itr = dialog_->getItemIterator()))
			goto unlock;
		if(!(dat = idcListBase_->GetData())){
			while((*itr).current() != 0 ){
				item = (*itr).current() ; 
				if(item->groupId() == Q_CHECKBOXGROUP){
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
			item = (*itr).current(); 
			++(*itr);
			if(item->groupId() == Q_CHECKBOXGROUP){
				cbg = (qCheckBoxGroup *)(item->w());
				id = item->id() + dialog_->currentIndex()
											* dialog_->numRowsPerTab();
				if (id >= idcListBase_->getNumRow())
					cbg->setShown( false );
				else {
					for (j = 0; j < (int)(dialog_->numColumns()); j++) {
						check = (int)(dat->getDouble(
									id * dialog_->numColumns() + j ) + 0.5);
						cbg->setItem( j, check );
					}
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
					st->show();
				}
			}
			else if (item->groupId() == Q_LABELTEXT) {
				// we believe that column labels never change.
				st = (QLabel *)(item->w());
				st->show();
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

void qCheckBoxDialogMedBase::cbHelp(){
    if(!lock_){
		lock_=1;
		helpManager->showHelp( QString("%1.html").arg(idcListBase_->getHelpPage()) );
		lock_=0;
	}
}

void qCheckBoxDialogMedBase::cbApply(){
	if(!lock_){
		lock_=1;
		idcListBase_->ApplyCache();
		lock_=0;
        idcListBase_->invalidate();
	}
}

void qCheckBoxDialogMedBase::cbOK(){
	if(!lock_){
		lock_=1;
		idcListBase_->ApplyCache();
        idcListBase_->invalidate();
		lock_=0;
	}

	/* This is the only notification we will ever get that our underlying
	 *  window object is going away. So deleting the mediator machinery
	 *  is unconditional. */
	unRegisterEvents();
	emit evRemove_(this);
	delete this;
}

void qCheckBoxDialogMedBase::cbCancel(){
	if(!lock_){
		lock_=1;
		idcListBase_->ClearCache();
		lock_=0;
	}

	/* This is the only notification we will ever get that our underlying
	 *  window object is going away. So deleting the mediator machinery
	 *  is unconditional. */
	unRegisterEvents();
	emit evRemove_(this);
	delete this;
}

void qCheckBoxDialogMedBase::cbReset(){
	if(!lock_){
		lock_=1;
		idcListBase_->ClearCache();
		listUpdate();
		lock_=0;
	}
}

void qCheckBoxDialogMedBase::cbTabSelected(int i){
	if(!lock_){
		lock_=1;
		listUpdate();
		lock_=0;
	}
}

void qCheckBoxDialogMedBase::cbInvalidate(){
	if(!lock_){
		lock_=1;
	    listUpdate();
		lock_=0;
	}
}
/*
void qCheckBoxDialogMedBase::_cbCheckBoxModified(void *msg, const int box, const int checked, const int id){
	qCheckBoxDialogMedBase *dm = (qCheckBoxDialogMedBase *)msg;
	dm->cbCheckBoxModified(box, checked,id);
}
void qCheckBoxDialogMedBase::_cbInvalidate(void *msg){
    qCheckBoxDialogMedBase *zm = (qCheckBoxDialogMedBase *)msg; 
    zm->cbInvalidate();
}
void qCheckBoxDialogMedBase::_cbApply(void *msg, const int i){
    qCheckBoxDialogMedBase *zm = (qCheckBoxDialogMedBase *)msg; 
    zm->cbApply(i);
}
void qCheckBoxDialogMedBase::_cbOK(void *msg, const int i){
    qCheckBoxDialogMedBase *zm = (qCheckBoxDialogMedBase *)msg;
    zm->cbOK(i);
}
void qCheckBoxDialogMedBase::_cbCancel(void *msg, const int i){
    qCheckBoxDialogMedBase *zm = (qCheckBoxDialogMedBase *)msg;
    zm->cbCancel(i);
}
void qCheckBoxDialogMedBase::_cbReset(void *msg, const int i){
    qCheckBoxDialogMedBase *zm = (qCheckBoxDialogMedBase *)msg;
    zm->cbReset(i);
}
void qCheckBoxDialogMedBase::_cbTabSelected(void *msg, const int i){
    qCheckBoxDialogMedBase *zm = (qCheckBoxDialogMedBase *)msg;
    zm->cbTabSelected(i);
}
void qCheckBoxDialogMedBase::_cbHelp(void *msg, const int i){
    qCheckBoxDialogMedBase *zm = (qCheckBoxDialogMedBase *)msg;
    zm->cbHelp(i);
}
void qCheckBoxDialogMedBase::_cbControlRegistration(void *msg, qItem *zi){
    qCheckBoxDialogMedBase *zm = (qCheckBoxDialogMedBase *)msg;
    zm->cbControlRegistration(zi);
}
*/



