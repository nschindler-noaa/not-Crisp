#ifndef _qSelectDialogMedBase_h_
#define _qSelectDialogMedBase_h_

#include "qIdcSelectBase.h"
#include "qSelectDialogBase.h"

class qSelectDialogMedBase : public QObject {

	Q_OBJECT
    
  protected:
	
    qSelectDialogBase *dialog_;
	int lock_, luLock_, ref_, helpPage_, delta_, cbColumn_, activeTracking_;
	qIdcSelectBase *idcSelectBase_;

    // If we have one "feature" in combobox 0 and a bunch of "species"
    //  in combobox 1, we are in a different situation: rows are assumed
    //  to be river features.
    int rowsAreFeatures_;

  public:
    
    qSelectDialogMedBase(qIdcSelectBase *idc, 
							qSelectDialogBase *dialog, const char *title);
    virtual ~qSelectDialogMedBase();

	//signalSelectDialog *evRemove(){ return &evRemove_; }

	virtual void listUpdate();
    virtual void RegisterEvents(void);
	void unRegisterEvents(void);

  public slots:
    virtual void cbApply( int i);
    virtual void cbApplyAll( int i);
    virtual void cbOK( int i);
    virtual void cbCancel( int i);
	virtual void cbReset( int i);
	virtual void cbResetAll( int i);
	virtual void cbHelp( int i);

	virtual void cbTabSelected(  int i );
	virtual void cbComboSelected(  int i );
	virtual void cbControlRegistration(qItem *zi);
	virtual void cbEditSliderDoubleModified( double delta,  int i);
	virtual void cbDeltaModified( bool checked, int id, int box);	
	virtual void cbCheckAllModified( bool checked, int id, int box);	
	virtual void cbCheckBoxModified( bool checked, int id, int box);	
	virtual void cbInvalidate();
	virtual void cbPurge(); //for the subclasses

signals:
	void evRemove();

};

#endif





