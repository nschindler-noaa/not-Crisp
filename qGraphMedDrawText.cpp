
#include "qGraphDataToken.h"
#include "qGraphMedDrawText.h"
#include "qGraphMedTypes.h"

#include "log.h"

int qGraphMedDrawText::isA(){
	return GMT_DRAWTEXT;
}


void qGraphMedDrawText::evDraw(){

	
	QRect r, rVis;
	QPoint p1, p2;
	int vTitleSize, vLabelSize; //, vCharSize;
	qGraphDataToken *pDataToken;
	int x, xTitle;

	if(!lock_){
		lock_=1; 
		
		pDataToken = GraphPane_->getDataToken();
		rVis = GraphPane_->rect();

		GraphPane_->painter->setPen(QPen(black,1,SolidLine));
		GraphPane_->painter->setBrush(black);
	    
	
		QSize vCharSize = (GraphPane_->painter->fontMetrics()).size(SingleLine, "X",1);
		r = GraphPane_->getGraphRegion( 
                            vCharSize.width(), vCharSize.height() );
	 
		if(r.right() <= r.left() || r.bottom() <= r.top() || 
                        !pDataToken || !pDataToken->dataArray_.count())
            goto err_exit;

		vTitleSize = GraphPane_->painter->fontMetrics().width(
							pDataToken->getTitleStr());
		vLabelSize = GraphPane_->painter->fontMetrics().width(
							pDataToken->getGraphYAxisLabel());

		xTitle = (rVis.width() - vTitleSize) / 2;
		x = 10 + vLabelSize + 5 * vCharSize.width();
		if (xTitle < x)
			xTitle = x;

		GraphPane_->painter->drawText(
			QPoint(xTitle, GraphPane_->painter->fontMetrics().height()), 
			pDataToken->getTitleStr().ascii());

		GraphPane_->painter->drawText(
			QPoint(10,GraphPane_->painter->fontMetrics().height()),
			pDataToken->getGraphYAxisLabel().ascii());

err_exit:	
		lock_=0;

	}

}
