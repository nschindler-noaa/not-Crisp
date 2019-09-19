#ifndef _qGraphDataToken_H_
#define _qGraphDataToken_H_

/* This is the base class for graph data tokens. The goal is that all
    GraphDataTokens will eventually be members of derived classes which
	will fill themselves with data via the "Fill" virtual function. */

#include <math.h>
#include <qptrlist.h>
#include <qvaluevector.h>
#include <qstringlist.h>
#include <qstring.h>
#include <qwidget.h>

#include "crisp_defs.h"

const int GR_POSITIVE = 0;
const int GR_NEGATIVE = 1;
const int GR_BOTH	  = 2;

const int GRT_DOUBLE  = 0;
const int GRT_INTEGER = 1;

class qGraphDataToken : public QObject {

	Q_OBJECT

protected:
	int graphType_;			 // Flow graph, spill etc

	int graphDataTypeXAxis_; // grt_integer, grt_double
	int graphDataTypeYAxis_; // grt_integer, grt_double
	int graphDataRange_;	 // gr_positive, gr_negative, gr_both
	int numLines_;
	int numElementsPerLine_;
	QString graphTitle_;
	QString graphTitleStr_;  // Fixed title plus source str.
	QString graphXAxisLabel_;
	QString graphYAxisLabel_;
	QString sourceStr_;	 // the name of the source(Bonneville Pool, Hells Canyon Dam,etc)
	QString extraStr_;     // extra info about data source (e.g. species).
	int	evTrigger_;			 // dam, headwater, reach etc
	double Y[2];	
	double X[2];
    
    // hack to rescale X-axis during display
    double X_axis_scale_;

	int isDirtyBase_;		// "dirtyness" may be changed in derived classes.

	int helpPage_;          // page number in help file (assigned by derived class)

	int isEdit_;            // 1 = display as editable; 0 = don't

    int outputType_;        // what type of output are we?

	int blockUpdate_;       // 1 = don't allow updating; 0 = normal case.

	int showDots_;          // 1 = show stale/dirty dots; 0 = don't

	int allowAutoscale_;    // 1 = allow setYBounds() to set Ymin & Ymax; 0 = don't

    int histogramMode_;     // 1 = histogram, 0 = normal data - x-axis is handled
                            //  slightly differently in ZasGraphMedDrawLines

public:

	QStringList legendArray_;
	QValueVector<double> dataArray_;

	qGraphDataToken(const char *graphTitle, const char *sourceStr, 
					int graphType, int evTrigger, 
					int graphDataRange = GR_POSITIVE,
					int graphXAxisType = GRT_INTEGER);
	virtual ~qGraphDataToken();

	QString getGraphTitle();
	QString getGraphXAxisLabel(){ return graphXAxisLabel_; }
	QString getGraphYAxisLabel(){ return graphYAxisLabel_; }
	QString getSourceStr(){ return sourceStr_; }
	QString getExtraStr(){ return extraStr_; }
	QString getTitleStr(){ return graphTitle_; }
	int getGraphType(){ return graphType_; }
	int getGraphDataTypeXAxis(){ return graphDataTypeXAxis_; }
	int getGraphDataTypeYAxis(){ return graphDataTypeYAxis_; }
	int getGraphDataRange(){ return graphDataRange_; }
	int getNumLines(){ return numLines_; }
	int getNumElementsPerLine(){ return numElementsPerLine_; }
	int getEventTrigger(){return evTrigger_;}
	int getHelpPage() { return helpPage_; }
	double getXMin(){ return X[0];}
	double getXMax(){ return X[1];}
	double getYMin(){ return Y[0];}
	double getYMax(){ return Y[1];}
    double getXAxisScale() { return X_axis_scale_; }
    int getHistogramMode() { return histogramMode_; };

	void setGraphTitle(const char *label){ graphTitle_ = label; }
	void setGraphXAxisLabel(const char *label){ graphXAxisLabel_ = label; }
	void setGraphYAxisLabel(const char *label){ graphYAxisLabel_ = label; }
	void setXMin(double val){ X[0] = val; }
	void setXMax(double val){ X[1] = val; }
	void setYMin(double val){ Y[0] = val; }
	void setYMax(double val){ Y[1] = val; }
    void setXAxisScale( double val ) { X_axis_scale_ = val; }
	void setHelpPage( int i ) { helpPage_ = i; }
	void setGraphType(int type){graphType_ = type;}
	void setNumLines(int lines){numLines_ = lines;}
	void setNumElementsPerLine(int num){numElementsPerLine_ = num; X[1] = (double)num;}
    void setHistogramMode( int i ) { histogramMode_ = i != 0; };

	double at(unsigned int index){ 
		if(index < dataArray_.count())
			return (dataArray_.at(index));
		return 0.0;
	}
	int count() {return dataArray_.count();}
	void insert(double data, int rangeUpdate=1);
	void insertLabel(const char *label){legendArray_.append( QString(label));}
	void clearLabels( void ) { legendArray_.clear(); }

	void setYBounds( int forbidZeroRange = 0 );

	/* Base class action for filling a GraphDataToken with data.
	 *  NOTE that we don't want the bare base class action to be done
	 *  without the derived class action, which actually gets data. So
	 *  this routine returns a 0; the derived class routines should return 1 */
	virtual int Fill( const char *feature = NULL, const char *species = NULL );

	/* Base class action for saving data to a file. Derived classes may
	 *  do this more cleverly. "w" is the window which the Save As...
	 *  dialog will be a child of. */
	virtual void fileSave( QWidget *w );
	virtual void writeHeader( FILE **fpp );
	virtual void writeData( FILE **fpp );

	/* Derived classes may have a concept that the token is "dirty" w.r.t
	 *  the underlying data structures. Base class just has a flag set by
	 *  someone else.
	 */
	virtual int isDirty( void ) { return isDirtyBase_; };
	virtual void setDirty( void ) { isDirtyBase_ = 1; };
	virtual void clearDirty( void ) { isDirtyBase_ = 0; };

	/* Are we an edit graph data token or an output only graph data token?
	 */
	int isEdit( void ) { return isEdit_; };
	void setIsEdit( int i ) { isEdit_ = i; };

    /* What type of output graph are we (0 .. nGOutputsValid_-1).
     */
    int outputType( void ) { return outputType_; };
    void setOutputType( int i ) { outputType_ = i; };

	/* Most data tokens will be updated automatically by mediators. Some
	 *  can only be updated by explicit user action - auto updating is
	 *  blocked.
	 */
	int blockUpdate( void ) { return blockUpdate_; };
	void setBlockUpdate( int i ) { blockUpdate_ = i; };

	/* Most data tokens show dots in the corners of the graph (red dots
	 *  for dirty input data, yellow dots for stale output data). Some
	 *  do not.
	 */
	int showDots( void ) { return showDots_; };
	void setShowDots( int i ) { showDots_ = i; };

	/* the setYBounds() routine may or may not do anything, depending on
	 *  whether the owning ZatGraphDialog has the "Fix" button on or off.
	 */
	int allowAutoscale( void ) { return allowAutoscale_; };
	void setAllowAutoscale( int i ) { allowAutoscale_ = i; };

	/* These virtual functions access YMin/YMax values which are maintained
	 *  by derived classes, and also an event defined by each derived class.
	 *  These values are used to implement the "grouping" of Y-scale values.
	 *  Each derived type of graph data token (e.g. reach passage graphs)
	 *  will maintain a single, static YMin and YMax, and a single event
	 *  which is signalled when those values change.
	 */
	virtual void setGroupYMin( double d );
	virtual void setGroupYMax( double d );
	virtual double getGroupYMin( void );
	virtual double getGroupYMax( void );
	virtual void cbGroupYScaleChanged() ;
	virtual void emitgroupScaleUpdated(){ emit groupScaleUpdated(); }
signals:
	virtual void groupScaleUpdated();
};

#endif	
											 

