/* qIOHistGDT.h
 * This file defines a qGraphDataToken descendant that is set up for use
 * with the I/O tool (spreadsheet) GUI. This is needed since the I/O GUI
 * provides for finer selection of releases than just location and species
 * (namely, it allows for the user to select a single stock, although it
 * can theoretically be used to select a random assortments of releases). */

#ifndef	__qIOHISTGDT_H__
#define	__qIOHISTGDT_H__

#include "qPassHistGDT.h"
#include "qIOTool.h"

class qIODamPassageCell;
class qIODamWaterAverageCell;
class qIOCell;

/* qIODamPassageGDT is for showing passage over a year (currently just for scenario mode) */
class qIODamPassageGDT : public qBasePassHistGDT {
protected:
	qIOSheet *sheet;

	/* Note that since we're referencing by row and column, if the I/O sheet
	 * changes, then we'll be displaying the wrong data. Hopefully this won't
	 * happen.
	 *
	 * TODO: come up with a scheme that would allow the users to keep histograms
	 * open even after switching I/O sheets. This would cause problems with
	 * the way that they update automatically at the moment, though... */

	int row; /* Can't use a pointer to a cell since cells may be deleted */
	int col;
	int game;

public:
	qIODamPassageGDT(qIODamPassageCell *_cell, const char *title, const char *xlabel, const char *ylabel);

	/* This routine fills up a passage histogram with data. A virtual
	 *  function fetches the data - derived classes control this. */
	virtual void FillHistogram();
	/* This version of HistData uses qDamSpecific's getHistData(i) method */
	virtual float HistData( struct rls_seg *rls_seg, int i );

	/* Virtual function called on base class to fill GraphDataToken with data. */
	virtual int Fill( const char *feature = NULL, const char *species = NULL );

};

/* qIODamWaterAverageGDT for showing flow/spill in a graph */
class qIODamWaterAverageGDT : public qBasePassHistGDT {
protected:
	qIOSheet *sheet;

	/* Note that since we're referencing by row and column, if the I/O sheet
	 * changes, then we'll be displaying the wrong data. Hopefully this won't
	 * happen.
	 *
	 * TODO: come up with a scheme that would allow the users to keep histograms
	 * open even after switching I/O sheets. This would cause problems with
	 * the way that they update automatically at the moment, though... */

	int row; /* Can't use a pointer to a cell since cells may be deleted */
	int col;

public:
	qIODamWaterAverageGDT(qIODamWaterAverageCell *_cell);

	virtual void FillHistogram();
	virtual float HistData( struct rls_seg *rls_seg, int i );
	virtual int Fill( const char *feature = NULL, const char *species = NULL );

};

/* qIOMonteCarloGDT is for showing Monte Carlo cell values indexed by game
 * (this may be combined with qHistogramGDT for a histogram to see if the
 * distribution looks normal) */
class qIOMonteCarloGDT : public qBasePassHistGDT {
protected:
	qIOSheet *sheet;

	/* Note that since we're referencing by row and column, if the I/O sheet
	 * changes, then we'll be displaying the wrong data. Hopefully this won't
	 * happen.
	 *
	 * TODO: come up with a scheme that would allow the users to keep histograms
	 * open even after switching I/O sheets. This would cause problems with
	 * the way that they update automatically at the moment, though... */

	int row; /* Can't use a pointer to a cell since cells may be deleted */
	int col;

public:
	qIOMonteCarloGDT(qIOCell *_cell, const QString &title);

	/* This routine fills up a passage histogram with data. A virtual
	 *  function fetches the data - derived classes control this. */
	virtual void FillHistogram();
	/* This version of HistData uses qDamSpecific's getHistData(i) method */
	virtual float HistData( struct rls_seg *rls_seg, int i );

	/* Virtual function called on base class to fill GraphDataToken with data. */
	virtual int Fill( const char *feature = NULL, const char *species = NULL );

};

#endif
