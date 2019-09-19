#include "version.h"
#include "qHelpWindow.h"
#include <qdir.h>
#include "Qcrisp.h"
#include "Qmain.h"
#include <stdlib.h>
#include "qHelpManager.h"

qHelpManager::qHelpManager( QWidget *parent, const char *name )
    : QObject( parent, name )
{	
	helpWindow = NULL; /* Don't create the window until help is requested */
}

qHelpManager::~qHelpManager(){
}


void qHelpManager::showHelp( QString  page )
{
	/* The help hasn't been updated for COMPASS yet, so just tell the user that it doesn't exist yet */
	QMessageBox::information( NULL, "Help not available", "The documentation for COMPASS has not yet been written.");

	return; /* The code after this should be used once the documentation is written */

	if (helpWindow == NULL) helpWindow = new qHelpWindow("1.html");
	helpWindow->showPage(page);
}

void qHelpManager::shutdown(){
	if (helpWindow != NULL) delete helpWindow;
}


void qHelpManager::displayPage()
{
}

void qHelpManager::about() {
	QMessageBox::about( NULL, "About COMPASS v" COMPASS_VERSION_MAJOR_STRING "." COMPASS_VERSION_MINOR_STRING,
		"This is the COMPASS model version " COMPASS_VERSION_MAJOR_STRING "." COMPASS_VERSION_MINOR_STRING " as released on " COMPASS_VERSION_RELEASE_DATE_STRING);
}

