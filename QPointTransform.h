// qPointTransform : abstract transform base class.
//
//   Given a set of screen coordinates (zRect) and a set of map coordinates (typically lat/lon),
//   an object derived from this class will convert back and forth between the two.
//   there are () operators to make these conversions a bit simpler.  revX takes a screen x coordinate
//   and returns a longitude.  If you notice in the ZatMercatorTransform class this doesn't work as
//   the x position is based on the y so you need both values(x and y) in order to calculate X, the longitude.


#ifndef _qPointTransform_h_
#define _qPointTransform_h_

#include "mapPoint.h"
#include <math.h>
#include <qrect.h>
#include <qpoint.h>

class qPointTransform {
    
  public:

	qPointTransform(QRect rv, mapCordset *mpc, int PO=0);
	qPointTransform(QRect rv, double x1, double y1, double x2, double y2, int PO=0);
    virtual ~qPointTransform(){}

    virtual double getScale()=0;
    virtual double getOffsetX()=0;
    virtual double getOffsetY()=0;
	virtual double revX(int x)=0;
	virtual double revY(int y)=0;

	virtual mapPoint revXY(int x, int y)=0;
	virtual mapPoint operator()(int x, int y)=0;

    virtual QPoint operator()(double x, double y)=0;
	virtual QPoint operator()(mapPoint &p)=0;
    
  protected:

    QRect _rv;  // screen space rectangle
    double _scale;  // default scale factor  (screen width)/(map width)
	double _centerLong, _centerScreen;  // map center coordinate (lon) and the screen center (x)
	double _latScale; // x scale factor.. based on y with Mercator
    double _ox, _oy;  // upper left hand coordinates of the map taken from _mpc
	mapCordset *_mpc; // map coordinates
    int _ProcessOrder; // Process order is here incase multiple transforms are used.  In such a case it
					  // is vital to process the transforms in proper order.
    
};

#endif

