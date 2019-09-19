// qEquationDialogBase.h 
//
// HOW TO:
// QUICK OVERVIEW:

#ifndef _qEquationDialogBase_h_
#define _qEquationDialogBase_h_

#include "qDialogBase.h"
#include "qGraphPane.h"
#include <qlabel.h>
#include <qwidget.h>


class qEquationDialogBase : public qDialogBase {

  protected:
	qGraphPane *gp_;

	QGridLayout * gridl; //Holds sliders to modify the equations
	QFrame * dividerWidget; //Horizontal bar to divide params
	QLabel *stPositionX_;
	QLabel *stPositionY_;
	QLabel *stDateX_;
    QWidget *spacers_[10];  /* lines sitting between sliders. */

    int dividerParam_;   /* -1, or slider after which to insert some blank space. */
	
  public:

   // qEquationDialogBase(QWidget *w );
    qEquationDialogBase(QWidget *w, int dw=WStyle_Dialog, const char *nt=NULL);
	virtual ~qEquationDialogBase();

	//virtual int command(zCommandEvt *Ev);

	virtual int drawDialog(unsigned topMargin);
	virtual void drawStandardLayout(int sliderType);
	qGraphPane *getGraphPane(){ return gp_; }

	

	int setPositionXLabel(const char *pstr);
	int setPositionYLabel(const char *pstr);
	int setPositionDateLabel( const char *pstr );
	void showPositionDateLabel( const bool iShow );

    void setDividerParam( int i );
};

#endif


