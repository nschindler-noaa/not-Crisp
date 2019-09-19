/* qScheduleDialogMedBase.h
 *  This is the base class for mediators which handle a qScheduleDialogBase
 *  and an idcScheduleBase - to edit PERIOD_LIST objects from the underlying
 *  database.
 */

#ifndef _qScheduleDialogMedBase_h_
#define _qScheduleDialogMedBase_h_

#include "qIdcScheduleBase.h"
#include "qScheduleDialogBase.h"
#include <qobject.h>

class qScheduleDialogMedBase : QObject{

	Q_OBJECT

    
  protected:

	
    qScheduleDialogBase *dialog_;
	int lock_, ref_;
	qIdcScheduleBase *idcScheduleBase_;

  public:
    
    qScheduleDialogMedBase(qIdcScheduleBase *idc, 
							qScheduleDialogBase *dialog, const char *title, const char *feature = NULL);
    virtual ~qScheduleDialogMedBase();

	//signalScheduleDialog *evRemove(){ return &evRemove_; }
	//inline int ref(){ return ++ref_; }
	//inline int unref(){ return --ref_; }

    virtual void RegisterEvents(void);
	void unRegisterEvents(void);
	//int operator==(const qScheduleDialogMedBase &ref){ 
	//	if(&ref == this)
	//		return 1;
	//	return 0;
	//}

	/* Update dialog from current state of IDC.
	 */
	void updateDialog( int l1, int l2 );

	/* Event handlers
	 */
  protected slots:
    virtual void cbApply();
    virtual void cbApplyAll();
    virtual void cbOK();
    virtual void cbCancel();
	virtual void cbReset();
	virtual void cbResetAll();
	virtual void cbHelp();

	virtual void cbComboSelected( int i );
	virtual void cbList1Selected( int i );
	virtual void cbList2Selected( int i );
	virtual void cbLeftarrow();
	virtual void cbInvalidate();

signals:
	 void evRemove_();

};

//typedef utSmartPtr<qScheduleDialogMedBase> spqScheduleDialogMedBase;

#endif





