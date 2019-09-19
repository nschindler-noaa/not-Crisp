
#include "qGraphDataToken.h"
#include "qGraphMedZoom.h"
#include "qGraphDialog.h"
#include "qGraphMedTypes.h"



int qGraphMedZoom::isA(){
	return GMT_ZOOM;
}

	
void qGraphMedZoom::RegisterEvents(){
	connect(GraphPane_, SIGNAL(evRButtonAreaSelection(QRect)), this, SLOT(evRButtonAreaSelection(QRect)) );
	connect(GraphPane_, SIGNAL(evRButtonPressed(QPoint)), this, SLOT(evRButtonPressed(QPoint)) );

}

void qGraphMedZoom::DetachEvents(){

}


void qGraphMedZoom::evRButtonAreaSelection(QRect rect){
	/* If the rectangle is "reasonably" small, it probably was meant to
	 *  be a click at a single point, so treat it as such.
	 */
	if (rect.width() < 7 && rect.height() < 7)
		evRButtonPressed( rect.topLeft() );
}

void qGraphMedZoom::evRButtonPressed(QPoint pos){

	/* Right-mouse click in bottom half of graph zooms in,
	    click in upper half zooms out. */
	QRect r;
	qGraphDataToken *dataToken_ = GraphPane_->getDataToken();
	double factor;

	if(!lock_){
		lock_=1; 
		
		r = GraphPane_->getGraphRegion();
		
		if(r.right() == 0 || r.bottom() == 0 || 
			!dataToken_ || !dataToken_->dataArray_.count()){
			lock_=0;
			return;
		}
		
		if (GraphDialog_ != NULL) {
			// Some graph dialogs want to handle zooming themselves
			if (pos.y() > (r.top() + r.bottom()) / 2)
				GraphDialog_->decreaseScale();
			else
				GraphDialog_->increaseScale();
		}
		else {
			// others don't
			if (pos.y() > (r.top() + r.bottom()) / 2)
				factor = 0.8;
			else
				factor = 1.2;
			dataToken_->setYMax(dataToken_->getYMax()*factor);
			dataToken_->setYMin(dataToken_->getYMin()*factor);
			GraphPane_->repaint();			

		}

		lock_=0;
	}
}

