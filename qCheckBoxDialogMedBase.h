#ifndef _qCheckBoxDialogMedBase_h_
#define _qCheckBoxDialogMedBase_h_

#include "qIdcListBase.h"
#include "qCheckBoxDialogBase.h"


class qCheckBoxDialogMedBase : public QObject{

	Q_OBJECT
    
  protected:


    qCheckBoxDialogBase *dialog_;
	int lock_, luLock_, ref_, helpPage_, delta_, cbColumn_, activeTracking_;
	qIdcListBase *idcListBase_;

  public:
    
    qCheckBoxDialogMedBase(qIdcListBase *idc, 
							qCheckBoxDialogBase *dialog, const char *title);
    virtual ~qCheckBoxDialogMedBase();

	

	virtual void listUpdate();
    virtual void RegisterEvents(void);
	void unRegisterEvents(void);

	/* This virtual routine is called when a box is checked or unchecked. 
	 *  It handles any interactions with other check boxes. The default, 
	 *  base class behavior assumes that each row is a radio button 
	 *  group. Derive a class to override this behavior.
	 *  Returns 1 if the user's selection should be overridden and ignored
	 */
	virtual int doBoxInteractions( int row, int column, bool check );

  protected slots:
	  
	virtual void cbApply();
    virtual void cbOK();
    virtual void cbCancel();
	virtual void cbReset();
	virtual void cbHelp();

	virtual void cbTabSelected(int i);
	virtual void cbControlRegistration(qItem *zi);
	virtual void cbCheckBoxModified( bool check, int id, int box);	
	virtual void cbInvalidate();

  signals:
		void evRemove_(qCheckBoxDialogMedBase *);


};

#endif





