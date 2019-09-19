/* qDamReachGDT.h
 *  This file contains several classes derived from qGraphDataToken. These
 *   classes know how to fill themselves with data of a certain type. They are
 *   sharing a file because each class is small and there are a lot of classes.
 */

#ifndef __QDAMREACHGDT_H__
#define __QDAMREACHGDT_H__

#include "crisp.h"
#include "qGraphDataToken.h"

class qDamFlowGDT: public qGraphDataToken {
    friend class qReachFlowGDT;

protected:
	static QPtrList<qDamFlowGDT> * openInstances_;
	static double groupYMin_, groupYMax_;

public:
	qDamFlowGDT( const char *sourceStr );
	~qDamFlowGDT();

	/* Virtual function called on base class to fill GraphDataToken with data. */
	virtual int Fill( const char *feature = NULL, const char *species = NULL );

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

class qReachFlowGDT: public qGraphDataToken {

protected:
	static QPtrList<qReachFlowGDT> * openInstances_;
	static double groupYMin_, groupYMax_;


public:
	qReachFlowGDT( const char *sourceStr );
	~qReachFlowGDT();

	/* Virtual function called on base class to fill GraphDataToken with data. */
	virtual int Fill( const char *feature = NULL, const char *species = NULL );

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

class qDamSpillGDT: public qGraphDataToken {
protected:

	static QPtrList<qDamSpillGDT> * openInstances_;
	static double groupYMin_, groupYMax_;

public:
	qDamSpillGDT( const char *sourceStr );
	~qDamSpillGDT();

	/* Virtual function called on base class to fill GraphDataToken with data. */
	virtual int Fill( const char *feature = NULL, const char *species = NULL );

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

class qDamNSatGDT;

class qDamNSatGDT: public qGraphDataToken {

	friend class qReachNSatGDT;

protected:
	//static signal_null evScaleChanged_;
	static QPtrList<qDamNSatGDT> * openInstances_;
	static double groupYMin_, groupYMax_;

public:
	qDamNSatGDT( const char *sourceStr );
	~qDamNSatGDT();

	/* Virtual function called on base class to fill GraphDataToken with data. */
	virtual int Fill( const char *feature = NULL, const char *species = NULL );

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

class qReachNSatGDT: public qGraphDataToken {

protected:
	//static signal_null evScaleChanged_;
	static QPtrList<qReachNSatGDT> * openInstances_;
	static double groupYMin_, groupYMax_;

public:
	qReachNSatGDT( const char *sourceStr );
	~qReachNSatGDT();

	/* Virtual function called on base class to fill GraphDataToken with data. */
	virtual int Fill( const char *feature = NULL, const char *species = NULL );

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

class qDamWaterTempGDT: public qGraphDataToken {
    friend class qReachWaterTempGDT;
    
protected:
	static QPtrList<qDamWaterTempGDT> * openInstances_;
	static double groupYMin_, groupYMax_;

public:
	qDamWaterTempGDT( const char *sourceStr );
	~qDamWaterTempGDT();

	/* Virtual function called on base class to fill GraphDataToken with data. */
	virtual int Fill( const char *feature = NULL, const char *species = NULL );

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

class qReachWaterTempGDT: public qGraphDataToken {
protected:
	static QPtrList<qReachWaterTempGDT> * openInstances_;
	static double groupYMin_, groupYMax_;

public:
	qReachWaterTempGDT( const char *sourceStr );
	~qReachWaterTempGDT();

	/* Virtual function called on base class to fill GraphDataToken with data. */
	virtual int Fill( const char *feature = NULL, const char *species = NULL );

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

class qDamTurbidityGDT: public qGraphDataToken {
    friend class qReachTurbidityGDT;
    
protected:
	static QPtrList<qDamTurbidityGDT> * openInstances_;
	static double groupYMin_, groupYMax_;

public:
	qDamTurbidityGDT( const char *sourceStr );
	~qDamTurbidityGDT();

	/* Virtual function called on base class to fill GraphDataToken with data. */
	virtual int Fill( const char *feature = NULL, const char *species = NULL );

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

class qReachTurbidityGDT: public qGraphDataToken {
protected:
	static QPtrList<qReachTurbidityGDT> * openInstances_;
	static double groupYMin_, groupYMax_;

public:
	qReachTurbidityGDT( const char *sourceStr );
	~qReachTurbidityGDT();

	/* Virtual function called on base class to fill GraphDataToken with data. */
	virtual int Fill( const char *feature = NULL, const char *species = NULL );

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

class qReachVelGDT: public qGraphDataToken {
protected:
	static QPtrList<qReachVelGDT> * openInstances_;
	static double groupYMin_, groupYMax_;

public:
	qReachVelGDT( const char *sourceStr );
	~qReachVelGDT();

	/* Virtual function called on base class to fill GraphDataToken with data. */
	virtual int Fill( const char *feature = NULL, const char *species = NULL );

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


#endif
