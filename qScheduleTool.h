/* qScheduleTool.h
 * This class defines a modal dialog which edits qGraphDataToken-s
 *  via a listbox and edit boxes. The dialog is based on the 
 *  IDD_SCHEDULETOOL resource template.
 *
 * This class edits a "qEditGraphDataToken" object. Updating the object
 *  into the underlying data structure is also handled.
 *
 * NOTE: this class only edits the first "line" of data in the proferred
 *         object.
 */

#ifndef __qSCHEDULETOOL_H__
#define __qSCHEDULETOOL_H__


#include <qmainwindow.h>
#include "qEditGraphDataToken.h"
#include "ScheduleToolBase.h"

class qScheduleTool : public ScheduleToolBase {
	Q_OBJECT
protected:
	qEditGraphDataToken *edt_;

public:
	qScheduleTool( QWidget *w, qEditGraphDataToken *edt );
	~qScheduleTool();

	/* Find out if a scenario or monte carlo run is being started/stopped.
	 *  If we are running, disable "OK", "Apply", "Apply All", etc.
	 */
	

	void FillFromToken( void );
	void AttachEvents( void );
	void DetachEvents( void );
	void Format( char *cb, int start, int stop, double value );

public slots:
	virtual void cbRunStatusChanged( void );
	virtual void cbOK();
	virtual void cbCancel();
	virtual void cbApply();
	virtual void cbReset();
	virtual void cbApplyAll();
	virtual void cbResetAll();
	virtual void cbHelp();
	virtual void cbLeftArrow();
	virtual void cbList1();
	virtual void cbInvalidate();
	virtual void cbFilled();

signals:
	void evRemove();

};

#endif

