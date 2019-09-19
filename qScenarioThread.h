/*
qScenarioThread - a thread class that calls the run_scenario , thereby allowing
the GUI to update while running the model, and providing user output to the 
status bar
*/

#ifndef _QSCENARIOTHREAD_
#define _QSCENARIOTHREAD_

#include <qthread.h>
#include "main.h"
#include "run_mode.h"



class qScenarioThread : public QThread {


    public:
		virtual void run(){
			run_scenario_mode();
			run_mode.mode = SCENARIO-1;		
		}

    };



#endif

