/* qDataReportTool.h
 * This class defines a modal dialog which edits report structures
 *  via listboxes, and also causes reports to be printed. The dialog is based 
 *  on the InputReportToolBase template. This is an abstract base
 *  class which is not expected to be instantiated by itself.
 *
 */

#include "qDataReportTool.h"
#include "qTextOutDialog.h"
#include "Qcrisp.h"
#include "resource.h"
#include <stdlib.h>
#include "qlistbox.h"
#include "qcombobox.h"
#include "qpushbutton.h"
#include "qlabel.h"
#include "log.h"
#include "qHelpManager.h"

#ifdef SOLARIS2
#include <unistd.h>
#endif

qDataReportTool::qDataReportTool( QWidget *w, struct report_info *info,
                                         int rememberSelections )
	: InputReportToolBase ( NULL, "DataReportTool", FALSE, WType_TopLevel | WStyle_SysMenu | WStyle_Minimize | WStyle_Dialog | WDestructiveClose )
{
	info_ = info;
    rememberSelections_ = rememberSelections;
	//Set the controls up with new names:
	stLabel_[0] = Label1;
    stLabel_[1] = Label2;
	stLabel_[2] = Label3;
    stLabel_[3] = Label4;
    bView_ = ViewButton;
    bViewAll_ = ViewAllButton;
    bClear_ = ClearButton;
    bClearAll_ = ClearAllButton;
    bDone_ = DoneButton;
    bHelp_ = HelpButton;
    cmbCategory_ = comboBox1;
    cmbLength_ = comboBox2;
    listbox_[0] = listBox1;
    listbox_[1] = listBox2;    
    listbox_[2] = listBox3;
	listbox_[3] = listBox4;

	consCommon();
}

/* Common code called by all constructors.
 */
void qDataReportTool::consCommon( void ) {
	
	lock_ = 1;

	/* Several line lengths are available in the reports
	 */
	cmbLength_->insertItem("80");
	cmbLength_->insertItem( "104" );
	cmbLength_->insertItem( "140" );
	cmbLength_->insertItem( "1024" );
	cmbLength_->setCurrentItem(0);  //set the first item selectred
	cmbLength_->updateGeometry();

	/* The list of choices passed in is presented in this combobox.
	 */
	connect(cmbCategory_, SIGNAL(activated(int)), this, SLOT(cbCategory(int)));
	//cmbCategory_->setNotifySelChange( this, (NotifyProc)cbCategory );
	int i = 0;
	for (i = 0; info_->choices_list[i].title != NULL; i++)
		cmbCategory_->insertItem( info_->choices_list[i].title );
    nCategory_ = i;
	cmbCategory_->setCurrentItem( 0 );  //select the first item
    if (nCategory_ <= 1)
        cmbCategory_->setEnabled(false);
	curPage_ = 0;

	/* Get the child window controls hooked up. */
	connect(bView_, SIGNAL(clicked()), this, SLOT(cbView()));
	connect(bViewAll_, SIGNAL(clicked()), this, SLOT(cbViewAll()));
   
	if (nCategory_ <= 1)
        bViewAll_->setEnabled(false);

	connect(bClear_, SIGNAL(clicked()), this, SLOT(cbClear()));
	connect(bClearAll_, SIGNAL(clicked()), this, SLOT(cbClearAll()));
    
	if (nCategory_ <= 1)
        bClearAll_->setEnabled(false);

	connect(bDone_, SIGNAL(clicked()), this, SLOT(cbDone()));
	connect(bHelp_, SIGNAL(clicked()), this, SLOT(cbHelp()));
	
	listbox_[0]->setSelectionMode(QListBox::Multi);
	listbox_[1]->setSelectionMode(QListBox::Multi);
	listbox_[2]->setSelectionMode(QListBox::Multi);
	listbox_[3]->setSelectionMode(QListBox::Multi);


	connect(listbox_[0], SIGNAL(selectionChanged()), this, SLOT(cbLbSel1()));
	connect(listbox_[1], SIGNAL(selectionChanged()), this, SLOT(cbLbSel2()));
  	connect(listbox_[2], SIGNAL(selectionChanged()), this, SLOT(cbLbSel3()));
	connect(listbox_[3], SIGNAL(selectionChanged()), this, SLOT(cbLbSel4()));

	lock_ = 0;
	updateDialog();
}

qDataReportTool::~qDataReportTool()
{
}

/* Routine to set dialog's fields properly, including listbox contents.
 */
extern "C" {
	extern struct report_struct *get_var_list( int type, char **(*get_proc)() );
}

void qDataReportTool::updateDialog( void ) {
	int i;
	struct report_struct *prs;

	if (lock_)
		return;
	lock_ = 1;


	/* Create the report structure for variables if we haven't already.
	 */
    if(!info_->choices_list[curPage_].variables &&
         info_->choices_list[curPage_].get_variables)
         info_->choices_list[curPage_].variables =
            get_var_list(info_->choices_list[curPage_].type,
                         info_->choices_list[curPage_].get_variables);
	

	/* Decide how many listboxes will be active, and fill them appropriately.
	 */
	if (info_->choices_list[curPage_].get_index2 != NULL) {
		/* All four listboxes are active.
		 */
		for (i = 0; i < 4; i++)
			listbox_[i]->setEnabled(true);

		setReportStruct( info_->choices_list[curPage_].variables, 
							listbox_[3], stLabel_[3] );
		
		prs = (*info_->choices_list[curPage_].get_locations)();
		setReportStruct( prs, listbox_[0], stLabel_[0] );

		prs = (*info_->choices_list[curPage_].get_index)();
		setReportStruct( prs, listbox_[1], stLabel_[1] );

		prs = (*info_->choices_list[curPage_].get_index2)();
		setReportStruct( prs, listbox_[2], stLabel_[2] );

		//adjustSize( listbox_[3] ); 
		adjustSize( 3 ); 
	}
	else if (info_->choices_list[curPage_].get_index != NULL) {
		/* The first three listboxes are active.
		 */
		for (i = 0; i < 3; i++)
			listbox_[i]->setEnabled(true);

		setReportStruct( info_->choices_list[curPage_].variables, 
							listbox_[2], stLabel_[2] );
		
		prs = (*info_->choices_list[curPage_].get_locations)();
		setReportStruct( prs, listbox_[0], stLabel_[0] );

		prs = (*info_->choices_list[curPage_].get_index)();
		setReportStruct( prs, listbox_[1], stLabel_[1] );

		//adjustSize( listbox_[2] );
		adjustSize( 2);

		listbox_[3]->setShown( false );
		stLabel_[3]->setText( "" );
		listbox_[3]->clear();
	}
	else if (info_->choices_list[curPage_].get_locations) {
		/* The first two listboxes are active.
		 */
		for (i = 0; i < 2; i++)
			listbox_[i]->setEnabled(true);

		setReportStruct( info_->choices_list[curPage_].variables, 
							listbox_[1], stLabel_[1] );
		
		prs = (*info_->choices_list[curPage_].get_locations)();
		setReportStruct( prs, listbox_[0], stLabel_[0] );

		//adjustSize( listbox_[1] );
		adjustSize( 1 );

        for (i = 2; i < 4; i++) {
		    listbox_[i]->setShown( false );
		    stLabel_[i]->setText( "" );
		    listbox_[i]->clear();
	    }
    }
	else {
		/* Only one listbox is active.
		 */
		listbox_[0]->setEnabled(true);

		setReportStruct( info_->choices_list[curPage_].variables, 
							listbox_[0], stLabel_[0] );

		//adjustSize( listbox_[0] );
		adjustSize( 0 );

        for (i = 1; i < 4; i++) {
		    listbox_[i]->setShown( false );
		    stLabel_[i]->setText( "" );
		    listbox_[i]->clear();
	    }
	}

	lock_ = 0;
}

/* Routines to manage the text in the listboxes, and to adjust the size
 *  of the entire dialog to fit the listboxes currently active.
 */
//void qDataReportTool::adjustSize( QListBox *plb ) {
void qDataReportTool::adjustSize( int size ) {
	/* Adjust size of dialog to just show all of the given listbox or 
	 *  all of cmbLength_, whichever is right-most.
	 */

	/*
	QRect rLb, rCb;
	int  rightmost, newwidth;

	rLb = plb-> geometry ();
	rCb = cmbLength_->geometry();
	if (rLb.right() > rCb.right() )
		rightmost = rLb.right();
	else
		rightmost = rCb.right();
	
	newwidth = rightmost + 20 ;
	resize( newwidth, rect().height() );
	*/
	for(int i = 0; i < 4; i++){
		if(i <= size){
			listbox_[i]->setShown(TRUE);
			stLabel_[i]->setShown(TRUE);
		}
		else{
			stLabel_[i]->setShown(FALSE);
			listbox_[i]->setShown(FALSE);
		}
	}
	resize(0,0);

	


}

void qDataReportTool::setReportStruct( struct report_struct *prs, 
										QListBox *plb, QLabel *pst ) {
	int i, j;
	//QValueVector<int> selList;
    QString dest;

    if (rememberSelections_ && (prs != NULL)) {
        // current and incoming selections are combined.
        //plb->getSelection( &selList );
        for (i = 0; (unsigned)i < plb->count(); i++)
            if (plb->isSelected( i ) ) {
			//if (selList.find( &i ) != NULL) {
                //plb->getItem( dest, i );
				dest = plb->text(i);
                for (j = 0; j < prs->number; j++)
                    if (!strcmp( dest.ascii(), prs->items[j] )) {
                        prs->set[j] = 1;
                        break;
                    }
            }
        //selList.clearAndDestroy();
    }

	/* "prs" is a pointer to a report_struct which contains an array of
	 *  pointers to names and an array of flags to show which are selected.
	 */
    plb->clear();
    pst->setText( "" );
	if (prs) {
		pst->setText( prs->title ? prs->title : "" );
		for (i = 0; i < prs->number; i++)
			plb->insertItem( prs->items[i] );
		for (i = 0; i < prs->number; i++)
			if (prs->set[i])
				plb->setSelected ( i, true );  //set this item to selected
	}

	plb->show();
}

void qDataReportTool::getSelections( struct report_struct *prs, 
									  QListBox *plb ) {
	
	int i;

	/* get current selections from listbox, copy them to "set" array of
	 *  report structure.
	 */
	for (i = 0; (unsigned)i < plb->count(); i++) {
		if(plb->isSelected( i )){
			prs->set[i] = TRUE;
		}
		else
			prs->set[i] = FALSE;
	}
	
}

	
/* Notification functions for controls in dialog.
 */
void qDataReportTool::cbDone( ) {
	close();
	return ; // event handled.
}

void qDataReportTool::cbHelp( ) {
	helpManager->showHelp( QString("%1.html").arg(getHelpPage()) );
	return ; // event handled.
}

extern "C" {
extern char * write_current_report_file(struct report_info *info, 
										int current_choice);
}

void qDataReportTool::cbView(  ) {
	char *fname;
	qTextOutDialog *tld;

	if (lock_)
		return ;  // event handled by ignoring.
	lock_ = 1;

	/* Hijack the legacy code to create a report on the currently selected
	 *  data. Then display that report in a qTextOutDialog. The legacy
	 *  code writes to a text file, which is suboptimal, but would take a
	 *  lot of work to correct.
	 */
	getLineLength();
	fname = write_current_report_file( info_, curPage_ );
	if (fname == NULL)
		return ;

	tld = new qTextOutDialog( 0  ); //make it it's own dialog
	tld->addFile( fname );
	unlink( fname );
	free( fname );

	lock_ = 0;

	return ; // event handled.
}

void qDataReportTool::cbViewAll(  ) {
	char *fname;
	qTextOutDialog *tld;

	if (lock_)
		return ;  // event handled by ignoring.
	lock_ = 1;

	/* Hijack the legacy code to create a report on the currently selected
	 *  data. Then display that report in a qTextOutDialog. The legacy
	 *  code writes to a text file, which is suboptimal, but would take a
	 *  lot of work to correct.
	 */
	getLineLength();
	fname = write_all_report_file( info_ );
	if (fname == NULL)
		return ;

	tld = new qTextOutDialog( 0 );
	tld->setFamily(QString("Courier") );
	tld->addFile( fname );
	unlink( fname );
	free( fname );

	lock_ = 0;

	return ; // event handled.
}

void qDataReportTool::cbClear(  ) {
    int i;
    
    if (lock_)
		return ;  // event handled by ignoring.
	lock_ = 1;

	for (i = 0; i < 4; i++)
		listbox_[i]->clearSelection();

    clearSettingsType( curPage_ );

    lock_ = 0;

	return ; // event handled.
}

void qDataReportTool::clearSettings( struct report_struct *prs ) {
    int i;
    if(prs != NULL){
		for (i = 0; i < prs->number; i++)
			prs->set[i] = FALSE;
	}
}

void qDataReportTool::clearSettingsType( int page ) {
    int i;
	struct report_struct *prs;

	for (i = 0; i < 4; i++)
		listbox_[i]->clearSelection();

	prs = info_->choices_list[page].variables;
    if (prs != NULL)
        clearSettings( prs );
		
    if (info_->choices_list[page].get_locations != NULL) {
        prs = (*info_->choices_list[page].get_locations)();
        clearSettings( prs );
    }

    if (info_->choices_list[page].get_index != NULL) {
        prs = (*info_->choices_list[page].get_index)();
        clearSettings( prs );
    }

    if (info_->choices_list[page].get_index2 != NULL) {
        prs = (*info_->choices_list[page].get_index2)();
        clearSettings( prs );
    }
}

void qDataReportTool::cbClearAll( ) {
    int i;

	if (lock_)
		return ;  // event handled by ignoring.
	lock_ = 1;

	for (i = 0; i < 4; i++)
		listbox_[i]->clearSelection();

    for (i = 0; i < nCategory_; i++)
        clearSettingsType( i );

	lock_ = 0;

	return ; // event handled.
}

/* Helper routine to read the line length selection and put it in the global
 *  variable we use to communicate with the report writing code (KLUDGE).
 */
extern "C" int g_report_line_length; // pathological data connection
void qDataReportTool::getLineLength( void ) {
	switch (cmbLength_->currentItem()) {
	default:
	case 0:
		g_report_line_length = 80;
		break;
	case 1:
		g_report_line_length = 104;
		break;
	case 2:
		g_report_line_length = 140;
		break;
	case 3:
		g_report_line_length = 1024;
		break;
	}
}

void qDataReportTool::cbLbSel1() {
	struct report_struct *prs;

	if (lock_)
		return ;  // event handled by ignoring.
	lock_ = 1;

	if (info_->choices_list[curPage_].get_locations) {
		prs = (*info_->choices_list[curPage_].get_locations)();
		getSelections( prs, listbox_[0] );
	}
	else
		getSelections( info_->choices_list[curPage_].variables, listbox_[0] );

	lock_ = 0;

	return ; // event handled.
}

void qDataReportTool::cbLbSel2() {
	struct report_struct *prs;

	if (lock_)
		return ;  // event handled by ignoring.
	lock_ = 1;

	if (info_->choices_list[curPage_].get_index) {
		prs = (*info_->choices_list[curPage_].get_index)();
		getSelections( prs, listbox_[1] );
	}
	else if (info_->choices_list[curPage_].get_locations)
		getSelections( info_->choices_list[curPage_].variables, listbox_[1] );

	lock_ = 0;

	return ; // event handled.
}

void qDataReportTool::cbLbSel3() {
	struct report_struct *prs;

	if (lock_)
		return ;  // event handled by ignoring.
	lock_ = 1;

	if (info_->choices_list[curPage_].get_index2) {
		prs = (*info_->choices_list[curPage_].get_index2)();
		getSelections( prs, listbox_[2] );
	}
	else if (info_->choices_list[curPage_].get_index)
		getSelections( info_->choices_list[curPage_].variables, listbox_[2] );

	lock_ = 0;

	return ; // event handled.
}

void qDataReportTool::cbLbSel4() {
	if (lock_)
		return ;  // event handled by ignoring.
	lock_ = 1;

	if (info_->choices_list[curPage_].get_index2)
		getSelections( info_->choices_list[curPage_].variables, listbox_[3] );

	lock_ = 0;

	return  ; // event handled.
}

void qDataReportTool::cbCategory(int ind ) {
	int iSel;

	if (lock_)
		return ;  // event handled by ignoring.

	//iSel = cmbCategory_->selection();
	iSel = ind;
	if (iSel != curPage_ && iSel >= 0) {
		curPage_ = iSel;
		updateDialog();
	}

	return ; // event handled.
}


