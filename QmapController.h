// QmapController class definintion

#ifndef _QmapController_
#define _QmapController_

//#include "QmapPoint.h"
#include "QmapObject.h"
#include "mapConstants.h"
#include <qptrlist.h>
#include <stdio.h>


class QmapController {
    
  public:
    
    QmapController(char *filename);
    ~QmapController();
    
    inline int mapEntries(void){return mapList.count();}
	inline int iconEntries(void){return iconList.count();}
    inline QmapObject *at(int i){return (QmapObject *)mapList.at(i);}
	inline QmapObject *iconAt(int i){return (QmapObject *)iconList.at(i);}
    QPtrListIterator<QmapObject> *getIterator();
    QPtrListIterator<QmapObject> *getIconIterator();

    int ReadMap(char *filename);
    int ReadMap();
 
    int BuildObject(int type, char *label, FILE *input);
    
    double minLon() {return minLon_;}
    double minLat() {return minLat_;}
    double maxLon() {return maxLon_;}
    double maxLat() {return maxLat_;}

	

  private:
    
    QPtrList<QmapObject> mapList;
    QPtrList<QmapObject> iconList;
    char *_filename;  //map input file
  
  protected:
	
	double minLon_, maxLon_, minLat_, maxLat_;
	double mine;
    
    
};

#endif

