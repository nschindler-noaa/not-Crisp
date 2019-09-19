/* qPassHistGDT.h
 * This file defines the derived classes for GraphDataTokens for the various
 *  types of passage histograms. All of them are in the same file to
 *  prevent needless proliferation of files. All are derived from
 *  qDamPassHistGDT via a common intermediate class ZasBasePassHistGDT.
 */

#ifndef __QPASSHISTGDT_H__
#define __QPASSHISTGDT_H__

#include "crisp.h"
#include "qGraphDataToken.h"
#include <qvaluevector.h>

class qBasePassHistGDT: public qGraphDataToken {
public:
	qBasePassHistGDT( const char *graphTitle, const char *sourceStr, 
		                int graphType, int evTrigger, 
						int graphDataRange = GR_POSITIVE);
	~qBasePassHistGDT();

	/* This routine fills up a passage histogram with data. A virtual
	 *  function fetches the data - derived classes control this. */
	virtual void FillHistogram();
	virtual float HistData( struct rls_seg *rls_seg, int i ) = 0;

	/* Virtual function called on base class to fill GraphDataToken with data. */
	virtual int Fill( const char *feature = NULL, const char *species = NULL );
	
	/* Try to be a little more clever with Save As... */
	virtual void writeHeader( FILE **fpp );
};


/* Each derived class implements a particular type of passage histogram. */

class qDamPassHistGDT: public qBasePassHistGDT {
	friend class qReachPassHistGDT;
protected:
	//static signal_null evScaleChanged_;
	static QPtrList<qDamPassHistGDT> * openInstances_;
	static double groupYMin_, groupYMax_;

public:
	qDamPassHistGDT( const char *sourceStr );
	~qDamPassHistGDT();
	virtual float HistData( struct rls_seg *rls_seg, int i );

	/*  These values are used to implement the "grouping" of Y-scale values.
	 *  Each derived type of graph data token will maintain a single, static
	 *  YMin and YMax, and a single event which is signalled when those 
	 *  values change.
	 */
	virtual void setGroupYMin( double d );
	virtual void setGroupYMax( double d );
	virtual double getGroupYMin( void );
	virtual double getGroupYMax( void );
	//virtual signal_null *evScaleChanged( void );
	virtual void cbGroupYScaleChanged() ;  //event trigger for group
};

class qReachPassHistGDT: public qBasePassHistGDT {
protected:
	static QPtrList<qReachPassHistGDT> * openInstances_;
	static double groupYMin_, groupYMax_;
public:
	qReachPassHistGDT( const char *sourceStr );
	~qReachPassHistGDT();
	virtual float HistData( struct rls_seg *rls_seg, int i );

	/*  These values are used to implement the "grouping" of Y-scale values.
	 *  Each derived type of graph data token will maintain a single, static
	 *  YMin and YMax, and a single event which is signalled when those 
	 *  values change.
	 */
	virtual void setGroupYMin( double d );
	virtual void setGroupYMax( double d );
	virtual double getGroupYMin( void );
	virtual double getGroupYMax( void );
	//virtual signal_null *evScaleChanged( void );
	virtual void cbGroupYScaleChanged() ;  //event trigger for group
};

class qReachLengthHistGDT: public qBasePassHistGDT {
protected:
	static QPtrList<qReachLengthHistGDT> * openInstances_;
	//static signal_null evScaleChanged_;
	static double groupYMin_, groupYMax_;
	QValueVector<double> numberArray_;

public:
	qReachLengthHistGDT( const char *sourceStr );
	~qReachLengthHistGDT();
	virtual void FillHistogram();
	virtual float HistData( struct rls_seg *rls_seg, int i );

	/*  These values are used to implement the "grouping" of Y-scale values.
	 *  Each derived type of graph data token will maintain a single, static
	 *  YMin and YMax, and a single event which is signalled when those 
	 *  values change.
	 */
	virtual void setGroupYMin( double d );
	virtual void setGroupYMax( double d );
	virtual double getGroupYMin( void );
	virtual double getGroupYMax( void );
	//virtual signal_null *evScaleChanged( void );
	virtual void cbGroupYScaleChanged() ;  //event trigger for group
};

#ifdef ROUTE_TRACKING

class qBypassPassHistGDT: public qBasePassHistGDT {
protected:
	static QPtrList<qBypassPassHistGDT> * openInstances_;
	//static signal_null evScaleChanged_;
	static double groupYMin_, groupYMax_;

public:
	qBypassPassHistGDT( const char *sourceStr );
	~qBypassPassHistGDT();
	virtual float HistData( struct rls_seg *rls_seg, int i );

	/*  These values are used to implement the "grouping" of Y-scale values.
	 *  Each derived type of graph data token will maintain a single, static
	 *  YMin and YMax, and a single event which is signalled when those 
	 *  values change.
	 */
	virtual void setGroupYMin( double d );
	virtual void setGroupYMax( double d );
	virtual double getGroupYMin( void );
	virtual double getGroupYMax( void );
	//virtual signal_null *evScaleChanged( void );
	virtual void cbGroupYScaleChanged() ;  //event trigger for group
};

class qSluicewayPassHistGDT: public qBasePassHistGDT {
protected:
	static QPtrList<qSluicewayPassHistGDT> * openInstances_;
	static double groupYMin_, groupYMax_;

public:
	qSluicewayPassHistGDT( const char *sourceStr );
	~qSluicewayPassHistGDT();
	virtual float HistData( struct rls_seg *rls_seg, int i );

	/*  These values are used to implement the "grouping" of Y-scale values.
	 *  Each derived type of graph data token will maintain a single, static
	 *  YMin and YMax, and a single event which is signalled when those 
	 *  values change.
	 */
	virtual void setGroupYMin( double d );
	virtual void setGroupYMax( double d );
	virtual double getGroupYMin( void );
	virtual double getGroupYMax( void );
	virtual void cbGroupYScaleChanged() ;  //event trigger for group
};

class qSpillwayPassHistGDT: public qBasePassHistGDT {
protected:
	static QPtrList<qSpillwayPassHistGDT> * openInstances_;
	//static signal_null evScaleChanged_;
	static double groupYMin_, groupYMax_;

public:
	qSpillwayPassHistGDT( const char *sourceStr );
	~qSpillwayPassHistGDT();
	virtual float HistData( struct rls_seg *rls_seg, int i );

	/*  These values are used to implement the "grouping" of Y-scale values.
	 *  Each derived type of graph data token will maintain a single, static
	 *  YMin and YMax, and a single event which is signalled when those 
	 *  values change.
	 */
	virtual void setGroupYMin( double d );
	virtual void setGroupYMax( double d );
	virtual double getGroupYMin( void );
	virtual double getGroupYMax( void );
	//virtual signal_null *evScaleChanged( void );
	virtual void cbGroupYScaleChanged() ;  //event trigger for group
};

class qRSWPassHistGDT: public qBasePassHistGDT {
protected:
	static QPtrList<qRSWPassHistGDT> * openInstances_;
	static double groupYMin_, groupYMax_;

public:
	qRSWPassHistGDT( const char *sourceStr );
	~qRSWPassHistGDT();
	virtual float HistData( struct rls_seg *rls_seg, int i );

	/*  These values are used to implement the "grouping" of Y-scale values.
	 *  Each derived type of graph data token will maintain a single, static
	 *  YMin and YMax, and a single event which is signalled when those 
	 *  values change.
	 */
	virtual void setGroupYMin( double d );
	virtual void setGroupYMax( double d );
	virtual double getGroupYMin( void );
	virtual double getGroupYMax( void );
	virtual void cbGroupYScaleChanged() ;  //event trigger for group
};

class qTurbinePassHistGDT: public qBasePassHistGDT {
protected:
	static QPtrList<qTurbinePassHistGDT> * openInstances_;
	//static signal_null evScaleChanged_;
	static double groupYMin_, groupYMax_;

public:
	qTurbinePassHistGDT( const char *sourceStr );
	~qTurbinePassHistGDT();
	virtual float HistData( struct rls_seg *rls_seg, int i );

	/*  These values are used to implement the "grouping" of Y-scale values.
	 *  Each derived type of graph data token will maintain a single, static
	 *  YMin and YMax, and a single event which is signalled when those 
	 *  values change.
	 */
	virtual void setGroupYMin( double d );
	virtual void setGroupYMax( double d );
	virtual double getGroupYMin( void );
	virtual double getGroupYMax( void );
//	virtual signal_null *evScaleChanged( void );
	virtual void cbGroupYScaleChanged() ;  //event trigger for group
};

class qTransportPassHistGDT: public qBasePassHistGDT {
protected:
	static QPtrList<qTransportPassHistGDT> * openInstances_;
//	static signal_null evScaleChanged_;
	static double groupYMin_, groupYMax_;

public:
	qTransportPassHistGDT( const char *sourceStr );
	~qTransportPassHistGDT();
	virtual float HistData( struct rls_seg *rls_seg, int i );

	/*  These values are used to implement the "grouping" of Y-scale values.
	 *  Each derived type of graph data token will maintain a single, static
	 *  YMin and YMax, and a single event which is signalled when those 
	 *  values change.
	 */
	virtual void setGroupYMin( double d );
	virtual void setGroupYMax( double d );
	virtual double getGroupYMin( void );
	virtual double getGroupYMax( void );
//	virtual signal_null *evScaleChanged( void );
	virtual void cbGroupYScaleChanged() ;  //event trigger for group
};

#endif /* ROUTE_TRACKING */

#endif /* __qPASSHISTGDT_H__ */

