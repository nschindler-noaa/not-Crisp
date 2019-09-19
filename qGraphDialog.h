#ifndef _QcGraph_h_
#define _QcGraph_h_

#include "qGraphPane.h"
#include "qEditLine.h"
#include <qmainwindow.h>
#include <qlabel.h>
#include <qtoolbar.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qtoolbutton.h>
#include <qbuttongroup.h>
#include <qptrlist.h>
#include <qlayout.h>
#include "log.h"
/*
 * possible values for iChildOpt constructor argument. May be OR'ed.
 */
#define HIDE_DONE   1   /* Hide the "Done" button */
#define SHOW_DATE   2	/* Show the date edit line. */
#define SHOW_COMBO  4	/* Show the combo box. */
#define SHOW_BOTTOM 8	/* Show the bottom line of buttons. */
#define HIDE_ACTIVE 16  /* Hide the Active Update ("Wand") button */


class qGraphDialog : public QMainWindow {

	Q_OBJECT

	QString tLabel_;

	QPtrList<QColor> colorList_;
	QPtrList<QString> legendList_;

	int xIncrement_,yIncrement_;
	int numXDivisions_;
	int numYDivisions_;

	QComboBox *cmbStatic_;

	QPushButton* pbOK_; 
	QPushButton* pbCancel_ ;
	QPushButton* pbApply_ ; 
	QPushButton* pbReset_  ;
	QPushButton* pbApplyAll_ ;
	QPushButton* pbResetAll_ ;
	QPushButton* pbSchedule_ ;
	QPushButton* pbDone_ ;
	QToolButton* pbPrint_ ;
	QToolButton* pbSave_ ;
	QToolButton* pbHelp_ ;
	QToolButton* pbWand_ ;
	QButtonGroup * graphType;

	QToolBar *tbar_, *tbar2_, *tbar3_;

	QLineEdit *edYMax_, *edYMin_;
	qGraphPane *gp_;
	QLabel *stTitle_;
	QLabel *stPositionX_;
	QLabel *stPositionY_;
	QLabel *stDateX_;
	QPushButton *bbActiveUpdate_;    
	QPushButton *bbFix_, *bbRound_, *bbGroup_;
	QToolButton *bbUpArrow_, *bbDownArrow_;
	QToolButton *bbConDots_, *bbCityScape_, *bbHollowBars_;
	QToolButton *bbFilledBars_;
	//	Layout managers:
	QVBoxLayout* mainLayout;
	QHBoxLayout* legendLayout;
	//Main widget
	QWidget* mainWidget;

	QRect graphArea_;
	int TitleSet_,rightMouseButtonPressed_; 
	bool isActive_;
	int isAutoscale_;
	int isRounded_;
	int isGrouped_;
    int wasRemoveNotified_; /* evRemove_.notify() will be called once only. */
	qGraphDataToken *dataToken_;

	int lock_;

protected:

	virtual void closeEvent ( QCloseEvent * e );
	void buildGraphDialog( int iChildOpt );
	QPixmap saveImage;
	QPixmap printImage;
	QPixmap helpImage;	
	QPixmap wandImage;	
	QPixmap hollowDown;
	QPixmap hollowUp;
	QPixmap filledDown;
	QPixmap filledUp;
	QPixmap cityUp;
	QPixmap cityDown;
	QPixmap connectUp;
	QPixmap connectDown;


public:

	/*
	 * NOTE: iChildOpt controls creation of optional child window controls.
	 *  a zero value builds the base class object.
	 */
	qGraphDialog(QWidget *w, int iChildOpt,WFlags dw =WType_TopLevel,  const char *label = NULL);
	qGraphDialog(QWidget *w, int iChildOpt,WFlags dw =WType_TopLevel);
	~qGraphDialog();

	/* Set the data token that our graph pane is displaying.
	 */
	void setDataToken( qGraphDataToken *dt );

	void printMe( void );
	void drawLegend( void );

	void lockCanvas();
	void unlockCanvas();

	int addLegendLabel(const char *cs, QColor *zc, int labelNum=-1);

	int setPositionXLabel(const char *pstr);
	int setPositionYLabel(const char *pstr);
	int setPositionDateLabel( const char *pstr );

	int setCurrentPen(int colorNum);
	void setTitleLabel(const char *label = NULL);
	double getXpercent(int x);	
	double getYpercent(int y);	
	qGraphPane *getGraphPane(){return gp_;}

	// stuff to handle Yaxis scale functions.
	bool isActive(){ return isActive_; }
	int isRounded() { return isRounded_; };
	int isAutoscale() { return isAutoscale_; };

	void scaleUpdated( void );

	//this selects an item in the static combo box, such as a species
	// with default argument, returns current selection.
	int selectComboBox(int i=-1);
	//for adding text to a combo box, (ie "Steelhead")
	void addComboBoxText(const char *text);
	void insertComboBoxText( const char *text, int pos );
	void clearComboBox(){ cmbStatic_->clear();}
	// Retrieve text from combo box - "i" is position, -1 is current selection
	int getComboBoxText( QString &dest, int i = -1 );

	
	



	

	// "Rounding" means moving up, or down, to the next "natural-seeming"
	//  number. "y" should be positive.
	double roundUp( double y );
	double roundDown( double y );

public slots:
	virtual void cbRunStatusChanged(  );
	virtual void cbGroupYScaleChanged();
	// Notification procedures for the edit fields in tbar2_.
	virtual void typedYMinMax( void );
	virtual void YMinMaxKillFocus();
	virtual void FixChanged();
	virtual void graphViewChange(int );
	virtual void upArrow();
	virtual void downArrow();
	virtual void roundButton();
	virtual void okPressed();
	virtual void savePressed();
	virtual void helpPressed();
	virtual void wandPressed();
	virtual void printPressed();
	virtual void cancelPressed();
	virtual void applyPressed();
	virtual void resetPressed();
	virtual void applyAllPressed();
	virtual void resetAllPressed();
	virtual void schedulePressed();
	virtual void groupPressed();
	virtual void groupDataChanged(); //invoked by the DataToken's update method
	virtual	void decreaseScale();
	virtual void increaseScale();
	virtual void cbCombobox(int);  /* Notification routine. for combobox change */

	// Support for printing - this dialog gets notified of draw events in
	//  the graph pane.
	virtual void evDraw( );  //drawing notification to draw legend during printing

signals:
	void evRemove_();	/* dialog is closing. */
    void evCombobox_(); /* combobox selection event. */
    void evSave_();	/* Save button pushed. */
	void evHelp_();    /* Help button pushed. */
	void evOK_();		/* OK button pushed. */
	void evCancel_();	/* Cancel button pushed. */
	void evApply_();	/* Apply button pushed. */
	void evReset_();	/* Reset button pushed. */
	void evApplyAll_(); /* Apply All button pushed. */
	void evResetAll_(); /* Reset All button pushed. */
	void evSchedule_(); /* Schedule button pushed. */


};

#endif
