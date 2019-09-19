//This mediator tells the mouseTool what button has been pressed
//and on what(dam, river, release site, etc) so that it can tell the
//ZasMediatorBuilder what kind of dialog to build.


#ifndef _qMapMedDialogBuilder_h_
#define _qMapMedDialogBuilder_h_

#include "qMapSubMediator.h"

class qMouseTool;
class QmapObject;

class qMapMedDialogBuilder : public qMapSubMediator {

	Q_OBJECT

	protected:

		qMouseTool *mouseTool_;

		//Store the last features the mouse has moved over
		QmapObject *geoObject1_;
		QmapObject *geoObject2_;
				
		virtual void RegisterEvents();
		virtual void DetachEvents();

	public:

		qMapMedDialogBuilder(qMapPane *mp, qMouseTool *mt, char* name);
		virtual ~qMapMedDialogBuilder();

		virtual int isA();
		
		//Hook up the the all mighty evAnnounce event so that
		//we can be notifed when the ZasMapMedSelector finds something
		//interesting.
		//Check out the "ZasMapMedAnnouncements.h" header
		//to get a list of all the announcements.
		/*
		static void _evAnnounce(void *ptr, const int type, void *data);
		*/
		//virtual void evAnnounce(const int type, void *data);

	protected slots:
		void evAnnounce(const int , void *);
};



#endif


