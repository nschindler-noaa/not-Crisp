/* qEditGraphGDT.h
 * This file contains a lot of small classes which define graph data
 *  token types derived from ZatEditGraphDataToken.
 */

#ifndef __QEDITGRAPHGDT_H__
#define __QEDITGRAPHGDT_H__

#include "crisp.h"
#include "qEditGraphDataToken.h"

class qHeadwaterFlowGDT : public qEditGraphDataToken {
protected:
	//static signal_null evScaleChanged_;
	static QPtrList<qHeadwaterFlowGDT> * openInstances_;
	static double groupYMin_, groupYMax_;

public:
	qHeadwaterFlowGDT( const char *id );
	~qHeadwaterFlowGDT();

	virtual void Apply( void );
	virtual void ApplyAll( void );
	virtual void Reset( void );
	virtual void ResetAll( void );
	virtual void SetData( void );
	void prodBasin( void );

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
	//virtual signal_null *evScaleChanged( void );
};

class qHeadwaterTempGDT : public qEditGraphDataToken {
protected:
	//static signal_null evScaleChanged_;
	static QPtrList<qHeadwaterTempGDT> * openInstances_;
	static double groupYMin_, groupYMax_;

public:
	qHeadwaterTempGDT( const char *id );
	~qHeadwaterTempGDT();

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
	//virtual signal_null *evScaleChanged( void );
};

class qHeadwaterTurbidityGDT : public qEditGraphDataToken {
protected:
	//static signal_null evScaleChanged_;
	static QPtrList<qHeadwaterTurbidityGDT> * openInstances_;
	static double groupYMin_, groupYMax_;

public:
	qHeadwaterTurbidityGDT( const char *id );
	~qHeadwaterTurbidityGDT();

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
	//virtual signal_null *evScaleChanged( void );
};

class qBasinVolumeGDT : public qEditGraphDataToken {
protected:
	//static signal_null evScaleChanged_;
	static QPtrList<qBasinVolumeGDT> * openInstances_;
	static double groupYMin_, groupYMax_;

public:
	qBasinVolumeGDT( const char *id );
	~qBasinVolumeGDT();

	virtual void Apply( void );
	virtual void ApplyAll( void );
	virtual void Reset( void );
	virtual void ResetAll( void );
	virtual void SetData( void );

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
	//virtual signal_null *evScaleChanged( void );
};

class qEditDamFlowGDT : public qEditGraphDataToken {
protected:
	//static signal_null evScaleChanged_;
	static QPtrList<qEditDamFlowGDT> * openInstances_;
	static double groupYMin_, groupYMax_;

public:
	qEditDamFlowGDT( const char *id );
	~qEditDamFlowGDT();

	virtual void Apply( void );
	virtual void ApplyAll( void );
	virtual void Reset( void );
	virtual void ResetAll( void );
	virtual void SetData( void );
	virtual int isDirty( void );

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
//	virtual signal_null *evScaleChanged( void );
};

class qFlowLossGDT : public qEditGraphDataToken {
protected:
	//static signal_null evScaleChanged_;
	static QPtrList<qFlowLossGDT> * openInstances_;
	static double groupYMin_, groupYMax_;

public:
	qFlowLossGDT( const char *id );
	~qFlowLossGDT();

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
//	virtual signal_null *evScaleChanged( void );
};

class qElevationChangeGDT : public qEditGraphDataToken {
protected:
	//static signal_null evScaleChanged_;
	static QPtrList<qElevationChangeGDT> * openInstances_;
	static double groupYMin_, groupYMax_;

public:
	qElevationChangeGDT( const char *id );
	~qElevationChangeGDT();

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
//	virtual signal_null *evScaleChanged( void );
};

#endif
