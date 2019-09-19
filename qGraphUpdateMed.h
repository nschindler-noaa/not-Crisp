/* Graph update Mediator, this class recieved events from the map about highlighted
objects, with the mouse.   The events are sent to the this mediator, which then disbatches 
them to all availiable graphs.  It include a static method for constructing only one instance of itself
creating a gaurantee of just one instance of this class. */


#ifndef _qGraphUpdateMed_h_
#define _qGraphUpdateMed_h_

#include <qobject.h>

class qGraphUpdateMed : public QObject{

	Q_OBJECT

	public:	
		static qGraphUpdateMed* instance();
		void updateGraphs( char *index,  int graphType);


	//protected:   //Gave tons of warnings when this was protected
		qGraphUpdateMed()  {}
		virtual ~qGraphUpdateMed(){}		
	
	private:
		static qGraphUpdateMed* _instance;		
	
	signals:
		void evGraphUpdate(char *,  int ); 

};


#endif

