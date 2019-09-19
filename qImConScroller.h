#ifndef _qImConScroller_
#define _qImConScroller_

#include <qwidget.h>
//#include <zapp.hpp>
//#include "implicit.h"
//typedef utImplicit1<const int> signalInt;

class qImConScroller : public QWidget {

	Q_OBJECT
	
	//signalInt evScrollBar_;
	int lock_;
	int activeTracking_;
	//int scrollerType;
	//zScrollBarCtrl *sbc_;
	double rDoubleMin_; 
	double rDoubleMax_;
	long rLongMax_;
	long rLongMin_;
	int editSliderType;
 
	//int labelWidth;
	int pos; //very important holds the size in pixels of the rect
	int max; //holds the maximum number of pixels in the scrollbars range
	char * Format;

	//zScrollBarCtrl *sbc_;


	//Custom Painter
	virtual void paintEvent ( QPaintEvent * );
	//Mouse listeners
	virtual void mousePressEvent ( QMouseEvent * );
	virtual void mouseMoveEvent ( QMouseEvent * );

  public:

	//signalInt *evScrollBar(){ return &evScrollBar_; }

	qImConScroller(QWidget * w, char* name, int editSliderType, int activeTracking = 0);

	int activeTracking(){return activeTracking_;}
	void setActiveTracking(int at){ activeTracking_ = at; }

	void setDoubleRange(double r1,double r2);
	void setLongRange(long r1,long r2);

	void setDoubleValue(double val);
	void setLongValue(long val);

	int getPos();

	void setNumberFormat( char * numFormat);

	long getLongValue();
	double getDoubleValue();

	virtual int horzThumbPos(){
		if(!lock_){	
			lock_=1;
			//evScrollBar_.notify(sbc_->info()->pos());
			lock_=0;
		}
		return 1;
	}

	virtual int horzThumbTrack(){
		if(!lock_){	
			lock_=1;
			if(activeTracking_)
			//	evScrollBar_.notify(sbc_->info()->pos());
			lock_=0;
		}
		return 1;
	}

signals:
	void valueChanged(long);
	void valueChanged(double);


};


#endif



