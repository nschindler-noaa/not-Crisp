#include <stdio.h>

#ifdef WIN32
#include <io.h>
#endif
#ifdef SOLARIS2
#include <unistd.h>
#endif

#include <stdlib.h>
#include "qGraphDataToken.h"
#include "log.h"
#include <qfiledialog.h>
#include <qmessagebox.h>

qGraphDataToken::qGraphDataToken(const char *graphTitle, 
									 const char *sourceStr, 
									 int graphType, int evTrigger, 
									 int graphDataRange, int graphXAxisType )
 : graphType_(graphType), graphDataRange_(graphDataRange), graphTitle_(graphTitle),
	sourceStr_(sourceStr), evTrigger_(evTrigger)
    
{ 
	Y[0] = 0.0;
	Y[1] = 0.0;
	X[0] = 0.0;
	X[1] = 0.0;
	graphDataTypeYAxis_= GRT_DOUBLE;
	graphDataTypeXAxis_= graphXAxisType;
	extraStr_ = "";
	isDirtyBase_ = 0;
	helpPage_ = 1;
	isEdit_ = 0;
    outputType_ = 0;
	showDots_ = 1;
	blockUpdate_ = 0;	
	allowAutoscale_ = 1;
    X_axis_scale_ = 1.0;
    histogramMode_ = 0;
}

qGraphDataToken::~qGraphDataToken(){

}

/* This routine sets the Y min and max values for the data array. */
void qGraphDataToken::setYBounds( int forbidZeroRange )
{
	if (!allowAutoscale_)
		return;

	size_t i;
	Y[0] = Y[1] = 0.0;

	for (i = 0; i < dataArray_.count(); i++) {
		if((graphDataRange_ == GR_POSITIVE) && (Y[1] < dataArray_.at(i))) 
			Y[1] = dataArray_.at(i);
		if((graphDataRange_ == GR_NEGATIVE) && (Y[0] > dataArray_.at(i))) 
			Y[0] = dataArray_.at(i);
		if((graphDataRange_ == GR_BOTH) && (fabs(Y[1]) < fabs(dataArray_.at(i)))) {
			Y[1] = fabs(dataArray_.at(i));
			Y[0] = -1.0*Y[1];
		}
	}

	/* Some users of these objects don't ever want to see Ymin == Ymax */
	if (forbidZeroRange) {
		if (Y[0] == Y[1]) {
			switch (graphDataRange_) {
			case GR_POSITIVE:
				Y[1] = 10.0;
				break;
			case GR_NEGATIVE:
				Y[0] = -10.0;
				break;
			case GR_BOTH:
				Y[1] = 10.0;
				Y[0] = -10.0;
				break;
			}
		}
	}
}

void qGraphDataToken::insert(double data, int rangeUpdate){
	dataArray_.append(data);
	if(rangeUpdate && allowAutoscale_){
		if((graphDataRange_ == GR_POSITIVE) && (Y[1] < data)) 
			Y[1] = data;
		if((graphDataRange_ == GR_NEGATIVE) && (Y[0] > data)) 
			Y[0] = data;
		if((graphDataRange_ == GR_BOTH) && (fabs(Y[1]) < fabs(data))) {
			Y[1] = fabs(data);
			Y[0] = -1.0*Y[1];
		}
	}
}


/* Base class action for filling a GraphDataToken with data.
 *  NOTE that we don't want the bare base class action to be done
 *  without the derived class action, which actually gets data. So
 *  this routine returns a 0; the derived class routines should return 1 */
int qGraphDataToken::Fill( const char *feature, const char *species )
{
	if (feature != NULL)
		sourceStr_ = QString(feature);  /* remember where the data comes from. */
	return 0;
}

/* This routine gives the graph title string, which is a fixed title
 *  concatentated with the current data source location 
 *  - e.g. "Flow (kcfs) Bonneville Dam". This supports live update of
 *  GraphDataTokens.
 */
QString  qGraphDataToken::getGraphTitle()
{
	graphTitleStr_ = graphTitle_ + " " + sourceStr_;
	return graphTitleStr_;
 }


/* Base class action for saving data to a file. Derived classes may
 *  do this more cleverly. "w" is the window which the Save As...
 *  dialog will be a child of. */

static char *types={"Text Files (*.txt);;All Files (*.*)"  };

void qGraphDataToken::fileSave( QWidget *w ) {

	QString fName = QFileDialog::getSaveFileName ( QString::null, types, 
			w,"save dialog", "Save data as..") ;

	if(fName != QString::null){
		
		if (! (access( fName, 0 ))) {
			int answer = QMessageBox::warning ( w, "COMPASS", "File already exists, overwrite it?", 
				"&Cancel", "&OK",  QString::null, 1, -1 );
			if (answer != 1)
				return;		
		}	
		
		FILE *fp = fopen( fName.ascii(), "w" );
			
		if (fp == NULL){
			QMessageBox::warning( 0, "Unable to open file.", "Graph Save Error", 
						QMessageBox::Ok,  QMessageBox::NoButton);			
		}
		else {
			writeHeader( &fp );
			writeData( &fp );
			fclose( fp );
		}
	}

}

void qGraphDataToken::writeHeader( FILE **fpp )
{
	fprintf( *fpp, "%s\n", getGraphTitle().ascii() );
	if(getGraphXAxisLabel() != QString::null)  //X axis label isn't always set
	    fprintf( *fpp, "%s\n", getGraphXAxisLabel().ascii() );
	
	fprintf( *fpp, "X-axis    " );
    
	    QStringList::iterator it;
	for ( it = legendArray_.begin(); it != legendArray_.end(); ++it ){
        	log_msg( 0, (char*)(*it).ascii());
		fprintf( *fpp, "%s\t", (char*)(*it).ascii() );
	}
	fprintf( *fpp,"\n");	
		
}

void qGraphDataToken::writeData( FILE **fpp )
{
	int i, j;

	for (i = 0; i < numElementsPerLine_; i++) {
		if (!histogramMode_) fprintf( *fpp, "%6i    ", i );
		else fprintf( *fpp, "%6g    ", (X[1] - X[0]) / numElementsPerLine_ * (i + 1) + X[0] );
		for (j = 0; j < numLines_; j++)
			fprintf( *fpp, "%13.2f   ", dataArray_.at(i + j * numElementsPerLine_) );
		fprintf( *fpp, "\n" );
	}
}

/* These virtual functions access YMin/YMax values which are maintained
 *  by derived classes, and also an event defined by each derived class.
 *  These values are used to implement the "grouping" of Y-scale values.
 *  Each derived type of graph data token (e.g. reach passage graphs)
 *  will maintain a single, static YMin and YMax, and a single event
 *  which is signalled when those values change.
 * The base class provides default behavior for those derived classes which
 *  do not implement this function (principally equation graphs).
 */
void qGraphDataToken::setGroupYMin( double d ) {
}

void qGraphDataToken::setGroupYMax( double d ) {
}

double qGraphDataToken::getGroupYMin( void ) {
	return 0.0;
}

double qGraphDataToken::getGroupYMax( void ) {
	return 10.0;
}


void qGraphDataToken::cbGroupYScaleChanged(){ }
