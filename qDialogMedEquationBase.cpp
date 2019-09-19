
#include "crisp.h"
#include "qCheckBoxGroup.h"
#include "qZabelSlider.h"
#include "qDialogMedEquationBase.h"
#include "qGraphDataTokenBuilder.h"
#include "qGraphDataTokenTypes.h"
#include "qGraphMedDrawLines.h"
#include "qGraphMedDrawAxisYD.h"
#include "qGraphMedDrawAxisXI.h"
#include "qGraphMedDrawText.h"
#include "qGraphMedPosition.h"
#include "qGraphMedZoom.h"
#include "Qcrisp.h"
#include <qstring.h>
#include <qmessagebox.h>
#include "qHelpManager.h"

/* NOTE this code assumes that min <= actual <= max on entry for all
 *  valid parameters of any equation. If this condition is met on entry,
 *  it will be maintained thereafter.
 */

qDialogMedEquationBase::qDialogMedEquationBase(qIdcEquationBase *idc,
												   qEquationDialogBase *dp,
												   const char *source)
				: qDialogMedBase(idc,dp), idcEquationBase_(idc), dp_(dp) {
	
    currSel0_ = -2;
    currSel1_ = -2;
    currSel2_ = -2;
	
	RegisterEvents();

	eqCurrent_ = NULL;
	eqFactory_ = qEquationFactory::instance();

	dialog_->addComboBox(0);
	dialog_->addComboBox(1);
	dialog_->addComboBox(2);
	addListItems( source );                     //fills dialog boxes
	dialog_->setCaption(idc->GetCaption());
	dialog_->setNumItems(idc->maxParams());
	dialog_->drawDialog(qDefRowHeight);
	dialog_->drawStandardLayout(Q_ZABELSLIDER);
	gp_ = dp->getGraphPane();


	setupGraph();

	lock_=1;
	listUpdate();
	lock_=0;

}

qDialogMedEquationBase::~qDialogMedEquationBase()
{
	graphSubMedList_.clear();
	if (eqCurrent_)
		delete eqCurrent_;
	delete dataToken_;
}

void qDialogMedEquationBase::setupGraph(){
	int i, id;
	qDatContainer *dat;

	/* Retrieve and display equation from underlying database.
	 */
	if(!(dat = idcEquationBase_->GetData(dialog_->getComboBoxText(0).ascii(),dialog_->getComboBoxText(1).ascii()))){
		QMessageBox::information(0,"getdata returned 0", "getdata returned 0",  QMessageBox::Ok);
		return;
	}
	dataToken_ = qGraphDataTokenBuilder::instance()->getToken(idcEquationBase_->GetDialogID(), dat);
	dataToken_->setIsEdit( 1 );
	gp_->setDataToken(dataToken_);
	eqCurrent_ = eqFactory_->BuildEquation(dat->getInt(0));
	

	/* Set initial selection in equation types combo box.
	 */
	id = dat->getInt(0);
	for (i = 0; i < idcEquationBase_->nEqTypes(); i++)
		if (id == idcEquationBase_->eqType(i)) {
			dialog_->selectComboBox(i,2);
			break;
		}

	insertSubMediator(new qGraphMedDrawLines(gp_));
	insertSubMediator(new qGraphMedDrawAxisYD(gp_));
	insertSubMediator(new qGraphMedDrawAxisXI(gp_));
	insertSubMediator(new qGraphMedDrawText(gp_));
	insertSubMediator(new qGraphMedZoom(gp_,NULL));
	insertSubMediator( new qGraphMedPosition( gp_, dp_ ) );
	gp_->repaint();
}

void qDialogMedEquationBase::insertSubMediator(qGraphSubMediator *msm){
	graphSubMedList_.append(msm);
}

void qDialogMedEquationBase::removeSubMediator(int medId){
	int j;
	for(size_t i=0;i<graphSubMedList_.count();i++){
		j = graphSubMedList_.at(i)->isA();
		if(medId == j){
			delete graphSubMedList_.take(i);
			i=0;
		}
	}
}

void qDialogMedEquationBase::cbControlRegistration(qItem *zi){

	qZabelSlider *zsl;
	QLabel *st;
	qDatContainer *dat;

	if(!lock_){
		lock_=1;
		if(!(dat = idcEquationBase_->GetData(dialog_->getComboBoxText(0),dialog_->getComboBoxText(1))))
			return;

		if(zi->groupId() == Q_CHECKBOXDELTA){  
			zsl = (qZabelSlider *)zi->w();
			zsl->setRange(dat->getDouble(1+3*zi->id()),dat->getDouble(2+3*zi->id()));
			zsl->setSliderValue(dat->getDouble(0+3*zi->id()));
			connect (zsl, SIGNAL(evDoubleChanged_(double, int)), this, SLOT(cbEditSliderDoubleModified(double, int)));			
			
			zsl->show();
		}
		else if(zi->groupId() == Q_STATICTEXT){  
			st = (QLabel *)zi->w();
			st->setText(dat->getString(3+zi->id()));
			st->show();
		}
		lock_=0;
	}

}

void qDialogMedEquationBase::cbEditSliderDoubleModified(double delta,  int id){
	if(!lock_){	
		lock_=1;
		idcEquationBase_->ModifyCache(
			id, 
			dialog_->isBoxChecked(id,1),
			dialog_->isBoxChecked(id,2), 
			delta, 
			dialog_->getComboBoxText(0),
			dialog_->getComboBoxText(1),
			delta_);
		updateGraph();
		lock_=0;
	}
}
void qDialogMedEquationBase::updateEquation(){
	qDatContainer *dat;
	if(!(dat = idcEquationBase_->GetData(dialog_->getComboBoxText(0),dialog_->getComboBoxText(1))))
		return;

    // Don't need to update equation if none of the comboboxes have changed.
    if (currSel0_ == dialog_->selectComboBox(-1,0) &&
        currSel1_ == dialog_->selectComboBox(-1,1) &&
        currSel2_ == dialog_->selectComboBox(-1,2))
        return;
	currSel0_ = dialog_->selectComboBox(-1,0);
	currSel1_ = dialog_->selectComboBox(-1,1);
	currSel2_ = dialog_->selectComboBox(-1,2);

	delete dataToken_;
	dataToken_ = qGraphDataTokenBuilder::instance()->getToken(idcEquationBase_->GetDialogID(), dat);
	dataToken_->setIsEdit( 1 );
	gp_->setDataToken(dataToken_);
	if (eqCurrent_)
		delete eqCurrent_;
	eqCurrent_ = eqFactory_->BuildEquation(dat->getInt(0));

}

void qDialogMedEquationBase::updateGraph(){
	qDatContainer *dat;
	if(!(dat = idcEquationBase_->GetData(dialog_->getComboBoxText(0),dialog_->getComboBoxText(1))))
		return;

	eqCurrent_->process(dat, dataToken_);
	if (dat->isDirty())
		dataToken_->setDirty();
	else
		dataToken_->clearDirty();

	gp_->repaint();
}

/* Set up combo boxes - list of dams, list of species, list of allowed
 *  equation types. "source" argument may be NULL, or may be the dam we
 *  want to be initially selected.
 */
void qDialogMedEquationBase::addListItems( const char *source ){
	int i, cb0Sel = 0;
    river_segment *seg;

	if(!lock_){
		lock_=1;
	    dialog_->clearComboBox(0);
		dialog_->clearComboBox(1);
		dialog_->clearComboBox(2);

		/* The species selection combobox is filled as required by the
		 *  particular IDC we are editing.
		 */
		if (idcEquationBase_->selBySpecies()) {
			for(i=0;i<num_specs;i++){
				dialog_->addComboBoxText(spec_names[i],1);
			}
		}
		if (idcEquationBase_->selByStock()) {
			for(i=0;i<stocks.num;i++) {
				dialog_->addComboBoxText(stocks.s[i]->name, 1);
			}
		}
		if (!(idcEquationBase_->selBySpecies()) && 
			!(idcEquationBase_->selByStock())) {
			dialog_->addComboBoxText( "all species", 1 );
			dialog_->showComboBox( FALSE, 1 );
		}

		/* The feature (dam) selection combobox is filled as required
		 *  by the particular IDC we are editing.
		 */
		if (idcEquationBase_->selByPowerhouse()) {
			for(i=0;i<num_powerhouses;i++){
				dialog_->addComboBoxText(phouses[i]->seg_name,0);
				if (source != NULL)
					if (!strcmp( phouses[i]->seg_name, source ))
						cb0Sel = i;
			}
		}
		else if (idcEquationBase_->selByDam()) {
			for(i=0;i<num_dams;i++){
				dialog_->addComboBoxText(dams[i]->seg_name,0);
				if (source != NULL)
					if (!strcmp( dams[i]->seg_name, source ))
						cb0Sel = i;
			}
		}
        else if (idcEquationBase_->selByReach()) {
    		for (i=0,seg = river_mouth; seg != NULL; seg = seg->next) {
			    if (seg->seg_type == REACH_SEG) {
				    dialog_->addComboBoxText(seg->seg_name,0);
				    if (source != NULL)
					    if (!strcmp( seg->seg_name, source ))
						    cb0Sel = i;
                    i++;
                }
            }
        }
        else if (idcEquationBase_->selByReachClass()) {
    		for (i=0; i < num_reach_classes; i++) {
				dialog_->addComboBoxText(reach_class_names[i],0);
				if (source != NULL)
					if (!strcmp( reach_class_names[i], source ))
						cb0Sel = i;
            }
            if (num_reach_classes < 2)
                dialog_->showComboBox( false, 0 );
        }
		else {
			dialog_->addComboBoxText( "all dams", 0 );
			dialog_->showComboBox( FALSE, 0 );
		}

		/* This list is a permanent, read-only part of the IDC.
		 */
		for (i = 0; i < idcEquationBase_->nEqTypes(); i++) {
			dialog_->addComboBoxText(
				find_default_eq( idcEquationBase_->eqType(i) )->formula, 2 );
		}
		if (idcEquationBase_->nEqTypes() <= 1)
			dialog_->showComboBox( FALSE, 2 );

		currSel0_ = dialog_->selectComboBox(cb0Sel,0);
		currSel1_ = dialog_->selectComboBox(0,1);
		currSel2_ = dialog_->selectComboBox(0,2);
		lock_=0;
	}
}

void qDialogMedEquationBase::listUpdate(){
	
	QPtrListIterator<qItem> * itr;
	qZabelSlider *es; 
	QLabel *st; 
	qItem *item;
	qDatContainer *dat;
	int i;

	if(!(itr = dialog_->getItemIterator()))return;
	if(dialog_->selectComboBox() < 0) goto cleanup;
	if(dialog_->selectComboBox(-1,1) < 0) goto cleanup;
	if(!(dat = idcEquationBase_->GetData(dialog_->getComboBoxText(0),dialog_->getComboBoxText(1)))){
		while((*itr).current() != 0){
			item = (*itr).current(); 
			if(item->groupId() == Q_CHECKBOXDELTA){
				es = (qZabelSlider *)(item->w());
				es->setShown(TRUE);
				es->show();
			}
			++(*itr);
		}
		goto cleanup;
	}
    /* Say where the divider between the two types of parameters is, if
     *  any.
     */
    dp_->setDividerParam( 
            idcEquationBase_->getDividerParam( dat->getInt(0) ) );

    /* Update all labels and interactive fields.
     */

	while((*itr).current() != 0){
		item = (*itr).current(); 

		if(item->groupId() == Q_CHECKBOXDELTA){
			es = (qZabelSlider *)(item->w());
			if (item->id() < dat->getInt(2)) {
				es->setRange(dat->getDouble(1+3*item->id()),dat->getDouble(2+3*item->id()));
				es->setSliderValue(dat->getDouble(0+3*item->id()));
				es->setShown(TRUE);
			}
			else
				es->setShown( FALSE );
		
		}
		else if(item->groupId() == Q_STATICTEXT){  
			st = (QLabel *)(item->w());
			if (item->id() < dat->getInt(2)) {
				st->setText(dat->getString(3+item->id()));
				st->setShown( TRUE );
			}
			else
				st->setShown(FALSE );
		}
		++(*itr);
	}

	/* Set equation selection combobox to correspond to "id" field of
	 *  data container.
	 */
	for (i = 0; i < idcEquationBase_->nEqTypes(); i++)
		if (dat->getInt(0) == idcEquationBase_->eqType(i))
			dialog_->selectComboBox( i, 2 );


	updateEquation();
	updateGraph();

cleanup:
	delete itr;
}

/* Something has happened to the combo box we are using to select the equation
 *  type. Get that equation type and put it into the IDC.
 */
void qDialogMedEquationBase::cbStaticComboBox3( int i){
	qDatContainer *dat, *ulDat;
	int isel, id, ulId;
	equation *def_eq;
	
//	QMessageBox::information(0,"combo3 changed", "equation alert",  QMessageBox::Ok);
	if (lock_)
		return;
	if(dialog_->selectComboBox() < 0)
		return;
	if(dialog_->selectComboBox(-1,1) < 0)
		return;
	if ((isel = dialog_->selectComboBox(-1,2)) < 0)
		return;
	if(!(dat = idcEquationBase_->GetData(dialog_->getComboBoxText(0),
		                                 dialog_->getComboBoxText(1))))
		return;

	id = idcEquationBase_->eqType(isel);

	if (id != dat->getInt(0)) {
        /* First, get the underlying data (without reference to any cached
         *  edits). See if we are changing back to the equation type of the
         *  underlying data. If so, do a "Reset" action. If not, proceed.
         */
        ulDat = idcEquationBase_->GetULData(dialog_->getComboBoxText(0),
		                                    dialog_->getComboBoxText(1));
        if (ulDat) {
            ulId = ulDat->getInt(0);
            delete ulDat;
            if (id == ulId) {
                cbReset( );
                return;
            }
        }

		/* Set equation type, and set default value, min, and max for all
		 *  parameters.
		 */
		if ((def_eq = find_default_eq( id )) == NULL)
			return;

		idcEquationBase_->GetEquationData( def_eq, dat );
		dat->setDirty();
		idcEquationBase_->invalidate();
	}
}

void qDialogMedEquationBase::cbReset(){
	qDatContainer *dat;

	if(!lock_){
		lock_=1;

		if(dialog_->selectComboBox() < 0)
			return;
		if(dialog_->selectComboBox(-1,1) < 0)
			return;
		if(!(dat = idcEquationBase_->GetData(dialog_->getComboBoxText(0),
											 dialog_->getComboBoxText(1))))
			return;

		idc_->ClearCache( dat );
		listUpdate();
		lock_=0;
	}
}

void qDialogMedEquationBase::cbApply(){
	qDatContainer *dat;

	if(!lock_){
		lock_=1;

		if(dialog_->selectComboBox() < 0)
			return;
		if(dialog_->selectComboBox(-1,1) < 0)
			return;
		if(!(dat = idcEquationBase_->GetData(dialog_->getComboBoxText(0),
											 dialog_->getComboBoxText(1))))
			return;

		idc_->ApplyData( dat );
		lock_=0;
	    idcEquationBase_->invalidate();
	}
}

void qDialogMedEquationBase::cbApplyAll(){
	if(!lock_){
		lock_=1;
		idc_->ApplyCache();
		lock_=0;
	    idcEquationBase_->invalidate();
	}
}

void qDialogMedEquationBase::cbHelp(){

	helpManager->showHelp( QString("%1.html").arg(dataToken_->getHelpPage()) );
}
