
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mapPoint.h"
#include "QmapObject.h"

QmapObject::QmapObject(int objType, const char *objLabel, objGeometry geo) {
	pointList.setAutoDelete ( true); //enable delete of removed objects
    _objType = objType;
	init_ = 0;
	refresh = 1;
	_geo = geo;
	selected_ = 0;
    if(objLabel)_objLabel = objLabel;
    
}
    
QmapObject::~QmapObject(){
    pointList.clear();
}

int QmapObject::insert(double x, double y){
    setMinMax(x,y);
    mapPoint *mp = new mapPoint(x,y);
    pointList.prepend(mp);
    return pointList.count();
}

int QmapObject::insert(mapPoint *mp){
    setMinMax(mp->lon(),mp->lat());
    pointList.prepend(mp);
    return pointList.count();
}

void QmapObject::setMinMax(double x, double y){
    if(!init_){
	mx=x;
	MX=x;
	my=y;
	MY=y;
	init_++;
    }
    else{
	if(x<mx)mx=x;
	if(x>MX)MX=x;
	if(y<my)my=y;
	if(y>MY)MY=y;
    }
}

int QmapObject::intersects(mapPoint p1, mapPoint p2){
	return intersects(p1.lon(),p1.lat(),p2.lon(),p2.lat());
}
int QmapObject::intersects(double x1, double y1, double x2, double y2){
    double X, Y;
    if(x1>x2){X=x2;x2=x1;x1=X;}
    if(y1>y2){Y=y2;y2=y1;y1=Y;}
	if(x1>=mx && x1<=MX && y1>=my && y1<=MY) return 1; //x1,y1
	if(x2>=mx && x2<=MX && y2>=my && y2<=MY) return 1; //x2,y2
    if(x1<=mx && x2>=mx && y1<=my && y2>=my) return 1; //mx,my
    if(x1<=MX && x2>=MX && y1<=MY && y2>=MY) return 1; //MX,MY

	if(x1>=mx && x1<=MX && y2>=my && y2<=MY) return 1; //x1,y2
	if(x2>=mx && x2<=MX && y1>=my && y1<=MY) return 1; //x2,y1
    if(x1<=mx && x2>=mx && y1<=MY && y2>=MY) return 1; //mx,MY
    if(x1<=MX && x2>=MX && y1<=my && y2>=my) return 1; //MX,my

	if(x1>=mx && x2<=MX && y1<=my && y2>=MY) return 1; // No verticies, just center sections.
	if(x1<=mx && x2>=MX && y1>=my && y2<=MY) return 1;
    return 0;
}

