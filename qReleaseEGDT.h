/* ZasReleaseEGDT.h
 *  This is a class derived from ZatEditGraphDataToken. This class
 *  uses the local list of "struct release" objects maintained by the
 *  ZatReleaseTool as the underlying data structure. This class redefines
 *  the ZatEditGraphDataToken virtual functions which access the underlying
 *  data structure.
 *
 * All ZatEditGraphDataTokens are assumed to contain a single line of
 *  DAYS_IN_SEASON data points.
 */

#ifndef __qRELEASEEGDT_H__
#define __qRELEASEEGDT_H__

class qReleaseTool;
#include "qEditGraphDataToken.h"

class qReleaseEGDT: public qEditGraphDataToken {
protected:
	qReleaseTool *relTool_;	// the release tool which created us.
	
public:

	qReleaseEGDT( qReleaseTool *tool, const char *graphTitle );
	virtual ~qReleaseEGDT();

	void RegisterEvents(void);
	virtual void DetachEvents(void);

	/* Methods to clear or apply some or all data in underlying IDC. */
	virtual void Apply( void );
	virtual void ApplyAll( void );
	virtual void Reset( void );
	virtual void ResetAll( void );
	virtual void SetData( void );
	virtual int Fill( const char *feature = NULL, const char *species = NULL );

	/* token is dirty if underlying release structure is dirty */
	virtual int isDirty( void );
	virtual void SetDirty( void );

	/* Is underlying data structure writable?
	 */
	int isWritable( void );
};
#endif
