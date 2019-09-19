/* This class has been merged with qMapMedDetector since they're both
 * handling mouse events to handle selection/highlighting. It checks
 * to see if you clicked on a dam, reach, or other feature and announces
 * if you did. It also detects icons and features as the mouse moves
 * over them and announces them. Additionally, it requests graphical
 * updates for when icons must be selected. */

#ifndef _qMapMedSelector_h_
#define _qMapMedSelector_h_

#include "qMapSubMediator.h"
#include "QmapController.h"
#include <qpoint.h>

class qMapMedSelector : public qMapSubMediator {
	
	Q_OBJECT

	protected:

		int radius_;
		QmapController *mapCon_;

		bool lock;

		//These pointers are here so that we know what to clean up
		//when the mouse moves to a new object
		mapPoint oldPos_, newPos_;
		QmapObject *oldObj_, *newObj_;
		QmapObject *oldGeoObj_, *newGeoObj_;

		//We're always using the same pens and brushes
		QPen *defaultPen_;
		QPen *highlightPen_;
		QBrush *defaultBrush_;

		//These functions check for intersection of the point pos with
		//any of the mapObjects in the mapController.
		QmapObject *intersectsIcon(const QPoint &pos);
		QmapObject *intersectsGeography(const QPoint &pos);

		//Event Registration..
		virtual void RegisterEvents();
		virtual void DetachEvents();
		
		virtual void updateIcon();
		virtual void updateGeo();

		//I broke these up because the icons and geo features
		//are redrawn independently at times.
		virtual void evDrawGeo(QmapObject *);
		virtual void evDrawIcon(QmapObject *);

	public:

		qMapMedSelector(qMapPane *mp, QmapController *mc);
		virtual ~qMapMedSelector();

		virtual int isA();
		
		//We want to get notifed if a mouse button is pressed on
		//the screen so we hook these guys up with the ZatMapPane
	 //	static void _evLButtonPressed(void *ptr, const zPoint p);
	 //	static void _evMButtonPressed(void *ptr, const zPoint p);
		//static void _evRButtonPressed(void *ptr, const zPoint p);
	protected slots:
		void evLButtonPressed(QPoint p);
		void evMButtonPressed(QPoint p);
		void evRButtonPressed(QPoint p);
		void evMouseMoved(const QPoint );

};



#endif


