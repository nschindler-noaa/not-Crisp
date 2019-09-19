#include "qGraphPane.h"
#include "log.h"
#include "stdlib.h"
#include <qpixmap.h>
#include "qGraphDialog.h"

qGraphPane::qGraphPane(QWidget *w) : 
	QWidget(w, "Graph Pane", WNoAutoErase  ), w_(w), isPrintMode_(0) {

	drawMethod_ = 0;  // use default
	minZoomSize_ = 10;
	setPaletteBackgroundColor(white);
	setEraseColor(white);
        charWidth_ = charHeight_ = -1;
	rightMouseButtonPressed_ = 0;
	leftMouseButtonPressed_ = 0;
	painter = new QPainter();
	//No Background to do double buffering
	setBackgroundMode (NoBackground);
	setMouseTracking ( true);
	printing = FALSE;
		

}

void qGraphPane::print(QPainter *p, QRect body){

	printRect = body;
	printing  = TRUE;
	
	QPainter * oldPainter = painter;
	painter = p;
	painter->setBackgroundColor(white);
	emit evDraw(); //let the mediators draw on printer QPainter	
	//restore the old painter
	painter = oldPainter;
	printing = FALSE;

}


void qGraphPane::paintEvent ( QPaintEvent * pEvt){ 

	QRect r;

	r = rect();

	/* No need to draw if no visible area. */
	if(r.right() <= 0 || r.bottom() <= 0 || printing){
		return ;
	}


	//Resize , if neccesary
	pm.resize ( QMAX( size().width(), pm.size().width()), 
					QMAX( size().height(), pm.size().height())) ;				
		
	//clear the canvas
	pm.fill();	
	
    painter->begin(&pm, this);
	painter->setBackgroundColor(white);
	emit evDraw(); //let the mediators draw on the pixmap
	painter->end();
	//transfer the image to the widget
	bitBlt(this, 0, 0, &pm);	
	
	return;
}


QRect qGraphPane::getClipedGraphRegion( int charWidth, int charHeight ){
	QRect tr;
	tr = getGraphRegion( charWidth, charHeight );
	
	if(painter != NULL && painter->isActive()){
		QRect p(tr);
		p.setTop ( tr.top()-1 ); //add 1 to top to account for a clipping error	
		painter->setClipRect(p);
	}

	return tr;
}

QRect qGraphPane::getGraphRegion( int charWidth, int charHeight ){
	QRect r, tr;
	
	if(printing)
		r = printRect;
	else
		r = rect();	

    // Use character sizes passed in, or stored values.
    if (charWidth == -1)
        charWidth = charWidth_;
    else
        charWidth_ = charWidth;
    if (charHeight == -1)
        charHeight = charHeight_;
    else
        charHeight_ = charHeight;

    if (charWidth == -1 || charHeight == -1)
        tr = QRect( 0, 0, 0, 0 );
    else
    	tr = QRect( r.left() + 12*charWidth, r.top() + 3*charHeight,
				r.width() - 15*charWidth,r.height() - 6*charHeight );	
	return tr;
}

void qGraphPane::resetCliping(){
	QRect r;
	if(printing)
		r = printRect;
	else
		r = rect();

	if(painter != NULL && painter->isActive()){
		painter->setClipRect(r);
	}
}

//	qGraphPane::mouseButtonDown(zMouseClickEvt *ev)
//
//	Pretty straight forward.. this is the virtual zPane::mouseButtonDown
//  it's been modified so that it remembers which mouse button was pressed
//  and captures the mouse in case the user does an area selection.
void qGraphPane::mousePressEvent ( QMouseEvent * e ){
	ButtonState btns = e->button();
	if(btns == Qt::LeftButton ){

		leftMouseButtonPressed_=1;
		p0_ = p1_ = p2_ = e->pos();

    }
    if(btns == RightButton){
		rightMouseButtonPressed_ = 1;
		p0_ = p1_ = p2_ = e->pos();	
    }

}

int qGraphPane::minZoomSize( int value ) {
	if (value > 0)
		minZoomSize_ = value;
	return minZoomSize_;
}

// Make a QRect from two QPoints - so that left, right, top, and bottom
//  have the proper relationships.
void qGraphPane::makeRect( QRect &r, QPoint &p0, QPoint &p1 ) {
	if (p0.x() < p1.x()) {
		r.setLeft(p0.x());
		r.setRight(p1.x());
	} else {
		r.setLeft(p1.x());
		r.setRight( p0.x());
	}

	if (p0.y() < p1.y()) {
		r.setTop(p0.y());
		r.setBottom(p1.y());
	} else {
		r.setTop(p1.y());
		r.setBottom(p0.y());
	}
}

//	qGraphPane::mouseButtonUp(zMouseClickEvt *ev)
//  
//	This function looks a little involved but it basically handles notification
//  of the button pressed and the area selection events based on weather an area
//  was selected or a single button was pressed.  It does this for both the left
//  and the right mouse buttons, and the AreaSelection_ notifications always send
//  a zRect so it's a bit long.

void qGraphPane::mouseReleaseEvent ( QMouseEvent * e ){
	QRect r = QRect();
	ButtonState btns = e->button();

	if(btns == LeftButton  && leftMouseButtonPressed_){
		leftMouseButtonPressed_=0;
		if(p0_ == p1_){			
			emit evLButtonPressed(e->pos());
		} else {
			p0_ = p2_ = e->pos();
			makeRect( r, p0_, p1_ );
			emit evLButtonAreaSelection(r);
		}
    }

    else if( btns == RightButton && rightMouseButtonPressed_){
		rightMouseButtonPressed_=0;
		if(p0_ == p1_){
			emit evRButtonPressed(e->pos());
		} else {
			p0_ = p2_ = e->pos();
			makeRect( r, p0_, p1_ );
			emit evRButtonAreaSelection(r);
		}
    }

}

//  qGraphPane::mouseMove(zMouseMoveEvt *ev)
//
//  This function is an override of the zPane::mouseMove event.
//  It's been customized to handle area selections with the left and right 
//  mouse buttons, and does a notification to the evMouseMoved_ event
//  to notify the registered ZatMapSubMediators of the change

void qGraphPane::mouseMoveEvent ( QMouseEvent * e ){


    if( leftMouseButtonPressed_){
		p2_ = p0_;
		p0_ = e->pos();
		emit evMouseMovedLButtonPressed(p0_);
    }
    else if( rightMouseButtonPressed_){
		p2_ = p0_;
		p0_ = e->pos();
		emit evMouseMovedRButtonPressed(p0_);
    }

	//emit standard mouse move event
	emit evMouseMoved(e->pos());

    return ;
}

/* There are different ways to have ourselves drawn.
 */
int qGraphPane::drawMethod( int meth ) {
	if (meth != -1)
		drawMethod_ = meth;
	return drawMethod_;
}
