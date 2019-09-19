/* qReleaseTool.h
 * This class defines a modal dialog which edits release structures
 *  via comboboxes, scrollbars, and edit boxes. An editable graph is also
 *  used. The dialog is based on the IDD_RELEASETOOL resource template.
 *
 * This is a special-purpose dialog which will probably not be subclassed.
 */

#ifndef __QRELEASETOOL_H__
#define __QRELEASETOOL_H__


#include "crisp.h"
#include "qEditGraphDataToken.h"
#include "qGraphDialog.h"
#include "ReleaseToolBase.h"

class qReleaseEGDT;
class qReleaseEGM;

class qReleaseTool : public ReleaseToolBase {

	Q_OBJECT 

protected:
	qEditGraphDataToken *edt_;


	struct release *localList_, *current_;
	
	qReleaseEGDT *egdt_;
	qReleaseEGM *egm_;
	qGraphDialog *gd_;

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
	void ApplyLocalList( struct release *applyMe = NULL );
	void ResetLocalList( struct release *resetMe = NULL );

	void CreateRlsSegList( struct release *rel );

	/* These help us remember what we were editing so that we can try to get
	 *  back to it.
	 */
	static QString csSiteSave_, csSpeciesSave_, csStockSave_;
	void saveLocation( void );

public:

	qReleaseTool( QWidget *w, const char *feature );
	qReleaseTool( QWidget *w, int iExRel );
	qReleaseTool( QWidget *w );
	~qReleaseTool();
	void AttachEvents( void );
	void DetachEvents( void );
	void setShown(bool show);

	/* Routines to allow the menu system to select a release or release site
	 *  while the dialog is open.
	 */
	void selectRelease( const char *feature );
	void selectRelease( int iExRel );


	/* Routine to format the name string for a release. (Static so that it
	 *  can be called without a "this" pointer.
	 */
	static void formatName( struct release *rel, QString *pstr );

	/* Local list element currently being edited (available for others to
	 *  inspect). Result may be NULL.
	 */
	struct release *currentStruct( void ) { return current_; };

	/* Make the dialog reflect the current list element being edited.
	 */
	void updateDialog( void );

	/* Helper routine */
    void setDateToField( QLineEdit *pzel );
	
	/* Find out if a scenario or monte carlo run is being started/stopped.
	 *  If we are running, disable "OK", "Apply", "Apply All", etc.
	 */
	void emitInvalidate() { emit evInvalidate(); }
public slots:
	virtual void cbRunStatusChanged( void );
	//static void _cbRunStatusChanged( void *msg );
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
	virtual void cbComboSelChange();
	virtual void cbComboSelExisting();

	/* The edit boxes and scrollers all need seperate notifications so that
	 *  we can show the date for the one that is changing (only).
	 */
	virtual void SStartEditChanged( void );
	virtual void SStopEditChanged( void );
	virtual void RStartEditChanged( void );
	virtual void LengthEditChanged( void );

	//static void _cbSStartReturn( zEvH *obj );
	virtual void cbSStartKillFocus();
	virtual void cbSStartSetFocus();
	virtual void cbSStartScrollerChanged( int pos );

    //static void _cbSStopReturn( zEvH *obj );
	virtual void cbSStopKillFocus();
	virtual void cbSStopSetFocus();
	virtual void cbSStopScrollerChanged( int pos );

    //static void _cbRStartReturn( zEvH *obj );
	virtual void cbRStartKillFocus();
	virtual void cbRStartSetFocus();
	virtual void cbRStartScrollerChanged( int pos );

    //static void _cbLengthReturn( zEvH *obj );
	virtual void cbLengthKillFocus();
	virtual void cbLengthSetFocus();
	virtual void cbLengthScrollerChanged( int pos );

   
	/* Invalidation handler.
	 */
	virtual void cbInvalidate();


	/* Purge all handler (called when database has changed out from under us.
	 */
	virtual void cbPurgeAll();

	/* Graph object update handler (live updating via mouse).
	 */
	virtual void cbGraphUpdate( char *index, int type);

	/* Tells anyone interested, that we are dying or that the underlying data
	 *  has changed.
	 */
signals:
	void evRemove();
	void evInvalidate(); 
    
};

class focusLostEventFilter : public QObject {

	Q_OBJECT
public:
	focusLostEventFilter( QObject * w ) : QObject(w){}

	protected:
        bool eventFilter( QObject *o, QEvent *e ){
			if(e->type() == QEvent::FocusIn)
				emit focusGained();	
						
			return FALSE;  //pass on event to object

		}

signals:
		void focusGained();		

};





#endif

