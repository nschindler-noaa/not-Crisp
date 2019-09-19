#include "qMonteCarloManager.h"
#include "qScenarioManager.h"
#include "altern_list.h"
#include "file.h"
#include "token.h"
#include "monte_frame.h"
#include "parse_alt.h"
#include "log.h"
#include "dir.h"
#include "run_mode.h"
#include <stdlib.h>
#include "strtools.h"
#ifdef WIN32
#include <direct.h>
#else
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#endif
#include <qdir.h>
#include <qstringlist.h>

static qMonteCarloManager *manager = NULL;

qMonteCarloManager* qMonteCarloManager::get() {
	if (manager) return manager;
	else return manager = new qMonteCarloManager();
}

qMonteCarloManager::qMonteCarloManager()
:	monte_data(NULL)
{
	createLocalList();
}

qMonteCarloManager::~qMonteCarloManager() {
}

void qMonteCarloManager::createLocalList( void ) {
	/* TODO: fix or remove this comment, because it ain't right */
	/* NOTE: we create the in-memory structures when the dialog is constructed,
	 *  and destroy them when the dialog is destructed. The run_mode.monte_data
	 *  member should be NULL when this dialog does not exist.
	 */
	altern_update_directory();
	readAltsToRun();
	readMonteFiles();
}

/* These functions read and write the .compass-alts file in the current
 *  directory - the list of alternatives to be run.
 */
void qMonteCarloManager::readAltsToRun( void ) {
    FILE *asfp;
    char buf[MAXPATHLEN];

	clear_all_altern_flags();
	
    if (!(asfp = buf_open(ALTERN_LIST_FILE, "r"))) 
		return;
	
    while(pop_token(asfp, buf, "Alternative File -> ") != EOF) {
	    for (reset_altern_list(); get_current_list_altern(); inc_altern_list()) 
		{
			if (!strcmp( get_current_list_altern(), buf )) {
				set_current_altern_list_flag( 1 );
				break;
			}
		}
    }
	runListDirty = FALSE;
    buf_close(asfp);
}

/* Routine to read all monte alternatives from current directory - whether or
 *  not they are selected to run. Assumes list of alternatives is valid, but
 *  may be empty.
 */
void qMonteCarloManager::readMonteFiles( void ) {

    struct monte_carlo_data *monte_data_new;
    int data_count = 10;

	monte_data_new = make_monte_object();
	monte_data_new->alts = (struct altern_data *)
	    malloc(data_count * sizeof(struct altern_data));

	if (!monte_data_new->alts) {
//	    log_msg(L_FATAL, "Out of memory loading alternatives!\n");
		return;
	}

	monte_data_new->nalternatives = 0;
    for (reset_altern_list() ; get_current_list_altern() ; inc_altern_list()) {
	    bzero(&monte_data_new->alts[monte_data_new->nalternatives],
									sizeof(struct altern_data));
	    set_altern_defaults(&monte_data_new->alts[monte_data_new->nalternatives]);
		if (open_and_parse_alternative(
						monte_altern_name( get_current_list_altern() ), 
						&monte_data_new->alts[monte_data_new->nalternatives]))
			continue;
		monte_data_new->alts[monte_data_new->nalternatives].changed = 0;

	    if (++monte_data_new->nalternatives == data_count) {
			data_count *= 2;
			monte_data_new->alts = (struct altern_data *)
				realloc(monte_data_new->alts, data_count
					* sizeof(struct altern_data));
			if (!monte_data_new->alts) {
//				log_msg(L_FATAL, "Out of memory resizing alternative list!\n");
				return;
			}
	    }
	}

	if (monte_data_new->nalternatives != 0) {
		monte_data_new->alts = (struct altern_data *)
			realloc(monte_data_new->alts,
				monte_data_new->nalternatives * sizeof(struct altern_data));
		if (!monte_data_new->alts) {
//			log_msg(L_FATAL, "Out of memory downsizing alternative list! (odd)\n");
			return;
		}
	}

	monte_data = monte_data_new;
}

/* Routine to see if some alternative in the current local list of alternatives
 *  has been edited.
 */
int qMonteCarloManager::someAltDirty() {
	int i;

	if (monte_data == NULL)
		return 0;

	for (i = 0; i < monte_data->nalternatives; i++)
		if (monte_data->alts[i].changed)
			return 1;

	return 0;
}

int qMonteCarloManager::createAlternative(const QString& name) {
	/* Try to create directory - ignore errors since directory may already
	*  exist. Try to create file. This makes sure the file name we were
	*  given is more-or-less legitimate.
	*/
#ifdef WIN32
	mkdir(name.ascii());
#else
	mkdir(name.ascii(), 0777);
#endif
	
	FILE *outfile;
	outfile = fopen(monte_altern_name(name.ascii()), "w");
	if (outfile == NULL) {
		log_msg(L_ERROR, "Monte Carlo: Could not write to %s\n", name.ascii());
		return -1; /* Error */
	}
	fclose(outfile);

	/* Make a new place in the "alts" array. Make a default alternative in
	*  there. NOTE: be careful to do the big allocation first,
	*  so that the structure is left in a good state if it fails.
	*/
	struct altern_data *new_alt = (struct altern_data*) malloc((monte_data->nalternatives + 1) * sizeof(struct altern_data));
	if (new_alt == NULL) return -1; /* Error */

	altern_list_add(name.ascii());
	
	/* Copy from old to new "alt" array, opening a space for the new
	*  alternative. 
	*/
	int current = get_current_list_item_number();
	int i;
	for (i = 0; i < current; i++) bcopy(monte_data->alts+i, new_alt+i, sizeof(struct altern_data));
	for (; i < monte_data->nalternatives; i++)	bcopy(monte_data->alts+i, new_alt+i+1, sizeof(struct altern_data) );
	free(monte_data->alts);
	monte_data->alts = new_alt;
	new_alt = monte_data->alts+current;
	monte_data->nalternatives++;
	
	/* Put in default values for the new alternative.
	*/
	bzero(new_alt, sizeof(struct altern_data));
	new_alt->load_directory = (char*) malloc(MAXPATHLEN);
	getcwd(new_alt->load_directory, MAXPATHLEN);
	new_alt->alternative_name = strdup(name.ascii());
	new_alt->nyears = 1;
	new_alt->input_files = (struct input_file*) malloc(sizeof(struct input_file));
	new_alt->input_files->list = NULL;
	new_alt->input_files->file_name = strdup("No_filename");
	new_alt->changed = 1;
	
	/* write new alternative to disk.
	*/
	applyLocalList(current);
	emit alternativesUpdated();
	
	return current;
}

void qMonteCarloManager::applyLocalList( int index ) {
	/* list of alternatives to run (".compass-alts" file) is updated
	 *  automatically on Apply, ApplyAll, Run, OK. Update is unconditional
     *  because date on file is used as "changed" flag by legacy code.
	 */
	if (runListDirty) {
		writeAltsToRun();
		runListDirty = 0;
	}

	if (index < 0) {
		/* apply globally.
		 */
		for (int i = 0; i < monte_data->nalternatives; i++) applyLocalList(i);
	}
	else {
		/* Apply a single alternative.
		 */
		if (monte_data->alts[index].changed == 0) return;

		FILE *outfile;
		outfile = fopen(monte_altern_name(monte_data->alts[index].alternative_name), "w");

		if (outfile == NULL) return;

		write_altern_data(outfile, &(monte_data->alts[index]));
		fclose(outfile);
		monte_data->alts[index].changed = 0;

		/* TODO: show that the current isn't dirty? */
		/*showCurrDirty(0);
		showSomeDirty(someAltDirty());*/
	}
}

void qMonteCarloManager::writeAltsToRun() {
#ifdef WIN32
    time_t time_var;
#else
    long time_var;
#endif

    FILE *asfp;

    if (!(asfp = fopen(ALTERN_LIST_FILE, "w")))
		return;
    time(&time_var);

#if defined WIN32 | defined SOLARIS2
    fprintf(asfp, "# Written on %s\n", ctime(&time_var));
#else
    fprintf(asfp, "# Written by %s on %s\n", getenv("USER"), ctime(&time_var));
#endif
	for (reset_altern_list(); get_current_list_altern(); inc_altern_list()) 
	{
		if (get_current_altern_list_flag())
			fprintf(asfp, "\"%s\"\n", get_current_list_altern());
    }
    if (ferror(asfp)) {
//		log_msg(L_ERROR, "Error writing alternative setup file.\n");
    }
    fclose(asfp);
	runListDirty = 0;
}

int qMonteCarloManager::getAlternative(const QString& name) {
	if (search_altern_list(name.ascii())) {
		return get_current_list_item_number();
	}
	else return -1;
}

int qMonteCarloManager::deleteAlternative(const QString& name) {
	int index;

	/* See if this alt is due to be run, and if so mark the run list as dirty */
	if (search_altern_list(name.ascii())) {
		index = get_current_list_item_number();
		if (get_current_altern_list_flag()) {
			runListDirty = 1;
		}
		altern_list_remove(name.ascii());
	}
	else return -1;

	killDirectory(name.ascii());
	
	/* Shuffle the alts structure together.
	*/
	for (int i = index; i < monte_data->nalternatives - 1; i++) {
		bcopy(monte_data->alts+i+1, monte_data->alts+i, sizeof(struct altern_data));
	}

	monte_data->nalternatives--;
	
	emit alternativesUpdated();
	
	return 0;
}

/* Remove the given directory and all it contains. Only used on subdirectories
* of the current directory. Failures are silently ignored.
*/
void qMonteCarloManager::killDirectory(const char *theDir) {
	
	QDir d(QDir::current());
	if( !d.cd(theDir) ){  //if the directory is not found
		return;
	}
	
	QFileInfo* finfo;	
	const QFileInfoList* dirContents = d.entryInfoList(QDir::All);
	if(dirContents != NULL) {
		QPtrListIterator<QFileInfo> p(*dirContents);
		while(p.current()){               //iterate through the current directory's contents
			
			finfo = p.current();		
			if(!finfo->fileName().compare("..") || !finfo->fileName().compare(".")){
				++p;
				continue;
			}

			if(finfo->isWritable()){
				if(finfo->isFile() ){
					d.remove(finfo->fileName());					 
				}
				else{
					
					if(!QDir::setCurrent(d.absPath())){  //set the appliations current dir to the delete dir
					//	QMessageBox::warning ( this, "Error Changing Dir", "Could not CD to Dir", QMessageBox::Ok, QMessageBox::NoButton );
					}
					killDirectory(finfo->fileName());   //do the kill
					
					d.cdUp(); //get out of that directory
					
					if(!QDir::setCurrent(d.absPath())){   //set the system directory back where it belongs
					//	QMessageBox::warning ( this, "Error Changing Dir", "Could not CD to Dir", QMessageBox::Ok, QMessageBox::NoButton );
					}
					d.cd(theDir);
				}
				
			}
			//else{
		//		log_msg(0,"unwrittable file , unable to delete alternative");
		//		QMessageBox::warning ( this, "Error Deleting File", "Could not remove file fom dir", QMessageBox::Ok, QMessageBox::NoButton );
		//	}
			++p;
		}
	}

	d.cdUp();   //goto the parent	
	d.rmdir(theDir);  //remove the directory


}

int qMonteCarloManager::renameAlternative(const QString &oldName, const QString &newName) {
	int current_, flag;

	/* Try to create directory. */
#ifdef WIN32
	if (mkdir(newName.ascii()))
#else
	if (mkdir(newName.ascii(), 0777))
#endif
	{
		log_msg(L_ERROR, "Monte Carlo: Could not create directory %s\n", newName.ascii());
		return -1; /* Error */
	}
	
	/* Rename this file in the alterns list, maintaining to-be-run flag. */
	flag = 0;
	if (search_altern_list(oldName.ascii())) {
		flag = get_current_altern_list_flag();
		current_ = get_current_list_item_number();
	}
	else return -1; /* Error */
	altern_list_remove(oldName.ascii());
	altern_list_add(newName.ascii());
	search_altern_list(newName.ascii());
	set_current_altern_list_flag(flag);
	
	/* Change the name in the alts data structure. */
	struct altern_data *cur_alt = &monte_data->alts[get_current_list_item_number()];
	free(cur_alt->alternative_name);
	cur_alt->alternative_name = strdup(newName.ascii());
	cur_alt->changed = 1;
	
	/* Restore the sorted order of the alts array. Since only the renamed
	*  record is out of order, it can be bubbled into place. */
	struct altern_data talt;
	
	while (current_ > 0
		&& strcmp(monte_data->alts[current_].alternative_name, monte_data->alts[current_-1].alternative_name) < 0)
	{
		bcopy(monte_data->alts+current_, &talt, sizeof(struct altern_data));
		bcopy(monte_data->alts+current_-1, monte_data->alts+current_, sizeof(struct altern_data));
		bcopy(&talt, monte_data->alts+current_-1, sizeof(struct altern_data));
		current_--;
	}
	while (current_ < monte_data->nalternatives - 1
		&& strcmp(monte_data->alts[current_].alternative_name,	monte_data->alts[current_+1].alternative_name) > 0)
	{
		bcopy(monte_data->alts+current_, &talt, sizeof(struct altern_data));
		bcopy(monte_data->alts+current_+1, monte_data->alts+current_, sizeof(struct altern_data));
		bcopy(&talt, monte_data->alts+current_+1, sizeof(struct altern_data));
		current_++;
	}
	
	/* Write the new alternative out to disk. */
	applyLocalList(current_);
	
	/* Now try to copy the output files to the new directory, and delete
	*  the entire contents of the old directory. If thisfails, no big problem,
	*  but we try to make things tidy and as the user expects.
	*/
	moveOutputFile(oldName.ascii(), newName.ascii(), ".ind");
	moveOutputFile(oldName.ascii(), newName.ascii(), ".out");
	killDirectory(oldName.ascii());

	emit alternativesUpdated();
	
	return 0;
}

/* This routine tries to copy a Monte Carlo output file from one directory 
*  to another. This is called once each for the the "altern.ind" and 
*  "altern.out" files when renaming an alternative. The files must also be
*  slightly edited as they are copied across. Failures are silently ignored.
*  For example, "altern1/altern1.out" becomes "newalt/newalt.out"
*/
void qMonteCarloManager::moveOutputFile( const char *oldDir, 
									 const char *newDir, const char *extension ) {
	FILE *infile, *outfile;
    int edited = 0;
    char buffer[256];
    char *found;
	
    /* Open input and output files. */
    strcpy( buffer, oldDir );
    strcat( buffer, "/" );
    strcat( buffer, oldDir );
    strcat( buffer, extension );
	infile = fopen( buffer, "r" );
    if (infile == NULL)
        return;
	
    strcpy( buffer, newDir );
    strcat( buffer, "/" );
    strcat( buffer, newDir );
    strcat( buffer, extension );
	outfile = fopen( buffer, "w" );
    if (outfile == NULL) {
        fclose( infile );
		return;
    }
	
    /* Loop copying data. First time "alternative oldDir" is seen, replace
	*  it with "alternative newDir".
	*/
    while (1) {
        if (fgets( buffer, 255, infile ) == NULL)
            break;
        if (!edited) {
            found = strstr( buffer, "alternative" );
            if (found) {
                edited = 1;
                fprintf( outfile, "alternative %s\n", newDir );
                continue;
            }
        }
        fputs( buffer, outfile );
    }
	
    fclose( infile );
    fclose( outfile );
}

int qMonteCarloManager::changeDirectory(const QString &newDir) {
	/* Try to make the directory change and give up if it cannot be made. */
	if (chdir(newDir.ascii())) {
		log_msg(L_ERROR, "Monte Carlo: %s\n", strerror(errno));
		return -1; /* Error */
	}

	altern_update_directory();
	readAltsToRun();
	emit alternativesUpdated();
	return 0;
}

void qMonteCarloManager::clearRunList() {
	setRunList(QStringList());
}

void qMonteCarloManager::setRunList(QStringList altsToRun) {
	QStringList old = getRunList();

	if (old == altsToRun) return; /* No change */

    for (reset_altern_list() ; get_current_list_altern(); inc_altern_list())
	{
		set_current_altern_list_flag(altsToRun.contains(get_current_list_altern()));
	}

	runListDirty = TRUE;

	emit alternativesUpdated();
}

QStringList qMonteCarloManager::getRunList() {
	QStringList altsToRun;
    for (reset_altern_list() ; get_current_list_altern(); inc_altern_list())
	{
		if (get_current_altern_list_flag()) altsToRun += get_current_list_altern();
	}

	return altsToRun;
}

void qMonteCarloManager::runMonteCarlo() {
	writeAltsToRun();
	
	/* create runable monte carlo structure. NOTE: the structure we edit
	*  is all available alternatives, whereas the structure to be run
	*  should only contain the alternatives to be run. So we create a new
	*  "alts" array and copy those into it. The subsidiary structures will
	*  not be harmed, so we can continue to point to them.
	*/
	int i, j, n;
	n = 0;
	for (reset_altern_list(); get_current_list_altern(); inc_altern_list())
		if (get_current_altern_list_flag())
			n++;
	
	run_mode.monte_data = (struct monte_carlo_data *)
		malloc(sizeof(struct monte_carlo_data));

	/* TODO: need to be able to turn on summary output with run_mode.monte_data->flags = SUMMARY or 0 */
	
	run_mode.monte_data->alts = (struct altern_data *)
		malloc(n * sizeof( struct altern_data ));
	run_mode.monte_data->nalternatives = n;
	i = j = 0;
	for (reset_altern_list();get_current_list_altern();inc_altern_list()) {
		if (get_current_altern_list_flag()) {
			bcopy(monte_data->alts + i, run_mode.monte_data->alts + j, sizeof( struct altern_data ));
			j++;
		}
		i++;
	}

	scenarioManager.runMonteCarlo();
}

#if 0

/* Attach/detach with the data token's invalidate event and get notified
 *  when data token has changed. Also find out when scenario runs start and
 *  stop.
 */
void qMonteCarloTool::AttachEvents( void ) {
	//GEPurgeAllIDCs_.attach( this, &qMonteCarloTool::_cbPurgeAll );
	//GERunStatusChanged_.attach( this, &qMonteCarloTool::_cbRunStatusChanged );
	if( !connect(qEvents::Instance(), SIGNAL(GERunStatusChanged_()), SLOT(cbRunStatusChanged()))
		|| !connect(qEvents::Instance(), SIGNAL(GEPurgeAllIDCs_()), SLOT(cbPurgeAll()))){
		QMessageBox::warning (this, "Crisp Passage Error", "Couldn't connect to global events in qMonteCarloTool!",
				 QMessageBox::Ok, QMessageBox::NoButton);		
	}


}
/*
void qMonteCarloTool::DetachEvents( void ) {
	//GEPurgeAllIDCs_.detach( this, &qMonteCarloTool::_cbPurgeAll );
	//GERunStatusChanged_.detach( this, &qMonteCarloTool::_cbRunStatusChanged );
}
*/
void qMonteCarloTool::cbPurgeAll(){
	DestroyLocalList();
	CreateLocalList();
	updateDialog();
}
#endif

/* Routines to manage the local list of monte carlo alternative structures.
 */
void qMonteCarloManager::destroyLocalList( void ) {
	empty_altern_list();
	free_monte_object(monte_data);
	monte_data = NULL;
}

void qMonteCarloManager::resetLocalList( int index ) {
	if (index < 0) {
		/* Reset entire local list.
		 */
		destroyLocalList();
		createLocalList();
	}
	else {
		struct altern_data *cur_alt = &(monte_data->alts[index]);
		char *name = strdup (cur_alt->alternative_name);
		free_alternative_object(cur_alt);
	    bzero(cur_alt, sizeof(struct altern_data));
		open_and_parse_alternative(monte_altern_name(name), cur_alt);
		cur_alt->changed = 0;
		free(name);
	}

	emit alternativesUpdated();
}

#if 0

void qMonteCarloTool::RunStatusChanged( void ) {
	qEvents::Instance()->emitGERunStatusChanged_();
}

void qMonteCarloTool::UpdateAllDataTokens( void ) {
	qEvents::Instance()->emitGEUpdateAllDataTokens_();
	//GEUpdateAllDataTokens_.notify();
}

/* Routine to inform other Monte Carlo dialogs that the alternatives
 *  on disk have changed.
 */
void qMonteCarloTool::prodMonteDialogs( void ) {
    // .compass-alts last mod date is the flag used to trigger re-read
    //   of alternatives.
    writeAltsToRun();

    // Tell the legacy code which handles monte carlo reports to toss
    //  away everything it knows and start over with the current directory.
    report_update_directory();

    readAltsToRun();

    qMonteReportTool *mrp = qMonteReportTool::instance( NULL );
    if (mrp != NULL)
		mrp->altsUpdated();

	qMonteAltCmpDlg *mcd = qMonteAltCmpDlg::instance( NULL );
	if (mcd != NULL)
		mcd->altsUpdated();

	qMonteAnalDlg *mad = qMonteAnalDlg::instance( NULL );
	if (mad != NULL)
		mad->altsUpdated();

    // Now repair the altern list, based on available alternatives, not
    //  only those which have valid output data.
    altern_update_directory();
    cbListBoxAlterns();
}

void monteThread::run() {
	int i = run_monte_carlo( run_mode.monte_data );
	run_mode.mode = MONTE-1;	
}

#endif

