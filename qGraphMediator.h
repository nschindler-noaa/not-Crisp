
#ifndef _qGraphMediator_H_
#define _qGraphMediator_H_

#include "qGraphUpdateMed.h"
#include "qGraphDialog.h"
#include "qGraphPane.h"
#include "qGraphDataToken.h"
#include "qGraphSubMediator.h"
#include <qptrlist.h>


class qGraphMediator : public QObject{

	Q_OBJECT

	protected:

		qGraphDialog *_gd;
		qGraphPane *_gp;
		qGraphDataToken *dataToken_;
		qGraphUpdateMed* _gu;
		
		int _isData, _lock, _graphType, _isDisabled, _ref;
		int yAxisType_, xAxisType_;
		long lX_[2], lY_[2];
		double dX_[2], dY_[2];

		QPtrList<qGraphSubMediator> graphSubMedList_;

	public:

		qGraphMediator(qGraphDialog *gd, qGraphDataToken *dt);
		virtual ~qGraphMediator();

		void DrawLabels(void);
		void setGraphType(const int type){ _graphType = type; }
		int isDisabled(void){return _isDisabled;} 

		//Sub Mediator methods
	    virtual void insertSubMediator(qGraphSubMediator *msm);
		virtual void removeSubMediator(int medId);

		//Event methods 
		/* RegisterEvents cannot be virtual because it is called in the
		 *  constructor. DetachEvents is not virtual for consistency. */
		void RegisterEvents(void);
		void DetachEvents(void);
	

signals:
		void evRemove(qGraphMediator * );

	protected slots:
		virtual void cbRemove();
		virtual void cbGraphUpdate(char *index, int type);
		virtual void cbReload();
		virtual void cbHelp();
		virtual void cbSave();



};


#endif

