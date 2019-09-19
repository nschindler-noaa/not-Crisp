// qDialogBase.h 
//
// HOW TO:
// Base class for most dialog windows.  Simply inherit from this class and
// override the virtual functions drawDialog(), and command().  
// The drawDialog function in your new class should build all the buttons, 
// floatsliders, check boxes and such that make up your dialog. 
// The command function comes from the zApp class zWindow and is called whenever
// a event is triggered on your dialog. See the zApp manual for more information 
// on this function.
//
// QUICK OVERVIEW:
// Events from the pushbuttons pbCancel, pbApply, pbOK, pbHelp, and pbReset are 
// designed to communicate back to a mediator (ZatDialogBaseMed) via their respective 
// implicit invocation objects evCancel, evApply, etc.  
// The evControlRegistration event can be used to register events with controls
// in the process of the drawDialog() function.  For example, if you build a dialog
// and you won't know how many sliders it should have until after it's been built, 
// you can pass that number to the drawDialog() function and it can then use the 
// evControlRegistration event to contact the mediator after you build each new control
// so that the mediator may register events with a specific control.  
// This allows you to design custom controls and maintains a separation between the 
// view of the data (ZatDialogBase) and the data itself, cached in a interfaceDataControl 
// class.  The mediator attaches static functions to the dialog via the 
// dlg_cancel_event() functions and the like.

#ifndef _qDialogBase_h_
#define _qDialogBase_h_

#include "qRowColumnBox.h"
#include "qCheckBoxGroup.h"
#include "qDialogBase.h"
#include <qmainwindow.h>
#include <qvaluevector.h>
#include <qstring.h>
#include <qpushbutton.h>
#include <qcombobox.h>
#include <qbuttongroup.h>
#include <qlabel.h>
#include "qEditSliderDouble.h"
#include "qEditSliderLong.h"
#include "qZabelSlider.h"
#include <qlayout.h>
#include <qdialog.h>
#include <qlabel.h>


class qDialogBase : public QMainWindow {

	Q_OBJECT

  protected:

	// tells the status of the ZatComboBoxGroups for each numItems_ 
	// This previously wasn't necessary, but with the advent of the TabDialogs..
	QValueVector <int> cbgTags_; 
	QValueVector <QString> cbgBoxName_;
	
	//List of sizers which are not attached to windows, and thus must
	//  be cleaned up explicitly.
	QVBoxLayout *outerVertLayout;
	QVBoxLayout *vertLayout;
	QHBoxLayout *horizLayout[2];


	//Controls
	QPushButton * pbCancel_;
	QPushButton * pbApply_;
	QPushButton * pbApplyAll_;
	QPushButton * pbOK_;
	QPushButton * pbHelp_, * pbReset_, * pbResetAll_;
	QComboBox *cmbStatic_[3];
	qRowColumnBox *rc_, *rcDefControls_;     


	//numItems is the total number of Items in the tab, rowColumn obj or whatever.
	unsigned numItems_, topMargin_;
	void buildDefaultButtons();

	/* Called only by constructors. */
	void consCommon( void );

	int wasCloseNotified_;  /* prevents multi-notification of evOK_ or evCancel_ */
	
	/* This pointer is used to show/hide the Delta checkbox.
	 */
	//ZatCheckBoxGroup *cbgDelta_;
	//QButtonGroup *cbgDelta_;
	qCheckBoxGroup *cbgDelta_;

	/* These pointers are used to show/hide the dirtyness indicators.
	 */
	QPixmap dirtyPix;
	QLabel *bpDirty1_, *bpDirty2_, *bpDirty3_, *bpDirty4_;
	QToolBar *tbar_bottom;


  public:

    /* These routines show/hide the Delta checkbox and the dirty indicator */
	virtual void showDelta( bool iShow = true );
	virtual void showDirty( bool iShow = true );

    //qDialogBase(QWidget *w );
    qDialogBase(QWidget *w, WFlags dw=WStyle_Dialog, const char *nt=NULL);
	virtual ~qDialogBase();



	QString getComboBoxText(int box);
	void setTopMargin(unsigned top){ topMargin_ = top; }
	virtual int drawDialog(unsigned topMargin);
	//Draws the dialog in it's standard layout..
	virtual void drawStandardLayout(int sliderType);

	virtual int buildRowColumnControl();

	//This is the total number of data items your dialog will maintain.. typically
	//this is set to Chronographer->nyears();
	unsigned numItems(){return numItems_;}	
	void setNumItems(unsigned ni){ numItems_ = ni; }
	//Tells the number of boxes in your check box groups, (ie "Y" "S" "F" would be 3)
	unsigned numBoxes(){return cbgBoxName_.count();}
	//For inserting boxes into your cbg(check box group) column
	virtual void addBoxName(const char *tag);
	//Since some derived classes, such as the TabDialogs, won't be maintaining
	//the status of the check box groups directly, we give you the
	//option to store it in the dialog
	virtual int isBoxChecked(unsigned item, unsigned box=0);
	virtual void setCheckBox(unsigned box, unsigned checked, unsigned id);
	//this selects an item in one of the static combo boxes, such as a fishery or stock
	int selectComboBox(int i=-1, unsigned box=0);
	//for adding text to a combo box, (ie "North T")
	void addComboBoxText(const char *text, unsigned box=0);
	void clearComboBox(unsigned box=0){ if(box<=1) cmbStatic_[box]->clear();}
	void addComboBox(unsigned box=0);
	void showComboBox( bool iShow, unsigned box=0 );

	//this grabs an iterator from the main row column widget so that you might traverse
	//through the control objects (cbgs, edit sliders, buttons, etc)
	virtual QPtrListIterator<qItem> *getItemIterator(){if(rc_)return rc_->getItemIterator();return NULL;}    

		/* Called if underlying window system kills this window. */
	virtual void closeEvent ( QCloseEvent * e );

	void callCancel(){ cbCancel(); }   //manually calls cancel from an outside class (qReachMedEquation)

  protected slots:
	//Standard Dialog Events
	virtual void cbCancel();
	virtual void cbApply();
	virtual void cbApplyAll();
	virtual void cbOK();
	virtual void cbReset();
	virtual void cbResetAll();
	virtual void cbHelp();
	virtual void cbCmdStatic1();
   	virtual void cbCmdStatic2();
    virtual void cbCmdStatic3();
	/* Find out if a scenario or monte carlo run is being started/stopped.
	 *  If we are running, disable "OK", "Apply", "Apply All", etc.
	 */
	virtual void cbRunStatusChanged();


signals:
	//Events
	void evCancel_(int);
	void evApply_();
	void evApplyAll_();
	void evOK_();
	void evReset_();
	void evResetAll_();
	void evHelp_();
	void evCmbStatic1_(int);
	void evCmbStatic2_(int);
	void evCmbStatic3_(int);
	void evControlRegistration_(qItem*);
  

};

#endif


