/* qMonteCarloManager.h
 * qMonteCarloManager is a global class for handling Monte Carlo runs of the
 * model. Most of this code was originally inside of qMonteCarloTool but has
 * been extracted into here so that other GUIs (e.g. qIOTool) may use Monte
 * Carlo mode as well. */

#ifndef	__QMONTECARLOMANAGER_H__
#define	__QMONTECARLOMANAGER_H__

#include <qobject.h>
#include <qthread.h>
#include "monte.h"

class qMonteCarloManager : public QObject {
	Q_OBJECT

protected:
	/* local data structure of all monte carlo data in current directory
	 *  (whether or not it is selected to run right now).
	 */
	struct monte_carlo_data *monte_data;

	/* Local flag to manage changes to the "to be run" list.
	 */
	int runListDirty;

public:

	qMonteCarloManager();
	~qMonteCarloManager();

	static qMonteCarloManager* get();

	/* Find out if a scenario run is being started/stopped; signal if a
	 *  monte carlo run is being started/stopped.
	 */
	void UpdateAllDataTokens();
	void RunStatusChanged();

	struct monte_carlo_data *getMonteData() {return monte_data;}

	int someAltDirty();

	/* Interface to the list of alternatives, these return zero on success and nonzero on error */
	int changeDirectory(const QString &newDir);
	/* Create an alternative with the given name, note that the name should not contain quotation marks
	 * Returns the index of the new alternative on success and -1 on error. */
	int createAlternative(const QString& name);
	int getAlternative(const QString& name); /* Returns < 0 if the alternative doesn't exist (note: this DOES modify the position in altern_list) */
	int deleteAlternative(const QString& name);
	int renameAlternative(const QString& oldName, const QString& newName);
	void runMonteCarlo(); /* Perform any necessary setup and then run Monte Carlo mode through the qScenarioManager */

	void clearRunList(); /* Unselect all alternatives */
	void setRunList(QStringList altsToRun); /* Set the run list */
	QStringList getRunList();

	/* Applying and resetting changes to all alternatives (or a specific one if index != -1) */
	void applyLocalList(int index = -1);
	void resetLocalList(int index = -1);

	/* TODO: editing, run, cancel run, status updates (signal), set alts to run, signal(s) for changes */

	void writeAltsToRun();

protected:
	/* Manage the .compass-alts file (list of alternatives to run).
	 */
	void readAltsToRun();

	/* Routines to manage the local list of alternative structures.
	 */
	void destroyLocalList();
	void createLocalList();
	void readMonteFiles();

    /* Routine to inform other Monte Carlo dialogs that the alternatives
     *  on disk have changed.
     */
    void prodMonteDialogs();

    /* Kill a subdirectory of the current directory, and all it contains.
     */
    void killDirectory(const char *theDir);

    /* Copy a Monte Carlo output file from one directory to another. 
     *  For example, "altern1/altern1.out" becomes "newalt/newalt.out"
     */
    void moveOutputFile(const char *oldDir, const char *newDir, 
                                             const char *extension);

public slots:

signals:

	void alternativesUpdated(); /* When the alternatives have been updated (so that the monte carlo tool can redraw, for example) */

};

#endif

