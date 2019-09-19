
#include "qGraphDataToken.h"
#include "qGraphMedDrawLines.h"
#include "qGraphMedTypes.h"
#include "qGlobalEvents.h"
#include <qpainter.h>
#include "log.h"

int qGraphMedDrawLines::isA(){
	return GMT_DRAWLINES;
}


void qGraphMedDrawLines::evDraw(){

							 	
	QRect r, rf;
	QPoint p1, p2, p3, p4;
	int x1, x2, y, yzero, i, j, k, ny, nlin, histMode;
	double yy, a, b, incr, left, ymax, ymin;
	qGraphDataToken *dataToken_;
	
	QSize vTextSize;

	if(!lock_){
		lock_=1; 
		
		dataToken_ = GraphPane_->getDataToken();
        histMode = dataToken_->getHistogramMode();
	
		vTextSize = (GraphPane_->painter->fontMetrics()).size(SingleLine, "X",1);
		
		r = GraphPane_->getClipedGraphRegion(
                            vTextSize.width(), vTextSize.height() );
		//no need to erase, this is already done for us
		//GraphPane_->painter->fillRect(r, white);
		if(r.right() <= r.left() || r.bottom() <= r.top() ||
				!dataToken_ || !dataToken_->dataArray_.count() ||
				dataToken_->getYMax() == dataToken_->getYMin()) {
			GraphPane_->resetCliping();
           	lock_=0;
			return;
		}
		
		// Derive scaling values to convert to pixel coordinates.
	    ny = dataToken_->getNumElementsPerLine();
        if (histMode)
		    incr = double(r.width())/double(ny);
        else
		    incr = double(r.width())/double(ny-1);
		left = double(r.left());

		ymax = dataToken_->getYMax();
		ymin = dataToken_->getYMin();
		
		a = -(r.height()) / (ymax - ymin);
		b = r.bottom() + ymin * (r.height()) / (ymax - ymin);
		
		if(dataToken_->getGraphDataRange() == GR_BOTH){
			GraphPane_->painter->setPen( QPen(black,1,SolidLine));
			GraphPane_->painter->setBrush( black);
			GraphPane_->painter->moveTo(r.left(),(int)b);
			GraphPane_->painter->lineTo(r.right(),(int)b);		
		}
	
        if (dataToken_->getNumLines() == 1 ||
            (GraphPane_->drawMethod() != CONNECTDOTS &&
             GraphPane_->drawMethod() != 0 /* default */))
        {
		for(j = 0; j < dataToken_->getNumLines(); j++){
			GraphPane_->painter->setPen(getPen(j));
			GraphPane_->painter->setBrush(getBrush(j));

			if (GraphPane_->drawMethod() != CONNECTDOTS &&
				GraphPane_->drawMethod() != 0) {
				/* The CITYSCAPE, HOLLOWBARS, and FILLEDBARS methods
				 *  are closely related - they all use the same corner
				 *  points and just differ in how they outline and fill
				 *  the rectangles.
				 */
				yzero = (int)(b);
				for (i = 0; i < ny; i++){
					if (i == 0)
						x1 = r.left();
                    else {
                        if (histMode)
						    x1 = int(left+i*incr);
                        else
						    x1 = int(left+(i-0.5)*incr);
                    }
					
					yy = dataToken_->at(i+ny*j) * a + b;
					if (yy > 30000.0)
						yy = 30000.0;
					if (yy < -30000.0)
						yy = -30000.0;
					y = int(yy);

					if (i == ny-1)
						x2 = r.right();
                    else {
                        if (histMode)
						    x2 = int(left+(i+1)*incr);
                        else
						    x2 = int(left+(i+0.5)*incr);
                    }

					p1 = QPoint( x1, y );
					p2 = QPoint( x2, y );

					if (GraphPane_->drawMethod() == CITYSCAPE) {

						if (i == 0) {
							p3= QPoint( x1, yzero );
							GraphPane_->painter->moveTo( p3 );
						}
						GraphPane_->painter->lineTo( p1 );
						GraphPane_->painter->lineTo( p2 );
						if (i == ny-1) {
							p3 = QPoint( x2, yzero );
							GraphPane_->painter->lineTo( p3 );
						}
					}
					else if (GraphPane_->drawMethod() == HOLLOWBARS) {
						p3 = QPoint( x1, yzero );
						p4 = QPoint( x2, yzero );
						GraphPane_->painter->moveTo( p3 );
						GraphPane_->painter->lineTo( p1 );
						GraphPane_->painter->lineTo( p2 );
						GraphPane_->painter->lineTo( p4 );
					}
					else /* if (GraphPane_->drawMethod() == FILLEDBARS) */ {
						if (y < yzero)
							rf = QRect( x1, y, x2-x1, yzero-y );
						else
							rf = QRect( x1, yzero, x2-x1, y-yzero );
						GraphPane_->painter->fillRect( rf, getBrush(j) );
					}
				}
			}
			else if (GraphPane_->drawMethod() == CONNECTDOTS ||
					 GraphPane_->drawMethod() == 0 /* default */ ) {
				for (i = 0; i < ny-1; i++){
					/* CONNECTDOTS is also the default right now */
					yy = dataToken_->at(i+ny*j) * a + b;
					if (yy > 30000.0)
						yy = 30000.0;
					if (yy < -30000.0)
						yy = -30000.0;
					p1 = QPoint( int(left+(double)i*incr), int(yy) );
					yy = dataToken_->at(i+1+ny*j) * a + b;
					if (yy > 30000.0)
						yy = 30000.0;
					if (yy < -30000.0)
						yy = -30000.0;
					p2 = QPoint( int(left+(double(i+1))*incr), int(yy) );
					GraphPane_->painter->moveTo(p1);
					GraphPane_->painter->lineTo(p2);
				}
			}
			
			//delete painter->popPen();
			//delete painter->popBrush();
		}
        }
        else {
            // Special hack for multiple lines, connect dots.
            // The "k" variable implements a candy-cane appearance
            //  when multiple lines are plotted on top of each other.
            QPen ppen[5];
            nlin = dataToken_->getNumLines();
		    for(j = 0; j < 5; j++)
                ppen[j] = getPen(j);

			for (i = 0; i < ny-1; i++){
            	for(j = 0; j < nlin; j++){
                    k = (i + j) % nlin;
                    GraphPane_->painter->setPen( ppen[k%5] );
					yy = dataToken_->at(i+ny*k) * a + b;
					if (yy > 30000.0)
						yy = 30000.0;
					if (yy < -30000.0)
						yy = -30000.0;
					p1 = QPoint( int(left+(double)i*incr), int(yy) );
					yy = dataToken_->at(i+1+ny*k) * a + b;
					if (yy > 30000.0)
						yy = 30000.0;
					if (yy < -30000.0)
						yy = -30000.0;
					p2 = QPoint( int(left+(double(i+1))*incr), int(yy) );
					GraphPane_->painter->moveTo(p1);
					GraphPane_->painter->lineTo(p2);
                    //painter->popPen();
                }
			}

  //          for (j = 0; j < 5; j++)
//                delete ppen[j];
            
        }
		GraphPane_->resetCliping();
		//Show data dirty dots
		if (dataToken_->showDots() && (dataToken_->isDirty()
					|| (!dataToken_->isEdit() 
                            && !GOutputsValid_[dataToken_->outputType()]))) {
			QRect vr;
			vr = GraphPane_->rect(); //getVisible(vr);
			QColor zc;
			if (dataToken_->isEdit())
				zc =  red;
			else
				zc = QColor( 255, 255, 0 );
			GraphPane_->painter->setPen(  QPen( zc, 1,SolidLine ));
			GraphPane_->painter->setBrush(  QBrush( zc ));
			
			GraphPane_->painter->drawEllipse( QRect( vr.left() , vr.top()  ,8,8) );
			GraphPane_->painter->drawEllipse( QRect( vr.right() - 8, vr.top() ,8,8) );
			GraphPane_->painter->drawEllipse( QRect( vr.left() , vr.bottom() - 8 ,8,8) );
			GraphPane_->painter->drawEllipse( QRect( vr.right() - 8, vr.bottom() - 8 ,8,8) );

			//delete pd->popPen();
			//delete pd->popBrush();
		}

		//pd->unlock();
		lock_=0;

	}
	//delete GraphPane_->painter; //done painting
}

/* NOTE: colors 1 & 2 are both blue - this is because graphs with three
 *  lines are always "mean, low, high" in Crisp Passage, so it makes sense
 *  to have the mean value be one color and the limits be another color.
 */
QPen qGraphMedDrawLines::getPen(int num){
    
    switch(num){
	default:
	case 0:
		return  QPen(red,1,SolidLine);
	case 1:
		return  QPen(blue,1,SolidLine);
	case 2:
		return  QPen(blue,1,SolidLine);
	case 3:
		return  QPen(black,1,SolidLine);
	case 4:
		return  QPen(green,1,SolidLine);
    }
}

QBrush qGraphMedDrawLines::getBrush(int num){
    
    switch(num){
    default:
    case 0:
		return QBrush(red);
	case 1:
		return QBrush(blue);
	case 2:
		return QBrush(blue);
    case 3:
		return QBrush(black);
    case 4:
		return QBrush(green);
    }
}
