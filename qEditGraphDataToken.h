/* qEditGraphDataToken.h
 *  This is a class derived from ZatGraphDataToken. This is a base class
 *  for further derivations which will support user-editable graphs.
 *  This class provides an invalidation event, a dirty-flag mechanism
 *  for either updating or not updating the underlying IDC, and Apply
 *  and ApplyAll methods for causing the underlying IDC to update the
 *  underlying data base.
 *
 * All ZatEditGraphDataTokens are assumed to contain a single line of
 *  DAYS_IN_SEASON data points.
 */

#ifndef __QEDITGRAPHDATATOKEN_H__
#define __QEDITGRAPHDATATOKEN_H__

#include "qGraphDataToken.h"
#include "qIdcEditGraphBase.h"

class qEditGraphDataToken: public qGraphDataToken {

	Q_OBJECT

protected:
	//signal_null evInvalidate_; /* signals underlying IDC has changed. */
	///signal_null evFilled_;   /* signals Fill() routine called. */
	int dirty_;			/* 1 if EGDT data modified in rel. to IDC data. */
	qIdcEditGraphBase *idc_;  /* Our underlying IDC. */
	
public:

	qEditGraphDataToken(const char *graphTitle, const char *sourceStr, 
		             int graphType, int evTrigger, qIdcEditGraphBase *idc,
					 int graphDataRange = GR_POSITIVE);
	virtual ~qEditGraphDataToken();

	//signal_null *evInvalidate(){ return &evInvalidate_; }
	//signal_null *evFilled(){ return &evFilled_; }

	void RegisterEvents(void);
	virtual void DetachEvents(void);

	/* Methods to clear or apply some or all data in underlying IDC. */
	virtual void Apply( void );
	virtual void ApplyAll( void );
	virtual void Reset( void );
	virtual void ResetAll( void );
	virtual void SetData( void );
	virtual int Fill( const char *feature = NULL, const char *species = NULL );
	virtual void SetDirty( void ) {dirty_ = 1; };
	/* Try to be a little more clever with Save As... */
	virtual void writeHeader( FILE **fpp );

	/* token is dirty if underlying datContainer is dirty */
	virtual int isDirty( void );

	/* Receive notification of idc invalidations. */
public slots:
	virtual void cbInvalidate( void );
	//static void _cbInvalidate(void *msg);


signals:
	void evInvalidate_(); /* signals underlying IDC has changed. */
	void evFilled_();   /* signals Fill() routine called. */
};
#endif
