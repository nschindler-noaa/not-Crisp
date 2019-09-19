//#include <zapp.hpp>
#include "mapPoint.h"
#include "qMercatorTransform.h"

#define PI 3.1415927

qMercatorTransform::qMercatorTransform(QRect rv, double x1, double y1, double x2, double y2, int PO)
	: qPointTransform(rv,x1,y1,x2,y2,PO) {
	
	_centerLong = x1-(x1-x2)/2.0;
	_centerScreen = (double)_rv.width()/2.0;
	double sx = (double)rv.width()/(x1-x2);
	double sy = (double)rv.height()/(y1-y2);
	if(sx<sy)_scale = sx;
	else _scale = sy;
	
}

qMercatorTransform::qMercatorTransform(QRect rv, mapCordset *mpc, int PO)
	: qPointTransform(rv,mpc,PO) {

	_latScale = cos((_oy/180.0)*PI);
//	atoi( "" ); /* KLUDGE - prevents optimization bug */
	_centerLong = _ox-(_ox-_mpc->X2())/2.0;
	_centerScreen = (double)_rv.width()/2.0;
	double sx = (double)rv.width()/(_latScale*(_ox-mpc->X2()));
	double sy = (double)rv.height()/(_oy-mpc->Y2());
	if(sx<sy)_scale = sx;
	else _scale = sy;
	
}

// operator(int,int)  returns a screen x, y given a map coordinate.
QPoint qMercatorTransform::operator()(mapPoint &p){

	return (*this)(p.lon(),p.lat());

}
// operator(int,int)  returns a screen x, y given a map coordinate.
QPoint qMercatorTransform::operator()(double x, double y){

	_latScale = cos((y/180.0)*PI);
//	atoi( "" ); /* KLUDGE - prevents optimization bug */
    return QPoint(
			(int)((_centerLong-x)*_latScale
			*_scale+_centerScreen)
			,(int)((_oy-y)*_scale));
}

// revXY & operator(int,int) : returns a map coordinate given a screen x, y.
mapPoint qMercatorTransform::operator()(int x, int y){
	return revXY(x,y);
}
mapPoint qMercatorTransform::revXY(int x, int y){
	_latScale = cos(((_oy-(double(y)/_scale))/180.0)*PI);
//	atoi( "" ); /* KLUDGE - prevents optimization bug */
	return mapPoint(_centerLong-(x-_centerScreen)/(_latScale*_scale),(_oy-(double(y)/_scale)));
}

//revX: Returns the map longitude given a screen x
double qMercatorTransform::revX(int x){
	//WARNING: This is not a proper calculation.. 
	//  	   the 0.67 is an estimate so please use revX(int,int) or revX(int,double)
	return (_ox-(double(x)/(0.67*_scale)));
}

//revX: Returns the map longitude given a screen x and a screen y coordinate
double qMercatorTransform::revX(int x, int y){
	_latScale = cos(((_oy-(double(y)/_scale))/180.0)*PI);
//	atoi( "" ); /* KLUDGE - prevents optimization bug */
	return _centerLong-(x-_centerScreen)/(_latScale*_scale);
}

//revX: Returns the map longitude given a screen x and a map y(latitude)
//
//  Mercator projection is done by taking the cosine of the latitude and multiplying your
//  x scale factor accordingly.  In this function we have to recalculate the latScale as it is 
//  based on the y coordinate passed in.
double qMercatorTransform::revX(int x, double y){
	_latScale = cos((y/180.0)*PI);	
//	atoi( "" ); /* KLUDGE - prevents optimization bug */
	return _centerLong-(x-_centerScreen)/(_latScale*_scale);
}

//revY: Returns the map longitude given a screen y
double qMercatorTransform::revY(int y){
    return (_oy-(double(y)/_scale));
}
