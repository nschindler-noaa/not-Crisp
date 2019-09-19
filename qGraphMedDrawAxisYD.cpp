
#include "qGraphDataToken.h"
#include "qGraphMedDrawAxisYD.h"
#include "qGraphMedTypes.h"
#include "log.h"


int qGraphMedDrawAxisYD::isA(){
	return GMT_DRAWYAXIS;
}


void qGraphMedDrawAxisYD::evDraw(){
			
	QRect r;
	char strTemp[30];
	QPoint p1, p2;
	QSize vTextSize;
	qGraphDataToken *pDataToken;
	int iDivisions, iDivWidth, iDistanceBetweenDivisions;
	double dIncrement, dt;
	int j;

	if(!lock_){
		lock_=1; 
		
		pDataToken = GraphPane_->getDataToken();
	
		GraphPane_->painter->setPen(QPen(black,1,SolidLine));
		GraphPane_->painter->setBrush(black);
	  
		vTextSize = (GraphPane_->painter->fontMetrics()).size(SingleLine, "X",1);
	    r = GraphPane_->getGraphRegion(
                            vTextSize.width(), vTextSize.height() );
		if(r.right() <= r.left() || r.bottom() <= r.top() ||
                    !pDataToken || !pDataToken->dataArray_.count())
            goto err_exit;
		
        GraphPane_->painter->moveTo(r.topLeft());
		GraphPane_->painter->lineTo(r.bottomLeft());

		/* Base division of axis upon size of characters in current font. */
		iDistanceBetweenDivisions = vTextSize.height() * 5;
		iDivWidth = vTextSize.width() / 2;

		iDivisions = r.height()/iDistanceBetweenDivisions;
		if(iDivisions < 1){
			/* Hop around drawing code and go straight to cleanup. */
			goto err_exit;
		}

		dIncrement = (pDataToken->getYMax()-pDataToken->getYMin())/(double)iDivisions;

		for(j = 0; j <= iDivisions; j++){
				p1 = QPoint(r.left()-iDivWidth,	(int) (r.bottom()-((double)j/(double)iDivisions)*(double)r.height()));
				p2 = QPoint(r.left()+iDivWidth,	(int) (r.bottom()-((double)j/(double)iDivisions)*(double)r.height()));
				GraphPane_->painter->moveTo(p1);
				GraphPane_->painter->lineTo(p2);	  	
				dt = j*dIncrement+pDataToken->getYMin();
				if (fabs( dt ) > 1e10)
					sprintf( strTemp, "%.5e", dt );
				else
					sprintf( strTemp, "%.2f", dt );
				vTextSize = GraphPane_->painter->fontMetrics().size(SingleLine,strTemp,strlen(strTemp));
				GraphPane_->painter->drawText(p1+QPoint(-vTextSize.width()-vTextSize.height()/3,vTextSize.height()/2), strTemp);
		}

err_exit:
		lock_=0;
	}

}
