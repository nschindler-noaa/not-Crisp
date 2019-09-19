/* qIOTool.h
 * This class is the main window for using the new spreadsheet-like
 * GUI that's somewhat similar to SIMPAS's input via Excel. */

#ifndef __qIOTOOL_H__
#define __qIOTOOL_H__

#include "crisp.h"
#include "IOToolBase.h"
#include "qIOSheet.h"
#include "qIONotes.h"
#include "qGraphDialog.h"
#include <qstringlist.h>
#include <qmap.h>
#include <qdir.h>
#include <qlineedit.h>
#include <qtextedit.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qspinbox.h>
#include <qptrlist.h>
#include <qtabwidget.h>

/* Time in milliseconds between updating list of data files */
#define	DATA_FILE_ENUMERATION_PERIOD	15000

class qIOTool : public IOToolBase {
	Q_OBJECT

protected:
	QPtrList<qIOSheet> sheets; /* I/O sheets */
	QStringList locationNames;
	QStringList speciesNames;

	/* If the I/O tool itself is dirty (e.g. different files are selected, but
	 * the scenario hasn't been re-run or a run was canceled). */
	int dirty;
	int output_dirty; /* If the sheet does not reflect the currently selected options, but these options do not need to be saved (i.e. Monte Carlo settings) */

	int loading; /* If the I/O tool is loading files (to prevent performing calculations on unfinished data) */

	/* If changes have been applied not been saved, then unsaved_changes
	 * should be TRUE, otherwise it should be FALSE.
	 * Note that you should also check isDirty() to check for unapplied
	 * changes. */
	int unsaved_changes;

	int retrospective; /* If we've loaded a retrospective input file */
	int full_scenario; /* If we've loaded a full scenario (i.e. one that has at least one release) */

	int monte_carlo; /* If Monte Carlo mode is enabled */

	QString userName; /* User name */

	QTabWidget *sheetStack; /* Stack of tabbed I/O sheets */

	/* User-adjustable averaging period (for spill averages) */
	int avg_start;
	int avg_stop;

	QDockWindow *averageDock;
	QComboBox *averageStartComboBox;
	QComboBox *averageStopComboBox;

	/* Scenario selection */
	/* These 4 variables map from a file name (e.g. "2003.water") to that file's
	 * full path. They are used to manage the combo boxes. */
	QMap<QString, QString> waterYearFileToPath;
	QMap<QString, QString> operationsFileToPath;
	QMap<QString, QString> calibrationFileToPath;
	QMap<QString, QString> postBonnevilleFileToPath;
	QMap<QString, QString> releaseFileToPath;

	/* Qt Designer doesn't support dock windows, so we have to create them
	 * by hand in the code. Here are the pieces of the GUI that are not
	 * contained in the IOToolBase.ui file that are instead coded by hand
	 * in qIOTool. */

	QDockWindow *stockDock;
	QComboBox *locationComboBox;
	QComboBox *speciesComboBox;

	QDockWindow *scenarioDock;
	QComboBox *waterYearComboBox;
	QComboBox *operationsComboBox;
	QComboBox *calibrationComboBox;
	QComboBox *postBonnevilleComboBox;
	QComboBox *releaseComboBox;

	QDockWindow *monteCarloDock;
	QCheckBox *monteCarloCheckBox;
	QSpinBox *monteCarloRuns;

	qIONotes *notes;

	/* Graph windows really only apply to the current scenario, so we keep track
	 * of them here and close them when the user opens a new file */
	QPtrList<qGraphDialog> graphDialogs;

public:

	qIOTool( QWidget *w );

	int isDirty(); /* If the I/O tool is dirty (i.e. not up-to-date) */
	int isOutputDirty(); /* If the tool is dirty but the changes don't need to be saved (i.e. Monte Carlo) */

	/* If the tool is loading files (in this case cells should not calculate as
	 * all the structures may not have been set up properly). This is mainly
	 * needed for the post-Bonneville hypothesis cell which causes calculations
	 * when it changes. By only calculating when the I/O tool isn't loading it
	 * avoids calculating on unfinished data so as not to crash. */
	int isLoading() {return loading;}

	int getFullScenario() {return full_scenario;} /* If the I/O tool has loaded a full scenario (i.e. it has at least one release) */

	int getAverageStart() {return avg_start;}
	int getAverageStop() {return avg_stop;}
	QString getAverageStartName() {return averageStartComboBox->currentText();}
	QString getAverageStopName() {return averageStopComboBox->currentText();}

	void enableTracking(); /* This enables tracking at locations that are necessary for I/O tool calculations */

	/* Add a graph window to the list (and have it remove itself when it is destroyed */
	void addGraphDialog(qGraphDialog *g);
	void closeGraphDialogs(); /* Close all open graph dialogs when a new file is opened */

protected:
	void drawDialog(); /* This adds the parts of the GUI that can't be generated from IOToolBase.ui */

	void addIOSheet(qIOSheet *sheet); /* Add a sheet to the stack (this adds it into the list of sheets automatically) */
	qIOSheet *getCurrentSheet(); /* Return the currently active sheet from the stack (for copy/paste UI purposes, mainly) */

	/* If the I/O tool is dirty, tell the user. This should be done before exporting,
	 * printing, or saving.
	 * The function returns nonzero if the I/O tool is up-to-date and can be saved. */
	int promptForDirty();

	/* If the user made changes and these were never saved, tell the user.
	 * Returns nonzero if the user saved the changes or there were no changes */
	int promptSaveChanges();

	/* Saves a scenario file. Note that this doesn't not check to see if
	 * anything is dirty or anything like that. */
	void saveAs();

	/* Scenario selection */
	void fillFileToPathMap(QMap<QString, QString> *map, QDir dir, const QString& extension); /* Utility function to add files to maps */
	void addDataFilesFromDir(QDir dir); /* Add the files from this directory (helper function for enumerateDataFiles()) */
	void enumerateDataFiles(); /* Insert packaged water, operations, etc. data files into the maps */
	void setupFileComboBox(QComboBox *cb, const QMap<QString, QString>& map, const char *name);
	/* Utility function for removing files that have been deleted while the model is running */
	int testExistence(QComboBox *cb, QMap<QString, QString>& map, const char *name);

	void updateMonteCarloDock();

	/* Stock selection */
	void setupLocations(int forceRecalculate = FALSE); /* Set up the location combo box */
	void setupSpecies(int forceRecalculate = FALSE); /* Set up the species combo box (this is called from setupLocations()) */

	/* Spill averaging */
	void setupAverage();
	void resetAverage();

	void showDirty(int show = TRUE); /* Show or hide dirty dots */

	/* Utility function for preserving user's selection across updates
	 * returns nonzero if the value has changed. */
	int updateComboBox(QComboBox *cb, QStringList *list, const QStringList *newList);

	/* Gets the user's name (this is pulled from the Windows registry under Windows
	 * if it exists, otherwise the user is prompted, defaulting to the user's login).
	 * This is used to set the Author field on saved scenarios.
	 * Returns QString::null if the user canceled or a name couldn't be obtained. */
	const QString& getUserName();
	int promptForUserName(); /* Prompt the user to set their user name */

	/* Timer for detecting new/deleted files */
	virtual void timerEvent(QTimerEvent *event);

	/* Prompt to save changes on a close event */
	virtual void closeEvent(QCloseEvent *e);

public slots:
	/* Event handlers. */
	virtual void cbNew();
	virtual void cbOpen();
	virtual void cbSaveAs();
	virtual void cbExportAsText();
	virtual void cbPrint();
	virtual void cbExit();
	virtual void cbCopy();
	virtual void cbPaste();
	virtual void cbEditUserName();
	virtual void cbHelp();
	virtual void cbHelpAbout();
	virtual void cbViewShowMessageLog();
	virtual void cbViewShowMap();

	/* Scenario selection */
	virtual void cbFileComboBox();

	virtual void cbMonteCarloCheckBox();
	virtual void cbMonteCarloRuns();

	/* Stock selection */
	virtual void cbLocation();
	virtual void cbSpecies();

	/* Spill averaging period */
	virtual void cbAverageComboBox();

	/* File opening-related callbacks */
	virtual void cbFileOpening();
	virtual void cbFileOpened();
	virtual void updateScenarioSelection(); /* Enable/disable based on whether we've loaded a retrospective file */
	virtual void updateStockSelection(); /* Enable/disable based on whether we've loaded a retrospective file that has no releases (i.e. it's not a full scenario) */

	virtual void cbRunScenario();
	virtual void cbCancelRun();

	virtual void cbRunCanceled();
	virtual void cbRunCompleted();

	virtual void cbApply();
	virtual void cbReset();

	/* This is also used to check RunStatusChanged since UpdateAllDataTokens is called
	 * before and after RunStatusChanged (or at least that's what I've observed) */
	virtual void cbRunStatusChanged();

	/* If data tokens are updated and the I/O sheet has not been recalculated, we
	 * notify the user by dirtying the qIOTool. */
	virtual void cbUpdateAllDataTokens();

	/* Not sure if this is necessary */
//	/* Purge all handler (called when database has changed out from under us. */
//	virtual void cbPurgeAll();

	virtual void updateDirty(); /* update dirty dots display */

	/* Callback to remove destroyed graph dialgos from the list */
	virtual void removeGraphDialog(QObject *g);

	virtual void cbZoomIn();
	virtual void cbZoomOut();

signals:
	void evApplyAll(); /* Apply all cached data (i.e. the value in the cell) to data structure */
	void evResetAll(); /* Reset all caches to data structure values */

	void evAverageChanged(); /* Spill averaging period has been changed */

	void evZoomIn();
	void evZoomOut();
};



#endif



