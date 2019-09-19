#include "qGlobalEvents.h"

void qEvents::emitGEPurgeAllIDCs_(){
	emit GEPurgeAllIDCs_();
}
void qEvents::emitGEUpdateAllDataTokens_(){
	emit GEUpdateAllDataTokens_();
}
void qEvents::emitGERunStatusChanged_(){
	emit GERunStatusChanged_();
}

qEvents* qEvents::Instance() {
	static qEvents theEventManager;
	return &theEventManager;
}
