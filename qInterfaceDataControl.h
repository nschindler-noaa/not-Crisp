#ifndef _qInterfaceDataControl_h_
#define _qInterfaceDataControl_h_


#ifdef WIN32
/* TODO: handle this issue more elegantly. */
#pragma warning( disable: 4284 )
/* warning C4284: return type for 'std::reverse_iterator... */
#endif

#include "qDatContainer.h"
#include <qptrlist.h>

#include <qobject.h>
#include "qGlobalEvents.h"
#include "log.h"
#include "var.h"



//this is a utlitu class to help in comparing items in a pointer list
//since they are all dat contrianers we override the function, and compare

template <class type> class compQPtrList: public QPtrList<type>{
	
protected:
	virtual int compareItems ( QPtrCollection::Item item1, QPtrCollection::Item item2 ){
				
		qDatContainer *df1 = (qDatContainer *)item1;
		qDatContainer *df2 = (qDatContainer *)item2;

		int result=0;

		if(df1->lengthStringVec() == df2->lengthStringVec()){
			for(int i=0;i<(df1->nStrComp(-1) ? df1->nStrComp(-1) : df1->lengthStringVec());i++){
				result = df1->getString(i).compare(df2->getString(i));
				if(result) 
					return result;
			}
			if(df1->lengthStringVec()){
					return 0;
			}
		}
		else{
			if(df1->lengthStringVec() < df2->lengthStringVec())
				return -1;
			if(df1->lengthStringVec() > df2->lengthStringVec())
				return 1;
		}
		
		
		if(df1->lengthIntVec() == df2->lengthIntVec()){
			for(int i=0;i<(df1->lengthIntVec());i++){
				if(df1->getInt(i) < df2->getInt(i))
					return -1;
				if(df1->getInt(i) > df2->getInt(i))
					return 1;
			}
		}
		else{
			if(df1->lengthIntVec() < df2->lengthIntVec())
				return -1;
			if(df1->lengthIntVec() > df2->lengthIntVec())
				return 1;
		}

		return 0;
	}
};


/*
 * This is the abstract base class for interface data controllers. IDCs
 *  access data of a given type from the underlying data base. IDCs are
 *  static objects (one per type of data) which may be accessed by several
 *  mediators at once. IDCs maintain caches of edited data; a seperate
 *  operation is needed to commit the cached data to the underlying data base,
 *  or conversely to discard the cached data without changing the underlying
 *  data base. In addition, other objects can be notified when the cached
 *  data changes (this allows multiple dialogs to display the same data, with
 *  live updating).
 */
class qInterfaceDataControl : public QObject {
	
	Q_OBJECT

protected:

	compQPtrList <qDatContainer>bt;/* A collection of datContainers */
		
	int helpPage_;
	int lock;

public:

	virtual qDatContainer *NewData()=0;
	virtual int FillData(qDatContainer *dat)=0;
	virtual void ApplyData(qDatContainer *dat);

	virtual void ApplyCache( void );

	virtual void ClearCache( void );

	virtual void ClearCache( qDatContainer *dat );

	qInterfaceDataControl( void );
	virtual ~qInterfaceDataControl();

	//signal_null *evInvalidate(){ return &evInvalidate_; }
    void invalidate( void ) { if(lock) 
									emit evInvalidate_(); }

	/* Pointer to a page in the help file. */
	int getHelpPage( void ) { return helpPage_; }
	void setHelpPage( int i ) { helpPage_ = i; }
	void setLock (int l) { lock = l; }   // 0= locked 1 = unlocked


	/* These two functions simplify use of variable_parameter-based stochastic
	 * parameters. They take care of unusual cases (no distribution, etc.)
	 * in a reasonable way, behind the scenes. */
	/* Fill in data from a broken stick parameter */
	static int FillBrokenStickData(qDatContainer *dat, int offset, float *variable);
	/* Apply data for a broken stick parameter. The last parameter may be used
	 * to keep a pointer to the distribution if non-NULL. */
	static void ApplyBrokenStickData(qDatContainer *dat, int offset, float *variable, variable_parameter **dist = NULL);

signals:
	void evInvalidate_();
protected slots:
	
	/* How to get the message from the global "purge all IDCs" event. */
	virtual void cbPurge( );

};


#endif


