/* qGlobalEvents.h
 * These events are used to force all dialogs to close or all IDCs to purge,
 *  or to announce that a scenario or monte carlo run has started or stopped.
 *  This is needed when major upheavals occur in the main data structures
 *  (such as reading in new data files).
 */

#ifndef __qGlobalEvents_h__
#define __qGlobalEvents_h__

#include <qobject.h>

class qEvents : public QObject{
	Q_OBJECT

public:

	qEvents(): QObject(0,"Global Event Manager"){};
		~qEvents(){};

		//Public members for ending off signals
		void emitGEPurgeAllIDCs_();
		void emitGEUpdateAllDataTokens_();
		void emitGERunStatusChanged_();
		
		static qEvents* Instance();

	signals:
		void GEPurgeAllIDCs_();
		void GEUpdateAllDataTokens_();
		void GERunStatusChanged_();

};

//the external global event manager, initialized in the IterfaceDataControl class
//extern qEvents * c1eventManager; // = new qEvents();
//static qEvents * c1eventManager = new qEvents();


//extern simple_event GEPurgeAllIDCs_;
//extern simple_event GEUpdateAllDataTokens_;
//extern simple_event GERunStatusChanged_;
extern int GOutputsValid_[2];
extern int nGOutputsValid_;
#endif


