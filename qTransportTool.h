/* qTransportTool.h
 * This class defines a modal dialog which edits transport structures
 *  via comboboxes, scrollbars, and edit boxes. The dialog is based on 
 *  the IDD_TRANSPORTTOOL resource template.
 *
 * This is a special-purpose dialog which will probably not be subclassed.
 */

#ifndef _QTRANSPORTTOOL_H_
#define _QTRANSPORTTOOL_H_

#include "crisp.h"
#include "TransportBase.h"
#include "qFocusEventFilter.h"


struct translist {
	struct transport *transport;  // the structure.
	int dirty;		// is this edited w.r.t. underlying list? (0 = no).
};

class qTransportTool : public TransportBase {

	Q_OBJECT

protected:
	
	//zGroupBox *gb1_, *gb2_, *gb3_;

	struct translist *current_;
	QPtrList<struct translist> localList_;

	/* Hopefully will prevent recursion during shutdown.
	 */
	int isRunning_;

	/* Hopefully this will prevent recursion during event processing.
	 */
	int lock_;
	
	/* Common code for all constructors.
	 */
	void consCommon( void );

	/* Routines to manage the local list of release structures.
	 */
	void DestroyLocalList( void );
	void CreateLocalList( void );
	void ApplyLocalList( struct translist *applyMe = NULL );
	void ResetLocalList( struct translist *resetMe = NULL );

	/* Routine to get the reach segment referred to by the reach combobox.
	 */
	struct river_segment *getReach( void );

	/* Helper routines for handling edit control and scrollbar events.
	 */
	int editChanged( QLineEdit *ed, QScrollBar *scr, int doDate );
	int scrollChanged( QLineEdit *ed, QScrollBar *scr, int doDate );

public:

	qTransportTool( QWidget *w );
	~qTransportTool();
	//virtual int command( zCommandEvt *Ev );
	void AttachEvents( void );
	void DetachEvents( void );

	/* Make the dialog reflect the current list element being edited.
	 */
	void updateDialog( void );

	 /* Helper routine for month/day field */
    void setDateToField( QLineEdit *pzel );
	/* Show or hide "dirty" annunciators.
	 */
	void showDirty( int dirty );


public slots:
	/* Find out if a scenario or monte carlo run is being started/stopped.
	 *  If we are running, disable "OK", "Apply", "Apply All", etc.
	 */
	virtual void cbRunStatusChanged( );
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
	virtual void cbFractions();
	virtual void cbRbOnce();
	virtual void cbRbNever();
	virtual void cbRbAsNec();
	virtual void cbComboDamChange();
	virtual void cbComboReachChange();
	virtual void cbComboExistingChange();
	virtual void cbComboStartChange();
	virtual void cbComboRestartChange();
	virtual void cbComboSpeciesChange();


	/* The edit boxes and scrollers all need seperate notifications so that
	 *  we can show the date for the one that is changing (only).
	 */
	virtual void cbStartTransKillFocus();
	virtual void cbStartTransSetFocus();
	virtual void cbStartTransScrollerChanged( int pos );
  
	virtual void cbRestartTransKillFocus();
	virtual void cbRestartTransSetFocus();
	virtual void cbRestartTransScrollerChanged( int pos );
    virtual void cbStopDayKillFocus();
	virtual void cbStopDaySetFocus();
	virtual void cbStopDayScrollerChanged( int pos );
    
	virtual void cbStopCountKillFocus();
	virtual void cbStopCountScrollerChanged( int pos );
    
	virtual void cbStopNDayKillFocus();
	virtual void cbStopNDayScrollerChanged( int pos );
   
	virtual void cbSepFlowKillFocus();
	virtual void cbSepFlowScrollerChanged( int pos );
   
	virtual void cbTermCountKillFocus();
	virtual void cbTermCountScrollerChanged( int pos );
    
	virtual void cbTransSpeedKillFocus();
	virtual void cbTransSpeedScrollerChanged( int pos );
   
   
	/* Purge all handler (called when database has changed out from under us.
	 */

	virtual void cbPurgeAll();
	////static void _cbPurgeAll(void *msg);
};


#endif

