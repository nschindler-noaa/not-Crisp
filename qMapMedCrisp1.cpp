

#include "qMapMedCrisp1.h" 
#include "qMapMediator.h"
#include "qMapSubMediator.h"
#include "QmapController.h"
#include "qMapPane.h"
#include "qMercatorTransform.h"
#include "mapPoint.h"
#include "qMapMedGridLines.h"
#include "qMapMedIcons.h"
#include "qMapMedGeography.h"
#include "qMapMedSelector.h"
#include "qMapMedZoom.h"
#include "qMapMedUnZoom.h"

qMapMedCrisp1::qMapMedCrisp1(qMapPane *mp, QmapController *mc) : qMapMediator(mp, mc) {

	mapStack_.push(new mapCordset(124.3,49.0,113.2,43.50));
    defaultCord_ = new mapCordset(124.3,49.0,113.2,43.50);

	insertSubMediator(new qMapMedIcons(mp, mc));
	insertSubMediator(new qMapMedGeography(mp, mc));
	insertSubMediator(new qMapMedSelector(mp, mc));
	insertSubMediator(new qMapMedZoom(mp, this));
	insertSubMediator(new qMapMedUnZoom(mp, this));

}

qMapMedCrisp1::~qMapMedCrisp1(){

}
 

void qMapMedCrisp1::setBoundingBox( double minLon, double minLat,
                                     double maxLon, double maxLat ) {
    mapStack_.clear();
	mapStack_.push(new mapCordset( maxLon, maxLat, minLon, minLat ));
    delete defaultCord_;
    defaultCord_ = new mapCordset( maxLon, maxLat, minLon, minLat );
}
