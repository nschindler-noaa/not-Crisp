#ifndef _qEditSliderBase_h_
#define _qEditSliderBase_h_

//This is an abstract base class for edit sliders.  An edit slider
//is a control with three windows: a static window box, a scrollbar/slider, and
//an edit window.  The derived base classes can then specify the type of
//data (long, double, unsigned, etc).

#include "qImConScroller.h"
#include "qDefines.h"
#include <qlabel.h>
#include <qlineedit.h>
#include <qvalidator.h>

class qEditSliderBase : public QWidget {

  protected:
    //zScrollBarCtrl *sb_;
	qImConScroller *ics_;

    /*zStaticText *st_;
	zNumFormatter *nft_;
    zFormattedEdit *el_;
	*/
	QLabel *st_;
	QValidator *nft_;
    //QLineEdit *el_;

    int baseId_;
    int lock_;
	int delta_;
	int editSliderType_; //type of edit slider, Q_LONGEDITSLIDER, Q_DOUBLEEDITSLIDER
    long divSize_;  //number of divisions across the range of the scroll bar.

  public:

	qEditSliderBase(QWidget *w, char *text, int baseId, int editSliderType, long divSize); 	
	virtual ~qEditSliderBase();

	inline int baseId() const { return baseId_; }
	inline int delta() const { return delta_; }
	inline int editSliderType() const { return editSliderType_; }
	
	virtual void setText(char *str);
	virtual void disable(void);
	virtual void enable(void);


//	virtual int cbEditLineChanged(zEvent *) = 0;
//	virtual void cbScrollBarChanged(const int val) = 0;
	//static void _cbScrollBarChanged(void *ptr, const int val);

	void setDelta(int delta){delta_ = delta;}
	void setActiveUpdates(int onOff){ ics_->setActiveTracking(onOff); }

	///protected slots:
//	virtual void cbScrollBarChanged( int );

};


#endif
