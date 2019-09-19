#ifndef _qDialogMedBase_h_
#define _qDialogMedBase_h_


#include "qInterfaceDataControl.h"
#include "qDialogBase.h"
#include <qobject.h>

class qDialogMedBase;

class qDialogMedBase : public QObject{
    
	Q_OBJECT 

  protected:

	//signalDialog evRemove_;
    qDialogBase *dialog_;
	int lock_, ref_, helpPage_, delta_, cbColumn_, activeTracking_;
	qInterfaceDataControl *idc_;

  public:
    
    qDialogMedBase(qInterfaceDataControl *idc, qDialogBase *dialog);
    virtual ~qDialogMedBase();

	//signalDialog *evRemove(){ return &evRemove_; }
	//inline int ref(){ return ++ref_; }
	//inline int unref(){ return --ref_; }

	virtual void listUpdate()=0;
    virtual void RegisterEvents(void);
	void unRegisterEvents(void);
//	int operator==(const qDialogMedBase &ref){ 
//		if(&ref == this)
//			return 1;
//		return 0;
//	}

public slots:

    virtual void cbApply( );
    virtual void cbApplyAll( );
    virtual void cbOK( );
    virtual void cbCancel( int i);
	virtual void cbReset( );
	virtual void cbResetAll( );
	virtual void cbHelp( );

	virtual void cbControlRegistration(qItem *zi);
	virtual void cbEditSliderLongModified( long delta,  int i){}
	virtual void cbEditSliderDoubleModified( double delta,  int i){}
	virtual void cbDeltaModified(bool checked, int id, int box);	
	virtual void cbCheckAllModified( bool checked, int id, int box);	
	virtual void cbCheckBoxModified( bool checked, int id, int box);	
    virtual void cbStaticComboBox1( int );
    virtual void cbStaticComboBox2( int );
    virtual void cbStaticComboBox3( int );
	virtual void cbInvalidate();

signals:
	void evRemove_(qDialogMedBase *);

/*
    static void _cbApply(void *msg, const int i);
    static void _cbApplyAll(void *msg, const int i);
    static void _cbOK(void *msg, const int i);
    static void _cbCancel(void *msg, const int i);
    static void _cbReset(void *msg, const int i);
    static void _cbResetAll(void *msg, const int i);
	static void _cbHelp(void *msg, const int i);

	static void _cbControlRegistration(void *msg, ZatItem *zi);
	static void _cbEditSliderLongModified(void *msg, const long delta, const int i);
	static void _cbEditSliderDoubleModified(void *msg, const double delta, const int i);
	static void _cbDeltaModified(void *msg, const int box, const int checked, const int id);
	static void _cbCheckBoxModified(void *msg, const int box, const int checked, const int id);
	static void _cbCheckAllModified(void *msg, const int box, const int checked, const int id);
    static void _cbStaticComboBox1(void *msg, const int i);
    static void _cbStaticComboBox2(void *msg, const int i);
    static void _cbStaticComboBox3(void *msg, const int i);
	static void _cbInvalidate( void *msg );
	*/

};

//typedef utSmartPtr<ZatDialogMedBase> spZatDialogMedBase;

#endif





