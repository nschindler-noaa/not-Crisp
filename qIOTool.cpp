#include "crisp.h"
#include "qIOTool.h"
#include "qIOCell.h"
#include "qGlobalEvents.h"
#include "qIOSheet.h"
#include <qlayout.h>
#include <qcombobox.h>
#include "log.h"
#include "run_mode.h"
#include "parse.h"
#include "dataio.h"
#include "file.h"
#include "qFileManager.h"
#include "qScenarioManager.h"
#include "qHelpManager.h"
#include "qPromptForString.h"
#include "Qmain.h"
#include "Qcrisp.h"
#include "QLogWindow.h"
#include "qMonteCarloManager.h"
#include "output_flags.h"
#include "version.h"
#include "qIOSIMPASSheet.h"
#include "qIOPostBonnevilleSheet.h"
#include <qlabel.h>
#include <qpushbutton.h>
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qsimplerichtext.h>
#include <qpainter.h>
#include <qpaintdevicemetrics.h>
#include <qprinter.h>
#include <qgroupbox.h>
#include <qaction.h>
#include <qvbox.h>
#include <qhbox.h>
#include <stdlib.h>
#include <qmenubar.h>
#include <qregexp.h>
#include <qclipboard.h>
#include <qcheckbox.h>
#include <qprogressdialog.h>

/* For accessing the Windows registry */
#ifdef WIN32
#include <windows.h>
#endif

/* The name for the Monte Carlo alternative that is used by the I/O tool */
#define IO_TOOL_MONTE_CARLO_ALTERN	"io-altern"
  
qIOTool::qIOTool( QWidget *w )
	  : IOToolBase ( NULL ), /* top-level window */
		userName(NULL)
{
	setCaption("COMPASS I/O Tool v" COMPASS_VERSION_MAJOR_STRING "." COMPASS_VERSION_MINOR_STRING);
	drawDialog();

	/* Connect signals */
	if (!connect(qEvents::Instance(), SIGNAL(GERunStatusChanged_()), SLOT(cbRunStatusChanged()))
		|| !connect(qEvents::Instance(), SIGNAL(GEUpdateAllDataTokens_()), SLOT(cbUpdateAllDataTokens()))
		|| !connect(&fileManager, SIGNAL(fileOpening()), this, SLOT(cbFileOpening()))
		|| !connect(&fileManager, SIGNAL(fileOpened()), this, SLOT(cbFileOpened()))
		|| !connect(&scenarioManager, SIGNAL(runCanceled()), this, SLOT(cbRunCanceled()))
		|| !connect(&scenarioManager, SIGNAL(runCompleted()), this, SLOT(cbRunCompleted())))
		log_msg(L_ERROR, "Could not connect I/O Tool signal handlers\n");

	dirty = FALSE;
	output_dirty = FALSE;
	unsaved_changes = FALSE;
	retrospective = FALSE;

	monte_carlo = FALSE;

	/* Populate lists of component input files */
	enumerateDataFiles();
	/* Load in the three default input files */
	cbFileComboBox();

	setupAverage();

	setupLocations();

	/* Start timer for detecting new files */
	startTimer(DATA_FILE_ENUMERATION_PERIOD);

	show();

	updateDirty();
	cbRunScenario();
}

void qIOTool::drawDialog() {
	QVBoxLayout *vbox = new QVBoxLayout(sheetFrame);

	/* Create stock selection dock */
	stockDock = new QDockWindow(this);
	stockDock->setCaption("Fish Stock");
	moveDockWindow(stockDock, Qt::DockTop, TRUE, 1);
	QLabel *stockTitle = new QLabel("<b>Fish Stock</b>:", stockDock);
	stockTitle->setAlignment(Qt::AlignAuto | Qt::AlignVCenter | Qt::SingleLine);
	stockTitle->setTextFormat(Qt::RichText);
	stockDock->boxLayout()->addWidget(stockTitle);
	stockDock->boxLayout()->addWidget(new QLabel("Location: ", stockDock));
	stockDock->boxLayout()->addWidget(locationComboBox = new QComboBox(stockDock));
	stockDock->boxLayout()->addSpacing(10);
	stockDock->boxLayout()->addWidget(new QLabel("Species: ", stockDock));
	stockDock->boxLayout()->addWidget(speciesComboBox = new QComboBox(stockDock));
	stockDock->boxLayout()->addStretch(1);
	stockDock->setHorizontallyStretchable(TRUE);

	/* Create scenario selection dock */
	scenarioDock = new QDockWindow(this);
	scenarioDock->setCaption("Scenario");
	moveDockWindow(scenarioDock, Qt::DockTop, TRUE, 2);
	QLabel *scenarioTitle = new QLabel("<b>Scenario</b>:", scenarioDock);
	scenarioTitle->setTextFormat(Qt::RichText);
	scenarioDock->boxLayout()->addWidget(scenarioTitle);
	scenarioDock->boxLayout()->addWidget(new QLabel("Water Year: ", scenarioDock));
	scenarioDock->boxLayout()->addWidget(waterYearComboBox = new QComboBox(scenarioDock));
	scenarioDock->boxLayout()->addSpacing(10);
	scenarioDock->boxLayout()->addWidget(new QLabel("Operations: ", scenarioDock));
	scenarioDock->boxLayout()->addWidget(operationsComboBox = new QComboBox(scenarioDock));
	scenarioDock->boxLayout()->addSpacing(10);
	scenarioDock->boxLayout()->addWidget(new QLabel("Calibration: ", scenarioDock));
	scenarioDock->boxLayout()->addWidget(calibrationComboBox = new QComboBox(scenarioDock));
	scenarioDock->boxLayout()->addSpacing(10);
	scenarioDock->boxLayout()->addWidget(new QLabel("Release: ", scenarioDock));
	scenarioDock->boxLayout()->addWidget(releaseComboBox = new QComboBox(scenarioDock));
	scenarioDock->boxLayout()->addSpacing(10);
	scenarioDock->boxLayout()->addWidget(new QLabel("Post-Bonneville: ", scenarioDock));
	scenarioDock->boxLayout()->addWidget(postBonnevilleComboBox = new QComboBox(scenarioDock));
	scenarioDock->boxLayout()->addStretch(1);
	scenarioDock->setHorizontallyStretchable(TRUE);

	/* Create Monte Carlo dock */
	monteCarloDock = new QDockWindow(this);
	monteCarloDock->setCaption("Monte Carlo Mode");
	moveDockWindow(monteCarloDock, Qt::DockTop, TRUE, 3);
	QLabel *monteCarloTitle = new QLabel("<b>Monte Carlo</b>:", monteCarloDock);
	monteCarloTitle->setAlignment(Qt::AlignAuto | Qt::AlignVCenter | Qt::SingleLine);
	monteCarloTitle->setTextFormat(Qt::RichText);
	monteCarloDock->boxLayout()->addWidget(monteCarloTitle);
	monteCarloDock->boxLayout()->addWidget(monteCarloCheckBox = new QCheckBox("Run the model ", monteCarloDock));
	monteCarloCheckBox->setChecked(FALSE);
	monteCarloDock->boxLayout()->addWidget(monteCarloRuns = new QSpinBox(monteCarloDock));
	monteCarloRuns->setMinValue(2);
	monteCarloRuns->setMaxValue(9999);
	monteCarloRuns->setEnabled(FALSE);
	monteCarloDock->boxLayout()->addWidget(new QLabel(" times", monteCarloDock));
	monteCarloDock->boxLayout()->addStretch(1);
	monteCarloDock->setHorizontallyStretchable(TRUE);

	/* Create spill averaging period dock */
	averageDock = new QDockWindow(this);
	averageDock->setCaption("Flow/Spill Averaging");
	moveDockWindow(averageDock, Qt::DockTop, TRUE, 4);
	QLabel *averageTitle = new QLabel("<b>Flow/Spill Averaging</b>:", averageDock);
	averageTitle->setAlignment(Qt::AlignAuto | Qt::AlignVCenter | Qt::SingleLine);
	averageTitle->setTextFormat(Qt::RichText);
	averageDock->boxLayout()->addWidget(averageTitle);
	averageDock->boxLayout()->addWidget(averageStartComboBox = new QComboBox(averageDock));
	averageDock->boxLayout()->addWidget(new QLabel(" through ", averageDock));
	averageDock->boxLayout()->addWidget(averageStopComboBox = new QComboBox(averageDock));
	averageDock->boxLayout()->addStretch(1);
	averageDock->setHorizontallyStretchable(TRUE);

	/* Create notes dock */
	notes = new qIONotes(this);
	moveDockWindow(notes, Qt::DockBottom);

	/* Connect signal handlers */
	if (!connect(locationComboBox, SIGNAL(activated(int)), SLOT(cbLocation()))
		|| !connect(speciesComboBox, SIGNAL(activated(int)), SLOT(cbSpecies()))
		|| !connect(releaseComboBox, SIGNAL(activated(int)), SLOT(cbFileComboBox()))
		|| !connect(waterYearComboBox, SIGNAL(activated(int)), SLOT(cbFileComboBox()))
		|| !connect(operationsComboBox, SIGNAL(activated(int)), SLOT(cbFileComboBox()))
		|| !connect(calibrationComboBox, SIGNAL(activated(int)), SLOT(cbFileComboBox()))
		|| !connect(postBonnevilleComboBox, SIGNAL(activated(int)), SLOT(cbFileComboBox()))
		|| !connect(monteCarloCheckBox, SIGNAL(toggled(bool)), SLOT(cbMonteCarloCheckBox()))
		|| !connect(monteCarloRuns, SIGNAL(valueChanged(int)), SLOT(cbMonteCarloRuns()))
		|| !connect(averageStartComboBox, SIGNAL(activated(int)), SLOT(cbAverageComboBox()))
		|| !connect(averageStopComboBox, SIGNAL(activated(int)), SLOT(cbAverageComboBox()))
		|| !connect(this, SIGNAL(evApplyAll()), notes, SLOT(cbApply()))
		|| !connect(this, SIGNAL(evResetAll()), notes, SLOT(cbReset()))
		|| !connect(&notesIDC, SIGNAL(dirtyChanged()), this, SLOT(updateDirty())))
		log_msg(L_ERROR, "Could not connect I/O tool signal handlers\n");

	/* Create tabbed stack for the sheets */
	sheetStack = new QTabWidget(sheetFrame);
	vbox->addWidget(sheetStack);

	/* Add the main hydrosystem sheet */
	addIOSheet(new qIOSIMPASSheet(sheetStack, this, "Hydrosystem"));
	addIOSheet(new qIOPostBonnevilleSheet(sheetStack, this, "Post-Bonneville"));

	/* Create the View menu after creating the sheet because it references the sheet */
	/* Create View menu for docks/toolbars (count() - 1 position so Help menu is last) */
	QPopupMenu *viewMenu = new QPopupMenu();
	viewMenu->insertItem("Zoom In", this, SLOT(cbZoomIn()), CTRL+Key_Equal);
	viewMenu->insertItem("Zoom Out", this, SLOT(cbZoomOut()), CTRL+Key_Minus);
	viewMenu->insertItem("Toolbars", createDockWindowMenu());
	/* Add option to show the message log and the old CRiSP GUI */
	viewMenu->insertSeparator();
	viewShowMessageLog->addTo(viewMenu);
	viewShowMap->addTo(viewMenu);
	menuBar()->insertItem("View", viewMenu, -1, menuBar()->count() - 1);
}

void qIOTool::addIOSheet(qIOSheet *sheet) {
	sheets.append(sheet);
	sheetStack->addTab(sheet, sheet->getName());
}
qIOSheet* qIOTool::getCurrentSheet() {
	return (qIOSheet*) sheetStack->currentPage();
}

int qIOTool::isDirty() {
	for (QPtrListIterator<qIOSheet> it(sheets); it.current(); ++it) {
		qIOSheet *sheet = it.current();
		if (sheet->isDirty()) return true;
	}
	return dirty || notes->isDirty();
}
int qIOTool::isOutputDirty() {
	return output_dirty || isDirty();
}

int qIOTool::promptForDirty() {
	if (isDirty()) {
		if (QMessageBox::critical(this, "Apply Changes?", "The I/O tool outputs are not up to date. Changes to input cells should be\napplied and the model may need to be run again.\n\nApply changes and run the model?", "&Cancel", "Appl&y", QString::null, 1, 0)) {
			/* Apply and check to see if that's all that was needed */
			cbApply();
			if (isDirty()) {
				/* Run the scenario and hope that's all that was needed */
				cbRunScenario();
				return 0;
			}
			else return 1;
		}
		else return 0;
	}
	else return 1;
}

int qIOTool::promptSaveChanges() {
	if (unsaved_changes || isDirty()) {
		if (QMessageBox::critical(this, "Unsaved Changes", "You may have made changes that have not been saved.\n\nSave changes to a new scenario file?", "Dis&card", "&Save", QString::null, 1, 0)) {
			if (isDirty()) {
				if (QMessageBox::critical(this, "Apply Changes?", "Changes to input cells have not been applied.\n\nApply these changes before saving?", "Dis&card", "Appl&y", QString::null, 1, 0)) {
					cbApply();
				}
			}

			saveAs();
		}
	}

	/* There actually isn't a way for the user to cancel the operation they were performing,
	 * but it seems unlikely that the user would want to cancel what they were doing, so
	 * this probably isn't an issue. */
	return 1;
}

void qIOTool::saveAs() {
	QString userName = getUserName();
	if (userName == QString::null) return; /* The user canceled */

	char *old_creator = file_creator;
	char *old_modifier = file_modifier;

	if (!file_creator) file_creator = strdup(userName.ascii());
	else file_modifier = strdup(userName.ascii());

	if (fileManager.fileSaveAs()) {
		/* Error or cancel, revert file creator and modifier */
		if (file_creator && file_creator != old_creator) free(file_creator);
		if (file_modifier && file_modifier != old_modifier) free(file_modifier);

		file_creator = old_creator;
		file_modifier = old_modifier;
	}
	else {
		/* Can free old creator/modifier if we're not still using them */
		if (old_creator && old_creator != file_creator) free(old_creator);
		if (old_modifier && old_modifier != file_modifier) free(old_modifier);
	}

	fileCreatorIDC.reset();
	fileCreationDateIDC.reset();
	fileModifierIDC.reset();
	fileModificationDateIDC.reset();

	/* File was just saved */
	unsaved_changes = FALSE;
}

const QString& qIOTool::getUserName() {
	/* Perhaps under UNIX we should just use the user name without even asking. */
	if (userName == QString::null) {
#ifdef WIN32
	/* Check the Windows registry to find user's name */
	HKEY hKey;
	unsigned long userSize = 256;
	unsigned char user[256];
	unsigned long type;
	HKEY hive = HKEY_CURRENT_USER;

	if (RegOpenKeyExA(hive, WINDOWS_REGISTRY_SUBKEY, 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS
		&& RegQueryValueExA(hKey, "Name", NULL, &type, user, &userSize) == ERROR_SUCCESS
		&& type == REG_SZ)
	{
		userName = QString((char*) user);
		RegCloseKey(hKey);
	}
#endif

	if (userName == QString::null && !promptForUserName()) return QString::null;
	}
	return userName;
}
int qIOTool::promptForUserName() {
	QString initVal = userName;
	const char *initValC = getenv("USER");
	if (!initValC) initValC = getenv("USERNAME");
	if (initVal == QString::null) initVal = QString(initValC);

	bool ok;
	QString username = qPromptForString::getString("User Name", "Please enter your name", initVal, &ok);
	if (ok) {
		userName = username;
#ifdef WIN32
	/* Set the user's name in the Windows registry */
	HKEY hKey;
	HKEY hive = HKEY_CURRENT_USER;

	if (RegCreateKeyExA(hive, WINDOWS_REGISTRY_SUBKEY, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, NULL, &hKey, NULL) == ERROR_SUCCESS)
	{
		RegSetValueExA(hKey, "Name", 0, REG_SZ, (const unsigned char*) userName.ascii(), userName.length() + 1);
		RegCloseKey(hKey);
	}
#endif
		return TRUE;
	}
	else return FALSE;
}


/* Note that in these two functions, we could not simply continue if we called
 * scenarioManager.RunCancel(). This is because the model may still be running
 * (i.e. it is still inside do_seg_new()) and if we delete data out from under
 * it, it will crash. */
void qIOTool::cbNew() {
    if (run_mode.running) return;

	cbFileComboBox();
}
void qIOTool::cbOpen() {
    if (run_mode.running) return;

	QString s = QFileDialog::getOpenFileName(
		QString::null,
		"Input Files (*.scn;*.dat;*.riv;*.ops;*.clb;*.etc;*.ctrl;*.rls);;Retrospective (*.scn;*.dat);;River Environment (*.riv);;Operations (*.ops);;Calibration (*.clb);;Post-Bonneville (*.pbn);;Config/Override (*.etc);;Control (*.ctrl)",
		this,
		"Open file dialog"
		"Read File");

	if (s == QString::null) return;

	if (s.endsWith(".scn") || s.endsWith(".dat") || s.endsWith(".ctrl")) {
		fileManager.openFile(s);

		/* New scenario file should replace everything */
		unsaved_changes = FALSE;

		return; /* The rest of the code is for file combo boxes */
	}

	/* File combo boxes */
	/* Note that these files are not actually loaded in here (they are loaded in
	 * cbFileComboBox()), but they are added to the appropriate drop-down box and
	 * then selected, which then causes them to be loaded. */
	log_msg(L_ERROR, "Opening %s for writing\n", s.ascii());
	QMap<QString, QString> *map;
	QComboBox *cb;
	const char *name;
	if (s.endsWith(".riv")) {
		cb = waterYearComboBox;
		map = &waterYearFileToPath;
		name = "water year";
	}
	else if (s.endsWith(".ops")) {
		cb = operationsComboBox;
		map = &operationsFileToPath;
		name = "operations";
	}
	else if (s.endsWith(".clb")) {
		cb = calibrationComboBox;
		map = &calibrationFileToPath;
		name = "calibration";
	}
	else if (s.endsWith(".rls")) {
		cb = releaseComboBox;
		map = &releaseFileToPath;
		name = "release";
	}
	else if (s.endsWith(".pbn")) {
		cb = postBonnevilleComboBox;
		map = &postBonnevilleFileToPath;
		name = "post-Bonneville";
	}
	else {
		log_msg(L_ERROR, "Unknown file type for %s\n", s.ascii());
		return;
	}

	QFileInfo info(s);
	waterYearFileToPath.insert(info.fileName(), info.filePath());
	setupFileComboBox(cb, *map, name);
	cb->setCurrentText(info.fileName());
	cbFileComboBox();
}
void qIOTool::cbSaveAs() {
	if (promptForDirty()) {
		saveAs();
	}
}

void qIOTool::cbFileOpening() {
	loading = TRUE;

	/* Close open graph windows since data structure will probably change and
	 * the windows would not make sense (and might crash the application). */
	closeGraphDialogs();

	/* This will be set to FALSE if the file opening is being done from the scenario
	 * selection box in the I/O tool. */
	retrospective = TRUE;

	/* Monte Carlo currently can't be saved, so when a file is loaded, it's not Monte Carlo */
	monte_carlo = FALSE;

	/* The sheet will not reflect the newly-loaded file, initially */
	dirty = TRUE;
}

void qIOTool::cbFileOpened() {
	updateScenarioSelection();
	updateMonteCarloDock();

	full_scenario = run->release_list != NULL;
	updateStockSelection();

	if (!full_scenario) {
		/* Notify the user that no releases are available */
		QMessageBox::critical(NULL, "No Releases", "There are no releases in the specified scenario, so the I/O tool will not be helpful.\n\nYou may want to use the Map GUI (View -> Show Map).", QMessageBox::NoButton, QMessageBox::Ok);
	}

	/* Run the scenario to fill in cells with correct data */
	scenarioManager.runScenario();

	loading = FALSE;
}

void qIOTool::cbExportAsText() {
	if (promptForDirty()) {
		QString name = QFileDialog::getSaveFileName (QString::null, "Text Documents (*.txt)", this, 
			"Export to Text File", "Export to Text File...");


		if (name != QString::null) {
			if (!name.endsWith(".txt")) name.append(".txt");

			if (QFile::exists(name)) {
				int answer = QMessageBox::warning ( this, "COMPASS", "File already exists, overwrite it?", 
					"&Cancel", "&OK",  QString::null, 1, -1 );
				if (answer != 1)
					return;		
			}

			FILE *f = fopen(name.ascii(), "w");
			if (f) {
				/* TODO: more appropriate header */
				fprintf(f, "%s: %s\n", locationComboBox->currentText().ascii(), speciesComboBox->currentText().ascii());

				for (QPtrListIterator<qIOSheet> it(sheets); it.current(); ++it) {
					qIOSheet *sheet = it.current();
					fprintf(f, "%s:\n", sheet->getName().ascii());
					sheet->exportAsText(f);
				}
				fclose(f);
			}
			else log_msg(L_ERROR, "Could not open %s for writing\n", name.ascii());
		}
	}
}

void qIOTool::cbPrint() {
	if (promptForDirty()) {
		QValueList<QString> htmlPages; /* Pages (one HTML document per page) */
		for (QPtrListIterator<qIOSheet> it(sheets); it.current(); ++it) {
			QString html = "<html><body>\n";

			html += "<h3>" + locationComboBox->currentText() + ": " + speciesComboBox->currentText() + " (" + waterYearComboBox->currentText().remove(QRegExp("\\..*$")) + ", " + operationsComboBox->currentText() + ")</h3>\n";
			qIOSheet *sheet = it.current();
			html += "<h4>" + sheet->getName() + "</h4>\n";
			html += sheet->exportAsHTML();

			/* Append notes to the last page */
			if (it.current() == sheets.last()) {
				if (input_notes && input_notes[0] != '\0') {
					/* Only bother printing the notes section if input_notes is not null
					 * and input_notes is not an empty string */
					html += "<h3>Notes:</h3>\n";
					html += "<p>" + QString(input_notes).replace("&", "&amp;").replace("<", "&lt;").replace(">", "&gt;").replace(QRegExp("\n\n*"), "</p><p>") + "</p>\n";
					html += "</html></body>\n";
				}
			}

			html += "</html></body>\n";
			htmlPages.append(html);
		}

// This is copied from Trolltech's example code, so it should work. Print to file works
#ifndef QT_NO_PRINTER
		QPrinter printer( QPrinter::HighResolution );
		printer.setFullPage(TRUE);
		printer.setOrientation(QPrinter::Portrait);
		if ( printer.setup( this ) ) {
		QPainter p( &printer );
		if( !p.isActive() ) // starting printing failed
			return;
		QPaintDeviceMetrics metrics(p.device());
		int dpiy = metrics.logicalDpiY();
		int margin = (int) ( (0.5)*dpiy ); // 0.5 in margins
		QRect body( margin, margin, metrics.width() - 2*margin, metrics.height() - 2*margin );
		QRect view( body );
		int page = 1;
		int index = 0;

		for (QValueListConstIterator<QString> it = htmlPages.constBegin(); it != htmlPages.end(); ++it, index++) {
			if (it != htmlPages.begin()) {
				printer.newPage();
				page++;
			}

			QSimpleRichText richText( *it,
						  QFont(),
						  QString::null,
						  NULL,
						  NULL,
						  body.height() );
			richText.setWidth( &p, body.width() );
			int offset = (page - 1)*body.height();
			do {
				richText.draw( &p, body.left(), body.top() + offset, view, colorGroup() );
				view.moveBy( 0, body.height() );
				p.translate( 0 , -body.height() );
				p.drawText( view.right() - p.fontMetrics().width( QString::number(page) ),
					view.bottom() + p.fontMetrics().ascent() + 5, QString::number(page) );
				if ( view.top()  >= richText.height() )
				break;
				printer.newPage();
				page++;
			} while (TRUE);
		}

		}
#endif
	}
}
void qIOTool::cbExit() {
	close();
}

void qIOTool::cbCopy() {
	QApplication::clipboard()->setData(getCurrentSheet()->copy());
}
void qIOTool::cbPaste() {
	getCurrentSheet()->paste(QApplication::clipboard()->data());
}

void qIOTool::cbEditUserName() {
	promptForUserName();
}

void qIOTool::cbHelpAbout() {
	helpManager->about();
}

void qIOTool::cbHelp() {
	/* TODO: pick a help page!!! */
	/* TODO: how does a help page work if we shouldn't reference the main window? */
//	mainWindow->openHelpPage(QString("75.html"));
}

void qIOTool::cbViewShowMessageLog() {
	QLogWindow *logw = QLogWindow::instance(TRUE);
	logw->show();
	logw->raise();
}

void qIOTool::cbViewShowMap() {
	getQCrisp()->raise();
}

void qIOTool::cbRunStatusChanged() {
	/* Enable/disable widgets */
	fileNew->setEnabled(!run_mode.running);
	fileOpen->setEnabled(!run_mode.running);
	fileSaveAs->setEnabled(!run_mode.running);
	filePrint->setEnabled(!run_mode.running);
	fileExportAsText->setEnabled(!run_mode.running);
	editCopy->setEnabled(!run_mode.running);
	editPaste->setEnabled(!run_mode.running);
	stockDock->setEnabled(!run_mode.running);
	averageDock->setEnabled(!run_mode.running);
	for (QPtrListIterator<qIOSheet> it(sheets); it.current(); ++it) {
		qIOSheet *sheet = it.current();
		sheet->setEnabled(!run_mode.running);
	}
	runScenarioButton->setEnabled(!run_mode.running);
	cancelRunButton->setEnabled(run_mode.running);
	applyButton->setEnabled(!run_mode.running);
	resetButton->setEnabled(!run_mode.running);
	notes->setEnabled(!run_mode.running);

	if (!run_mode.running) {
		updateScenarioSelection(); /* Enable/disable scenario selection box as appropriate */
		updateStockSelection(); /* Enable only if there is a release */

		/* Assuming a model run just finished */
		setupLocations(TRUE);
	}
	else {
		scenarioDock->setEnabled(FALSE);
		monteCarloDock->setEnabled(FALSE);
	}
}

void qIOTool::fillFileToPathMap(QMap<QString, QString> *map, QDir dir, const QString& extension) {
	/* Only show readable files that have the given extension */
	dir.setFilter(QDir::Files  | QDir::Readable);
	dir.setNameFilter("*." + extension);

	if (dir.count() <= 0) {
		log_msg(L_WARNING, "Could not find any *.%s files in %s! Try running the program from its install directory.\n", extension.ascii(), dir.absPath().ascii());
	}

	/* Add files and their paths to the map */
	for (unsigned int i=0; i < dir.count(); i++) {
		/* Don't add files with the same name multiple times.
		 *
		 * NOTE: this means that if the user has two files with the same name,
		 * only one of them will be able to be selected. This could be
		 * confusing! */

		if (!map->contains(dir[i])) map->insert(dir[i], dir.filePath(dir[i]));
	}
}

void qIOTool::addDataFilesFromDir(QDir dir) {
	if (dir.cd("riv")) {
		fillFileToPathMap(&waterYearFileToPath, dir, "riv");
		dir.cdUp();
	}
	if (dir.cd("ops")) {
		fillFileToPathMap(&operationsFileToPath, dir, "ops");
		dir.cdUp();
	}
	if (dir.cd("clb")) {
		fillFileToPathMap(&calibrationFileToPath, dir, "clb");
		dir.cdUp();
	}
	if (dir.cd("pbn")) {
		fillFileToPathMap(&postBonnevilleFileToPath, dir, "pbn");
		dir.cdUp();
	}
	if (dir.cd("rls")) {
		fillFileToPathMap(&releaseFileToPath, dir, "rls");
		dir.cdUp();
	}
}

void qIOTool::enumerateDataFiles() {
	QDir dir;

	addDataFilesFromDir(dir); /* Current directory */
	/* Now try the directory that the executable is in */
	addDataFilesFromDir(QDir(QString(get_image_location())));

#ifdef WIN32
	/* Check the Windows registry to find the installation path */
	HKEY hKey;
	unsigned long pathSize;
	unsigned char path[1024];
	unsigned long type;
	/* Check both the current user and the local machine */
	HKEY hives[] = {HKEY_LOCAL_MACHINE, HKEY_CURRENT_USER};

	for (int i=0; i < 2; i++) {
		pathSize = 1024;
		if (RegOpenKeyExA(hives[i], WINDOWS_REGISTRY_SUBKEY, 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS
			&& RegQueryValueExA(hKey, "Path", NULL, &type, path, &pathSize) == ERROR_SUCCESS
			&& type == REG_SZ)
		{
			addDataFilesFromDir(QDir(QString((char*) path)));
			RegCloseKey(hKey);
		}
	}
#endif

	/* TODO: some UNIX-specific code would be nice if this thing needs to run on UNIX */

	setupFileComboBox(waterYearComboBox, waterYearFileToPath, "water year");
	setupFileComboBox(operationsComboBox, operationsFileToPath, "operations");
	setupFileComboBox(calibrationComboBox, calibrationFileToPath, "calibration");
	setupFileComboBox(releaseComboBox, releaseFileToPath, "release");
	setupFileComboBox(postBonnevilleComboBox, postBonnevilleFileToPath, "post-Bonneville");
}

void qIOTool::setupFileComboBox(QComboBox *cb, const QMap<QString, QString>& map, const char *name) {
	QMap<QString, QString>::ConstIterator it;

	/* Save current selection if possible */
	QString current = cb->currentText();

	cb->clear();
	for (it = map.begin(); it != map.end(); ++it) {
		//printf("Inserting key %s\n",it.key());
		cb->insertItem(it.key());
	}

	if (cb->count() <= 0) log_msg(L_ERROR, "Could not find %s input files!\n", name);
	else {
		/* Attempt to keep the same file selected */
		for (int i=0; i < cb->count(); i++) {
			if (current == cb->text(i)) {
				cb->setCurrentItem(i);
				break;
			}
		}
	}
}

int qIOTool::testExistence(QComboBox *cb, QMap<QString, QString>& map, const char *name) {
	QString path = map[cb->currentText()];
	QFileInfo info(path);

	if (!info.exists()) {
		log_msg(L_ERROR, "Could not locate %s file: %s\n", name, cb->currentText().ascii());
	}
	else if (!info.isReadable()) {
		log_msg(L_ERROR, "Unable to read %s file: %s\n", name, cb->currentText().ascii());
	}
	else return true;

	map.remove(cb->currentText());
	setupFileComboBox(cb, map, name);

	return false;
}

void qIOTool::cbFileComboBox() {
	/* We handle the file loading ourselves here (as opposed to using qFileManager's generic routines) */
	QString pbnFile = postBonnevilleFileToPath[postBonnevilleComboBox->currentText()];
	QString clbFile = calibrationFileToPath[calibrationComboBox->currentText()];
	QString rivFile = waterYearFileToPath[waterYearComboBox->currentText()];
	QString rlsFile = releaseFileToPath[releaseComboBox->currentText()];
	QString opsFile = operationsFileToPath[operationsComboBox->currentText()];

	if (pbnFile != "" && clbFile != "" && rivFile != "" && opsFile != "" && rlsFile != "") {
		log_msg(L_DBGMSG, "Loading %s: %s (%s, %s)\n", waterYearComboBox->currentText().ascii(), operationsComboBox->currentText().ascii(), calibrationComboBox->currentText().ascii(), postBonnevilleComboBox->currentText().ascii());

		if (!testExistence(waterYearComboBox, waterYearFileToPath, "water year")
			|| !testExistence(operationsComboBox, operationsFileToPath, "operations")
			|| !testExistence(calibrationComboBox, calibrationFileToPath, "calibration")
			|| !testExistence(releaseComboBox, releaseFileToPath, "release")
			|| !testExistence(postBonnevilleComboBox, postBonnevilleFileToPath, "post-Bonneville"))
		{
			return;
		}

		promptSaveChanges();

		/* Reset all */
		reset_all_spill();
		reset_all_dams();
		reset_all_releases();

		fileManager.emitFileOpening();
		retrospective = FALSE; /* Override value from fileOpening() signal since this is the only place that non-retrospective loads take place */

		/* Load the three component input files */

		/* Replace upon loading first file */
		parse_datafile(postBonnevilleFileToPath[postBonnevilleComboBox->currentText()].ascii(), 1, 0);
		parse_datafile(calibrationFileToPath[calibrationComboBox->currentText()].ascii(), 1, 0);
		parse_datafile(waterYearFileToPath[waterYearComboBox->currentText()].ascii(), 0, 0);
		parse_datafile(operationsFileToPath[operationsComboBox->currentText()].ascii(), 0, 1);
		parse_datafile(releaseFileToPath[releaseComboBox->currentText()].ascii(), 0, 1);

		/* I used to think this would cause problems with the CRiSP GUI, but it doesn't
		 * appear to and it's necessary in order for the files to be reloaded properly. */
		qEvents::Instance()->emitGEPurgeAllIDCs_();
		qEvents::Instance()->emitGEUpdateAllDataTokens_();

		/* Tell classes (e.g. topLevel) that a file has been opened */
		fileManager.emitFileOpened();

		/* Files have been reloaded */
		unsaved_changes = FALSE;
	}
}

int qIOTool::updateComboBox(QComboBox *cb, QStringList *list, const QStringList *newList) {
	if (*newList != *list) {
		int i;
		QString current = cb->currentText();

		*list = *newList;
		cb->clear();
		cb->insertStringList(*list);
		/* Attempt to select the same location */
		for (i=0; i < cb->count(); i++) {
			if (current == cb->text(i)) {
				cb->setCurrentItem(i);
				return 1;
			}
		}
		/* Otherwise select the first one */
		if (i >= cb->count()) {
			cb->setCurrentItem(0);
			return 1;
		}
	}

	return 0;
}

void qIOTool::setupLocations(int forceRecalculate) {
	QStringList newLocationNames;
	struct river_segment *hwseg;

	/* Find all the headwater segments */
	for (hwseg = river_mouth; hwseg; hwseg = hwseg->next) {
		if (hwseg->seg_type == HEADWATER_SEG) {
			QString riverName = QString(hwseg->river_name);

			/* Only add locations where a release exists on that river */
			struct release *rel;
			for (rel = run->release_list; rel; rel = rel->next) {
				if (riverName == rel->rls_seg->river_segment->river_name) {
					newLocationNames.append(riverName);
					break;
				}
			}
		}
	}

	full_scenario = newLocationNames.size() > 0;
	updateStockSelection();

	/* Check to see if selection must change or we want to pretend it changed */
	if (updateComboBox(locationComboBox, &locationNames, &newLocationNames)
		|| forceRecalculate)
		cbLocation();
}

void qIOTool::setupSpecies(int forceRecalculate) {
	QStringList newSpeciesNames;
	QString riverName = locationComboBox->currentText();

	/* Find all releases from that river to determine list of species */
	struct release *rel;
	for (rel = run->release_list; rel; rel = rel->next) {
		if (riverName == rel->rls_seg->river_segment->river_name
			&& !newSpeciesNames.contains(QString(rel->species->name))) {

			newSpeciesNames.append(rel->species->name);
		}
	}

	/* Check to see if selection must change */
	if (updateComboBox(speciesComboBox, &speciesNames, &newSpeciesNames)
		|| forceRecalculate)
		cbSpecies();
}

void qIOTool::cbSpecies() {
	struct release *rel;
	QPtrList<struct release> releases;

	for (rel = run->release_list; rel; rel = rel->next) {
		if (locationComboBox->currentText() == rel->rls_seg->river_segment->river_name
			&& speciesComboBox->currentText() == rel->species->name) {

			releases.append(rel);
			log_msg(L_DBGMSG, "Selecting stock %s\n", rel->stock->name);
		}
	}

	for (QPtrListIterator<qIOSheet> it(sheets); it.current(); ++it) {
		qIOSheet *sheet = it.current();
		sheet->setReleases(releases);
	}
}

void qIOTool::cbLocation() {
	setupSpecies(TRUE);
}

void qIOTool::showDirty(int show) {
	bpDirty1->setShown(show);
	bpDirty2->setShown(show);
	bpDirty3->setShown(show);
	bpDirty4->setShown(show);
}

void qIOTool::updateScenarioSelection() {
	scenarioDock->setEnabled(!retrospective && !run_mode.running);
	monteCarloDock->setEnabled(!run_mode.running);
}
void qIOTool::updateStockSelection() {
	stockDock->setEnabled(full_scenario && !run_mode.running);
	runScenarioButton->setEnabled(full_scenario && !run_mode.running);
}

void qIOTool::enableTracking() {
	struct river_segment *seg;

	/* Make sure that route tracking and passage are on at all dams so that
	 * we can actually do useful calculations after the run is completed */
	for (seg = river_mouth; seg; seg = seg->next) {
		if (seg->seg_type == DAM_SEG) seg->output_settings |= ROUTING_FLAG | PASSAGE_FLAG | TRANSPORT_FLAG;
		else if (seg->seg_type == REACH_SEG) seg->output_settings |= TRANSPORT_FLAG;
	}
}

void qIOTool::cbRunScenario() {
	runScenarioButton->setFocus(); /* Get the focus so that a cell that is being edited will be forced to update */

	/* If some of the inputs have been changed, ask the user to apply them first,
	 * otherwise, we cancel the model run because the sheet wouldn't really make
	 * sense upon completion. */
	for (QPtrListIterator<qIOSheet> it(sheets); it.current(); ++it) {
		qIOSheet *sheet = it.current();
		if (sheet->isDirty()) {
			if (QMessageBox::question ( this, "Apply Changes?",
				"Changes have been made to input cells that should either be reset or applied.\n\nApply all changes and execute model run?", 
				"&Cancel", "Appl&y",  QString::null, 1, 0 )) {
				cbApply();
			}
			else return;
		}
	}

	if (monteCarloCheckBox->isChecked()) {
		/* Monte Carlo run */
		/* Create a Monte Carlo alternative with appropriate settings */

		/* First get the alternative (because this will create a directory) */
		int index = qMonteCarloManager::get()->getAlternative(IO_TOOL_MONTE_CARLO_ALTERN);
		/* Delete the alternative if it alread exists */
		if (index >= 0) qMonteCarloManager::get()->deleteAlternative(IO_TOOL_MONTE_CARLO_ALTERN);
		
		index = qMonteCarloManager::get()->createAlternative(IO_TOOL_MONTE_CARLO_ALTERN);

		if (index < 0) {
			log_msg(L_ERROR, "Could not create Monte Carlo alternative\n");
			return;
		}

		/* Set input file and years */
		QString fileName = QDir::current().absPath().append("/" IO_TOOL_MONTE_CARLO_ALTERN "/input.scn");
		struct altern_data *alt = &(qMonteCarloManager::get()->getMonteData()->alts[index]);
		alt->nyears = 1;
		alt->ngames = monteCarloRuns->value();
		alt->changed = TRUE;
		alt->flow_archive = NULL;
		alt->use_flow_from = FALSE;
		alt->use_spill_from = FALSE;
		alt->use_elev_from = FALSE;
		alt->input_files = (struct input_file*) malloc(1*sizeof(struct input_file));
		alt->input_files[0].file_name = strdup(fileName.ascii());
		alt->input_files[0].list = NULL;

		qMonteCarloManager::get()->applyLocalList(index);

		/* Set this alternative as the only one to run */
		QStringList altsToRun;
		altsToRun << IO_TOOL_MONTE_CARLO_ALTERN;
		qMonteCarloManager::get()->setRunList(altsToRun);

		qMonteCarloManager::get()->writeAltsToRun();

		/* Now save an input file */
		enableTracking(); /* Tracking must be enabled */
		if (fileManager.saveFile(fileName)) {
			log_msg(L_ERROR, "Could not write output file %s\n", fileName.ascii());
			return;
		}

		/* Create a progress dialog so the user knows computation is working */
		QProgressDialog *pd = new QProgressDialog("Running the model...", "Cancel", 100, this, "progressdialog", FALSE, WDestructiveClose);
		pd->setCaption("Monte Carlo Progress");
		pd->setMinimumDuration(0); /* Just show the dialog immediately */
		connect(&scenarioManager, SIGNAL(runCanceled()), pd, SLOT(close()));
		connect(&scenarioManager, SIGNAL(runCompleted()), pd, SLOT(close()));
		connect(pd, SIGNAL(canceled()), &scenarioManager, SLOT(runCancel()));
		connect(&scenarioManager, SIGNAL(runProgressUpdate(int)), pd, SLOT(setProgress(int)));

		/* Finally, run the model */
		qMonteCarloManager::get()->runMonteCarlo();
	}
	else {
		/* Normal scenario run */
		scenarioManager.runScenario();
	}
}
void qIOTool::cbCancelRun() {
	scenarioManager.runCancel();
}
void qIOTool::cbRunCanceled() {
	dirty = TRUE;
	updateDirty();
}
void qIOTool::cbRunCompleted() {
	dirty = FALSE;
	output_dirty = FALSE;
	updateDirty();
}

void qIOTool::cbUpdateAllDataTokens() {
	/* If data tokens are being updated, this means that the I/O sheet may be
	 * displaying data that hasn't been updated. */
	dirty = TRUE;
	updateDirty();
}

void qIOTool::cbApply() {
	/* If ANY changes at all have been made, they have not yet been saved */
	unsaved_changes = TRUE;

	applyButton->setFocus(); /* Get focus so that an active cell will be updated */

	emit evApplyAll();
}

void qIOTool::cbReset() {
	resetButton->setFocus(); /* Get focus so that an active cell will be updated */

	emit evResetAll();
}

void qIOTool::updateDirty() {
	/* Show/hide dirty dots */
	showDirty(isOutputDirty());
}

void qIOTool::timerEvent(QTimerEvent *event) {
	enumerateDataFiles();
}

void qIOTool::closeEvent(QCloseEvent *e) {
	promptSaveChanges();

	e->accept();
}

/* Spill average selection */
typedef struct {
	const char *month;
	int day;
} Month;

Month months[] = {
	{"Jan",	1},
	{"Feb",	32},
	{"Mar",	60},
	{"Apr1",	91},
	{"Apr2",	106},
	{"May",	121},
	{"Jun",	152},
	{"Jul",	182},
	{"Aug1",	213},
	{"Aug2",	228},
	{"Sep",	244},
	{"Oct",	274},
	{"Nov",	305},
	{"Dec",	335},
	{NULL,	366}
};

Month *default_start = &months[3];
Month *default_stop = &months[6];

void qIOTool::resetAverage() {
	averageStartComboBox->setCurrentText(default_start->month);
	averageStopComboBox->setCurrentText(default_stop->month);

	avg_start = default_start->day;
	avg_stop = (default_stop + 1)->day - 1;
}
void qIOTool::setupAverage() {
	for (Month *month = months; month->month != NULL; month++) {
		averageStartComboBox->insertItem(QString(month->month));
		averageStopComboBox->insertItem(QString(month->month));
	}

	resetAverage();
}
void qIOTool::cbAverageComboBox() {
	int start = -1;
	int stop = -1;

	for (Month *month = months; month->month != NULL; month++) {
		if (averageStartComboBox->currentText() == month->month) {
			start = month->day;
		}
		if (averageStopComboBox->currentText() == month->month) {
			stop = (month + 1)->day - 1;
		}
	}

	if (start < 1 || stop < 1 || start == stop) {
		log_msg(L_ERROR, "Error setting averaging period\n");

		resetAverage();
	}
	else {
		if (start > stop) {
			/* Swap */
			int tmp;
			tmp = start;
			start = stop;
			stop = tmp;

			QString t = averageStartComboBox->currentText();
			averageStartComboBox->setCurrentText(averageStopComboBox->currentText());
			averageStopComboBox->setCurrentText(t);
		}

		avg_start = start;
		avg_stop = stop;
	}

	evAverageChanged();
}

/* Monte Carlo */

void qIOTool::updateMonteCarloDock() {
	if (((bool)monte_carlo) != monteCarloCheckBox->isChecked()) {
		monteCarloCheckBox->setChecked(monte_carlo);
	}
	monteCarloRuns->setEnabled(monte_carlo);
}
void qIOTool::cbMonteCarloCheckBox() {
	monte_carlo = monteCarloCheckBox->isChecked();

	/* Output is dirty but it doesn't necessarily need to be saved (since Monte Carlo settings aren't saved) */
	output_dirty = TRUE;
	updateDirty();

	updateMonteCarloDock();
}
void qIOTool::cbMonteCarloRuns() {
	if (output_dirty == FALSE) {
		/* Output is dirty but it doesn't necessarily need to be saved (since Monte Carlo settings aren't saved */
		output_dirty = TRUE;
		updateDirty();
	}
}


/* Graph windows */
void qIOTool::addGraphDialog(qGraphDialog *g) {
	graphDialogs.append(g);
	if (!connect(g, SIGNAL(destroyed(QObject*)), this, SLOT(removeGraphDialog(QObject*)))) {
		log_msg(L_ERROR, "Could not connect graph window delete handler!\n");
	}
}
void qIOTool::removeGraphDialog(QObject *g) {
	graphDialogs.remove((qGraphDialog*)g);
}
void qIOTool::closeGraphDialogs() {
	/* Copy the list of graph dialogs and then delete them all (iterators won't
	 * work since the graph dialogs will be removed from the list by callbacks). */
	unsigned int numDialogs = graphDialogs.count();
	qGraphDialog **dialogs = (qGraphDialog**) malloc(sizeof(qGraphDialog*) * graphDialogs.count());
	unsigned int i;

	for (i=0; i < graphDialogs.count(); i++) {
		dialogs[i] = graphDialogs.at(i);
	}

	for (i=0; i < numDialogs; i++) {
		dialogs[i]->close();
	}

	graphDialogs.clear();

	free(dialogs);
}

void qIOTool::cbZoomIn() {
	emit evZoomIn();
}
void qIOTool::cbZoomOut() {
	emit evZoomOut();
}
