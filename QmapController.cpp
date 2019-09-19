/* QmapController.cpp  The qt friendly implementation of the 
 * class that holds a list current map objects for the map.
 */

// QmapController.cc
//
//   Loads and stores the map into a mapList of mapObjects which contains 
//   a list of mapPoints in a QPtrList.  mapObjects represent islands, coastlines, etc. and
//   mapPoints are the actual latitude longitude points that make up the mapObject.  The mapContoller maintains
//   and builds the mapList from the map.dat file.  The mapController may export events in the future but as it
//   sits, an event will never be triggered by the data segment as new data is never added at runtime.

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include "mapPoint.h"
#include "QmapController.h"
#include "crisp.h"
#include <qptrlist.h>
#include "QmapObject.h"
#include "log.h"

int mapPoint::compareTo(const mapPoint * p1) const {
        
    if((p1->latitude == latitude) && (p1->longitude == longitude)) return 0;
    else if((p1->latitude+p1->longitude) > (longitude+latitude)) return 1;
    else return -1;
    
}
double mapPoint::distance(const mapPoint &p){
	double p1,p2;
	p1 = p.lat()-latitude;
	if(p1 < 0.0) p1 = -p1;
	p2 = p.lon()-longitude;
	if(p2 < 0.0) p2 = -p2;
	return (p1+p2);
}

QmapController::QmapController(char *filename) : _filename(filename){


}

QmapController::~QmapController(){
    mapList.clear();
    iconList.clear();
}

// Builds an iterator for object list mapList.. object lists in turn have iterators for their data.
// Note: you must handle the cleanup of the iterator and should you cycle to NULL, it's position
// is undefinded and must be nuked.
// -Steve

QPtrListIterator<QmapObject> *QmapController::getIterator(){
    return new QPtrListIterator<QmapObject>(mapList);
}

QPtrListIterator<QmapObject> *QmapController::getIconIterator(){
    return new QPtrListIterator<QmapObject>(iconList);
}

int QmapController::BuildObject(int type, char *label, FILE *input){

    char inbuf[100], dirlat, dirlon, tag[30], name[40];
    int deglat,minlat,seclat,deglon,minlon,seclon;
    int is_icon=0;
    double lat, lon;
    
    
	QString objlabel;
    QmapObject *mo = new QmapObject(type);
    
    while(!feof(input)){
		lat = 0.0; lon = 0.0;
		fgets(inbuf, 100, input);
		if(inbuf[0] == '#')
			continue;

		if(strstr(inbuf,"end") && inbuf[0] == 'e'){
			int i=0,j=0;
			if(type == DAM || type == RELEASE){
				while(label[i] != '\0'){
					if(label[i++] == ' '){
						while(label[i] != '\0' && j < 40 && i < 100)
							name[j++]=label[i++];
						name[j--]='\0';
						while(j>0 && !isalpha(name[j]))
							name[j--]='\0';
					}
				}
			}
			else {
				while(inbuf[i] != '\0'){
					if(inbuf[i++] == '('){
						while(inbuf[i] != ')' && j < 40 && i < 100)
							name[j++]=inbuf[i++];
						name[j]='\0';
					}
				}
			}
			objlabel = name;
			if(!name){
				objlabel = "No Label.";
			}
			mo->setLabel(objlabel);
			if(!is_icon)
				mapList.prepend(mo);
			else 
				iconList.prepend(mo);
			
			return 0;
		}
		if(strstr(inbuf,"latlon")){
			sscanf(inbuf,"%s %d %d %d %c %d %d %d %c", tag,
				   &deglat, &minlat, &seclat, &dirlat,
				   &deglon, &minlon, &seclon, &dirlon);
			
			lat = (double)deglat + (double)minlat/60.0 + (double)seclat/3600.0;
			lon = (double)deglon + (double)minlon/60.0 + (double)seclon/3600.0;

			if(lat && lon){
				if(type == DAM || type == RELEASE)
					is_icon = 1;
				mo->insert(lon, lat);
			}
			else 
				printf("ERROR: Invalid data from map file.");
		}
	}
    return 1;
}

static char *types[] = {"reach","coastline","stock", 
							"release_site", "dam", "boundary"};

int QmapController::ReadMap(char *filename){

    FILE *md = NULL;  //Map Datafile
    char inbuf[100], buf[100], *p;
	/* NOTE: objType values must agree with constant declarations in mapConstants.h
	 */
    const int numtypes = 6;
    int j=0;
    
    if(!filename) filename = _filename;
    if((md = fopen(filename, "r"))==NULL){
		printf("ERROR: QmapController::ReadMap -> Unable to open map file\n");
		return 0;
    }

    while(!feof(md)){
		inbuf[0] = '\0'; buf[0] = '\0';	p=NULL;
		fgets(inbuf, 100, md);
		if(inbuf[0] == '#')
			continue;
		strcpy(buf, inbuf);
		for(j=0;j<numtypes;j++){ 
			if((p = strstr(buf, types[j])))
				BuildObject(j*10+10, buf, md);
		}
    }
    fclose(md);
	return 1;
}

int QmapController::ReadMap(){

    struct river_segment *segCurrent = river_mouth;
	struct release_site  *siteCurrent = release_site;
    double x,y;
	
    minLon_ = 1000.0;
	minLat_ = 1000.0;
    maxLon_ = -1000.0;
	maxLat_ = -1000.0;
	
	QmapObject *mObject = NULL;
	int type;
	point *pCoord = NULL;
	while(segCurrent){
		type = 0;
		switch(segCurrent->seg_type){
			case DAM_SEG:
				type = DAM;
				break;
			case REACH_SEG:
				type = REACH;
				break;
			case HEADWATER_SEG:
				type = HEADWATER;
				break;
			case POWERHOUSE_SEG:
				type = POWERHOUSE;
				break;
			default:
				break;
		}
		if(type != POWERHOUSE) {
			mObject = new QmapObject(type, segCurrent->seg_name);
			pCoord = segCurrent->bottom;
			while(pCoord){
                x = fabs( pCoord->lon );
                y = fabs( pCoord->lat );
				mObject->insert(x,y);
                minLon_ = x < minLon_ ? x : minLon_;
                minLat_ = y < minLat_ ? y : minLat_;
                maxLon_ = x > maxLon_ ? x : maxLon_;
                maxLat_ = y > maxLat_ ? y : maxLat_;
				pCoord = pCoord->next;
			}
			if(type == DAM){
				iconList.prepend(mObject);
			}
			else 
				mapList.prepend(mObject);
		}
		segCurrent = segCurrent->next;
    }
	while(siteCurrent){
		mObject = new QmapObject(RELEASE,siteCurrent->name);
		mObject->insert(fabs(siteCurrent->latlon->lon),fabs(siteCurrent->latlon->lat));
		iconList.prepend(mObject);
		siteCurrent = siteCurrent->next;
	}

	return 1;


}
