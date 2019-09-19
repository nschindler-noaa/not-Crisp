#include <qfiledialog.h>
#include <qmessagebox.h>
#include "qFileManager.h"
#include "crisp.h"
#include "run_mode.h"
#include "dir.h"
#include "dataio.h"
#include "parse.h"
#include "qGlobalEvents.h"
#include <qregexp.h>

/* Differing headers for file access routines */
#ifdef WIN32
#include <io.h>
#endif
#ifdef SOLARIS2
#include <unistd.h>
#endif

qFileManager fileManager; /* Global instance */

/* Character strings */
static char readname[MAXPATHLEN] = "*.scn";
static char* types="Retrospective Files (*.scn;*.dat);;River Environment (*.riv);;Release Files (*.rls);;Operations Files (*.ops);;Calibration Files (*.clb);;Post-Bonneville Files (*.pbn);;Config/Override Files (*.etc);;Control Files (*.ctrl);;All files (*)";

/* writename may be updated outside of this file */
char writename[MAXPATHLEN] = "*.scn";

qFileManager::qFileManager()
:	topLevel(NULL)
{
}

void qFileManager::openFile(const QString& s) {	
	int len, answer;
	int replace = 0;

	if (s != QString::null) {
		strncpy( readname, (s).ascii(), MAXPATHLEN );
		len = strlen( readname );
		if (!strcasecmp( readname+len-6, ".spill" )
				|| !strcasecmp( readname+len-4, ".spi" )) {
			answer = QMessageBox::question ( topLevel, "Reading Spill", "Reset all spill?", 
				"&No", "&Yes",  QString::null, 1, -1 );
			if (answer == 1)
				reset_all_spill();
		}
		else if (!strcasecmp( readname+len-4, ".dam" )) {
			answer = QMessageBox::question ( topLevel, "Reading Dams", "Reset all dams?", 
				"&No", "&Yes",  QString::null, 1, -1 );
     		if (answer == 1)
				reset_all_dams();
		}
		else if (!strcasecmp( readname+len-4, ".rls" )) {
			answer = QMessageBox::question ( topLevel, "Reading Releases", "Reset all releases?", 
				"&No", "&Yes",  QString::null, 1, -1 );
     		if (answer == 1)
				reset_all_releases();
		}
		else if (strcasecmp( readname+len-4, ".beh" ) &&
				 strcasecmp( readname+len-4, ".res" ) &&
				 strcasecmp( readname+len-4, ".flo" ) &&
				 strcasecmp( readname+len-5, ".flow" ) &&
				 strcasecmp( readname+len-4, ".riv" ) &&
				 strcasecmp( readname+len-6, ".river" ) &&
				 strcasecmp( readname+len-4, ".cnf" )) {
			/* Reading all, assume this means reading a .dat or 
             *  .ctrl file where we clear everything out first.
			 */
			reset_all_spill();
			reset_all_dams();
			reset_all_releases();
			replace = 1;
		}

		emit fileOpening();

		parse_datafile( readname, replace, 1 );

		/* IDCs won't track changes in database. */
		qEvents::Instance()->emitGEPurgeAllIDCs_();
		/* output graphs invalidate */
		qEvents::Instance()->emitGEUpdateAllDataTokens_();

		/* Tell classes (e.g. topLevel) that a file has been opened */
		emit fileOpened();
	}
}

void qFileManager::fileOpen(){

    if (run_mode.running)
        return;

	openFile(QFileDialog::getOpenFileName(
                    "*.dat;*.scn",
                    types,
                    topLevel,
                    "Open file dialog"
                    "Read File" ));
}

int qFileManager::saveFile(const QString &s, int warn) {
	int len, answer;

	strncpy( writename, s.ascii(), MAXPATHLEN );
	len = strlen( writename );

	/* First, coerce extensions to be the full 4 or 5 characters
	 *  if they were truncated by Windows 95.
	 */
	if (!strcmp( writename+len-4, ".CTR" ))
		strcpy( writename+len-4, ".CTRL" );
	else if (!strcasecmp( writename+len-4, ".ctr" ))
		strcpy( writename+len-4, ".ctrl" );
	else if (!strcmp( writename+len-4, ".SPI" ))
		strcpy( writename+len-4, ".SPILL" );
	else if (!strcasecmp( writename+len-4, ".spi" ))
		strcpy( writename+len-4, ".spill" );
	else if (!strcmp( writename+len-4, ".FLO" ))
		strcpy( writename+len-4, ".FLOW" );
	else if (!strcasecmp( writename+len-4, ".flo" ))
		strcpy( writename+len-4, ".flow" );
	else if (!strcmp( writename+len-4, ".RIV" ))
		strcpy( writename+len-4, ".RIVER" );
	else if (!strcasecmp( writename+len-4, ".riv" ))
		strcpy( writename+len-4, ".river" );
	len = strlen( writename );

	if (warn) {
		/* Check for existence and warn - user may cancel action.
		 */
		if (! (access( writename, 0 ))) {
			answer = QMessageBox::warning ( topLevel, "File Exists", "File already exists, overwrite it?", 
				"&Cancel", "&OK",  QString::null, 1, -1 );
			if (answer != 1)
				return -1;		
		}
	}

    if (write_output_file( writename )) {		
		QMessageBox::critical(topLevel, "File Save Error","File Save Error");
		printf("%s\n", writename);
		return -1;
    }

	return 0;
}
int qFileManager::fileSaveAs(){
	QString selectedFilter;
	QString s = QFileDialog::getSaveFileName( QString::null,
	            types,
                    topLevel,
                    "Save As file dialog",
                    "Write File", &selectedFilter );
	if (s != QString::null) {
		/* Figure out what extension to append */
		QString extension = QString::null;

		if (selectedFilter != QString::null) {
			QRegExp re("\\*(\\.[a-zA-Z0-9]*)[^a-zA-Z0-9]");
			re.search(selectedFilter);

			extension = re.cap(1);

		}
		if (extension == QString::null) extension = ".scn";

		if (s.findRev(QRegExp("\\.....?$")) == -1) s.append(extension);

		saveFile(s, TRUE);

		return 0;
    }
		
	return -1;
}
