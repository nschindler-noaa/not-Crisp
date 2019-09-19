#include "qGraphUpdateMed.h"

qGraphUpdateMed* qGraphUpdateMed::_instance = 0;

void qGraphUpdateMed::updateGraphs(char *index, int graphType){

		emit evGraphUpdate((char*)index, graphType); 

}


qGraphUpdateMed* qGraphUpdateMed::instance(){

	if(_instance == 0 ){ 
		_instance = new qGraphUpdateMed();
	}
	return _instance;

}
