#include "qGraphDataToken.h"
#include "qGraphMedPosition.h"
#include "qGraphDialog.h"
#include "qGraphMedTypes.h"
#include "crisp_defs.h"
#include "qUtils.h"
#include <qpainter.h>
#include "log.h"


int qGraphMedPosition::isA(){
	return GMT_POSITION;
}

	
void qGraphMedPosition::RegisterEvents(){
	//Store these for later use...
	if (GraphPane_->painter->isActive()) vTextSize = (GraphPane_->painter->fontMetrics()).size(SingleLine, "X",1);
	else vTextSize = QSize(10, 16); /* fontMetrics() is only defined while the painter is active, so here's the backup plan */
	connect(GraphPane_, SIGNAL(evMouseMoved(QPoint)),this, SLOT(evMouseMoved(QPoint)));
}

void qGraphMedPosition::DetachEvents(){

}

void qGraphMedPosition::evMouseMoved( QPoint pos){
	

	QRect r;
	QPoint p1, p2;
	qGraphDataToken *dataToken_;
	QString cstr;
	double dt;
	QSize vTextSize;

	if(!lock_){
		lock_=1; 

		

		dataToken_ = GraphPane_->getDataToken();
		//Use the values obtained in the constructor to improve performance
		r = GraphPane_->getClipedGraphRegion(vTextSize.width(),vTextSize.height());

		if(r.right() == 0 || r.bottom() == 0 || 
			!dataToken_ || !dataToken_->dataArray_.count()){
			GraphPane_->resetCliping();
			lock_=0;
			return;
		}
		
		double minVal, maxVal;
		
		maxVal = dataToken_->getYMax();
		minVal = dataToken_->getYMin();
		
		double ypos = double(pos.y()-r.top())/(double)r.height();
		double xpos = double(pos.x()-r.left())/(double)r.width();
		if(pos.y() < r.top())
			ypos=0.0;
		if(pos.y() > r.bottom())
			ypos=1.0;
		if(pos.x() > r.right())
			xpos=1.0;
		if(pos.x() < r.left())
			xpos=0.0;
		xpos = 1.0-xpos;
		ypos = 1.0-ypos;
		char str[40];

		if(dataToken_->getGraphDataTypeYAxis() == GRT_DOUBLE) {
			dt = ypos * (maxVal - minVal) + minVal;
			if (fabs(dt) > 1e10)
				sprintf(str, "Y: %.5e", dt);
			else
				sprintf(str, "Y: %.2f", dt);
		}
		else
			sprintf(str, "Y: %f", ypos*maxVal);
		if (gd_ != NULL)
			gd_->setPositionYLabel(str);
		if (ed_ != NULL)
			ed_->setPositionYLabel(str);

		if(dataToken_->getGraphDataTypeXAxis() == GRT_DOUBLE){
			sprintf(str, "X: %.2f", (1.0-xpos)*dataToken_->getXMax());
			
		}
		else{
			sprintf(str, "X: %f",
			        dataToken_->getXMin() + 0.5 +
					     (1.0-(double)xpos)
                         * (dataToken_->getXMax()-dataToken_->getXMin()));
		}
	
		if (gd_ != NULL)
			gd_->setPositionXLabel(str);
		if (ed_ != NULL)
		ed_->setPositionXLabel(str);

		/* Convert the x-position to a month/day date and set it.
		 *  If this graph really isn't based on days of the year, this
		 *  set will be ignored by qGraphDialog.
		 */
		int day = (int)(dataToken_->getXMin() + 0.5 +
			             (1.0 - xpos)
                         * (dataToken_->getXMax()-dataToken_->getXMin()));
		formatDate( day, &cstr );

		if (gd_ != NULL)
			gd_->setPositionDateLabel( cstr.ascii() );
		if (ed_ != NULL)
			ed_->setPositionDateLabel( cstr.ascii() );


		GraphPane_->resetCliping();
	
		lock_=0;

	}
	

}
