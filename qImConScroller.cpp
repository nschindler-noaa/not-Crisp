

#include "qImConScroller.h"
#include <math.h>
#include "qDefines.h"
#include <qpainter.h>
#include "log.h"

//#include <zapp.hpp>
//#include "implicit.h"
//typedef utImplicit1<const int> signalInt;

	//signalInt evScrollBar_;
	

#define HORIZ_BORDER 4   //total horizontal border
#define VERT_BORDER 4   //total vertical border
 

	//zScrollBarCtrl *sbc_;

qImConScroller::qImConScroller(QWidget * w, char* name, int editSliderType, int activeTracking ) 
		: QWidget(w, name), lock_(0), activeTracking_(activeTracking) {
	qImConScroller::editSliderType = editSliderType;
	//set up some intial dummy values
	if(editSliderType == Q_DOUBLEEDITSLIDER){
		rDoubleMin_ = 0.0; 
		rDoubleMax_ = 10.0;
		max = width()-HORIZ_BORDER - fontMetrics().width(QString::number(rDoubleMax_));
	}

	if(editSliderType == Q_LONGEDITSLIDER){
		rLongMax_ = 100;
		rLongMin_ = 0;
		max = width()-HORIZ_BORDER - fontMetrics().width(QString::number(rLongMax_));
	}
		//sbc->scroller(this);
		//double conversionRatio;
		//int scrollerType;

	}	

	//Custom Painter
void qImConScroller::paintEvent ( QPaintEvent * pEvt ){

	QRect filledBox = QRect(HORIZ_BORDER/2, VERT_BORDER/2, pos, height()-VERT_BORDER);
	QPainter *p = new QPainter(this);
	p->fillRect(filledBox, QBrush(red));  //fill the box with red
	if(editSliderType == Q_DOUBLEEDITSLIDER)
		p->drawText(pos,height()/2, QString::number(getDoubleValue()) );
	if(editSliderType == Q_LONGEDITSLIDER)
		p->drawText(pos,height()/2, QString::number(getLongValue()) );
	p->end();
	delete p;
}

//Mouse listeners

void qImConScroller::mousePressEvent ( QMouseEvent * e ){
	ButtonState bs = e->button ();

	if(bs == LeftButton){
		if(e->x() > max-HORIZ_BORDER/2)
			pos = max;
		else {
			if(e->x() < HORIZ_BORDER/2)
				pos = HORIZ_BORDER/2;
			else
				pos = e->x() - HORIZ_BORDER/2;
		}

		//notify listeners of change
		if(editSliderType == Q_LONGEDITSLIDER){
			emit valueChanged(getLongValue());
		}
		if(editSliderType == Q_DOUBLEEDITSLIDER){
			emit valueChanged(getDoubleValue());
		}

		repaint();
	}
	if(bs == RightButton){
		//TODO ::
		//do nothing, put in input box later
	}
}

/*
void qImConScroller::mouseReleaseEvent ( QMouseEvent * e ){

}
*/
void qImConScroller::mouseMoveEvent ( QMouseEvent * e ){
	ButtonState bs = e->button();

	if(bs == LeftButton){
		if(e->x() > max-HORIZ_BORDER/2)
			pos = max;
		else if(e->x() < HORIZ_BORDER/2)
			pos = HORIZ_BORDER/2;
		else
			pos = e->x()- HORIZ_BORDER/2;
		//notify listeners of change
		if(editSliderType == Q_LONGEDITSLIDER)
			emit valueChanged(getLongValue());
		if(editSliderType == Q_DOUBLEEDITSLIDER)
			emit valueChanged(getDoubleValue());

		repaint();
	}
}

void qImConScroller::setDoubleRange(double r1,double r2){
	rDoubleMin_ = r1;
	rDoubleMax_ = r2;
	max = width()-HORIZ_BORDER - fontMetrics().width(QString::number(rDoubleMax_));
	
}


void qImConScroller::setLongRange(long r1,long r2){
	rLongMin_ = r1;
	rLongMax_ = r2;
	max = width()-HORIZ_BORDER - fontMetrics().width(QString::number(rLongMax_));
}

double qImConScroller::getDoubleValue(){

	return (double(pos) / double(max) * (rDoubleMax_ - rDoubleMin_));

}
	//signalInt *evScrollBar(){ return &evScrollBar_; }

	
void qImConScroller::setDoubleValue(double val){
	
	if(val > rDoubleMax_ || val  < rDoubleMin_){
		log_msg(1, "new slider val out of range");
		return;
	}
	double fraction  = val/(rDoubleMax_ - rDoubleMin_);
	pos = (int) (fraction * double(max) + 0.5);

	repaint();
}

void qImConScroller::setLongValue(long val){
	
	if(val > rLongMax_ || val  < rLongMin_){
		log_msg(1, "new slider val out of range");
		return;
	}
	double fraction  = val/(rLongMax_ - rLongMin_);
	pos = (int) (fraction * double(max) + 0.5);

	repaint();
}


int qImConScroller::getPos(){
	return pos;
}

void qImConScroller::setNumberFormat( char * numFormat){
	Format = numFormat;
}

long qImConScroller::getLongValue(){

	return (long) (double(pos) / double(max) * double(rLongMax_ - rLongMin_) + 0.5);

}

