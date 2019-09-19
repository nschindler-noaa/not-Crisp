//This used to do alot of stuff, now all of it's functionality is
//handled by the varous map submediators.. The code int the constructor
//is used to simply build those submediators now.  If you want to see
//the mess it used to be, look at the map mediators in Crisp2.

#ifndef _qMapMedCrisp1_h_
#define _qMapMedCrisp1_h_

#include "qMapMediator.h"
 
class qMapMedCrisp1 : public qMapMediator {


  public:

	qMapMedCrisp1(qMapPane *mp, QmapController *mc);
	virtual ~qMapMedCrisp1();

	void setBoundingBox( double minLon, double minLat, double maxLon, 
                              double maxLat );
};



#endif

