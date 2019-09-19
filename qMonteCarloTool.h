/* qMonteCarloTool.h
 * This class defines a modal dialog which edits transport structures
 *  via comboboxes, scrollbars, and edit boxes. The dialog is based on 
 *  the a QTDesigner created ui file, MonteCarloBase
 *
 * This is a special-purpose dialog which will probably not be subclassed.
 */

#ifndef __qMONTECARLOTOOL_H__
#define __qMONTECARLOTOOL_H__

#include "crisp.h"
#include "MonteCarloBase.h"
#include <qthread.h>
#include "monte.h"
#include "run_mode.h"
#include "Qmain.h"

//Separate Thread Class fo running montecarlo
class monteThread : public QThread{

public:
	virtual void run();
};



class qMonteCarloTool : public MonteCarloBase {
	Q_OBJECT 

protected:

	QCrisp *qcrisp; /* Pointer to QCrisp for lock output settings */

	/* index of alternative currently being edited.
	 */
	int current_;

	/* Button controls.
	 */
	QPushButton *bCreate_, *bDelete_, *bRename_;

	QPushButton *bOK_, *bCancel_, *bApply_, *bReset_,
						*bApplyAll_, *bResetAll_, *bHelp_;

	QPushButton *bSetNGame_, *bSetAllNGame_, *bGetDir_, *bGetFlowFile_,
				*bRunMonte_, *bSetFile_, *bSetAllFile_, *bGetYearFile_,
				*bGoToDir_, *bUseFlowFile_, *bUseNFiles_, *bCancelMonte_;

	QRadioButton *rbFlowFromArch_, *rbFlowFromUser_, *rbSpillFromArch_;
	QRadioButton *rbSpillFromUser_, *rbElevFromArch_, *rbElevFromUser_;
	QLabel *stSpillFrom_, *stElevFrom_;
	QCheckBox *ckSupplemental_, *ckLockOutputSettings_, *ckDisableVarianceSuppression_;

	/* Edit controls, comboboxes, and listboxes (and labels thereof).
	 */
	QLineEdit *edYear_, *edFilename_;
	QLineEdit *edCurDir_, *edFlowArch_, *edNGames_, *edNFiles_;
	QLabel *stCurDir_, *stFlowArch_, *stYear_;
	QListBox *lbAlterns_;
	QListBox *lbYears_;
	QListBox *lbFiles_;
	QComboBox *cbAltToEdit_;
	
	/* Progress and status indicators.
	 */
	QLabel *stNAltGames_, *stNAltYears_, *stNAlts_;
	QLabel *progNAltGames_, *progNAltYears_, *progNAlts_;

	/* These pointers are used to show/hide the dirtyness indicators.
	 */
	QLabel *bpDirty1_, *bpDirty2_, *bpDirty3_, *bpDirty4_;
	QLabel *bpDirty5_, *bpDirty6_, *bpDirty7_, *bpDirty8_;

	/* Groupbox for "alternative being edited" and "run control".
	 */
	//QGroupBox *gbCurrent_, *gbRun_;
	
	/* Common code for all constructors.
	 */
	void consCommon( void );

	/* Helper routines.
	 */
	void disableFlowArchGroup( void );
	void trimLoadDirName( QString &str );

public:

	qMonteCarloTool( QCrisp *w );
	~qMonteCarloTool();
	//virtual int command( zCommandEvt *Ev );
	void AttachEvents( void );
	//void DetachEvents( void );

	/* Cleans up data structures and re-enables controls after running
	 *  a monte carlo simulation.
	 */
	void MonteRunCleanup( void );

	/* Make the dialog reflect the current list element being edited.
	 */
	void updateAlternative( void ); /* the current alternative only */
	void updateStatus( void );  /* the status indicators only. */
	void clearProgress( void ); /* clears progress bars to 0% */
	void textProgress( char *pch, int prog );

public slots:
	void updateDialog();  /* The whole dialog */

	void setStatusProgress(int game, int gameprog,int year, int yearprog, int alt, int altprog);

protected slots:
	/* Event handlers. */
	virtual void cbOK();
	virtual void cbCancel();
	virtual void cbApply();
	virtual void cbReset();
	virtual void cbApplyAll();
	virtual void cbResetAll();
	virtual void cbHelp();
	virtual void cbDelete();
	virtual void cbCreate();
	virtual void cbRename();
	virtual void cbGoToDir();
	virtual void cbCurDir();
	virtual void cbFlowArch();
	virtual void cbFlowFromArch();
	virtual void cbFlowFromUser();
	virtual void cbSpillFromArch();
	virtual void cbSpillFromUser();
	virtual void cbElevFromArch();
	virtual void cbElevFromUser();
	virtual void cbSetNGame();
	virtual void cbNGames();
	virtual void cbSetAllNGame();
	virtual void cbGetDir();
	virtual void cbGetFlowFile();
	virtual void cbUseFlowFile();
	virtual void cbRunMonte();
	virtual void cbCancelMonte();
	virtual void cbNFiles();
	virtual void cbUseNFiles();
	virtual void cbSetFile();
	virtual void cbSetAllFile();
	virtual void cbGetYearFile();
	virtual void cbListBoxAlterns();
	virtual void cbListBoxYears();
	virtual void cbListBoxFiles();
	virtual void cbComboBoxAlterns();
    virtual void cbLockOutputSettings();
	virtual void cbRunStatusChanged( void );
	/* Purge all handler (called when database has changed out from under us.
	 */
	virtual void cbPurgeAll();

public:
    /* External entry point to set the "Lock current output settings" checkbox
     */
    void setLockOutputSettings( int i );

	
	//static void _cbPurgeAll(void *msg);

	/* Show or hide "dirty" annunciators. The "outside" set of dirty
	 *  indicators show that some alternative is edited; the "inside" set
	 *  show that the current alternative is edited.
	 */
	void showSomeDirty( bool dirty );
	void showCurrDirty( bool dirty );


};



#endif



