#include "qMapMedGeography.h"
#include "qMapMedTypes.h"
#include "QmapController.h"
#include "mapConstants.h"
#include <qptrlist.h>
#include <qpainter.h>

qMapMedGeography::qMapMedGeography(qMapPane *mp, QmapController *mc) : qMapSubMediator(mp, "Geography Drawing med"), mapCon_(mc) {
	RegisterEvents();
}

qMapMedGeography::~qMapMedGeography(){

}

void qMapMedGeography::RegisterEvents(){
	qMapSubMediator::RegisterEvents();
}

void qMapMedGeography::DetachEvents(){

}

int qMapMedGeography::isA(){
	return MMT_GEOGRAPHY;
}

/* This helper routine clips a point's coordinates to the range [-10000,10000]
 *  This is done because Win95 seems to have difficulties with enormous integer
 *  values for line drawing.
 */
void qMapMedGeography::clipPoint( QPoint *pp ) {
	if (pp->x() > 10000)
		pp->setX(10000);
	if (pp->x() < -10000)
		pp->setX(-10000);
	if (pp->y() > 15000)
		pp->setY(15000);
	if (pp->y() < -5000)
		pp->setY(-5000);
}

/* Static routine which draws a list of map points given a pointer to the
 *  map pane, the point transform, and the map object. This is made static
 *  so that it can also be called from qMapMedSelector. Pens and brushes
 *  are selected outside this routine.
 */
/* NOTE: we require a line segment to have minimum length
 *  three pixels (x or y) to increase drawing speed.
 */
void qMapMedGeography::drawMapObject( QmapObject *pmo, QPainter *p ,
										qMercatorTransform *pmt ) {
	
	QPtrListIterator<mapPoint> *pItr;
	mapPoint  *pmp;
	QPoint p1, p2;
	
	pItr = pmo->getIterator();

	if (pItr == NULL) return;
	pmp = pItr->current();
	QPoint zp = (*pmt)(*pmp);
	double latDiff = 0;
	double lonDiff = 0;
	double latSave = pmp->lat();
	double lonSave = pmp->lon();
	clipPoint(&zp);
	p2 = zp;

	while((pmp = pItr->current()) != 0 ){
		
		++(*pItr);		
		if (fabs( pmp->lat() - latSave ) >= latDiff ||
			fabs( pmp->lon() - lonSave ) >= lonDiff) {
			zp = (*pmt)(*pmp);
			clipPoint( &zp );
			p1 = p2;
			p2 = zp;
			p->drawLine (  p1, p2 );
			
			latSave = pmp->lat();
			lonSave = pmp->lon();
		}
	}

	if(pItr) delete pItr;
	pItr=NULL;
}


void qMapMedGeography::evDraw(){
						 	
	QRect r;
	qMercatorTransform *zpt; 
	QPtrListIterator<QmapObject> *itr;
	QmapObject *mo;	
	
	QPainter *p = mapPane_->currPainter;
	

	if(!lock_){
		lock_=1;
		r = mapPane_->drawArea;
		if(r.right() == 0 || r.bottom() == 0) {
			lock_ = 0; //unlock before we leave
			return;
		}

		zpt = (qMercatorTransform *)(mapPane_->getTransform());
		itr = mapCon_->getIterator();

		while((mo = itr->current()) != 0 ){
			mo = itr->current();
			++(*itr);
			if(mo->intersects((*zpt)(r.left(),r.top()),zpt->revXY(r.right(),r.bottom()))){
				p->setPen(getPen(mo));
				p->setBrush ( getPen(mo).color() );
				drawMapObject( mo, p, zpt );
			}
		}
		if(itr)delete itr;
		itr=NULL;

		lock_=0;
		
	}
	
	  
}


QPen qMapMedGeography::getPen(QmapObject*  mo){
    
    switch(mo->objType()){
      case COASTLINE:
	/*return QPen(QColor(Qt::black),1,SolidLine);*/
	return QPen(QColor(mo->isSelected() ? Qt::red : Qt::black),1,SolidLine);
	  case REACH:
	/*return QPen(QColor(Qt::blue),1,SolidLine);*/
	return QPen(QColor(mo->isSelected() ? Qt::red : Qt::blue),1,SolidLine);
      case RIVER:
	/*return QPen(QColor(Qt::gray),1,SolidLine);*/
	return QPen(QColor(mo->isSelected() ? Qt::red : Qt::gray),1,SolidLine);
      case DAM:
	return QPen(QColor(Qt::red),1,SolidLine);
      case RELEASE:
	return QPen(QColor(Qt::green),1,SolidLine);
      default:
	return QPen(QColor(Qt::red),1,SolidLine);
	break;
    };
}
