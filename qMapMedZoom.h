//qMapMedZoom
//
//
//This sub mediator handles the zooming of ZatMapPanes..
//It registers with the button area selection callbacks
//in the ZatMapPane, and converts the screen zRect to a
//set of mapCoordinates which is used to build a new transform
//Once the new transform pushed into the ZatMapPane, the 
//ZatMapPane is signaled to redraw, and the zoom is complete.
//The ZatMapMediator object is passed in as the acutal creation
//of the new transfrom from the mapCoordinate object is handled
//by one of it's methods.

#ifndef _qMapMedZoom_h_
#define _qMapMedZoom_h_

#include "qMapSubMediator.h"

class qMapMediator;

class qMapMedZoom : public qMapSubMediator {

	Q_OBJECT

		qMapMediator *mapMediator_;

	protected:


		virtual void evAreaSelection(const QRect &zr);
		virtual void RegisterEvents();
		virtual void DetachEvents();

	public:

		qMapMedZoom::qMapMedZoom(qMapPane *mp, qMapMediator *mm);
		virtual ~qMapMedZoom();

		virtual int isA();

		//Were hooking up to the area selection events so we know
		//when and where to zoom the map
		/*
		static void _evRButtonAreaSelection(void *ptr, const QRect r);
		static void _evLButtonAreaSelection(void *ptr, const QRect r);
		*/
	protected slots:
		void evRButtonAreaSelection(const QRect r);
		void evLButtonAreaSelection(const QRect r);

};


#endif


