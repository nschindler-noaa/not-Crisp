#ifndef _qDatContainer_
#define _qDatContainer_

//#include <rw/collect.h>
//#include <rw/tvvector.h>
//#include <rw/cstring.h>
#include <qvaluevector.h>

class qDatContainer {


 protected:

  	QValueVector<double> dVec_;
	QValueVector<int> iVec_;
	QValueVector<QString> strVec_;

	int lock_;
	//RWClassID classId_;
	int nStrComp_;  /* number of strings signif. in comparison (0 = all) */
	int isDirty_;	/* 1 = has been edited and not stored; 0 = hasn't */

  public:

	qDatContainer(int is, int ds);
	qDatContainer(int ds, int ss=0, int is=0);
	
	//virtual RWClassID isA() const;

	//virtual int compareItems(const qDatContainer *) const;

	virtual int		  getInt(int i) const;
	virtual double	  getDouble(int i);
	virtual QString   getString(int i);


	virtual int setInt(int i, int val); 
	virtual int setDouble(int i, double val, int delta=0); 
	virtual int setString(int i, const char *val);

	virtual int lengthIntVec(void) const;
	virtual int lengthDoubleVec(void) const;
	virtual int lengthStringVec(void) const; 

	int nStrComp( int n = -1 );

	int lock(int i=-1);

	int isDirty( void ) { return isDirty_; };
	void setDirty( void ) { isDirty_ = 1; };
	void clearDirty( void ) { isDirty_ = 0; };

};

#endif

