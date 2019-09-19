// qScheduleDialogBase.h 
//
// HOW TO:
// Base class for dialog windows which come from the IDD_SCHEDULE dialog
//  template. This template has two combo boxes, two list boxes, four
//  edit fields, a checkbox, and static label text for all of those, as
//  well as the standard buttons.
//
// QUICK OVERVIEW:
// Events from the pushbuttons pbCancel, pbApply, pbOK, pbHelp, and pbReset are 
// designed to communicate back to a mediator via their 
// respective implicit invocation objects evCancel, evApply, etc.  
// This allows you to design custom controls and maintains a separation between the 
// view of the data and the data itself, cached in a interfaceDataControl 
// class.  
#ifndef _qScheduleDialogBase_h_
#define _qScheduleDialogBase_h_

//#include <rw/tvvector.h>
//#include <rw/tvordvec.h>
//#include <rw/cstring.h>
//#include "implicit.h"
#include "ScheduleBase.h"

//typedef utImplicit1<const int> signalInt;

class qScheduleDialogBase : public ScheduleBase {

	Q_OBJECT

  protected:

	//Controls
	//zPushButton *pbCancel_, *pbApply_, *pbApplyAll_, *pbOK_;
	//zPushButton *pbHelp_, *pbReset_, *pbResetAll_;
	QComboBox *cmbStatic_[2];
	QPushButton* pbLeftArrow;
	
	QLineEdit *editLine_[4];
	QListBox *listBox_[2];
	QLabel *listLabel_[2];
	QLabel *editLabel_[4];	

	int wasCloseNotified_;  /* prevents multi-notification of evOK_ or evCancel_ */
	
	// Called only by constructor.
	void consCommon( void );

  public:

    qScheduleDialogBase(QWidget *w );
	virtual ~qScheduleDialogBase();

    /* This routine shows/hides the dirty indicator */
	virtual void showDirty( bool iShow = true );

	//for handling the combo boxes.
	void addComboBoxText(const char *text, unsigned box=0);
	QString getComboBoxText(unsigned box=0);
	void selectComboBox( int iSel, unsigned box=0 );
	void selectComboBox( const QString& str, unsigned box=0 );
	void showComboBox( bool iShow, unsigned box=0 );

	//for handling the list boxes.
	void addListBoxText(const char *text, unsigned box=0);
	QString getListBoxText(unsigned box=0);
	void selectListBox( int iSel, unsigned box=0 );
	void clearListBox( unsigned box=0 );
	void setListBoxLabel( const char *text, unsigned box=0 );
	void showListBox( bool iShow, unsigned box=0 );

	//for handling the edit boxes.
	void setEditBoxText(const char *text, unsigned box=0);
	QString getEditBoxText(unsigned box=0);
	void setEditBoxLabel( const char *text, unsigned box=0 );
	void showEditBox( bool iShow, unsigned box=0 );

	//The checkbox state.
	int isBoxChecked( void );
	void setCheckBox(bool checked);
	void setCheckBoxText( const char *text );
	void showCheckBox( bool iShow );

	// The title text.
	void showTitleText( bool iShow );
	void setTitleText( const char *text );

	/* Called if underlying window system kills this window. */
	virtual void closeEvent ( QCloseEvent * e );
		
/*
	signalInt *evCancel(){ return &evCancel_; }
    signalInt *evApply(){ return &evApply_; }
    signalInt *evApplyAll(){ return &evApplyAll_; }
    signalInt *evOK(){ return &evOK_; }
	signalInt *evReset(){ return &evReset_; }
	signalInt *evResetAll(){ return &evResetAll_; }
	signalInt *evHelp(){ return &evHelp_; }
    signalInt *evCmbStatic1(){ return &evCmbStatic1_; }
    signalInt *evCmbStatic2(){ return &evCmbStatic2_; }
	signalInt *evList1() { return &evList1_; }
	signalInt *evList2() { return &evList2_; }
	signalInt *evLeftarrow() { return &evLeftarrow_; }
	*/


signals:
	//Events
    void evCancel_(); 
	void evApply_(); 
	void evApplyAll_(); 
	void evOK_();
	void evReset_(); 
	void evResetAll_(); 
	void evHelp_();; 
	void evCmbStatic1_(int);  //sends the index selected
	void evCmbStatic2_(int);  //sends the index selected
	void evList1_(int); //sends the index selected
	void evList2_(int); //sends the index selected
	void evLeftarrow_();

  protected slots:
	  /* Find out if a scenario or monte carlo run is being started/stopped.
	 *  If we are running, disable "OK", "Apply", "Apply All", etc.
	 */
	virtual void cbRunStatusChanged( void );
	//Standard Dialog Events
	//virtual int command(zCommandEvt *Ev);
	//virtual void cb1SelChange(  );
	//virtual void cb2SelChange(  );
	virtual void cbCancel();
	virtual void cbOK();

	//static void _cbRunStatusChanged( void *msg );

};

#endif


