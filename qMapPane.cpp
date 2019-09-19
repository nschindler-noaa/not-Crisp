// qMapPane.cpp
//
// Last modified, 12-3-96, Steve Umbehocker

#include "qMapPane.h"
#include "QPointTransform.h"
#include <qpainter.h>
#include <qprinter.h>
#include <qpaintdevicemetrics.h>
#include "qMercatorTransform.h"
#include "qMapMediator.h"
#include "Qcrisp.h"
#include "log.h"
//#include "resource.h"

qMapPane::qMapPane(QWidget *p) 
	: QWidget(p,"Main Drawing Window") {
	zpt_ = NULL;
	leftMouseButtonPressed_ = 0;
	middleMouseButtonPressed_ = 0;
	rightMouseButtonPressed_ = 0;
	isPrintMode_ = false;
	
	setMouseTracking ( true);
	setPaletteBackgroundColor( white  );
	setBackgroundMode(Qt::NoBackground); //so we can double buffer
	pm.resize(size());  //make the pix the  size of the mappane
	currPainter = new QPainter();
	
}

qMapPane::~qMapPane() {
	clearTransforms();
}


void qMapPane::paintEvent ( QPaintEvent * ){


	// This is so that you can do some pre-draw configuring with the mediators 
	// before the actual drawing.  This is usefull for the building of transforms etc.
	if(isPrintMode_ == TRUE ) 
		return;

	drawArea = rect();

	if(!painting){
		//resize the pixmap, if we resized
		pm.resize( QMAX(pm.size().width(), size().width()),
				QMAX(pm.size().height(), size().height())); 
		pm.fill();
		
		painting = TRUE;
		currPainter->begin(&pm, this);
	}	
	
	emit evConfigure();
		
	// Now we notify the submediators hooked up to the evRefresh event to do their stuff
	emit evRefresh();
		
	currPainter->end();
	//do the actual drawing onto the widget
	bitBlt(this, 0, 0, &pm);
	painting = FALSE;

}

//helper methods other classes can draw on the map pane , when not in the draw loop.
//Used by the mouse selection listerners
void qMapPane::initiateDraw(){
	if(!painting){
		currPainter->begin(&pm, this);	
		painting =  TRUE;
	}	
	
}

void qMapPane::endDraw(){
		
	if(painting){
		currPainter->end();
		//do the actual drawing onto the widget
		bitBlt(this, 0, 0, &pm);
		painting = FALSE;
		}	
	
}

//	qMapPane::pushTransform(ZatPointTransform *zpt)
//  qMapPane::popTransform()
//
//	I thought it might be useful if multiple transforms could be
//	stored in a pane so this will just push a new one on to the stack
//	(haven't really found a need for more than one though..)
//  So, for now, a single-level of storage is provided.

void qMapPane::pushTransform(qPointTransform *zpt){ 
	if (zpt_ != NULL)
		delete zpt_;
	zpt_ = zpt; 
}

qPointTransform *qMapPane::popTransform(){
	qPointTransform *temp = zpt_;
	zpt_ = NULL;
	return temp;
}

void qMapPane::clearTransforms(){
	if (zpt_ != NULL)
		delete zpt_;
	zpt_ = NULL;
}

//	qMapPane::getTransform
//
//	No rocket science here.. this returns the current transform

qPointTransform *qMapPane::getTransform(){
	return zpt_;
}

//	qMapPane::printMap()
//

void qMapPane::printMap(){

	isPrintMode_ = TRUE;
	QPainter *old_painter = currPainter;

	QPrinter printer(QPrinter::PrinterResolution);
	printer.setOrientation(QPrinter::Landscape) ;
	printer.setPageSize(QPrinter::Letter) ;
	printer.setMinMax ( 1,1 );
	printer.setPrintRange(QPrinter::AllPages);

	if(printer.setup (0)){
		QPainter p;
        if( !p.begin( &printer ) ) {               // paint on printer
            return;
        }
	
        QPaintDeviceMetrics metrics( p.device() );
        int dpiy = metrics.logicalDpiY();
        int margin = (int) ( (2/2.54)*dpiy ); // 2 cm margins
        
		QRect body( 0,0, metrics.width(), metrics.height() );
		currPainter = &p; //set the painter
		drawArea = body;
		
		qMapMediator* mapMed = (qMapMediator*) ((QCrisp*)parentWidget() )->getMapMediator();
		QPtrStack <mapCordset> mapStack_ = mapMed->getCordStack();

		qMercatorTransform *trans;
		mapCordset *defaultCord_ = new mapCordset(124.3,49.0,113.2,43.50);
		if(mapStack_.top())
			trans = new qMercatorTransform(body,mapStack_.top());
		else	  
			trans = new qMercatorTransform(body,defaultCord_);

		pushTransform(trans);
		emit evRefresh();
		//done printing
		//popTransform();
		delete defaultCord_;
	}
	currPainter = old_painter;
	isPrintMode_ = FALSE;
	


}


//	qMapPane::mouseButtonDown(zMouseClickEvt *ev)
//
//	Pretty straight forward.. this is the virtual zPane::mouseButtonDown
//  it's been modified so that it remembers which mouse button was pressed
//  and captures the mouse in case the user does an area selection.


void qMapPane::resizeEvent ( QResizeEvent * qre){
	QPaintEvent  *p = new QPaintEvent(QRect());
	paintEvent(p);
}


void qMapPane::mousePressEvent ( QMouseEvent * qme ){

	ButtonState b = qme->button();

    if(b == Qt::LeftButton){
		leftMouseButtonPressed_=1;
		p0_ = p1_ = p2_ = qme->pos();
		return ;
    }
    if(b == Qt::MidButton){
		middleMouseButtonPressed_=1;
		p0_ = p1_ = p2_ = qme->pos();
		return ;
    }
    if(b == Qt::RightButton){
		rightMouseButtonPressed_=1;
		p0_ = p1_ = p2_ = qme->pos();
		return ;
    }

    return ;

}

//	qMapPane::mouseReleaseEvent ( QMouseEvent * qme )
//  
//	This function looks a little involved but it basically handles notification
//  of the button pressed and the area selection events based on weather an area
//  was selected or a single button was pressed.  It does this for both the left
//  and the right mouse buttons, and the AreaSelection_ notifications always send
//  a QRect with the upper left coordinate < the lower right so it's a bit long.

void qMapPane::mouseReleaseEvent ( QMouseEvent * qme ){

	const int minZoomSize = 10;
	ButtonState b = qme->button();
	//p1_ = original click point
	//p2_ and p0_ = last point released on
    if(b == Qt::LeftButton && leftMouseButtonPressed_){
		leftMouseButtonPressed_=0;
		if(p0_ == p1_){
			update();
			emit evLButtonPressed(qme->pos());			
			return ;
		}
		drawSelect(painting);
		p0_ = p2_ = qme->pos();
		if(p1_.x() == p0_.x() || p1_.y() == p0_.y()){
			update();
			return ;
			
		}
		else if(p1_.x()+minZoomSize < p0_.x()){  //Dragged to the Right
			if(p1_.y()+minZoomSize < p0_.y()){ //And Down
				emit evLButtonAreaSelection(QRect(p1_,p0_));
				return;
			}
			else if(p1_.y() > p0_.y()+minZoomSize){ //And Up
				emit evLButtonAreaSelection(QRect(p1_.x(),p0_.y(),p0_.x()-p1_.x(),p1_.y()-p0_.y() ));
				return;
			}
		}else{  //Area Selected was to the Left of the Original point 
			if(p1_.x() > p0_.x()+minZoomSize){
				if(p1_.y() > p0_.y()+minZoomSize){  //And Up
					emit evLButtonAreaSelection(QRect(p0_,p1_));
					return;
				}
				else if(p1_.y()+minZoomSize < p0_.y()){ //And Down
					emit evLButtonAreaSelection(QRect(p0_.x(),p1_.y(),p1_.x()-p0_.x(),p0_.y()-p1_.y() ));
					return;
				}
			}
		}				
		
    }

    if (b == MidButton && middleMouseButtonPressed_) {
	
		middleMouseButtonPressed_=0;
		if(p0_ == p1_){
			emit evMButtonPressed(qme->pos());
			return;
		}
	}

    if(b == RightButton && rightMouseButtonPressed_){
		rightMouseButtonPressed_=0;
		if(p0_ == p1_){
			update();
			emit evRButtonPressed(qme->pos());			
			return ;
		}
		p0_ = p2_ = qme->pos();
		if(p1_.x() == p0_.x() || p1_.y() == p0_.y()){
			update();
			return ;  //Draged a straight line.. don't do anything.
		}
		else if(p1_.x()+minZoomSize < p0_.x()){  //Dragged to the Right
			if(p1_.y()+minZoomSize < p0_.y()){ //And Down
				emit evRButtonAreaSelection(QRect(p1_,p0_));
				return;
			}
			else if(p1_.y() > p0_.y()+minZoomSize){ //And Up
				emit evRButtonAreaSelection(QRect(p1_.x(),p0_.y(),p0_.x()-p1_.x(),p1_.y()-p0_.y() ));
				return;
			}
		}else{  //Area Selected was to the Left of the Original point 
			if(p1_.x() > p0_.x()+minZoomSize){
				if(p1_.y() > p0_.y()+minZoomSize){  //And Down
					emit evRButtonAreaSelection(QRect(p0_,p1_));
					return;
				}
				else if(p1_.y()+minZoomSize < p0_.y()){ //And UP
					emit evRButtonAreaSelection(QRect(p0_.x(),p1_.y(),p1_.x()-p0_.x(),p0_.y()-p1_.y() ));
					return;
				}
			}
		}
    }
	//The Area selected was too small
	update();	
    return;
}

//  qMapPane::mouseMove(zMouseMoveEvt *ev)
//
//  This function is an override of the zPane::mouseMove event.
//  It's been customized to handle area selections with the left and right 
//  mouse buttons, and does a notification to the evMouseMoved_ event
//  to notify the registered ZatMapSubMediators of the change


void qMapPane::mouseMoveEvent ( QMouseEvent * qme ){
	
	ButtonState button = qme->state();
	//log_msg(0,"mouse moved, Drawing selected area");
	bool endPainter = TRUE;
    if(button == Qt::LeftButton && leftMouseButtonPressed_){
		
		if(painting != TRUE){
			//resize the pixmap, if we resized
			pm.resize( QMAX(pm.size().width(), size().width()),
					QMAX(pm.size().height(), size().height()));
			currPainter->begin(&pm, this);
			painting = TRUE;
			endPainter = FALSE;
		}		
	
		drawSelect(TRUE); //tell it we opened a painter
		p0_ = qme->pos();
		drawSelect(TRUE);
		if(endPainter){
			currPainter->end();
			//do the actual drawing onto the widget
			bitBlt(this, 0, 0, &pm);		
			painting = FALSE;
		}
    }
    else if((button == RightButton) && rightMouseButtonPressed_){
		if(painting != TRUE){
			//resize the pixmap, if we resized
			pm.resize( QMAX(pm.size().width(), size().width()),
					QMAX(pm.size().height(), size().height()));
			currPainter->begin(&pm, this);
			painting = TRUE;
			endPainter = FALSE;
		}		
	
		drawSelect(TRUE); //tell it we opened a painter
		p0_ = qme->pos();
		drawSelect(TRUE);
		if(endPainter){
			currPainter->end();
			//do the actual drawing onto the widget
			bitBlt(this, 0, 0, &pm);		
			painting = FALSE;
		}
    }
	else{
		emit evMouseMoved(qme->pos());
	}

    return ;
}
	
//	qMapPane::drawSelect()
//
//	This function handles the drawing of a XORed rectangle when an area of 
//	the QWidget is being selected.  Typically used for zooming in on a 
//	section of the map. openPainter tell if the caller opened a painter
//  and is taking care of all of the bitBlting.

void qMapPane::drawSelect(bool openPainter){

	QPainter *p = currPainter;	
	if(!openPainter	){
		pm.resize( QMAX(pm.size().width(), size().width()),
			QMAX(pm.size().height(), size().height()));
		p->begin(&pm, this);
		
		painting = TRUE;
	}
	
	p->setPen(QPen::white);
	p->setRasterOp(XorROP);

    
    if(p1_.x() == p0_.x() || p1_.y() == p0_.y()){
		p->drawLine(p1_, p0_);
	}
    else if(p1_.x() < p0_.x()){
		if(p1_.y()<p0_.y())   
			p->fillRect(p1_.x(),p1_.y(),p0_.x()-p1_.x(),p0_.y()-p1_.y(), QBrush::white);			
		else    
			p->fillRect(p1_.x(),p0_.y(),p0_.x()-p1_.x(),p1_.y()-p0_.y(), QBrush::white);			
    }
    else{
		if(p1_.y() > p0_.y())    
			p->fillRect(p0_.x(),p1_.y(),p1_.x()-p0_.x(),p0_.y()-p1_.y(), QBrush::white);			
		else    
			p->fillRect(p0_.x(),p1_.y(),p1_.x()-p0_.x(),p0_.y()-p1_.y(), QBrush::white);			
    }
	p->setRasterOp(CopyROP);
	p->setPen(QPen::black);


	if(!openPainter	){
		painting = FALSE; // release lock , since we open a new painter ourselves
		p->end();
		//do the actual drawing onto the widget
		bitBlt(this, 0, 0, &pm);
	}


	
}







