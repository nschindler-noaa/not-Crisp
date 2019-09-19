// qSelectDialogBase.h 
//
// HOW TO:
// Base class for dialog windows which has a list of values for each feature/
//  species combination. Values are
//  organized by row and columns (up to 3) and have a list of row labels and
//  a list of column labels. If there are too many rows, tabs are created
//  to allow switching between them. Two columns of checkboxes (for gang
//  edits) are also provided.
//
// QUICK OVERVIEW:
// Events from the pushbuttons pbCancel, pbApply, pbOK, pbHelp, and pbReset are 
// designed to communicate back to a mediator (qSelectDialogMedBase) via their 
// respective implicit invocation objects evCancel, evApply, etc.  
// The evControlRegistration event can be used to register events with controls
// in the process of the drawDialog() function.  For example, if you build a dialog
// and you won't know how many sliders it should have until after it's been built, 
// you can pass that number to the drawDialog() function and it can then use the 
// evControlRegistration event to contact the mediator after you build each new control
// so that the mediator may register events with a specific control.  
// This allows you to design custom controls and maintains a separation between the 
// view of the data (qDialogBase) and the data itself, cached in a interfaceDataControl 
// class.  
#ifndef _qSelectDialogBase_h_
#define _qSelectDialogBase_h_


#include "qItem.h"
#include "qRowColumnBox.h"
#include "qCheckBoxGroup.h"
#include "qImpTabCtrl.h"
#include "qmainwindow.h"
#include "qcombobox.h"
#include "qpushbutton.h"
#include "qvaluevector.h"
#include "qlabel.h"
#include <qdialog.h>

//typedef utImplicit1<const int> signalInt;
//typedef utImplicit1<qItem *> signalReg;

class qSelectDialogBase : public QDialog {

	Q_OBJECT

  protected:

	// Defines the tab control which will be created if needed.
	qImpTabCtrl *tc_;
	
    QValueVector<QString> tabNames_;  
	unsigned numRowsPerTab_, numTabs_, currentIndex_;
	void appendTab(int tabIndex, const char *tabName);
	int buildTabControl();

	//Layout Managers
	QVBoxLayout * VertLayout;
	QHBoxLayout * horizLayout;  //hold horizontal placing of the standard buttons

	// Keeps track of checkbox state on other tabs.
	QValueVector<int> cbgTags_;  

	// Needed only to create "ganged update" checkboxes.	
	QValueVector<QString> cbgBoxName_;  

	//Controls
	QPushButton *pbCancel_, *pbApply_, *pbApplyAll_, *pbOK_;
	QPushButton *pbHelp_, *pbReset_, *pbResetAll_;
	
	qRowColumnBox *rc_, *rcDefControls_;     
	QComboBox *cmbStatic_[2];
	
	//Dirty indicator pixmap 
	QPixmap image0;
	
	//Events
    
	//signalInt evCancel_, evApply_, evApplyAll_, evOK_;
	//signalInt evReset_, evResetAll_, evHelp_; 
	//signalInt evCmbStatic1_, evCmbStatic2_;
    //signalReg evControlRegistration_;

	//numItems is the total number of Items in the tab, rowColumn obj or whatever.
	unsigned numRows_, numColumns_;

	// numCbgCols_ is the number of columns of boxes in the checkboxgroup (1 or 2)
	int numCbgCols_;

    // featuresAreDams_ helps us tell how to label the check box groups.
    int featuresAreDams_;

	void buildDefaultButtons();

	int wasCloseNotified_;  /* prevents multi-notification of evOK_ or evCancel_ */
	
	/* This pointer is used to show/hide the Delta checkbox.
	 */
	qCheckBoxGroup *cbgDelta_;

	/* These pointers are used to show/hide the dirtyness indicators.
	 */
	
	//zBitmapPane *bpDirty1_, *bpDirty2_, *bpDirty3_, *bpDirty4_;

	QLabel *bpDirty1_, *bpDirty2_, *bpDirty3_, *bpDirty4_;

	// Called only by constructor.
	void consCommon( void );

	virtual void paintEvent ( QPaintEvent * se);

  public:

    /* These routines show/hide the Delta checkbox and the dirty indicator */
	virtual void showDelta( bool iShow = TRUE );
	virtual void showDirty( bool iShow = TRUE );

    qSelectDialogBase(QWidget *w, int numCbgCols = 2,
                                     int featuresAreDams = 1 );
	virtual ~qSelectDialogBase();

	/* Find out if a scenario or monte carlo run is being started/stopped.
	 *  If we are running, disable "OK", "Apply", "Apply All", etc.
	 */

//	static void _cbRunStatusChanged( void *msg );

	virtual int drawDialog(unsigned topMargin);
	//Draws the dialog in it's standard layout..
	virtual void drawStandardLayout();
	virtual int buildRowColumnControl();

	

	//for adding text to a combo box, (ie "North T")
	void addComboBoxText(const char *text, unsigned box=0);
	QString getComboBoxText(int box);
	void selectComboBox( int iSel, int box );
	void hideComboBox( unsigned box );

	// how many columns of check box?
	int numCbgCols( void ) { return numCbgCols_; };

	//This is the total number of data items your dialog will maintain.
	unsigned numItems(){return numRows_ * numColumns_;}	
	void setNumRows( unsigned nr ) { numRows_ = nr; }
	void setNumColumns( unsigned nc ) { numColumns_ = nc; }
	void setNumRowsPerTab( unsigned ni ) { numRowsPerTab_ = ni; }
	unsigned numRows() { return numRows_; }
	unsigned numColumns() { return numColumns_; }
	unsigned numRowsPerTab() { return numRowsPerTab_; }
	int currentIndex() { return currentIndex_; }

	//Since some derived classes won't be maintaining
	//the status of the check box groups directly, we give you the
	//option to store it in the dialog
	virtual int isBoxChecked(unsigned box, unsigned id);
	virtual void setCheckBox(unsigned box, unsigned checked, unsigned id);

	//this grabs an iterator from the main row column widget so that you might traverse
	//through the control objects (cbgs, edit sliders, buttons, etc)
	virtual QPtrListIterator<qItem> *getItemIterator()
		{ if(rc_)return rc_->getItemIterator(); return NULL;}    

	/* Called if underlying window system kills this window. */
	virtual void closeEvent( QCloseEvent *ev );

  public slots:
	//Standard Dialog Events
	virtual void cbCancel();
	virtual void cbApply();
	virtual void cbApplyAll();
	virtual void cbOK();
	virtual void cbReset();
	virtual void cbResetAll();
	virtual void cbHelp();
	virtual void cbCmbStatic2();
	virtual void cbCmbStatic1();
	virtual void cbTabSelected(int newIndex, int oldIndex);
	virtual void cbRunStatusChanged( void );

signals:
	void evTabSelected(int);	
	void evCancel(int);
    void evApply(int);
    void evApplyAll(int);
    void evOK(int);
	void evReset(int);
	void evResetAll(int);
	void evHelp(int);
	void evControlRegistration(qItem *);
    void evCmbStatic1(int);
    void evCmbStatic2(int);

	

//	static void _cbTabSelected(void *vp, const int newIndex, 
//								const int oldIndex);



};

#endif


