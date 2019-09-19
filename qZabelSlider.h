#ifndef _qZabelSlider_h_
#define _qZabelSlider_h_

//#include <zapp.hpp>
//#include <rw/cstring.h>
//#include "implicit.h"
#include <qwidget.h>
#include <qpixmap.h>

/*enum QColor {
	RedColor,GreenColor,BlueColor,BlackColor
};
*/
//typedef utImplicit2<const double, const int> signalDoubleInt;

class qZabelSlider : public QWidget {
	
	Q_OBJECT

protected:

	//QPixmap pm;  //the pixmap used in double buffering
	double dValue_, dRangeMax_, dRangeMin_, dScale_;
	int iPrecision_;
	QColor currentColor_;
	QBrush *defaultBrush_, *currentBrush_, *nullBrush_;
	QPen *defaultPen_, *dottedPen_;
	char strFormat_[20];
	char strExpFormat_[20];
    char strSmlFormat_[20];
    char strValue_[20];
	QString strText;
	int	lock_, focus_;
	int CtrlId; //holds the identity of this slider control
	
    void consCommon( void );
    void printValue( double val );
	//QPainter* painter;  //To hold the current painter

public:

	qZabelSlider(QWidget *w, unsigned long l,const char *caption=NULL,int ctrlId=0);
	qZabelSlider(QWidget *,int ctrlId);
	~qZabelSlider();

	
	void setDelta(int val);
	int  getDelta();
	void setRange(double min, double max);
	void getRange( double *pmin, double *pmax )
				{ *pmin = dRangeMin_; *pmax = dRangeMax_; };
	QColor getColor();
	void setColor( QColor);

	/* These routine set the slider value, and notify that the value has changed.
	 *  They are meant to be used in response to user actions.
	 */
	void setSliderValue(QPoint pos);
	void setSliderValue(double val);
	void setSliderValue(int val);
	/* This routine sets the slider value and does not notify. This is
	 *  meant to be used by internal housekeeping routines.
	 */
	void setSliderValueNN( double val );
	void setText(const char *val){ strText = val; }
	QString &getText( void ) { return strText; };
	double getSliderValue(){return dValue_;}

	virtual void mouseMoveEvent ( QMouseEvent * e );
	virtual void mousePressEvent ( QMouseEvent * e );
	virtual void mouseReleaseEvent ( QMouseEvent * e );
	virtual void paintEvent ( QPaintEvent * );
	virtual void resizeEvent ( QResizeEvent * );
	virtual void focusInEvent ( QFocusEvent * );
	virtual void focusOutEvent ( QFocusEvent * );
	virtual void keyPressEvent ( QKeyEvent * e );
    
signals:
	void evDoubleChanged_(double, int);

};	




#endif
