/* qInputReportTool.h
 * This class defines a modal dialog which edits transport structures
 *  via comboboxes, scrollbars, and edit boxes. The dialog is based on 
 *  the IDD_TRANSPORTTOOL resource template.
 *
 * This is a special-purpose dialog which will probably not be subclassed.
 */

#ifndef __QDATAREPORTTOOL_H__
#define __QDATAREPORTTOOL_H__

//#include <zapp.hpp>
//#include <rw/cstring.h>
//#include <rw/tpdlist.h>
#include "crisp.h"
#include "report.h"
#include <qlistbox.h>
#include "InputReportToolBase.h"

class qDataReportTool : public InputReportToolBase {

	Q_OBJECT 

protected:

	int curPage_;  /* index into choices_list[] */

	QComboBox *cmbLength_, *cmbCategory_;

	QPushButton *bView_, *bViewAll_, *bClear_,
                         *bClearAll_, *bDone_, *bHelp_;

	QLabel *stLabel_[4];

	QListBox *listbox_[4];
	
	/* Hopefully this will prevent recursion during event processing.
	 */
	int lock_;

	/* Where to go to find a help-file page about this dialog.
	 */
	int helpPage_;

    /*
     * If "1", selections will be conserved (where possible) when report
     *  structures are changed. Default "0"
     */
    int rememberSelections_;

	/* Pointer to a "report_info" structure, which define what
	 *  choices will be put in the listboxes and presented to the user.
	 */
	struct report_info *info_;
    int nCategory_;  // property of *info_;
	
	/* Common code for all constructors.
	 */
	void consCommon( void );

	/* Routines to manage the text in the listboxes, and to adjust the size
	 *  of the entire dialog to fit the listboxes currently active.
	 */
	//void adjustSize( QListBox *plb );
	void adjustSize( int size );
	void setReportStruct( struct report_struct *prs, 
							QListBox *plb, QLabel *pst );
	void getSelections( struct report_struct *prs, QListBox *plb );
	void getLineLength( void );
    void clearSettings( struct report_struct *prs );
    void clearSettingsType( int page );

public:

	qDataReportTool( QWidget *w, struct report_info *info,
                                         int rememberSelections = 0 );
	~qDataReportTool();
	//virtual int command( zCommandEvt *Ev );
		/* Help file support.
	 */
	int getHelpPage( void ) { return helpPage_; }
	void setHelpPage( int i ) { helpPage_ = i; }

	/* Routine to set dialog's fields properly, including listbox contents.
	 */
	void updateDialog( void );

protected slots:

	/* Event handlers. */
	virtual void cbView();
	virtual void cbViewAll();
	virtual void cbClear();
	virtual void cbClearAll();
	virtual void cbDone();
	virtual void cbHelp();

	virtual	void cbLbSel1();
	virtual void cbLbSel2();
	virtual void cbLbSel3();
	virtual void cbLbSel4();
	virtual void cbCategory(int);

};

#endif

