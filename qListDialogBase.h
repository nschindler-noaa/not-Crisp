// ZatListDialogBase.h 
//
// HOW TO:
// Base class for dialog windows which have a list of values. Values are
//  organized by row and columns (up to 3) and have a list of row labels and
//  a list of column labels. If there are too many rows, tabs are created
//  to allow switching between them. A single column of checkboxes (for gang
//  edits) is also provided.
//
// QUICK OVERVIEW:
// Events from the pushbuttons pbCancel, pbApply, pbOK, pbHelp, and pbReset are 
// designed to communicate back to a mediator (ZatListDialogMedBase) via their 
// respective implicit invocation objects evCancel, evApply, etc.  
// The evControlRegistration event can be used to register events with controls
// in the process of the drawDialog() function.  For example, if you build a dialog
// and you won't know how many sliders it should have until after it's been built, 
// you can pass that number to the drawDialog() function and it can then use the 
// evControlRegistration event to contact the mediator after you build each new control
// so that the mediator may register events with a specific control.  
// This allows you to design custom controls and maintains a separation between the 
// view of the data (ZatDialogBase) and the data itself, cached in a interfaceDataControl 
// class.  
#ifndef _qListDialogBase_h_
#define _qListDialogBase_h_

//#include <rw/tvvector.h>
//#include <rw/tvordvec.h>
//#include <rw/cstring.h>
//#include "implicit.h"
#include "qItem.h"
#include "qRowColumnBox.h"
#include "qCheckBoxGroup.h"
#include "qImpTabCtrl.h"
#include <qmainwindow.h>
#include <qdialog.h>
#include <qvaluevector.h>
#include <qstring.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qpixmap.h>

//typedef utImplicit1<const int> signalInt;
//typedef utImplicit1<ZatItem *> signalReg;

class qListDialogBase : public QDialog {

	Q_OBJECT

  protected:

	// List of sizers which are not attached to windows, and thus must
	//  be cleaned up explicitly.
	//RWTPtrDlist<zSizer> sizers_;
	
	// Defines the tab control which will be created if needed.
	qImpTabCtrl *tc_;
    //signalInt evTabSelected_;
    //RWTValOrderedVector<RWCString> tabNames_;  
	QValueVector<QString>  tabNames_;  
	unsigned numRowsPerTab_, numTabs_, currentIndex_;
	void appendTab(int tabIndex, const char *tabName);
	int buildTabControl();

	//Layout Managers
	QVBoxLayout * VertLayout;
	QHBoxLayout * horizLayout;  //hold horizontal placing of the standard buttons

	// Keeps track of checkbox state on other tabs.
	//RWTValVector<int> cbgTags_;  
	QValueVector<int> cbgTags_;  

	// Needed only to create "ganged update" checkbox.	
	//RWTValOrderedVector<RWCString> cbgBoxName_;  
	QValueVector<QString> cbgBoxName_;  

	//Controls
	QPushButton *pbCancel_, *pbApply_, *pbOK_;
	QPushButton *pbHelp_, *pbReset_;
	//zSizer *cbgContainer_;
	qRowColumnBox *rc_, *rcDefControls_;     

    /*signalInt evCancel_, evApply_, evOK_;
	signalInt evReset_, evHelp_; 
    signalReg evControlRegistration_;*/

	//numItems is the total number of Items in the tab, rowColumn obj or whatever.
	unsigned numRows_, numColumns_;

	void buildDefaultButtons();

	int wasCloseNotified_;  /* prevents multi-notification of evOK_ or evCancel_ */
	
	/* This pointer is used to show/hide the Delta checkbox.
	 */
	qCheckBoxGroup *cbgDelta_;

	/* These pointers are used to show/hide the dirtyness indicators.
	 */
	//zBitmapPane *bpDirty1_, *bpDirty2_, *bpDirty3_, *bpDirty4_;
	QLabel *bpDirty1_, *bpDirty2_, *bpDirty3_, *bpDirty4_;
	QPixmap image0;
	// Called only by constructor.
	void consCommon( void );
	//a paint event, needed to override to paint all objects.
	virtual void paintEvent ( QPaintEvent * );

  public:

    /* These routines show/hide the Delta checkbox and the dirty indicator */
	virtual void showDelta( bool iShow = true );
	virtual void showDirty( bool iShow = true );

    //qListDialogBase(QWidget *w);
    qListDialogBase(QWidget *w, WFlags dw=0, const char *nt=NULL);
	virtual ~qListDialogBase();

	/* Find out if a scenario or monte carlo run is being started/stopped.
	 *  If we are running, disable "OK", "Apply", "Apply All", etc.
	 */
  


	virtual int drawDialog(unsigned topMargin);
	//Draws the dialog in it's standard layout..
	virtual void drawStandardLayout();
	virtual int buildRowColumnControl();

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
	virtual int isBoxChecked(unsigned box=0);
	virtual void setCheckBox(unsigned checked, unsigned id);

	//this grabs an iterator from the main row column widget so that you might traverse
	//through the control objects (cbgs, edit sliders, buttons, etc)
	virtual QPtrListIterator<qItem> *getItemIterator()
			{if(rc_)return rc_->getItemIterator();return NULL;}    
/*
	//Standard Dialog Events
	virtual int cbCancel(zEvent *);
	virtual int cbApply(zEvent *);
	virtual int cbOK(zEvent *);
	virtual int cbReset(zEvent *);
	virtual int cbHelp(zEvent *);
	virtual int command(zCommandEvt *Ev);
	signalInt *evCancel(){ return &evCancel_; }
    signalInt *evApply(){ return &evApply_; }
    signalInt *evOK(){ return &evOK_; }
	signalInt *evReset(){ return &evReset_; }
	signalInt *evHelp(){ return &evHelp_; }
	signalReg *evControlRegistration(){ return &evControlRegistration_; }
*/
	/* Called if underlying window system kills this window. */
	//virtual int kill( zEvent *ev );
	virtual void closeEvent ( QCloseEvent * e );

	//static void _cbTabSelected(void *vp, const int newIndex, 
	//							const int oldIndex);
	//virtual void cbTabSelected(const int newIndex, const int oldIndex);
	//signalInt *evTabSelected(){ return &evTabSelected_; }

  protected slots:

	virtual void cbRunStatusChanged( void );
	//static void _cbRunStatusChanged( void *msg );
  public slots:
	  	//Standard Dialog Events
	virtual void cbCancel();
	virtual void cbApply();
	virtual void cbOK();
	virtual void cbReset();
	virtual void cbHelp();
	//virtual void command();
	virtual void cbTabSelected(int newIndex, int oldIndex);

	//Events
  signals:
	void evCancel_();
	void evApply_();
	void evOK_();
	void evReset_();
	void evHelp_();
	void evControlRegistration_(qItem *);
	void evTabSelected_(int newIndex);


};

#endif



