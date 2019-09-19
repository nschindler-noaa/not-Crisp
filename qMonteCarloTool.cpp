/* qMonteCarloTool.h
* This class defines a modeless dialog which edits the monte carlo in-memory
*  and on-disk structures and launches monte carlo execution runs.
*  The dialog is based on the IDD_MONTECARLO resource template.
*
* This is a special-purpose dialog which will probably not be subclassed.
*/

#include "qMonteCarloTool.h"
#include "resource.h"
#include "qGlobalEvents.h"
#include "qPromptForString.h"
#include "qUtils.h"
#include "qMonteReportTool.h"
#include <qcheckbox.h>
#include <qvalidator.h>
#include "qMonteAltCmpDlg.h"
#include "qMonteAnalDlg.h"
#include "qHelpManager.h"
#include "Qcrisp.h"
#include "altern_list.h"
#include "file.h"
#include "log.h"
#include "token.h"
#include "gui.h"
#include "qScenarioManager.h"
#include "qMonteCarloManager.h"
#include <time.h>
#ifdef WIN32
#include <direct.h>
#include <windows.h>
#else
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <dirent.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <qmessagebox.h>
#include <qfiledialog.h>
#include <qstringlist.h>


  extern qMonteCarloTool *theMonteCarloTool_;  // pathological data connection
  
	/* Legacy routines which do not have header files, or whose headers are too
	*  cluttered.
	*/
	#ifdef __cplusplus
	extern "C" {
	#endif
	extern struct monte_carlo_data *make_monte_object();
	extern int set_altern_defaults( struct altern_data* alt );
	extern unsigned char open_and_parse_alternative(
    char *filename,
    struct altern_data *alternative );
	extern void free_monte_object( struct monte_carlo_data *obj );
	extern struct archive_map *open_and_map_flow( struct altern_data *alt );
	extern void free_multi_data( struct multi_data *data );
	extern void free_ftells_alt( long **alt_off, int nyears );
	extern void remember_current_directory( void );
	extern void free_alternative_object( struct altern_data *object );
	extern int write_altern_data( FILE * outfile, struct altern_data *alternative );
	extern char * getwd( char *buf );
	extern int report_update_directory( void );
	#ifdef __cplusplus
	}
	#endif
	
qMonteCarloTool::qMonteCarloTool( QCrisp *w )
	  : MonteCarloBase ( NULL ),
		qcrisp(w)
{
	/* Make sure that the Monte Carlo structures have been initialized */
	qMonteCarloManager::get();

	//Set up the widget refrences   
	bRunMonte_ = RunButton;
	bCancelMonte_ = CancelRunButton;
	bOK_ = OKButton;
	bCancel_ = CancelButton;
	bApply_ = ApplyButton;
	bReset_ = ResetButton;
	bApplyAll_ = ApplyAllButton;
	bResetAll_ = ResetAllButton;
	bHelp_ = HelpButton;
	edCurDir_ = currentDir;
	bGoToDir_ = GoButton;
	bGetDir_ = BrowseAlts;
	bCreate_ = NewButton;
    bDelete_ = DeleteButton;
    bRename_ = RenameButton;
	cbAltToEdit_ = AltBeingEdited;
	bUseNFiles_ = YearlyInputUseButton;
	/* Progress and status indicators.
	 */
	stNAltYears_ = NumAltYearsLabel;
    stNAltGames_ =  NumAltGamesLabel;
    stNAlts_ = NumAlternsLabel;
	progNAlts_ = AlternsPercentLabel;
    progNAltGames_ = GamesPercentLabel;
    progNAltYears_ = YearsPercentLabel;
	ckSupplemental_ =  WriteSupplemental;
    ckLockOutputSettings_ = LockCurrentOutput;
    ckDisableVarianceSuppression_ = DisableVarianceSuppression;
	lbAlterns_= AltsToRun;
	lbFiles_ = YearlyInputBox;
	lbYears_  = GamesList;
	edFlowArch_ = FlowArchiveLineEdit;
	bUseFlowFile_ = FlowArchUseButton;
	bGetFlowFile_ = BrowseFlow;	
	rbElevFromArch_ = ElevationArchiveRadio;	
	rbElevFromUser_ = ElevationUserDataRadio;
	rbSpillFromUser_ = SpillUserDataRadio;
    rbSpillFromArch_ = SpillArchiveRadio;
    rbFlowFromArch_ = FlowArchiveRadio;
    rbFlowFromUser_ = FlowUserDataRadio;
	edNFiles_ = NumYearlyFiles;
	bUseNFiles_ = YearlyInputUseButton;
	edFilename_ = YearlyInputLineEdit;
	edNGames_ = numGamesLineEdit;
	edYear_ = YearLineEdit;
	edYear_->setReadOnly(true);
	bSetNGame_ = InsertGameButton;
	bSetAllNGame_ = InsertAllGameButton;
	bSetAllFile_ = InsertAllYearyButton;
    bSetFile_ = InsertYearlyButton;
	bGetYearFile_ = MoreYIFilesButton;
	stFlowArch_ = textLabel5;
	stSpillFrom_ = textLabel6;
	stElevFrom_ = textLabel7;
	stYear_ = textLabel8;
	
	consCommon();
}
	  
/* Common code called by all constructors.
*/
void qMonteCarloTool::consCommon( void ) {

	
//	lock_ = 1;
//	monte_data_ = NULL;
	current_ = 0;
//	runListDirty_ = 0;
//	timer_ = 0;

	//Put the radio buttons in a buttongroup
	QButtonGroup* bg = new QButtonGroup(this);
	bg->hide();	
	bg->insert( rbElevFromArch_ );
	bg->insert(rbElevFromUser_ );
	bg = new QButtonGroup(this);
	bg->hide();
	bg->insert(rbSpillFromUser_ );
    bg->insert(rbSpillFromArch_);
	
	edNGames_->setValidator(new QIntValidator(1, 1000, edNGames_));
	edNFiles_->setValidator(new QIntValidator(1, 200, edNFiles_));
	

	//Set the listbox to multi selection mode
	lbAlterns_->setSelectionMode(QListBox::Multi);

	/* TODO: print to log if connections don't work! */
	/* Get the child window controls hooked up. */
	connect(bCreate_, SIGNAL(clicked()), SLOT(cbCreate()));
	connect(bDelete_, SIGNAL(clicked()), SLOT(cbDelete()));
	connect(bRename_, SIGNAL(clicked()), SLOT(cbRename()));
	connect(bOK_, SIGNAL(clicked()), SLOT(cbOK()));
	connect(bCancel_, SIGNAL(clicked()), SLOT(cbCancel()));
	connect(bApply_, SIGNAL(clicked()), SLOT(cbApply()));
	connect(bReset_, SIGNAL(clicked()), SLOT(cbReset()));
	connect(bApplyAll_, SIGNAL(clicked()), SLOT(cbApplyAll()));
	connect(bResetAll_, SIGNAL(clicked()), SLOT(cbResetAll()));
	connect(bHelp_, SIGNAL(clicked()), SLOT(cbHelp()));
	connect(bSetNGame_, SIGNAL(clicked()), SLOT(cbSetNGame()));
	connect(bSetAllNGame_, SIGNAL(clicked()), SLOT(cbSetAllNGame()));	
	connect(bGoToDir_, SIGNAL(clicked()), SLOT(cbGoToDir()));
	connect(bGetDir_, SIGNAL(clicked()), SLOT(cbGetDir()));
	connect(bGetFlowFile_, SIGNAL(clicked()), SLOT(cbGetFlowFile()));	
	connect(bUseFlowFile_, SIGNAL(clicked()), SLOT(cbUseFlowFile()));	
	connect(bRunMonte_, SIGNAL(clicked()), SLOT(cbRunMonte()));
	connect(bCancelMonte_, SIGNAL(clicked()), SLOT(cbCancelMonte()));
	bCancelMonte_->setEnabled(FALSE);
	connect(bUseNFiles_, SIGNAL(clicked()), SLOT(cbUseNFiles()));
	connect(bSetFile_, SIGNAL(clicked()), SLOT(cbSetFile()));
	connect(bSetAllFile_, SIGNAL(clicked()), SLOT(cbSetAllFile()));
	connect(bGetYearFile_, SIGNAL(clicked()), SLOT(cbGetYearFile()));
	ckLockOutputSettings_->setChecked( lock_output_settings != 0 );
	connect(ckLockOutputSettings_, SIGNAL(clicked()), SLOT(cbLockOutputSettings()));
	connect(rbFlowFromArch_, SIGNAL(clicked()), SLOT(cbFlowFromArch()));
	rbFlowFromArch_->setChecked( FALSE );
	connect(rbFlowFromUser_, SIGNAL(clicked()), SLOT(cbFlowFromUser()));
	rbFlowFromUser_->setChecked( FALSE );
	connect(rbSpillFromArch_, SIGNAL(clicked()), SLOT(cbSpillFromArch()));
	rbSpillFromArch_->setChecked( FALSE );
	connect(rbSpillFromUser_, SIGNAL(clicked()), SLOT(cbSpillFromUser()));
	rbSpillFromUser_->setChecked( FALSE );
	connect(rbElevFromArch_, SIGNAL(clicked()), SLOT(cbElevFromArch()));
	rbElevFromArch_->setChecked( FALSE );
	connect(rbElevFromUser_, SIGNAL(clicked()), SLOT(cbElevFromUser()));
	rbElevFromUser_->setChecked( FALSE );
	connect(edCurDir_, SIGNAL(lostFocus()), SLOT(cbCurDir()));
	connect(edCurDir_, SIGNAL(returnPressed()), SLOT(_cbCurDir()));	
	connect(edFlowArch_, SIGNAL(lostFocus()), SLOT(cbFlowArch()));
	connect(edFlowArch_, SIGNAL(returnPressed()), SLOT(cbFlowArch()));
	connect(edNGames_, SIGNAL(textChanged(const QString&)), SLOT(cbNGames()));
//	connect(edNGames_, SIGNAL(lostFocus()), SLOT(cbNGames()));
	connect(edNGames_, SIGNAL(returnPressed()), SLOT(cbNGames()));
	connect(edNFiles_, SIGNAL(lostFocus()), SLOT(cbNFiles()));
	connect(edNFiles_, SIGNAL(returnPressed()), SLOT(cbUseNFiles()));
	connect(lbAlterns_, SIGNAL(selectionChanged()), SLOT(cbListBoxAlterns()));
	connect(lbYears_, SIGNAL(highlighted(int)), SLOT(cbListBoxYears()));
	connect(lbFiles_, SIGNAL(highlighted(int)), SLOT(cbListBoxFiles()));
	connect(cbAltToEdit_, SIGNAL(activated (int)), SLOT(cbComboBoxAlterns()));
	connect(&scenarioManager, SIGNAL(monteCarloStatusUpdate(int,int,int,int,int,int)), SLOT(setStatusProgress(int,int,int,int,int,int)));
	connect(qEvents::Instance(), SIGNAL(GERunStatusChanged_()), SLOT(cbRunStatusChanged()));

	/* Connect to Monte Carlo manager */
	connect(qMonteCarloManager::get(), SIGNAL(alternativesUpdated()), SLOT(updateDialog()));

	/* The status and progress indicators.
	*/

	clearProgress();

	//Don't  show any dirty indicators
	showSomeDirty(false);

//	AttachEvents();   //connects global signals 

//	lock_ = 0;

//	CreateLocalList();
	updateDialog();
	cbGoToDir();
	show();

	cbRunStatusChanged();  // check if scenario mode running.
//	isRunning_ = 1;
}

/* Routine to fill the dialog fields from the current directory.
 */
void qMonteCarloTool::updateDialog() {
	/* List of alternatives in current directory. */

	/* Update options in the boxes rather than rebuilding them from scratch */
	unsigned int index;
	int i;

	disconnect(lbAlterns_, SIGNAL(selectionChanged()), this, SLOT(cbListBoxAlterns()));

	for (reset_altern_list(), index=0, i=0; get_current_list_altern(); inc_altern_list(), index++, i++) {
		if (index < lbAlterns_->count()) {
			/* Update */
			if (lbAlterns_->text(index) != get_current_list_altern()) lbAlterns_->changeItem(get_current_list_altern(), index);
			if (lbAlterns_->isSelected(index) != ((bool)get_current_altern_list_flag())) lbAlterns_->setSelected(index, get_current_altern_list_flag());
		}
		else {
			/* Add */
			lbAlterns_->insertItem(get_current_list_altern());
		}

		if (i < cbAltToEdit_->count()) {
			if (cbAltToEdit_->text(i) != get_current_list_altern()) cbAltToEdit_->changeItem(get_current_list_altern(), i);
		}
		else {
			cbAltToEdit_->insertItem(get_current_list_altern());
		}
	}
	while (lbAlterns_->count() > index) lbAlterns_->removeItem(index);
	while (cbAltToEdit_->count() > i) cbAltToEdit_->removeItem(i);

	/* Update selection */
	for (reset_altern_list(), index=0; get_current_list_altern(); inc_altern_list(), index++) {
		/* Update */
		if (lbAlterns_->isSelected(index) != ((bool)get_current_altern_list_flag())) lbAlterns_->setSelected(index, get_current_altern_list_flag());
	}

	if (current_ >= get_altern_list_size()) current_ = get_altern_list_size() - 1;
	if (current_ < 0) current_ = 0;

	connect(lbAlterns_, SIGNAL(selectionChanged()), this, SLOT(cbListBoxAlterns()));

	/* What is the current directory, by the way? */
	char buf[MAXPATHLEN];
	char *pRet;
	pRet = getcwd( buf, MAXPATHLEN );
	if (pRet != NULL)
		edCurDir_->setText( buf );
	else
		edCurDir_->setText( "" );

	updateAlternative();
}

/* Show or hide "dirty" annunciators.
*/
void qMonteCarloTool::showSomeDirty( bool dirty ) {
	
	bpDirty1->setShown(dirty);
	bpDirty2->setShown(dirty);
	bpDirty3->setShown(dirty);
	bpDirty4->setShown(dirty);
}

void qMonteCarloTool::showCurrDirty( bool dirty ) {
	
	bpDirty5->setShown(dirty);
	bpDirty6->setShown(dirty);
	bpDirty7->setShown(dirty);
	bpDirty8->setShown(dirty);
	
	/* If the current alternative is dirty, then the "some alt is dirty"
	*  indicator must be activated too. 
	*/
	if (dirty)
		showSomeDirty( TRUE );
}

qMonteCarloTool::~qMonteCarloTool()
{
	/* Notify all and sundry that we died. */
	//DetachEvents();
//	DestroyLocalList();
	theMonteCarloTool_ = NULL;
}

void qMonteCarloTool::cbPurgeAll() {
	updateDialog();
}

/* Notification functions for controls in dialog.
*/
void qMonteCarloTool::cbOK(  ) {
	//if (!isRunning_) {
//		isRunning_ = 0;
		cbApplyAll(  );
		close();
	//}
	//close();
	return ; // event handled.
}

void qMonteCarloTool::cbCancel(  ) {
	//if (!isRunning_) {
//		isRunning_ = 0;
		cbResetAll(  );
		close();		
	//}
	

	return ; // event handled.
}

void qMonteCarloTool::cbApply(  ) {
	if (current_ >= 0) qMonteCarloManager::get()->applyLocalList(current_);
	return ; // event handled.
}

void qMonteCarloTool::cbReset(  ) {
	if (current_ >= 0) qMonteCarloManager::get()->resetLocalList(current_);
	return ; // event handled.
}

void qMonteCarloTool::cbApplyAll(  ) {
	qMonteCarloManager::get()->applyLocalList();
	return ; // event handled.
}

void qMonteCarloTool::cbResetAll(  ) {
	qMonteCarloManager::get()->resetLocalList();
	return ; // event handled.
}

void qMonteCarloTool::cbHelp(  ) {
	helpManager->showHelp( QString("75.html") );
	return ; // event handled.
}

// Handle the "Lock current output settings" checkbox.
void qMonteCarloTool::cbLockOutputSettings(  ) {
//    if (lock_)
//        return ;
	
//    lock_ = 1;
    lock_output_settings = ckLockOutputSettings_->isChecked() != 0;
    qcrisp->updatedOutputLocking();
//    lock_ = 0;
    return ; // event handled.
}


/* External entry point to set the "Lock current output settings" checkbox
*/
void qMonteCarloTool::setLockOutputSettings( int i ) {
    ckLockOutputSettings_->setChecked( i != 0 );
}

/* "New" alternative button pushed.
*/
void qMonteCarloTool::cbCreate() {
	int i;
	bool flag;
	QString nName;
	QString tstr = "Please enter name for new alternative";
	
	/* get a default name for the new alternative.
	*/
	char cBuf[20];
	for (i = 1; i < 1024; i++) {
		sprintf( cBuf, "altern%d", i );
		if (!search_altern_list( cBuf ))
			break;
	}
	
	qPromptForString *pfs = 
		new qPromptForString( this, "Monte Carlo Tool", tstr.ascii(), cBuf );
	nName = pfs->csText;
	flag = pfs->bOK;
	delete pfs;
	if (!flag || nName.length() == 0)
		return ;
	
		/* Make sure alternative doesn't already exist. */
	QString nameEscaped = nName.replace('"', ' ');
	if (search_altern_list(nameEscaped.ascii())) {
		tstr = nName + " already exists.";
		QMessageBox::warning(this, "Monte Carlo Tool", tstr, QMessageBox::Ok, QMessageBox::NoButton);			
		return ;
	}
	
	int current = qMonteCarloManager::get()->createAlternative(nameEscaped);
	if (current < 0) {
		QMessageBox::warning(this, "Monte Carlo Tool", "Error creating alternative", QMessageBox::Ok, QMessageBox::NoButton);			
		return ;
	}

	/* TODO: can I just use updateDialog instead? */
	current_ = current;

	/*
	lbAlterns_->insertItem( nName );
	cbAltToEdit_->insertItem( nName );
	current_ = (int)cbAltToEdit_->count() -1;*/
}

/* "Rename" alternative button pushed.
*/
void qMonteCarloTool::cbRename() {
	int flag;
	if (current_ < 0)
		return ;
	struct altern_data *cur_alt = &(qMonteCarloManager::get()->getMonteData()->alts[current_]);
	
	QString nName, oName = cur_alt->alternative_name;
	QString tstr = "Please enter new name for " + oName;
	
	qPromptForString *pfs = 
		new qPromptForString( this, "Monte Carlo Tool", tstr.ascii() );
	nName = pfs->csText;
	flag = pfs->bOK;
	delete pfs;
	if (!flag || nName.length() == 0)
		return ;

	qMonteCarloManager::get()->renameAlternative(oName, nName);
}

/* "Delete" alternative button pushed.
*/
void qMonteCarloTool::cbDelete() {
	int flag;
	
	QString oName = cbAltToEdit_->currentText();
	QString tstr = "Really delete " + oName + " and all it contains?";	
	flag = QMessageBox::question(this, "Monte Carlo Tool", tstr, QMessageBox::Cancel, QMessageBox::Ok | QMessageBox::Default);
	if (flag == QMessageBox::Cancel)
		return ;

	qMonteCarloManager::get()->deleteAlternative(oName);
}

/* Change in "current directory" entry field.
*/
void qMonteCarloTool::cbCurDir(  ) {
//	if (lock_)
//		return ;
	
	cbGoToDir( );
	
	return ; // event handled.
}

/* New "current directory" selected.
*/
void qMonteCarloTool::cbGoToDir(  ) {
	int iAction = IDC_RESET;

	/* Check if data structure is dirty and warn if so.
	*/
	/* ... uh... dirty ? TODO */
/*	if (someAltDirty()) {
		iAction = QMessageBox::question(this, "Monte Carlo Tool", "Save all edited data before changing directory?",
			QMessageBox::No,QMessageBox::Yes | QMessageBox::Default, QMessageBox::Cancel );
		
		if (iAction == QMessageBox::Cancel)
			return ;
		if (iAction == QMessageBox::Yes)
			cbApplyAll();
	}*/

	qMonteCarloManager::get()->changeDirectory(edCurDir_->text());
}

/* Radio button selection events.
*/
void qMonteCarloTool::cbFlowFromArch(  ) {
	struct altern_data *cur_alt = &(qMonteCarloManager::get()->getMonteData()->alts[current_]);
	
//	if (lock_)
//		return ;
	if (cur_alt->use_flow_from == ARCHIVE_FLOW)
		return ;
	
	//lock_ = 1;
	//rbFlowFromUser_->setChecked( FALSE );
	//lock_ = 0;
	
	cur_alt->use_flow_from = ARCHIVE_FLOW;
	cur_alt->changed = TRUE;
	updateAlternative();
	
	return ; // event handled.
}

void qMonteCarloTool::cbFlowFromUser(  ) {
	struct altern_data *cur_alt = &(qMonteCarloManager::get()->getMonteData()->alts[current_]);
	
//	if (lock_)
//		return ;
	if (cur_alt->use_flow_from != ARCHIVE_FLOW)
		return ;
	
//	lock_ = 1;
	//	rbFlowFromArch_->setChecked( FALSE );
	free_multi_data( cur_alt->multi );
	cur_alt->multi = NULL;
//	lock_ = 0;
	
	cur_alt->use_flow_from = 0;
	cur_alt->changed = TRUE;
	updateAlternative();
	
	return ; // event handled.
}

void qMonteCarloTool::cbSpillFromArch(  ) {
	struct altern_data *cur_alt = &(qMonteCarloManager::get()->getMonteData()->alts[current_]);
	
//	if (lock_)
//		return ;
	if (cur_alt->use_spill_from == ARCHIVE_SPILL)
		return ;
	
//	lock_ = 1;
	//rbSpillFromUser_->setChecked( FALSE );
//	lock_ = 0;
	
	cur_alt->use_spill_from = ARCHIVE_SPILL;
	cur_alt->changed = TRUE;
	
	/* Don't need to call an update method since I know this event has
	*  no wider effects.
	*/
	showCurrDirty( TRUE );
	
	return ; // event handled.
}

void qMonteCarloTool::cbSpillFromUser(  ) {
	struct altern_data *cur_alt = &(qMonteCarloManager::get()->getMonteData()->alts[current_]);
	
//	if (lock_)
//		return ;
	if (cur_alt->use_spill_from != ARCHIVE_SPILL)
		return ;
	
//	lock_ = 1;
//	rbSpillFromArch_->setChecked( FALSE );
//	lock_ = 0;
	
	cur_alt->use_spill_from = 0;
	cur_alt->changed = TRUE;
	
	/* Don't need to call an update method since I know this event has
	*  no wider effects.
	*/
	showCurrDirty( TRUE );
	
	return ; // event handled.
}

void qMonteCarloTool::cbElevFromArch(  ) {
	struct altern_data *cur_alt = &(qMonteCarloManager::get()->getMonteData()->alts[current_]);
	
//	if (lock_)
//		return ;
	if (cur_alt->use_elev_from == ARCHIVE_ELEV)
		return ;
	
//	lock_ = 1;
	rbElevFromUser_->setChecked( FALSE );
//	lock_ = 0;
	
	cur_alt->use_elev_from = ARCHIVE_ELEV;
	cur_alt->changed = TRUE;
	
	/* Don't need to call an update method since I know this event has
	*  no wider effects.
	*/
	showCurrDirty( TRUE );
	
	return ; // event handled.
}

void qMonteCarloTool::cbElevFromUser(  ) {
	struct altern_data *cur_alt = &(qMonteCarloManager::get()->getMonteData()->alts[current_]);
	
//	if (lock_)
//		return ;
	if (cur_alt->use_elev_from != ARCHIVE_ELEV)
		return ;
	
//	lock_ = 1;
	rbElevFromArch_->setChecked( FALSE );
//	lock_ = 0;
	
	cur_alt->use_elev_from = 0;
	cur_alt->changed = TRUE;
	
	/* Don't need to call an update method since I know this event has
	*  no wider effects.
	*/
	showCurrDirty( TRUE );
	
	return ; // event handled.
}

/* If the user changed the text in the years or games edit fields that 
*  means to apply the changes to the multi structure.
*/
void qMonteCarloTool::cbNGames(  ) {
//	if (lock_)
//		return ;
//	lock_ = 1;
	
	/* Change to "#games" edit field. If we are getting
	*  flow from user data, sets the "ngames" field.
	*/
	struct altern_data *cur_alt = &(qMonteCarloManager::get()->getMonteData()->alts[current_]);
	int n;
	
	/* If we don't have archived flow, then changing this field means
	*  to apply the #games field to the multi structure.
	*/


	if (cur_alt->use_flow_from != ARCHIVE_FLOW) {
		bool ok;
		n = edNGames_->text().toInt(&ok);

		if (ok && n >= 0 && n != cur_alt->ngames) {
			cur_alt->ngames = n;

			showCurrDirty( TRUE );
			cur_alt->changed = TRUE;
			updateStatus();
		}
	}
	
//	lock_ = 0;
	
	return ; // event handled.
}

void qMonteCarloTool::cbSetNGame(  ) {
//	if (lock_)
//		return;
//	lock_ = 1;
	
	/* Get the year name string and the number of games from the edit fields.
	*  Then search the listbox text for the same name string; if found,
	*  replace the number text and also update the underlying structure.
	*  If any mistakes, give up.
	*/

	QString csNGame = edNGames_->text();
	bool ok;
	int nGames = csNGame.toInt(&ok) ;

	if (nGames >= 0 && ok) {

		int i, l = lbYears_->count();
		QString csYear = edYear_->text();
		QString csTemp;
		char cBuf[20];
		struct altern_data *cur_alt = &(qMonteCarloManager::get()->getMonteData()->alts[current_]);
		for (i = 0; i < l; i++) {
			csTemp = lbYears_->text( i );
			if (csTemp.startsWith( csYear ) ) {
				int ind = csYear.length();
				if (csTemp[ind] == ':') {
					/* Found entry "nnnn:n" with matching year. */
					sprintf( cBuf, "%s:%d", csYear.ascii(), nGames );

					lbYears_->removeItem( i );
					lbYears_->insertItem( cBuf, i );
					cur_alt->multi->nruns[i] = nGames;
					cur_alt->changed = TRUE;
					showCurrDirty( TRUE );
					updateStatus();
					break;
				}
			}
		}
		
		/* Now update the count of total games fields.
		*/
		for (i = l = 0; i < cur_alt->multi->ngames; i++)
			l += cur_alt->multi->nruns[i];
		cur_alt->multi->gamesum = cur_alt->ngames = l;
		
		showCurrDirty( TRUE );
		cur_alt->changed = TRUE;
		updateStatus();
	}
	
//	lock_ = 0;
	return ; // event handled.
}

void qMonteCarloTool::cbSetAllNGame(  ) {
//	if (lock_)
//		return ;
//	lock_ = 1;
	
	/* Get the number of games from the edit field. Then run through 
	*  the listbox replacing the number text and also updating the 
	*  underlying structure. If any mistakes, give up.
	*/
	QString csNGame = edNGames_->text();
	bool ok;
	int nGames = csNGame.toInt(&ok) ;
	if (nGames >= 0 && ok) {
		int i, ind, l = lbYears_->count();
		QString cstr, csTemp;
		char cBuf[20];
		struct altern_data *cur_alt = &(qMonteCarloManager::get()->getMonteData()->alts[current_]);
		for (i = 0; i < l; i++) {
			cstr = lbYears_->text( i );
			if ((ind = cstr.find( ':' )) != 0) {
				/* Found entry "nnnn:n" with matching year. */
				csTemp = cstr.mid( 0, ind+1 );
				sprintf( cBuf, "%s%d", csTemp.ascii(), nGames );
				lbYears_->removeItem( i );
				lbYears_->insertItem( cBuf, i );
				cur_alt->multi->nruns[i] = nGames;
			}
		}
		
		/* Now update the count of total games fields.
		*/
		for (i = l = 0; i < cur_alt->multi->ngames; i++)
			l += cur_alt->multi->nruns[i];
		cur_alt->multi->gamesum = cur_alt->ngames = l;
		
		showCurrDirty( TRUE );
		cur_alt->changed = TRUE;
		updateStatus();
	}
	
//	lock_ = 0;
	return ; // event handled.
}

void qMonteCarloTool::cbGetDir(  ) {
/* Respond to the "..." button next to the current directory edit
 * field, by opening a dialog to retrieve a directory. */
	
	QString csName;
	csName = QFileDialog::getExistingDirectory(QString::null, this, "file dialog", 
		QString::null, TRUE, TRUE );

	if (csName != QString::null) {
//        lock_ = 1;
        edCurDir_->setText( csName );
//        lock_ = 0;
        cbGoToDir( );
    }
	
    //delete tmp;
	return ; // event handled.
}

static char* faTypes[]={
	"All Files (*.*)", "*.*",
		0,0
};

void qMonteCarloTool::cbGetFlowFile(  ) {
/* Open a dialog to get a new flow archive file name. Trim the
*  path information if it is in the current "load_directory" or
*  some subdir thereof.
	*/

	QString flowFile;
	flowFile = QFileDialog::getOpenFileName ( QString::null, "All Files (*.*)", this, 
		"open flow file", "Flow Archive File");

	if (flowFile != QString::null) {
	/* Have a file name: record it.
		*/
		struct altern_data *cur_alt = &(qMonteCarloManager::get()->getMonteData()->alts[current_]);
		//QString csName = tmp->name();
		
		trimLoadDirName( flowFile );
		if (cur_alt->flow_archive != NULL)
			free( cur_alt->flow_archive );
		cur_alt->flow_archive = strdup( flowFile.ascii() );
		cur_alt->changed = TRUE;
		updateAlternative();
	}
	
	
	return ; // event handled.
}

/* The user has typed some new text into the flow archive file edit field.
*  If that included a CR or LF, it indicates the user wants this new
*  flow archive file to take effect now.
*/
void qMonteCarloTool::cbFlowArch(  ) {
	
//	if (lock_)
//		return ;
	cbUseFlowFile();
	return ; // event handled.
}


void qMonteCarloTool::cbUseFlowFile(  ) {
/* Take whatever the user typed in as a flow archive file name and try
*  to make the best of it.
	*/
	struct altern_data *cur_alt = &(qMonteCarloManager::get()->getMonteData()->alts[current_]);
	if (cur_alt->flow_archive != NULL)
		free( cur_alt->flow_archive );
	cur_alt->flow_archive = strdup( edFlowArch_->text().ascii() );
	cur_alt->changed = TRUE;
	updateAlternative();
	
	return ; // event handled.
}

/* Here we have the callbacks for the "Run" and "Cancel Run" buttons.
*/
void qMonteCarloTool::cbRunMonte( ) {
    // The legacy code seems to lose track of this sometimes.
    cbListBoxAlterns( );
	
	/* If something is dirty, offer to write it out first.
	*/
	if (qMonteCarloManager::get()->someAltDirty()) {
		//zMessage *zms = new zMessage( this,  
		//	"Save all edited data before running?",
		//	"Monte Carlo Tool",
		//	MB_YESNOCANCEL | MB_ICONQUESTION );
		int ans = QMessageBox::question (this, "Monte Carlo Tool", "Save all edited data before running?",
			QMessageBox::No, QMessageBox::Yes | QMessageBox::Default, QMessageBox::Cancel);
		//unsigned int iValue = zms->value();
		//delete zms;
		if (ans == QMessageBox::Yes)
			cbApplyAll( );
		else if (ans == QMessageBox::Cancel)
			return ;  // event handled.
		/* else if (iValue == IDNO)
		fall through and run simulation. */
	}
	
	/* create runnable monte carlo structure. NOTE: the structure we edit
	*  is all available alternatives, whereas the structure to be run
	*  should only contain the alternatives to be run. So we create a new
	*  "alts" array and copy those into it. The subsidiary structures will
	*  not be harmed, so we can continue to point to them.
	*/
	int n;
	n = 0;
	for (reset_altern_list();get_current_list_altern();inc_altern_list())
		if (get_current_altern_list_flag())
			n++;
		
	if (n == 0) {
		//delete new zMessage( this, 
		//	"No alternatives selected - nothing to run.",
		///	"Monte Carlo Tool",
		//	MB_OK | MB_ICONEXCLAMATION );
		 QMessageBox::warning (this, "Monte Carlo Tool", "No alternatives selected - nothing to run.",
				 QMessageBox::Ok, QMessageBox::NoButton);
		return ;
	}

	/* TODO: do something about supplemental output */
	/*if (ckSupplemental_->isOn())
		run_mode.monte_data->flags = SUMMARY;
	else
		run_mode.monte_data->flags = 0;*/

	/* Disable variance suppression for this run if necessary */
	disable_variance_suppression = ckDisableVarianceSuppression_->isChecked() != 0;

	/* TODO: have these enable/disable things rely on qScenarioManager */
	 /* Turn off all buttons etc which could change data structure.
	 *  The first set is only disabled/enabled for monte carlo simulations.
	 */
	bCreate_->setEnabled(FALSE);
	bDelete_->setEnabled(FALSE);
	bRename_->setEnabled(FALSE);
	bOK_->setEnabled(FALSE);
	bCancel_->setEnabled(FALSE);
	bApply_->setEnabled(FALSE);
	bReset_->setEnabled(FALSE);
	bApplyAll_->setEnabled(FALSE);
	bResetAll_->setEnabled(FALSE);
	lbAlterns_->setEnabled(FALSE);
	edCurDir_->setEnabled(FALSE); // This causes  cbGoToDir to be called, so watch out
	bGetDir_->setEnabled(FALSE);
	bGoToDir_->setEnabled(FALSE);
	cbAltToEdit_->setEnabled(FALSE);
	bRunMonte_->setEnabled(FALSE);

	/* The second group is disabled/enabled in the normal course of editing
	 *  monte carlo alternatives.
	 */
	disableFlowArchGroup();
	edNGames_->setEnabled(FALSE);
	rbFlowFromArch_->setEnabled(FALSE);
	rbFlowFromUser_->setEnabled(FALSE);
	lbFiles_->setEnabled(FALSE);
	edNFiles_->setEnabled(FALSE);
	bUseNFiles_->setEnabled(FALSE);
	edFilename_->setEnabled(FALSE);
	bSetFile_->setEnabled(FALSE);
	bSetAllFile_->setEnabled(FALSE);
	bGetYearFile_->setEnabled(FALSE);

	/* And the "Cancel Run" button is only enabled when running a simulation.
	 */
	bCancelMonte_->setEnabled(TRUE);

	qMonteCarloManager::get()->runMonteCarlo();

	return ; // event handled.
}

/* The routine to actually launch a monte carlo simulation. It is static so
 *  that the hidden "this" argument will not get in the way. "arg" is ignored.
 */


void qMonteCarloTool::cbCancelMonte(  ) {
	scenarioManager.runCancel();
}

/* This routine is called either by the simulation run thread before
 *  exiting. It turns the buttons back on and discards the simulation
 *  data structures.
 */
void qMonteCarloTool::MonteRunCleanup( void ) {
	/* No longer disable variance suppression since the checkbox is
	 * ONLY for Monte Carlo runs */
	disable_variance_suppression = 0;

	/* Turn all buttons back on. Some are only turned on/off by running
	 *  a simulation; the rest can be turned on by calling updateDialog();
	 */
	bCreate_->setEnabled(TRUE);
	bDelete_->setEnabled(TRUE);
	bRename_->setEnabled(TRUE);
	bOK_->setEnabled(TRUE);
	bCancel_->setEnabled(TRUE);
	bApply_->setEnabled(TRUE);
	bReset_->setEnabled(TRUE);
	bApplyAll_->setEnabled(TRUE);
	bResetAll_->setEnabled(TRUE);
	lbAlterns_->setEnabled(TRUE);
	edCurDir_->setEnabled(TRUE);
	bGetDir_->setEnabled(TRUE);
	bGoToDir_->setEnabled(TRUE);
	cbAltToEdit_->setEnabled(TRUE);
	bRunMonte_->setEnabled(TRUE);
	clearProgress();
	updateDialog();

	/* Turn the "Cancel Run" button off.
	 */
	bCancelMonte_->setEnabled(FALSE);

	/* TODO: this code should probably be in qMonteCarloManager */
	/* discard data structures used only for run.
	 */
	free( run_mode.monte_data->alts );
	free( run_mode.monte_data );
	run_mode.monte_data = NULL;
}

/* Receive notification that run status has changed, or issue notification.
 */


void qMonteCarloTool::cbRunStatusChanged( void ){
	 /* Turn off all buttons etc which could change data structure.
	 *  The first set is only disabled/enabled for monte carlo simulations.
	 */
	bCreate_->setEnabled(!run_mode.running);
	bDelete_->setEnabled(!run_mode.running);
	bRename_->setEnabled(!run_mode.running);
	bOK_->setEnabled(!run_mode.running);
	bCancel_->setEnabled(!run_mode.running);
	bApply_->setEnabled(!run_mode.running);
	bReset_->setEnabled(!run_mode.running);
	bApplyAll_->setEnabled(!run_mode.running);
	bResetAll_->setEnabled(!run_mode.running);
	lbAlterns_->setEnabled(!run_mode.running);
	edCurDir_->setEnabled(!run_mode.running); // This causes  cbGoToDir to be called, so watch out
	bGetDir_->setEnabled(!run_mode.running);
	bGoToDir_->setEnabled(!run_mode.running);
	cbAltToEdit_->setEnabled(!run_mode.running);
	bRunMonte_->setEnabled(!run_mode.running);

	/* The second group is disabled/enabled in the normal course of editing
	 *  monte carlo alternatives.
	 */

	edNGames_->setEnabled(!run_mode.running);
	rbFlowFromArch_->setEnabled(!run_mode.running);
	rbFlowFromUser_->setEnabled(!run_mode.running);
	lbFiles_->setEnabled(!run_mode.running);
	edNFiles_->setEnabled(!run_mode.running);
	bUseNFiles_->setEnabled(!run_mode.running);
	edFilename_->setEnabled(!run_mode.running);
	bSetFile_->setEnabled(!run_mode.running);
	bSetAllFile_->setEnabled(!run_mode.running);
	bGetYearFile_->setEnabled(!run_mode.running);

	/* And the "Cancel Run" button is only enabled when running a simulation.
	 */
	bCancelMonte_->setEnabled(run_mode.running);

	if (run_mode.running) {
		disableFlowArchGroup();
	}
	else {
		/* TODO: enable flow archive group? */
	}
}

/* User has changed number in "# yearly inpuf files" edit field. If that
 *  change was typing in a CR or LF, that means to apply the current number
 *  in the field.
 */
void qMonteCarloTool::cbNFiles(  ) {
//	if (lock_)
//		return;

	cbUseNFiles( );

	return ; // event handled.
}


void qMonteCarloTool::cbUseNFiles( ) {
	/* Some change in the number of yearly input files field. Resize the
	 *  input_files structure.
	 */


	struct altern_data *cur_alt = &(qMonteCarloManager::get()->getMonteData()->alts[current_]);
	int nnew, nold, i;
	nold = cur_alt->nyears;
	nnew = edNFiles_->text().toInt();
	
	if (nnew > 0 && nnew != nold) {
		if (nnew < nold) {
			/* Dispose of some input file names.
			 */
			for (i = nnew; i < nold; i++)
				free( cur_alt->input_files[i].file_name );
		}

		/* Resize array of "input_file" structures.
		 */
		cur_alt->input_files = (struct input_file *)
			realloc( cur_alt->input_files, sizeof(struct input_file) * nnew );

		if (nnew > nold) {
			/* Put in some scratch names for any new "input_file" structures.
			 */
			for (i = nold; i < nnew; i++) {
				cur_alt->input_files[i].file_name = strdup( "No_filename" );
				cur_alt->input_files[i].list = NULL;
			}
		}
		cur_alt->nyears = nnew;
		if (cur_alt->multi != NULL)
			cur_alt->multi->nyears = nnew;
		cur_alt->changed = TRUE;
		updateAlternative();
	}

	
	return ; // event handled.
}

void qMonteCarloTool::cbSetFile(  ) {
//	if (lock_)
//		return ;
//	lock_ = 1;

	/* Take the current text in edFilename_ and make it the currently
	 *  selected input file. If any problems are encountered, do nothing.
	 */
	struct altern_data *cur_alt = &(qMonteCarloManager::get()->getMonteData()->alts[current_]);
	int n = lbFiles_->currentItem();
	QString cstr = edFilename_->text();
	if (n >= 0 && n < cur_alt->nyears && cstr.length() > 0) {
		free( cur_alt->input_files[n].file_name );
		cur_alt->input_files[n].file_name = strdup( cstr.ascii() );
		lbFiles_->removeItem( n );
		lbFiles_->insertItem( cstr, n );
		cur_alt->changed = TRUE;
		showCurrDirty( TRUE );
	}

//	lock_ = 0;
	return ; // event handled.
}

void qMonteCarloTool::cbSetAllFile(  ) {
//	if (lock_)
//		return ;
//	lock_ = 1;

	/* Take the current text in edFilename_ and make it be the text for all
	 *  input files. If any problems are encountered, do nothing.
	 */
	struct altern_data *cur_alt = &(qMonteCarloManager::get()->getMonteData()->alts[current_]);
	int i, n = lbFiles_->count();
	QString cstr = edFilename_->text();
	for(i = 0; i < n; i++) {
		free( cur_alt->input_files[i].file_name );
		cur_alt->input_files[i].file_name = strdup( cstr.ascii() );
		lbFiles_->removeItem( i );
		lbFiles_->insertItem( cstr, i );
	}
	cur_alt->changed = TRUE;
	showCurrDirty( TRUE );

//	lock_ = 0;
	return ; // event handled.
}

/* Helper routine for updateAlternative(). This routine disables a group of
 *  controls which are only enabled when the alternative being edited is
 *  valid and has flow coming from an archive file.
 */
void qMonteCarloTool::disableFlowArchGroup( void ) {
	stFlowArch_->setEnabled(FALSE);
	edFlowArch_->setText( "" );
	edFlowArch_->setEnabled(FALSE);
	bGetFlowFile_->setEnabled(FALSE);
	bUseFlowFile_->setEnabled(FALSE);
	rbSpillFromArch_->setChecked( FALSE );
	rbSpillFromArch_->setEnabled(FALSE);
	rbSpillFromUser_->setChecked( FALSE );
	rbSpillFromUser_->setEnabled(FALSE);
	stSpillFrom_->setEnabled(FALSE);
	rbElevFromArch_->setChecked( FALSE );
	rbElevFromArch_->setEnabled(FALSE);
	rbElevFromUser_->setChecked( FALSE );
	rbElevFromUser_->setEnabled(FALSE);
	stElevFrom_->setEnabled(FALSE);
	lbYears_->clear();
	lbYears_->setEnabled(FALSE);
	edYear_->setText( "" );
	edYear_->setEnabled(FALSE);
	stYear_->setEnabled(FALSE);
	bSetNGame_->setEnabled(FALSE);
	bSetAllNGame_->setEnabled(FALSE);
}

/* This helper routine gets a CString which contains a pathname returned
 *  by zFileOpenForm. It tries to trim off the prefix string if is the same
 *  as the load_directory string of the current alternative. If successful,
 *  the trim is done in place.
 */
void qMonteCarloTool::trimLoadDirName( QString &str ) {
	struct altern_data *cur_alt = &(qMonteCarloManager::get()->getMonteData()->alts[current_]);
	QString cstr = str;
	size_t i, l;

	/* See if we can trim load directory name from new file name.
	 */
	if (cur_alt->load_directory != NULL) {
#ifdef WIN32
		/* Translate to Unix pathname convention.
		 */
		while ((i = cstr.find( '\\' )) != -1)
			cstr[(int)i] = '/';
		if (cstr.find( cur_alt->load_directory, FALSE ) == 0) {
#else
		/* Unix pathnames.
		 */
		if (cstr.find( cur_alt->load_directory ) == 0)  {
#endif
			/* Load directory name is in fact a prefix string in
			 *  the new file name: make a string of the postfix only.
			 */
			l =	strlen( cur_alt->load_directory );
			if (cstr.at(l) == '/')
				l++;
			str = cstr.mid ( l  );
		}
	}
}

void qMonteCarloTool::cbGetYearFile(  ) {
	/* Get a yearly input file name string and put it in edFilename_
	 */
//	if (lock_)
//		return ;

	//zFileOpenForm *tmp;
	//tmp=new zFileOpenForm( this, "Yearly Input File", "", faTypes );
	QString yearFile;
	yearFile = QFileDialog::getOpenFileName ( QString::null, "All Files (*.*)", this, 
		"open flow file", "Yearly Input File");

	if (yearFile != QString::null) {	
		struct altern_data *cur_alt = &(qMonteCarloManager::get()->getMonteData()->alts[current_]);
		QString csName = yearFile;

		trimLoadDirName( csName );
//		lock_ = 1;
		edFilename_->setText( csName );
//		lock_ = 0;
	}

	return ; // event handled.
}

void qMonteCarloTool::cbListBoxAlterns(  ) {
	QStringList altsToRun;
	unsigned int i;

	for (i=0; i < lbAlterns_->count(); i++) {
		if (lbAlterns_->isSelected(i)) altsToRun << lbAlterns_->text(i);
	}

	qMonteCarloManager::get()->setRunList(altsToRun);
}

void qMonteCarloTool::cbListBoxYears(  ) {

	QString csSel;
	int i;

//	if (lock_)
//		return ;
//	lock_ = 1;

	if (lbYears_->currentItem() != -1) {
		csSel =   lbYears_->currentText();
		if ((i = csSel.find(':')) != -1) {
			edYear_->setText( csSel.left(  i ) );
			edNGames_->setText( csSel.mid( i+1 ) );
		}
	}

//	lock_ = 0;
	return ; // event handled.
}

void qMonteCarloTool::cbListBoxFiles(  ) {
//	if (lock_)
//		return ;
//	lock_ = 1;

	/* Selection event in the list of yearly input files. Put the selected
	 *  text in edFilename_.
	 */
	int i = lbFiles_->currentItem();
	edFilename_->setText( lbFiles_->text( i ) );

//	lock_ = 0;
	return ; // event handled.
}

void qMonteCarloTool::cbComboBoxAlterns(  ) {

	current_ = cbAltToEdit_->currentItem();
	updateAlternative();

	return ; // event handled.
}

/* Routine to fill part of the dialog from the current alternative.
 */
void qMonteCarloTool::updateAlternative( void ) {
	char cBuf[20];
	int i;

	showSomeDirty(qMonteCarloManager::get()->someAltDirty());

	if (current_ < 0) {
		/* No current selection - clear and/or deselect everything.
		 */
		rbFlowFromArch_->setChecked( FALSE );
		rbFlowFromArch_->setEnabled(FALSE);
		rbFlowFromUser_->setChecked( FALSE );
		rbFlowFromUser_->setEnabled(FALSE);
		disableFlowArchGroup();
		edNGames_->setText( "" );
		edNGames_->setEnabled(FALSE);
		lbFiles_->clear();
		lbFiles_->setEnabled(FALSE);
		edNFiles_->setText( "" );
		edNFiles_->setEnabled(FALSE);
		edFilename_->setText( "" );
		edFilename_->setEnabled(FALSE);
		bGetYearFile_->setEnabled(FALSE);
		bSetFile_->setEnabled(FALSE);
		bUseNFiles_->setEnabled(FALSE);
		bSetAllFile_->setEnabled(FALSE);

		showCurrDirty( FALSE );
	}
	else {
		struct altern_data *cur_alt = &(qMonteCarloManager::get()->getMonteData()->alts[current_]);
		/* Enable or disable, and fill in fields from current selection.
		 */
		rbFlowFromArch_->setEnabled(TRUE);
		rbFlowFromUser_->setEnabled(TRUE);

		// Ensure that the correct alternative is selected in the combo box
		cbAltToEdit_->setCurrentItem(current_);

		/* These fields are always enabled if we have a current alternative.
		 */
		lbFiles_->setEnabled(TRUE);
		lbFiles_->clear();
		for (i = 0; i < cur_alt->nyears; i++)
			lbFiles_->insertItem( cur_alt->input_files[i].file_name,
												lbFiles_->count() );
		lbFiles_->setSelected( 0, TRUE );  /* something should always be selected
									*  in this box. */
		edNFiles_->setEnabled(TRUE);
		sprintf( cBuf, "%d", cur_alt->nyears );
		edNFiles_->setText( cBuf );

		edFilename_->setText( "" );
		edFilename_->setEnabled(TRUE);
		bGetYearFile_->setEnabled(TRUE);
		bSetFile_->setEnabled(TRUE);
		bUseNFiles_->setEnabled(TRUE);
		bSetAllFile_->setEnabled(TRUE);

		edNGames_->setEnabled(TRUE);

		/* these fields depend on the flow source setting.
		 */
		if (cur_alt->use_flow_from == ARCHIVE_FLOW) {
			/* Flow from archive file */ 
			rbFlowFromArch_->setChecked( TRUE );
			rbFlowFromUser_->setChecked( FALSE );

			stFlowArch_->setEnabled(TRUE);
			edFlowArch_->setText( cur_alt->flow_archive != NULL ?
									cur_alt->flow_archive : "" );
			edFlowArch_->setEnabled(TRUE);
			bGetFlowFile_->setEnabled(TRUE);
			bUseFlowFile_->setEnabled(TRUE);

			rbSpillFromArch_->setChecked( cur_alt->use_spill_from == ARCHIVE_SPILL );
			rbSpillFromArch_->setEnabled(TRUE);
			rbSpillFromUser_->setChecked( cur_alt->use_spill_from != ARCHIVE_SPILL );
			rbSpillFromUser_->setEnabled(TRUE);
			stSpillFrom_->setEnabled(TRUE);
			rbElevFromArch_->setChecked( cur_alt->use_elev_from == ARCHIVE_ELEV );
			rbElevFromArch_->setEnabled(TRUE);
			rbElevFromUser_->setChecked( cur_alt->use_elev_from != ARCHIVE_ELEV );
			rbElevFromUser_->setEnabled(TRUE);
			stElevFrom_->setEnabled(TRUE);

			lbYears_->clear();
			lbYears_->setEnabled(TRUE);
			edYear_->setText( "" );
			edNGames_->setText( "" );

			/* Since we have a flow archive file, open it and grovel through
			 *  it to find out how many flow years are archived and their
			 *  names. Then make sure the multi_data structure for this
			 *  alternative matches that. This code will be executed whenever
			 *  the flow archive file is changed, so it will update the
			 *  year information automatically.
			 */
			struct archive_map *amap
					= open_and_map_flow( cur_alt );
			if (amap != NULL) {
				cur_alt->ngames = amap->ngames;
				if (cur_alt->multi != NULL &&
						cur_alt->multi->ngames != amap->ngames) {
					free_multi_data( cur_alt->multi );
					cur_alt->multi = NULL;
				}
				if (cur_alt->multi == NULL) {
					cur_alt->multi = 
						(struct multi_data *)malloc( sizeof( struct multi_data ) );
					bzero( cur_alt->multi, sizeof( struct multi_data ) );
					cur_alt->multi->nruns = 
							(int *)malloc( amap->ngames * sizeof( int ) );
					bzero( cur_alt->multi->nruns, amap->ngames * sizeof( int ) );
					cur_alt->multi->ngames = amap->ngames;
					cur_alt->multi->nyears = cur_alt->nyears;

					/* By default, one game per year.
					 */
					for (i = 0; i < amap->ngames; i++)
						cur_alt->multi->nruns[i] = 1;
					 
				}
				cur_alt->multi->gamesum = 0;
				for (i = 0; i < amap->ngames; i++) {
					sprintf( cBuf, "%s:%d", amap->gamenames[i],
									cur_alt->multi->nruns[i] );
					cur_alt->multi->gamesum += cur_alt->multi->nruns[i];
					lbYears_->insertItem( cBuf );
					free( amap->gamenames[i] );
				}
				cur_alt->ngames = cur_alt->multi->gamesum;
				free_ftells_alt( amap->offsets, amap->nyears );
				free( amap->gamenames );
				free( amap );

				edYear_->setEnabled(TRUE);
				stYear_->setEnabled(TRUE);
				bSetNGame_->setEnabled(TRUE);
				bSetAllNGame_->setEnabled(TRUE);
			}
			else {
				edYear_->setEnabled(FALSE);
				stYear_->setEnabled(FALSE);
				edNGames_->setEnabled(FALSE);
				bSetNGame_->setEnabled(FALSE);
				bSetAllNGame_->setEnabled(FALSE);
			}
		}
		else
		{
			/* Flow from user data. */
			rbFlowFromArch_->setChecked( FALSE );
			rbFlowFromUser_->setChecked( TRUE );

			disableFlowArchGroup();

			sprintf( cBuf, "%d", cur_alt->ngames );
			edNGames_->setText( cBuf );
		}
		showCurrDirty( cur_alt->changed != 0 );
	}

	cbListBoxFiles(); // Update current file editing box as well

	updateStatus();
}

/* Routine to update only the status indicators. (# alternatives, # games
 *  & # years for current alternative.
 */
void qMonteCarloTool::updateStatus( void ) {
	if (current_ < 0) {
		stNAltGames_->setText( "" );
		stNAltYears_->setText( "" );
		stNAlts_->setText( "" );
	}
	else {
		struct altern_data *cur_alt = &(qMonteCarloManager::get()->getMonteData()->alts[current_]);
		char cBuf[20];
		int n;
		sprintf( cBuf, "%d", cur_alt->ngames );
		stNAltGames_->setText( cBuf );
		sprintf( cBuf, "%d", cur_alt->nyears );
		stNAltYears_->setText( cBuf );

		/* Count alternatives which will actually be run.
		 */
		n = 0;
	    for (reset_altern_list();get_current_list_altern();inc_altern_list())
			if (get_current_altern_list_flag())
				n++;
		sprintf( cBuf, "%d", n );
		stNAlts_->setText( cBuf );
	}
}

/* Set all progress bars to 0%.
 */
void qMonteCarloTool::clearProgress( void ) {
	char cBuf[30];

	textProgress( cBuf, 0 );

	progNAltGames_->setText( cBuf );
	progNAltYears_->setText( cBuf );
	progNAlts_->setText( cBuf );
}

/* This helper routine makes a fake "progress bar" in a text string.
 *  The returned string will always be 25 characters long and will have
 *  the percentage (0 - 100) in the center.
 */
void qMonteCarloTool::textProgress( char *pch, int prog ) {
	char cBuf[20];
	int i, s, l;

	sprintf( cBuf, " %d%s ", prog, "%" );   // e.g. "50%"
	prog /= 4;
	for (i = 0; i < prog; i++)
		pch[i] = '+';
	for (; i < 25; i++)
		pch[i] = '_';
	pch[25] = 0;
	l = strlen( cBuf );
	s = 12 - l / 2;
	for (i = 0; i < l; i++)
		pch[i+s] = cBuf[i];
}

/* This routine set the status and progress indicators during a monte
 *  carlo run, and updates them to the screen immediately.
 */
void qMonteCarloTool::setStatusProgress( int game, int gameprog, 
										  int year, int yearprog,
										  int alt, int altprog ) {
	char cBuf[30];

	sprintf( cBuf, "%d", game );
	stNAltGames_->setText( cBuf );

	sprintf( cBuf, "%d", year );
	stNAltYears_->setText( cBuf );

	sprintf( cBuf, "%d", alt );
	stNAlts_->setText( cBuf );

	textProgress( cBuf, gameprog );
	progNAltGames_->setText( cBuf );
	
	textProgress( cBuf, yearprog );
	progNAltYears_->setText( cBuf );
	
	textProgress( cBuf, altprog );
	progNAlts_->setText( cBuf );
}


