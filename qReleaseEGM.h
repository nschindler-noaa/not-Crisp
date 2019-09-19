/* qReleaseEGM.h
 *  Main mediator for graphs which edit the release schedule in a "release"
 *  structure. Derived from ZatEditGraphMediator with, hopefully, minimal
 *  changes.
 *  Editing is by left-button drag-draw or by opening a "Schedule Tool"
 *  dialog.
 */


#ifndef __qReleaseEGM_H__
#define __qReleaseEGM_H__

#include "qEditGraphMediator.h"
#include "qReleaseEGDT.h"
#include "qScheduleTool.h"
class qReleaseTool;

class qReleaseEGM : public qEditGraphMediator {

	Q_OBJECT

protected:
	/* same pointer as ZatGraphMediator::dataToken_, but gives access 
	 *  to derived class methods. */
	qReleaseEGDT *relEGDT_;
	qReleaseTool *relTool_;

	/* Handle various shutdown cases.
	 */
	int gdClosing_;

public:
	qReleaseEGM(qGraphDialog *gd, qReleaseEGDT *relEGDT,
					qReleaseTool *relTool);
	virtual ~qReleaseEGM();

	// event methods.
	virtual void RegisterEvents(void);
	virtual void DetachEvents(void);

protected slots:
	virtual void cbOK();
	//static virtual void _cbOK(void *msg);
	virtual void cbCancel();
	////static void _cbCancel(void *msg);
	virtual void cbReltoolRemove();
	//static void _cbReltoolRemove(void *msg);
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
