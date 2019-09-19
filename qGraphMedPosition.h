#ifndef _qGraphMedPosition_h_
#define _qGraphMedPosition_h_

#include "qGraphSubMediator.h"
#include "qEquationDialogBase.h"
#include "log.h"

class qGraphDialog;
class qEquationDialogBase;

class qGraphMedPosition : public qGraphSubMediator {

	Q_OBJECT

	protected:

		qGraphDialog *gd_;
		qEquationDialogBase *ed_;
		QSize vTextSize ; //holds the default text size

		virtual void RegisterEvents();
		virtual void DetachEvents();

	public:

		qGraphMedPosition(qGraphPane *gp, qGraphDialog *gd)
					: qGraphSubMediator(gp), gd_(gd), ed_(NULL) {
	
			RegisterEvents();
		}

		qGraphMedPosition(qGraphPane *gp, qEquationDialogBase *ed)
					: qGraphSubMediator(gp), gd_(NULL), ed_(ed) {
			RegisterEvents();
		}

		virtual ~qGraphMedPosition(){
			
		}

	
			virtual int isA();
protected slots:
		virtual void evMouseMoved( QPoint );

	
};


#endif


