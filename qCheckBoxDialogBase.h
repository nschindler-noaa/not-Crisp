// qCheckBoxDialogBase.h 
//
// HOW TO:
// Base class for dialog windows which have a list of checkboxes. Checkboxes are
//  organized by row and columns (up to 3) and have a list of row labels and
//  a list of column labels. If there are too many rows, tabs are created
//  to allow switching between them.
//
// QUICK OVERVIEW:
// Events from the pushbuttons pbCancel, pbApply, pbOK, pbHelp, and pbReset are 
// designed to communicate back to a mediator (qCheckBoxDialogMedBase) via 
// their respective implicit invocation objects evCancel, evApply, etc.  
// The evControlRegistration event can be used to register events with controls
// in the process of the drawDialog() function.  For example, if you build a dialog
// and you won't know how many checkboxes it should have until after it's been built, 
// you can pass that number to the drawDialog() function and it can then use the 
// evControlRegistration event to contact the mediator after you build each new control
// so that the mediator may register events with a specific control.  
// This allows you to design custom controls and maintains a separation between the 
// view of the data (qDialogBase) and the data itself, cached in a interfaceDataControl 
// class.  
#ifndef _qCheckBoxDialogBase_h_
#define _qCheckBoxDialogBase_h_


#include "qItem.h"
#include "qRowColumnBox.h"
#include "qImpTabCtrl.h"
#include <qmainwindow.h>
#include <qvaluevector.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qdialog.h>

class qCheckBoxDialogBase : public QFrame {
	
	Q_OBJECT

  protected:

	QPixmap image0; //dirty image
	// Defines the tab control which will be created if needed.
	qImpTabCtrl *tc_;
   
   
	QValueVector<QString> tabNames_; 

	unsigned numRowsPerTab_, numTabs_, currentIndex_;
	void appendTab(int tabIndex, const char *tabName);
	int buildTabControl();

	//Layout Managers
	QVBoxLayout * VertLayout;
	QHBoxLayout * horizLayout;  //hold horizontal placing of the standard buttons


	//Controls
	QPushButton *pbCancel_, *pbApply_, *pbOK_;
	QPushButton *pbHelp_, *pbReset_;
	qRowColumnBox *rc_, *rcDefControls_;     

	//numItems is the total number of Items in the tab, rowColumn obj or whatever.
	unsigned numRows_, numColumns_;

	void buildDefaultButtons();

	int wasCloseNotified_;  /* prevents multi-notification of evOK_ or evCancel_ */
	
	/* These pointers are used to show/hide the dirtyness indicators.
	 */
	QLabel *bpDirty1_, *bpDirty2_, *bpDirty3_, *bpDirty4_;

	// Called only by constructor.
	void consCommon( void );

  public:

    /* This routine shows/hides  the dirty indicators */
	virtual void showDirty( bool iShow = true );
    
    qCheckBoxDialogBase(QWidget *w, WFlags dw=0, const char *nt=NULL);
	virtual ~qCheckBoxDialogBase();

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

	//this grabs an iterator from the main row column widget so that you might traverse
	//through the control objects (cbgs, edit sliders, buttons, etc)
	virtual QPtrListIterator<qItem> *getItemIterator()
			{if(rc_)return rc_->getItemIterator();return NULL;}    

	/* Called if underlying window system kills this window. */
	virtual void closeEvent( QCloseEvent *ev );

  protected slots:
		//Standard Dialog Events
	virtual void cbApply();
	virtual void cbOK();
	virtual void cbReset();
	virtual void cbHelp();
	virtual void cbTabSelected(int newIndex,int oldIndex);
	virtual void cbRunStatusChanged( void );

  public slots:
	virtual void cbCancel();

  signals:

	virtual void evTabSelected(int);
	virtual void evCancel();
    virtual void evApply();
    virtual void evOK();
	virtual void evReset();
	virtual void evHelp();
	virtual void evControlRegistration(qItem *);


};

#endif


