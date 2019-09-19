//	qMercatorTransform : transforms screen coordinates to map lat lon. This transform isn't a true
//						   Lambert Conformal Conic as we are not adding in the longitudinal curvature.
//						   
//	 Lambert Conformal Conic (Johanne Heinrich)
//    "Developed by J.H. Lambert in 1772 (fig. 2.7). The projection was almost unknown as a Lambert Projection for over a century. Harding, Herschel and
//     Boole had developed it independently in both spherical and ellipsoidal forms during the 19th century. World War I gave this projection new life, making
//     it the standard projection for intermediate - and large-scale maps of regions in midlatitudes for which the transverse Mercator is not used."
//


#ifndef _qMercatorTransform_h_
#define _qMercatorTransform_h_

#include "QPointTransform.h"
#include <qrect.h>
#include <qpoint.h>

class qMercatorTransform : public qPointTransform {
    
  public:

	qMercatorTransform(QRect rv, mapCordset *mpc, int PO=0);
	qMercatorTransform(QRect rv, double x1, double y1, double x2, double y2, int PO=0);
    ~qMercatorTransform(){}

    virtual double getScale(){
		return _scale;
	}
    virtual double getOffsetX(){
		return _ox;
	}
    virtual double getOffsetY(){
		return _oy;
	}

	virtual double revX(int x);
    virtual double revX(int x, int y);
	virtual double revX(int x, double y);
	virtual double revY(int y);

	virtual mapPoint revXY(int x, int y);
	virtual mapPoint operator()(int x, int y);

    virtual QPoint operator()(double x, double y);
	virtual QPoint operator()(mapPoint &p);
    
    
};

#endif
