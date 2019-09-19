/* This is a minimally derived class which modifies qGraphMedDrawData
 *  to add the ability to notice whether the underlying qReleaseEGDT
 *  (edit graph data token derived class to deal with release structures)
 *  is currently writable.
 */

#ifndef _qReleaseMedDrawData_h_
#define _qReleaseMedDrawData_h_

#include "qGraphMedDrawData.h"
#include "qReleaseEGDT.h"

class qReleaseMedDrawData : public qGraphMedDrawData {

	Q_OBJECT

	protected:

		qReleaseEGDT *regdt_;


		void RegisterEvents();
		//virtual void DetachEvents();
		
	public:

		qReleaseMedDrawData(qGraphPane *gp, qReleaseEGDT *regdt) 
					: qGraphMedDrawData(gp, regdt) {
			regdt_ = regdt;
			RegisterEvents();
		//	GraphPane_->evMouseMovedLButtonPressed()->detach(this, &qGraphMedDrawData::_evMouseMovedLButtonPressed);
		//	disconnect(GraphPane_, SIGNAL(evMouseMovedLButtonPressed()), this, SLOT(evMouseMovedLButtonPressed()));
		//	GraphPane_->evLButtonPressed()->detach(this, &qGraphMedDrawData::_evLButtonPressed);
		//	disconnect(GraphPane_, SIGNAL(evLButtonPressed()), this, SLOT(evLButtonPressed()));

		}

		virtual ~qReleaseMedDrawData(){
			//DetachEvents();
		}

		virtual int isA();

	public slots:
		//static void _evMouseMovedLButtonPressed(void *ptr, const zPoint p);
		void evMouseMovedLButtonPressed(QPoint p);
		//static void _evLButtonPressed(void *ptr, const zPoint p);
		void evLButtonPressed(QPoint p);

		
		
};


#endif
