//qMapMedGeography
//
//This is the big cheese of the map mediators as it draws 
//the rivers and features that make the map look like a map.
//It takes a handle to the mapController just like the 
//ZasMapMedIcons class as the mapController holds our list
//of mapObjects (reaches, etc).

#ifndef _qMapMedGeography_h_
#define _qMapMedGeography_h_


#include "qMapSubMediator.h"
#include "QmapObject.h"
#include "qMercatorTransform.h"

class QmapController;

class qMapMedGeography : public qMapSubMediator {


		QmapController *mapCon_;
		

	protected:

		virtual void RegisterEvents();
		virtual void DetachEvents();

	public:

		qMapMedGeography(qMapPane *mp, QmapController *mc);
		virtual ~qMapMedGeography();
		virtual int isA();

		//So that we get different color pens for different features
		//(ie, coastlines should be black, rivers blue, etc)
		static QPen getPen(QmapObject* mo);
	
		virtual void evDraw();
		static void clipPoint( QPoint *pp );
		static void drawMapObject( QmapObject *pmo, QPainter* p,
										qMercatorTransform *pmt );


};


#endif


