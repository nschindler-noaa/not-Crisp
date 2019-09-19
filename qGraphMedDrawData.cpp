
#include "qEditGraphDataToken.h"
#include "qGraphMedDrawData.h"
#include "qGraphDialog.h"
#include "qGraphMedTypes.h"
#include "log.h"
#include <stdlib.h>

qGraphMedDrawData::qGraphMedDrawData(qGraphPane *gp,
										 qEditGraphDataToken *edt) 
			: qGraphSubMediator(gp) {
	edt_ = edt;
	pSave_ = QPoint( -1, -1 );
	RegisterEvents();
}

qGraphMedDrawData::~qGraphMedDrawData(){
	DetachEvents();
}


int qGraphMedDrawData::isA(){
	return GMT_DRAWDATA;
}

	
void qGraphMedDrawData::RegisterEvents(){
	connect(GraphPane_, SIGNAL( evMouseMovedLButtonPressed(QPoint)), this, SLOT(evMouseMovedLButtonPressed(QPoint)));
	connect(GraphPane_, SIGNAL( evLButtonPressed(QPoint)), this, SLOT(evLButtonPressed(QPoint)));
	connect(GraphPane_, SIGNAL( evMouseMovedRButtonPressed(QPoint)), this, SLOT(evMouseMovedRButtonPressed(QPoint)));
	connect(GraphPane_, SIGNAL( evLButtonAreaSelection(QRect)), this, SLOT(evLButtonAreaSelection(QRect)));
	connect(GraphPane_, SIGNAL( evRButtonAreaSelection(QRect)), this, SLOT(evRButtonAreaSelection(QRect)));

}

void qGraphMedDrawData::DetachEvents(){
}


void qGraphMedDrawData::evLButtonAreaSelection( QRect pos){
	edt_->SetData();
	GraphPane_->repaint();
	pSave_ = QPoint( -1, -1 );
}

/* The idea here is that a left click, followed by another left click, defines
 *  a rectangle of data which will be modified. The two points define the X
 *  extent. The first point defines the Y value. Doing a drag resets the saved
 *  first point.
 */
void qGraphMedDrawData::evLButtonPressed( QPoint pos ) {
	QRect r;
	QSize vTextSize;
	QPoint p1, p2;
	double dBaseVal;
	int i, nXPos, nXPos2;

	if(!lock_){
		lock_=1; 

		if (pSave_.x() == -1) {
			pSave_ = pos;
			return;
		}

		vTextSize = (GraphPane_->painter->fontMetrics()).size(SingleLine, "X",1);
	    r = GraphPane_->getGraphRegion(
                            vTextSize.width(), vTextSize.height() );
	
		if(r.right() == 0 || r.bottom() == 0 || 
						!edt_ || !edt_->dataArray_.count())
			return;


		p1 = pSave_;
		p2 = QPoint( pos.x(), pSave_.y() );
		if(r.contains(p2) ==TRUE && r.contains(p1) == TRUE){
			SetXYPercent(r, p1, p2);

			nXPos = int(dXPercent_[0]*edt_->getXMax()-1);
			nXPos2 = int(dXPercent_[1]*edt_->getXMax()-1);
			if((nXPos2 < (int)edt_->getXMax()) && (nXPos > (int)edt_->getXMin())){
				if(edt_->getGraphDataRange() == GR_POSITIVE)
					dBaseVal = edt_->getYMax();
				else if(edt_->getGraphDataRange() == GR_BOTH)
					dBaseVal = edt_->getYMax()-edt_->getYMin();
				else if(edt_->getGraphDataRange() == GR_NEGATIVE)
					dBaseVal = edt_->getYMin();
				else {
					log_msg(L_ERROR, "Bad graph data range\n");
					return;
				}
				if (nXPos2 < nXPos) {
					i = nXPos;
					nXPos = nXPos2;
					nXPos2 = i;
				}
				for (i = nXPos; i <= nXPos2; i++)
					edt_->dataArray_[i] = dYPercent_[0] * dBaseVal;
				edt_->SetDirty();
				edt_->SetData();
				GraphPane_->repaint();
				pSave_ = QPoint( -1, -1 );
			}		
		}
		lock_=0;
	}

}

void qGraphMedDrawData::evRButtonAreaSelection( QRect pos){
	GraphPane_->repaint();
}

void qGraphMedDrawData::evMouseMovedLButtonPressed(QPoint pos){

	QRect r;
	QSize vTextSize;
	QPoint p1, p2;
//	log_msg(1, "left button moved/pressed in Mediator");
	if(!lock_){
		lock_=1; 
		vTextSize = (GraphPane_->painter->fontMetrics()).size(SingleLine, "X",1);
	    r = GraphPane_->getGraphRegion(
                            vTextSize.width(), vTextSize.height() );

		r.setTop ( r.top()-1 );  //kludge
		if(r.right() == 0 || r.bottom() == 0 || 
			!edt_ || !edt_->dataArray_.count()){
			lock_=0;
			return;
		}
		p1 = pos;
		p2 = GraphPane_->getMousePosition(2);
		if(r.contains(p2) && r.contains(p1)){
			SetXYPercent(r, p1, p2);
			int i =0;
			int j =10;
			int nXPos = int(dXPercent_[0]*edt_->getXMax()-1);
			int nXPos2 = int(dXPercent_[1]*edt_->getXMax()-1);
			if((nXPos < (int)edt_->getXMax()) && (nXPos > (int)edt_->getXMin())){
			
				ModifyData(nXPos,nXPos2);
				GraphPane_->painter->begin(&(GraphPane_->pm), GraphPane_);
				GraphPane_->painter->setPen( QPen(white,1,SolidLine));
				GraphPane_->painter->fillRect(QRect(p1.x(),r.top()+1,p2.x()-p1.x(),r.bottom()-2-r.top()), QBrush(white));
				GraphPane_->painter->setPen( QPen(red,1,SolidLine));
				GraphPane_->painter->drawLine(p1,p2);
				GraphPane_->painter->end();
				bitBlt(GraphPane_, 0, 0, &(GraphPane_->pm));
			}		
		}
		pSave_ = QPoint( -1, -1 );
		lock_=0;

		
	}

}
void qGraphMedDrawData::ModifyData(int i,int j){
	int k;
	int p1=0, p2=1;
	double dBaseVal;

	if(edt_->getGraphDataRange() == GR_POSITIVE)
		dBaseVal = edt_->getYMax();
	else if(edt_->getGraphDataRange() == GR_BOTH)
		dBaseVal = edt_->getYMax()-edt_->getYMin();
	else if(edt_->getGraphDataRange() == GR_NEGATIVE)
		dBaseVal = edt_->getYMin();
	else {
		log_msg(L_ERROR, "Bad graph data range\n");
		return;
	}

	if(i==j){
		edt_->dataArray_[i] = 
		(dYPercent_[p1]*dBaseVal);
		edt_->SetDirty();
		return;	
	}
	if(i+1 == j || j+1 == i){
		edt_->dataArray_[i] = (dYPercent_[p1]*dBaseVal);
		edt_->dataArray_[j] = (dYPercent_[p2]*dBaseVal);
		edt_->SetDirty();
		return;	
	}
	if(i>j){
		k=i;i=j;j=k;p1=1;p2=0;
	}

	if(i<0) 
		i=0;
	if(j>(int)edt_->getXMax())
		j = (int)edt_->getXMax();

	for(int m=i;m<=j;m++)
		edt_->dataArray_[m] = 
			((dYPercent_[p1]+(dYPercent_[p2]-dYPercent_[p1])*double(m-i)/double(j-i))*dBaseVal);
	edt_->SetDirty();
}


void qGraphMedDrawData::SetXYPercent(const QRect &r, const QPoint &p1, const QPoint &p2){

		//Find out what position it is at in percentages.		
		double ypos,xpos;
		if(!r.width() || !r.height())
			return;
		QPoint pos = p1;
		for(int i=0;i<2;i++){
			ypos = double(pos.y()-r.top())/(double)r.height();
			xpos = double(pos.x()-r.left())/(double)r.width();
			if(pos.y() < r.top())
				ypos=0.0;
			if(pos.y() > r.bottom())
				ypos=1.0;
			if(pos.x() > r.right())
				xpos=1.0;
			if(pos.x() < r.left())
				xpos=0.0;
			
			if(edt_->getGraphDataRange() == GR_POSITIVE){
				ypos = 1.0-ypos;
			}
			else if(edt_->getGraphDataRange() == GR_BOTH)
				ypos = 0.5-ypos;
			dXPercent_[i] = xpos;
			dYPercent_[i] = ypos;
			pos = p2;
		}

}

void qGraphMedDrawData::evMouseMovedRButtonPressed( QPoint pos){
	/* Does nothing at present. */
}

