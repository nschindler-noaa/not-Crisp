#ifndef _qGraphPane_h_
#define _qGraphPane_h_

#include "qGraphDataToken.h"
#include <qwidget.h>
#include <qevent.h>
#include <qpainter.h>
#include <qpixmap.h>
#include "log.h"

/* Possible ways to draw a graph (0 = use default).
 */
#define CONNECTDOTS  1  /* diagonal lines connect data points. */
#define CITYSCAPE    2  /* horizontal and vertical lines connect data points. */
#define HOLLOWBARS   3  /* Outlined bars from zero up or down to data value. */
#define FILLEDBARS   4  /* Filled bars from zero up or down to data value. */

class qGraphPane : public QWidget {

	Q_OBJECT

	int mouseOn;
	QWidget *w_;    
	int isPrintMode_;
	QPoint p1_, p2_, p0_;  
    int leftMouseButtonPressed_, rightMouseButtonPressed_; 
    
	qGraphDataToken *dataToken_;
	double xRange_[2], yRange_[2];


	int drawMethod_;      /* CONNECTDOTS, CITYSCAPE, or BARGRAPH (0 == default). */

	int minZoomSize_;    /* for area selection events. */

    // remember last values passed to getGraphRegion()
    int charWidth_, charHeight_;
	bool printing;
	QRect printRect;

  public:
	//What we draw on
	QPixmap pm;

	qGraphPane(QWidget *w); 
	~qGraphPane(){ delete painter; 
				//log_msg(0,"graph pane destroyed");
	}
	
	//Graph axis range settings
	double getXAxisRange(int i){ if(i) return xRange_[1]; return xRange_[0]; }
	double getYAxisRange(int i){ if(i) return yRange_[1]; return yRange_[0]; }

	//Graph data token storage
	void setDataToken(qGraphDataToken *dt){ dataToken_=dt; }
	qGraphDataToken *getDataToken(){ return dataToken_; }
	
	
	QPainter * painter;
	void print(QPainter *p, QRect body);
	
	// Make a QRect from two zPoints - an area selection event.
	void makeRect( QRect &r, QPoint &p0, QPoint &p1 );
	
	//User Events

	//Screen info functions
	void resetCliping();

    // getGraphRegion returns the rectangle in the pane which is used
    //  for plotting data, as opposed to axes and labels. This is based
    //  on the size of the current font, which should be passed in. If
    //  the default arguments are seen, the last non-default values are used.
	QRect getGraphRegion( int charWidth=-1, int charHeight=-1 );
	QRect getClipedGraphRegion( int charWidth=-1, int charHeight=-1 );

	virtual QPoint getMousePosition(int pos){
			if(!pos) 
				return p0_;    // current position
			if(pos==1)
				return p1_;		//first position
			return p2_;			//last dragged position
	}

	/* Set or examine drawing method for this graph. -1 means interrogate
	 *  only, 0 means set to default, defined values are CONNECTDOTS, 
	 *  CITYSCAPE, BARGRAPH, etc.
	 */
	int drawMethod( int meth = -1 );

	// For controlling the area selection function.
	int minZoomSize( int value = -1 );

	//print functions
	bool isPrintMode(){ return printing; }
	void setPrintMode(bool mode){ printing = mode; }
  protected:
	virtual void paintEvent( QPaintEvent * );
	virtual void mousePressEvent ( QMouseEvent *  );
	virtual void mouseReleaseEvent ( QMouseEvent *  );
	virtual void mouseMoveEvent ( QMouseEvent * );


signals:
	virtual void evMouseMoved(QPoint );
	virtual void evMouseMovedLButtonPressed(QPoint);
	virtual void evMouseMovedRButtonPressed(QPoint);
	virtual void evLButtonPressed(QPoint);
	virtual void evRButtonPressed(QPoint);
	virtual void evRButtonAreaSelection(QRect);
	virtual void evLButtonAreaSelection(QRect);
	virtual void evDisplay();
	virtual void evRefresh();
	virtual void evConfigure();
	virtual void evDraw();
  
	

};

#endif


