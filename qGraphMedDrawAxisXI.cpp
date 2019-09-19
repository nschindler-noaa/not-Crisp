#include "qGraphDataToken.h"
#include "qGraphMedDrawAxisXI.h"
#include "qGraphMedTypes.h"
#include "log.h"


int qGraphMedDrawAxisXI::isA(){
	return GMT_DRAWXAXIS;
}


void qGraphMedDrawAxisXI::evDraw(){

	
	QRect r, pane;
	char strTemp[20];
	QPoint p1, p2;
	QSize vTextSize, vCharSize;
	qGraphDataToken *pDataToken;
	int iDivisions, iDivWidth, iDistanceBetweenDivisions;
	double dIncrement;
	int iSave, j, histMode;
	
	pDataToken = GraphPane_->getDataToken();
	if (pDataToken == NULL)
		return;
	histMode = pDataToken->getHistogramMode();
	QString rXLabel = pDataToken->getGraphXAxisLabel();

	if(!lock_){
		lock_=1; 
		
		GraphPane_->painter->setPen(QPen(black,1,SolidLine));
		GraphPane_->painter->setBrush(black);
	
		vCharSize = (GraphPane_->painter->fontMetrics()).size(SingleLine, "X",1);
        r = GraphPane_->getGraphRegion( 
                            vCharSize.width(), vCharSize.height() );
		pane =  GraphPane_->painter->window(); //get the entire graph pane rect

        // No graph area, or no data to plot: exit early.
		if(r.right() <= r.left() || r.bottom() <= r.top() || 
			                    !pDataToken->dataArray_.count())
            goto err_exit;

		GraphPane_->painter->moveTo(r.bottomLeft());
		GraphPane_->painter->lineTo(r.bottomRight());

		/* Base division of axis upon size of characters in current font. */
		iDistanceBetweenDivisions = vCharSize.width() * 10;
		iDivWidth = vCharSize.height() / 2;

        if (histMode) {
            iDivisions = pDataToken->getNumElementsPerLine()/4;
		    iDistanceBetweenDivisions = r.width()/iDivisions;
		    dIncrement = (pDataToken->getXMax()-pDataToken->getXMin())/(double)iDivisions;
        }
        else {
            iDivisions = r.width()/iDistanceBetweenDivisions;
		    if(iDivisions > (int)pDataToken->getXMax()){
			    iDivisions = (int)(pDataToken->getXMax());
			    iDistanceBetweenDivisions = r.width()/iDivisions;
		    }
		    if(iDivisions < 1){
			    /* Hop around drawing code and go straight to cleanup. */
			    goto err_exit;
		    }

		    dIncrement = (pDataToken->getXMax()-pDataToken->getXMin())/(double)iDivisions;
        }

        iSave = -100000000;
		for(j = 0; j <= iDivisions; j++){
			p1 = QPoint(r.left()+int(((double)j/(double)iDivisions)*(double)r.width()),r.bottom()-iDivWidth);
			p2 = QPoint(r.left()+int(((double)j/(double)iDivisions)*(double)r.width()),r.bottom()+iDivWidth);
            if (histMode) {
                if (p1.x() < iSave + iDistanceBetweenDivisions)
                    continue;
                else
                    iSave = p1.x();
            }
			GraphPane_->painter->moveTo(p1);
			GraphPane_->painter->lineTo(p2);
            if (pDataToken->getXAxisScale() == 1.0)
			    sprintf(strTemp, "%.0f", 
                            j*dIncrement+pDataToken->getXMin());
            else
			    sprintf(strTemp, "%.4f", 
                        (j*dIncrement+pDataToken->getXMin())
                         /** pDataToken->getXAxisScale()*/);
            vTextSize = GraphPane_->painter->fontMetrics().size(SingleLine, strTemp,strlen(strTemp));
			GraphPane_->painter->drawText(p2 + QPoint(-vTextSize.width()/2,vTextSize.height()), strTemp);
		}
		
	

		/* Label for graph X axis, if any. Make a rectangle below the x-axis numbers, and
		 *  center the label text in that rectangle. */
		if (rXLabel.length() > 0) {
			vTextSize = GraphPane_->painter->fontMetrics().size(SingleLine, rXLabel, rXLabel.length() );
			GraphPane_->painter->drawText( (r.left() + r.right())/2 - vTextSize.width(),
				                        pane.bottom() - vCharSize.height() + iDivWidth,
										rXLabel.ascii(), rXLabel.length() );
		}

err_exit:
		lock_=0;
	}

}
