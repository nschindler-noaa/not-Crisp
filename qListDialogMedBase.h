#ifndef _qListDialogMedBase_h_
#define _qListDialogMedBase_h_

//#include <help/zhelp.hpp>
#include "qIdcListBase.h"
#include "qListDialogBase.h"
//#include "smart_ptr.h"

//class qListDialogMedBase;

//typedef utImplicit1<qListDialogMedBase *> signalListDialog;

class qListDialogMedBase : public QObject{

	Q_OBJECT
    
  protected:

	//signalListDialog evRemove_;
    qListDialogBase *dialog_;
	int lock_, luLock_, ref_, helpPage_, delta_, cbColumn_, activeTracking_;
	qIdcListBase *idcListBase_;

  public:
    
    qListDialogMedBase(qIdcListBase *idc, 
							qListDialogBase *dialog, const char *title);
    virtual ~qListDialogMedBase();

	//signalListDialog *evRemove(){ return &evRemove_; }
	//inline int ref(){ return ++ref_; }
	//inline int unref(){ return --ref_; }

	virtual void listUpdate();
    virtual void RegisterEvents(void);
	//void unRegisterEvents(void);
	//int operator==(const qListDialogMedBase &ref){ 
	//	if(&ref == this)
	//		return 1;
	//	return 0;
	//}

  protected slots:
    virtual void cbApply();
    virtual void cbOK();
    virtual void cbCancel();
	virtual void cbReset();
	virtual void cbHelp();

	virtual void cbTabSelected(  int i );
	virtual void cbControlRegistration(qItem *zi);
	virtual void cbEditSliderDoubleModified( double delta, int i);
	virtual void cbDeltaModified( bool checked, int id, int box);	
	virtual void cbCheckAllModified( bool checked, int id, int box);	
	virtual void cbCheckBoxModified(bool checked, int id, int box);	
	virtual void cbInvalidate();
signals:
	void evRemove_(qListDialogMedBase* );

/*
    static void _cbApply(void *msg, const int i);
    static void _cbOK(void *msg, const int i);
    static void _cbCancel(void *msg, const int i);
    static void _cbReset(void *msg, const int i);
	static void _cbHelp(void *msg, const int i);

	static void _cbTabSelected( void *msg, const int i );
	static void _cbControlRegistration(void *msg, qItem *zi);
	static void _cbEditSliderDoubleModified(void *msg, const double delta, const int i);
	static void _cbDeltaModified(void *msg, const int box, const int checked, const int id);
	static void _cbCheckBoxModified(void *msg, const int box, const int checked, const int id);
	static void _cbCheckAllModified(void *msg, const int box, const int checked, const int id);
	static void _cbInvalidate( void *msg );
*/

};

//typedef utSmartPtr<qListDialogMedBase> spqListDialogMedBase;

#endif






