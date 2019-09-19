//qDialogMedEquationBase
//
//Purpose:
//	This is the base class for "Equation dialog" mediators. 

#ifndef _qDialogMedEquationBase_h_
#define _qDialogMedEquationBase_h_

#include "qDialogMedBase.h"
#include "qEquationDialogBase.h"
#include "qIdcEquationBase.h"

#include "qEquationBase.h"
#include "qEquationFactory.h"

#include "qGraphUpdateMed.h"
#include "qGraphPane.h"
#include "qGraphDataToken.h"
#include "qGraphSubMediator.h" 
#include <qptrlist.h>

class qDialogMedEquationBase : public qDialogMedBase {
	

    
	qEquationBase *eqCurrent_;
	qEquationFactory *eqFactory_;
	qIdcEquationBase *idcEquationBase_;


	qGraphPane *gp_;
	qGraphDataToken *dataToken_;

	qEquationDialogBase *dp_;
	
	QPtrList<qGraphSubMediator> graphSubMedList_;
	
	int _isData, _lock, _graphType, _isDisabled, _ref;

    // Keep track of current combo box selections so that equations
    //  are not flushed too often.
    int currSel0_, currSel1_, currSel2_;

  public:
    
    qDialogMedEquationBase(qIdcEquationBase *idc, qEquationDialogBase *dp,
		                     const char *source );
    virtual ~qDialogMedEquationBase();

	void insertSubMediator(qGraphSubMediator *msm);
	void removeSubMediator(int medId);

	virtual void setupGraph();
	virtual void listUpdate();
	virtual void addListItems( const char *source );
	virtual void updateGraph();
	virtual void updateEquation();
  
    virtual void cbApply();
    virtual void cbApplyAll();
	virtual void cbReset();
	virtual void cbHelp();
	virtual void cbControlRegistration(qItem *zi);
	virtual void cbEditSliderDoubleModified(double delta, int id);
    virtual void cbStaticComboBox3( int i);
    
};


#endif

