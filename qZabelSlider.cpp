#include "qZabelSlider.h"
#include <qpen.h>
#include <qrect.h>
#include <math.h>
#include <qpainter.h>
#include "qPromptForString.h"
#include "log.h"
#include <qpixmap.h>


qZabelSlider::qZabelSlider(QWidget* w, unsigned long wStyle, const char* t, int ctrlId) 
	: QWidget(w,t) {
	CtrlId = ctrlId;
    consCommon();	
}

qZabelSlider::qZabelSlider(QWidget* w, int ctrlId)
	 : QWidget(w) {
	CtrlId = ctrlId;
    consCommon();
}

void qZabelSlider::consCommon( void ) {
	
	setFocusPolicy (  QWidget::StrongFocus );
    dValue_ = 0.0;
	iPrecision_ = 2;
    dRangeMax_ = 10.0;
    dRangeMin_ = 0.0;
    dScale_ = 0.01;
    lock_ = 0;		
	setBackgroundMode(Qt::NoBackground);
	currentColor_= red;
	//pm.resize(size()); //set the size of the pixmap
	defaultPen_ = new QPen(black,  1, Qt::SolidLine);
	dottedPen_ = new QPen(black, 1, Qt::DashLine);
	defaultBrush_ = new QBrush(black);
	nullBrush_ = new QBrush(white);
	currentBrush_ = new QBrush(red);
	setMinimumHeight ( 24 );
	//DEFAULT: strFormat_ = "%.2f"
	sprintf(strFormat_,"%%.%df",iPrecision_);
	// DEFAULT: strSmlFormat_ = "%.3f"
    sprintf( strSmlFormat_, "%%.%df", iPrecision_+1 );
	//DEFAULT: strExpFormat_ = "%.2e"
	sprintf(strExpFormat_,"%%.%de",iPrecision_);
	printValue( dValue_ );
	//QFont f = font();
	//f.setPointSize (f.pointSize() + 2);
	//setFont(f);
	
    focus_ = 0;
	show();
   
}

qZabelSlider::~qZabelSlider(){

	delete defaultPen_;
    delete dottedPen_;
	delete defaultBrush_;
	delete currentBrush_;
    delete nullBrush_;

}

void qZabelSlider::printValue( double val ) {
    double fabsval = fabs( val );

    if (fabsval < 0.01 && fabsval != 0.0)
        sprintf( strValue_, strExpFormat_, val );
    else if (fabsval < 1.0)
        sprintf( strValue_, strSmlFormat_, val );
    else // if (fabsval >= 1.0)
        sprintf( strValue_, strFormat_, val );
}

void qZabelSlider::resizeEvent ( QResizeEvent * ev){
	repaint();
	QWidget::resizeEvent(ev);
	return ;
}

QColor qZabelSlider::getColor() {
	return currentColor_;
}

void qZabelSlider::setColor( QColor c) {
	currentColor_ = c;
	repaint();
}

/* These routine set the slider value, and notify that the value has changed.
 *  They are meant to be used in response to user actions.
 */
void qZabelSlider::setSliderValue(QPoint pos){
	QRect r;
	double tValue;
	int width, xPos = pos.x() - 2;
	int textWidth;

	if(!lock_){
		lock_=1;

		r = rect();
	
        printValue( dValue_ );
	
		textWidth = fontMetrics().width(strValue_);
		
		if(r.contains(pos)){
			width = r.right()-textWidth-6;
			if(xPos > width)
				xPos = width;
			if (xPos < 0)
				xPos = 0;
			tValue = dRangeMin_+(double(xPos)/double(width))*(dRangeMax_-dRangeMin_);
			lock_ = 0;
			setSliderValue( tValue );
			return;
		}
		lock_=0;
	}
}

void qZabelSlider::setSliderValue(double val){
	if(!lock_){
		lock_=1;
		if(val <= dRangeMax_ && val >= dRangeMin_){
			if(dValue_ != val){
				dValue_ = val;
				emit evDoubleChanged_(dValue_, CtrlId);
	 			repaint();
			}
		}
		lock_=0;
	}
}

void qZabelSlider::setSliderValue(int val){
	setSliderValue((double)val);
}

/* This routine sets the slider value and does not notify. This is
 *  meant to be used by internal housekeeping routines.
 */
void qZabelSlider::setSliderValueNN( double val ){
	if(val <= dRangeMax_ && val >= dRangeMin_){
		if(dValue_ != val){
			dValue_ = val;
	 		repaint();
		}
	}
}

void qZabelSlider::paintEvent ( QPaintEvent * pEvt){
	
	int right;

    printValue( dValue_ );
	QRect r = rect();
	//We have to make a new painter each time
	// any initialization
    
    QRegion unpainted(pEvt->region());
    static QPixmap *doubleBuffer = 0;
    if (!doubleBuffer)
        doubleBuffer = new QPixmap;
    	

	//Always expand it if needed, never shrink, reduces resizes
	doubleBuffer->resize(QMAX(size().width(), doubleBuffer->width()),
				 QMAX(doubleBuffer->height(), size().height()));

	doubleBuffer->fill(backgroundColor());

	QPainter painter(doubleBuffer, this);
	//painter.begin(doubleBuffer, this);
	
    if (focus_) {
        painter.drawWinFocusRect(r);		
    }
	
	painter.setPen( *defaultPen_);
	painter.setBrush(*currentBrush_);
	right = int ((double)(r.right()-fontMetrics().width(strValue_)-6)*((dValue_-dRangeMin_)/(dRangeMax_-dRangeMin_)));

    if (right > 1){
		painter.setBrush(black);
		painter.drawRect(r.left()+1, r.top()+2, right-r.left(), r.bottom()-r.top()-3);
		painter.fillRect(QRect(r.left()+2, r.top()+3, right-r.left()-2, r.bottom()-r.top()-5),*currentBrush_ );
	}	
	else{
		painter.setBrush(black);
		painter.drawRect(r.left()+1, r.top()+2,right-r.left()+1, r.bottom()-r.top()-3); //draw small outline
	}	

	painter.drawText(right+3,(int)(r.top()+(double)(r.height()/2+fontMetrics().height()/2-3)), strValue_);
	
	//do the actual drawing onto the widget
	painter.end();
	QRect bounds = unpainted.boundingRect();

	bitBlt(this, bounds.x(), bounds.y(), doubleBuffer, 
				bounds.x(),	bounds.y(), bounds.width(), bounds.height());
}


void qZabelSlider::mousePressEvent ( QMouseEvent * e ){
    setFocus();
	if (e->button() & LeftButton != 0  ) {
		setSliderValue(( QPoint) e->pos());
	}
	else {
		char cBuf[50];
		sprintf( cBuf, "[%g - %g]", dRangeMin_, dRangeMax_ );
		QString str = "Enter a value " ;
		str = str + cBuf;
		bool ok;
		double res = qPromptForString::getDouble("Slider Value", str, dValue_, dRangeMin_, dRangeMax_,
			iPrecision_, &ok, this , "Manual value Entry");

		if (ok == true)
			setSliderValue(res);			
	}

	return ;
}

void qZabelSlider::mouseReleaseEvent ( QMouseEvent * e ){
	if (e->state()&LeftButton) {
		setSliderValue((QPoint) e->pos());
	}
	return ;
}

void qZabelSlider::mouseMoveEvent ( QMouseEvent * e ){
		
	if (e->state()&LeftButton) {
		setFocus();
		setSliderValue((QPoint)e->pos());
	}
	return;
}

void qZabelSlider::setRange(double min, double max) { 
    double range;
    dRangeMax_ = max; 
    dRangeMin_ = min; 

    range = max - min;
    dScale_ = 1.0;
    if (range <= 0.0)
        return;  // defensive programming.
    while (range > 1000.0) {
        range /= 10.0;
        dScale_ *= 10.0;
    }
    while (range <= 100.0) {
        range *= 10.0;
        dScale_ /= 10.0;
    }
}

void qZabelSlider::keyPressEvent ( QKeyEvent * e ){
 
    int delta = 0;
    double dv;

    switch (e->key()) {
    case Qt::Key_Left:       // Solaris key codes
    case Qt::Key_Up:
        delta = -1;
		e->accept();
        break;
    
    case Qt::Key_Right: //PC key codes
    case Qt::Key_Down :
        delta = 1;
		e->accept();
        break;

    default:
		e->ignore();
        return ;
    }

    /* Arrow key; move value up or down and round.
     */
    dv = dScale_ * (int)((dValue_ + dScale_/2) / dScale_);
    dv += dScale_ * delta;
    if (dv < dRangeMin_)
        dv = dRangeMin_;
    if (dv > dRangeMax_)
        dv = dRangeMax_;

    setSliderValue(dv);

    return ;
}

void qZabelSlider::focusInEvent ( QFocusEvent * e){
	focus_ = true;
	repaint();
}
void qZabelSlider::focusOutEvent ( QFocusEvent * e){
	focus_ = false;
	repaint();
}

