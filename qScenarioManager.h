/* qScenarioManager.h
 * qScenarioManager is a global object that handles scenario-mode runs of the model.
 * Most of this code was originally inside of QCrisp, but is now here so that other
 * GUIs (e.g. qIOTool) may be used without requiring the old GUI to be present. */

#ifndef	__QSCENARIOMANAGER_H__
#define __QSCENARIOMANAGER_H__

#include <qobject.h>
#include <qwidget.h>
#include <qthread.h>

/* qScenarioThread - a thread class that calls the run_scenario , thereby allowing
 * the GUI to update while running the model, and providing user output to the 
 * status bar.
*/
class qScenarioThread : public QThread {
public:
	void run();
};
class qMonteCarloThread : public QThread {
public:
	void run();
};

/* This event is used to update Monte Carlo status bars; this is done so that these updates are thread safe */
class qMonteCarloStatusEvent : public QCustomEvent{
public:
	int game, gameprog, year, yearprog, alt, altprog;
	qMonteCarloStatusEvent(int _game, int _gameprog, int _year, int _yearprog, int _alt, int _altprog)
		:	QCustomEvent(QEvent::User+70),
			game(_game), gameprog(_gameprog), year(_year), yearprog(_yearprog), alt(_alt), altprog(_altprog)
	{}
};

class qScenarioManager : public QObject {
	Q_OBJECT

protected:
	int timerID;
	QWidget *topLevel;
	int canceled; /* Whether the last run was canceled or not */
	int old_variance_suppression;	/* This is so that the variance suppression flag can be reset after a run */

public:
	qScenarioManager();
	void setTopLevel(QWidget *w) {topLevel = w;}
	int	getCanceled() {return canceled;}

protected:
	void run(int monte_carlo);
	void timerEvent( QTimerEvent *e );

	/* Message passing is used for updating Monte Carlo run status since events can be thread safe */
	virtual void customEvent( QCustomEvent * e );

public slots:
	void runScenario();	/* Run a scenario. Note that variance suppression is silently enabled for scenarios */
	void runMonteCarlo();	/* Run Monte Carlo. Note that variance is silently disabled for Monte Carlo mode */
	void runCancel();

signals:
	void running(); /* Just before a model run */
	void runCanceled();
	void runCompleted();

	void monteCarloStatusUpdate(int, int, int, int, int, int);
	void runProgressUpdate(int percent);
};

extern qScenarioManager scenarioManager;

#endif
