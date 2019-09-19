//Qmain.cpp

//Entry point for the QT interface

#include "Qmain.h"
#include "Qcrisp.h"
#include "QLogWindow.h"
#include <qapplication.h>
#include "batch.h"
#include <qstring.h>
#include <qmessagebox.h>
#include "log.h"
#include "main.h"
#include <qevent.h>
#include <stdio.h>
#include "file.h"
#include "qHelpManager.h"
#include "qFileManager.h"
#include "qScenarioManager.h"
#include "qIOTool.h"

#include "qGlobalEvents.h"
//need for unix use of unlink()
#ifndef WIN32
#include <unistd.h>
#endif

QCrisp *qcrisp = NULL;
qIOTool *iotool = NULL;
qHelpManager *helpManager;
qTopLevelManager topLevelManager;

QCrisp* getQCrisp() {
	if (!qcrisp) {
		qcrisp = new QCrisp(0, "mainWindow", 0);
		topLevelManager.connect(qcrisp, SIGNAL(destroyed()), SLOT(cbQCrispDestroyed()));
	}
	qcrisp->show();

	return qcrisp;
}
void qTopLevelManager::cbQCrispDestroyed() {
	qcrisp = NULL;
}

qIOTool* getIOTool() {
	if (!iotool) {
		iotool = new qIOTool(NULL);
		topLevelManager.connect(iotool, SIGNAL(destroyed()), SLOT(cbIOToolDestroyed()));
	}
	iotool->show();

	return iotool;
}
void qTopLevelManager::cbIOToolDestroyed() {
	iotool = NULL;
}

/* This routine reads the redirected error text, if any, from the stderr FILE
 *  (which is really a disk file now), shows it in a message box, rewinds the
 *  file,and returns.
 */

void Qmain::showStderrMsg( void ) {
	char cBuf[512];
	int i, c;

	rewind( stderr );
	for (i = 0; i < 505; i++)
		if ((c = fgetc( stderr )) == EOF)
			break;
		else
			cBuf[i] = c;
	if (i == 505)
		strcpy( cBuf+i, "..." );
	else
		strcpy( cBuf+i, "" );

	printf("COMPASS Error, %s", cBuf );
	rewind( stderr );
}


int main(int argc, char ** argv){

	/* This gets stored in Qt's global pointer qApp */
	QApplication a(argc, argv, true);

	Qmain localMain(argc, argv);

	return 0;
}

#ifdef	DEBUG
void qtToLogWindowBridge(QtMsgType type, const char *msg) {
	switch (type) {
		case QtDebugMsg: log_msg(L_DBGMSG, "QT (Debug):\t%s\n", msg); break;
		case QtWarningMsg: log_msg(L_DBGMSG, "QT (Warning):\t%s\n", msg); break;
		case QtFatalMsg: log_msg(L_DBGMSG, "QT (Fatal):\t%s\n", msg); break;
	}
}
#endif

Qmain::Qmain(int argc, char ** argv)
{
	int monk = 0;	

	/* Process command line */
	if (process_command_line(argc, argv, 0)) {
		showStderrMsg();
		return;
	}

	/* Create log file or log window if necessary */
	if (BATCH_MODE_ACTIVE){
		create_log_file();		
	}
	else {
#ifdef	DEBUG
		/* In debug mode, have Qt write messages to the log window as well */
		qInstallMsgHandler(qtToLogWindowBridge);
#endif
		
		/* Redirect stderr to a file in the current directory, which can be read
		 *  later to give an error message box. This is necessary because the legacy
		 *  C code reports errors to stderr, which doesn't go anyplace useful in
		 *  a Windows program.
		 */
		freopen( "compass.stderr", "w+", stderr );
	
	}

	/* Locate the executable */
	locate_image((qApp->argv())[0]);

	/* Model startup */
	if (model_startup()) {
		showStderrMsg();
		QMessageBox::critical(NULL, "Model Startup Failed",
			QString("The model failed to start. This is probably because it could not\nfind the river description file, \"") + QString(user_args.river_desc) + "\".",
			QMessageBox::Ok, QMessageBox::NoButton);
		return;
	}

	/* The GUI is actually started in this block (assuming we're not doing batch mode) */
	if (!BATCH_MODE_ACTIVE) {
		QWidget *topLevel;

		if (0) {
			/* Old GUI */
			topLevel = (QWidget*) getQCrisp();
		}
		else {
			/* I/O Tool GUI */
			topLevel = (QWidget*) getIOTool();
		}

		/* Set as the top-level widget */
		qApp->setMainWidget(topLevel);
		fileManager.setTopLevel(topLevel);
		scenarioManager.setTopLevel(topLevel);
		helpManager = new qHelpManager(topLevel, "helpManager");

		//ZatLogDialog::instance( this, 1 );
		messageLog(); //create a new message log

		topLevel->raise();
		topLevel->show();

		/* Main event loop */
		qApp->exec();
	}	
	

	//Close log files
	if (BATCH_MODE_ACTIVE)
		delete_log_file();
	else {
		QLogWindow::deleteLogFile();

		// Done with stderr redirection, so clean it up.
		fclose( stderr );
		unlink( "compass.stderr" );
	}

}


void addToLogWindow(char *msg, int id){

	QLogWindow * qlw = QLogWindow::instance(FALSE);
	if(qlw != NULL) QApplication::postEvent(qlw, new QLogEvent(msg, id));
	
}

void Qmain::messageLog(){
	//create a new mesage log using auto create
	QLogWindow *logw = QLogWindow::instance(TRUE);
	logw->show();
	logw->raise();
}


//Handles the fatal errors
//Makes a new message box
void AnnounceFatalError( void ) {

		qWarning("Fatal Error");
		QMessageBox::critical(NULL, "Fatal Error", "A Fatal Error Has Occurred", QMessageBox::NoButton, QMessageBox::Ok);

}

/* This routine allows C routines to put text into the main app's status line. */
extern "C" {
extern void SetStatusLine(char *text) {
	if (qcrisp) qcrisp->SetStatusLine(text);
}
}


