/* ZatEditGraphMediator.h
 *  Main mediator for graphs which contain a single, editable line of data.
 *  Editing is by left-button drag-draw or by opening a "Schedule Tool"
 *  dialog.
 */

#ifndef __QEDITGRAPHMEDIATOR_H__
#define __QEDITGRAPHMEDIATOR_H__

#ifdef WIN32
/* TODO: handle this issue more elegantly. */
#pragma warning( disable: 4284 )
/* warning C4284: return type for 'std::reverse_iterator... */
#endif

#include "qGraphMediator.h"
#include "qEditGraphDataToken.h"
#include "qScheduleTool.h"

class qEditGraphMediator : public qGraphMediator {

	Q_OBJECT

protected:
	/* same pointer as ZatGraphMediator::dataToken_, but gives access 
	 *  to derived class methods. */
	qEditGraphDataToken *edt_;
	qScheduleTool *sched_;

public:
	qEditGraphMediator(qGraphDialog *gd, qEditGraphDataToken *edt);
	virtual ~qEditGraphMediator();

	// event methods.
	virtual void RegisterEvents(void);
	virtual void DetachEvents(void);

public slots:
	virtual void cbOK();
	//static void _cbOK(void *msg);
	virtual void cbCancel();
	//static void _cbCancel(void *msg);
	virtual void cbApply();
	//static void _cbApply(void *msg);
	virtual void cbReset();
	//static void _cbReset(void *msg);
	virtual void cbApplyAll();
	//static void _cbApplyAll(void *msg);
	virtual void cbResetAll();
	//static void _cbResetAll(void *msg);
	virtual void cbSchedule();
	//static void _cbSchedule(void *msg);
	virtual void cbInvalidate();
	//static void _cbInvalidate(void *msg);
	virtual void cbSchedToolRemove();
	//static void _cbSchedToolRemove(void *msg);
};


#endif
