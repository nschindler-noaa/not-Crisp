#include "qScenarioManager.h"
#include "main.h"
#include "monte.h"
#include "run_mode.h"
#include "qGlobalEvents.h"
#include "gui.h"
#include "crisp.h"
#include <qmessagebox.h>
#include <qapplication.h>

qScenarioManager scenarioManager;

void qScenarioThread::run() {
	run_scenario_mode();
	run_mode.mode = MODE_NONE;
}

void qMonteCarloThread::run() {
	run_monte_carlo(run_mode.monte_data);
	run_mode.mode = MODE_NONE;
}

qScenarioManager::qScenarioManager()
:	timerID(0)
{
}

void qScenarioManager::run(int monte_carlo) {
	if (run_mode.running) return;

	canceled = FALSE;
	/* Enable variance for Monte Carlo and disable for scenario */
	old_variance_suppression = current->flags & VAR_SUPPRESS_FLAG;
	if (monte_carlo == FALSE) {
		current->flags |= VAR_SUPPRESS_FLAG;
		disable_variance_suppression = FALSE;
	}
	else {
		disable_variance_suppression = TRUE;
	}

	/* Clear data from any prior runs */
	{
		struct release *rel;
		for (rel = current->release_list; rel != NULL; rel = rel->next) {
			struct rls_seg *rseg;
			for (rseg = rel->rls_seg; rseg != NULL; rseg = rseg->down) {
				if (rseg->data != NULL) {
					int i;
					for (i=0; rseg->data[i] != NULL; i++) rseg->data[i]->stats.done = FALSE;
				}
			}
		}
	}


	/* Notify classes that we're starting to run a scenario */
	emit running();

	// run scenario mode - multithreaded.	
	run_mode.running = TRUE;
	if (monte_carlo) run_mode.mode = MONTE;
	else run_mode.mode = SCENARIO;

	/* Announce run status change - including, to this object. */
	qEvents::Instance()->emitGERunStatusChanged_();
	qEvents::Instance()->emitGEUpdateAllDataTokens_();

	QThread* threadHandle;
	if (monte_carlo) threadHandle = new qMonteCarloThread();
	else threadHandle = new qScenarioThread();

	if(threadHandle == 0)
	{
		QMessageBox::warning ( topLevel, "Crisp Passage Error", "Scenario Run failed!");
		run_mode.running = FALSE;
		run_mode.mode = 0;
		qEvents::Instance()->emitGERunStatusChanged_();
		qEvents::Instance()->emitGEUpdateAllDataTokens_();
		return;
	}

	//start thread
	threadHandle->start( QThread::LowPriority); 
	//give it low priority, or else will starve
	//the gui thread
		
	timerID = startTimer(500); //check for status updates every 1/2 second
}

void qScenarioManager::runScenario(){
	run(FALSE);
}
void qScenarioManager::runMonteCarlo() {
	run(TRUE);
}

void qScenarioManager::runCancel(){

	/* Cancel run. Thread will die when it sees run_mode.running = FALSE
	 *  since it periodicly checks this global value.
	 */
    if (!run_mode.running)
        return;

    run_mode.compute_flow_break = 1;
	run_mode.running = FALSE;
	
	canceled = TRUE;

	/* Reset variance suppression */
	current->flags = (current->flags & ~VAR_SUPPRESS_FLAG) | old_variance_suppression;

	/* Invalidate all the calculated values from the last run */
	if (FALSE) {
		struct release *rel;	
		for (rel = current->release_list; rel != NULL; rel = rel->next) {
			struct rls_seg *rseg;
			for (rseg = rel->rls_seg; rseg != NULL; rseg = rseg->down) {
				if (rseg->data != NULL) {
					int i;
					for (i=0; rseg->data[i] != NULL; i++) rseg->data[i]->stats.done = FALSE;
				}
			}
		}
	}

	emit runCanceled();
}

/* Helper routines for multithreaded runs. */
void qScenarioManager::timerEvent( QTimerEvent *e )
{
	if (run_mode.mode != SCENARIO && run_mode.mode != MONTE) {
		run_mode.running = FALSE;

		/* emit RunFinished(); */ /* RunStatusChanged is essentially the same thing */
		/* Signaling this event will do our GUI changes etc.
		 */
		qEvents::Instance()->emitGERunStatusChanged_();
		qEvents::Instance()->emitGEUpdateAllDataTokens_();

		killTimer(timerID);
		timerID = 0;

		/* Reset variance suppression */
		current->flags = (current->flags & ~VAR_SUPPRESS_FLAG) | old_variance_suppression;

		if (!canceled) emit runCompleted();
	}
}

/* This routine is the entry point for the legacy routine "run_monte_carlo()"
 * to display progress indicators. */
void SetMonteGauges(int game, int gameprog, int year, int yearprog,
					int alt, int altprog) {
	QApplication::postEvent(&scenarioManager, new qMonteCarloStatusEvent(game, gameprog, year, yearprog, alt, altprog));
}

void qScenarioManager::customEvent(QCustomEvent* e) {
	qMonteCarloStatusEvent *event = (qMonteCarloStatusEvent*) e;

	emit monteCarloStatusUpdate(event->game, event->gameprog, event->year, event->yearprog, event->alt, event->altprog);
	/* Since the progress dialog is only designed for one alternative and one year, ignore other events */
	if (event->yearprog == 0 && event->altprog == 0) emit runProgressUpdate(event->gameprog);
}
