#ifndef _QmapObject_h_
#define _QmapObject_h_
//#include <rw/cstring.h>
//#include <rw/dlistcol.h>
#include "mapPoint.h"
#include <qptrlist.h>
#include <qstring.h>

enum objGeometry {mLINE, mPOINT, mPOLYGON, mPOLYLINE, mICON};
typedef QPtrListIterator<mapPoint> mapPointListItr;
typedef QPtrList<mapPoint> mapPointList;

class QmapObject {

  public:
    
    QmapObject(int objType=0, const char *objLabel=NULL, objGeometry geo=mLINE);
    ~QmapObject();
    int entries(){ 
			return pointList.count ();
	}
    int clear(){
		//Auto delete will be enabled
		pointList.clear();
		return pointList.count();
    }
    mapPoint *at(int i){
		return (mapPoint *)pointList.at(i);
    }
	QString getLabel(){	
		return _objLabel; 
	}
	void setLabel(const QString label){
		_objLabel = label;
	}
    mapPointListItr *getIterator(){ 
		return new mapPointListItr(pointList); 
	}
    inline int objType(){
		return _objType;
	}

	mapPoint getMin(){ return mapPoint(mx,my); }
	mapPoint getMax(){ return mapPoint(MX,MY); }

    int insert(double x, double y);
    int insert(mapPoint *mp);

    void setMinMax(double x, double y);

    int intersects(double x1, double y1, double x2, double y2);
	int intersects(mapPoint p1, mapPoint p2);    
    
	int isSelected( void ) { return selected_; };
	void Selected( int i ) { selected_ = i; };

  private:
   
	int selected_;		// 1 if shown highlighted, else 0.
    double MX,MY,mx,my; // max and min points in the object;
    mapPointList pointList;   
	objGeometry _geo;
    int   _objType;
    QString _objLabel;
    int init_;
    int refresh;
    
};




#endif

