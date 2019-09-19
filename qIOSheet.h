/* qIOSheet.h
 * qIOSheet is the base class for I/O sheets, but all other I/O
 * sheets are currently defined and implemented in the same files. */

#ifndef __qIOSHEET_H__
#define __qIOSHEET_H__

#include "crisp.h"
#include <qtable.h>
#include <qptrlist.h>
#include <qmap.h>
#include "qTableItem.h"
#include <qdragobject.h>

class qIOTool;
class qIOInputCell;
class qIODragObject;
class qIOPostBonnevilleHypothesisInputCell;

/* Helper class for implementing drag and copy operations */
class qIODragObject : public QDragObject {
protected:
	QTable *table;
	int top, left, bottom, right;

public:
	qIODragObject(QTable *src, int _top, int _left, int _bottom, int _right);
	virtual bool provides(const char *mimeType) const;
	virtual const char* format(int i) const;
	virtual QByteArray encodedData(const char *mimeType) const;
};


/* This is the base class for different I/O sheets that defines the
 * interface and implements common code that all sheets might need. */
class qIOSheet : public QTable {
	Q_OBJECT

protected:
	qIOTool *ioTool;
	QString name; /* Name of the sheet (this is used in the tabbed interface) */
	QPtrList<struct release> releases; /* List of releases from which data is used in this sheet */
	int gameCount; /* This is the number of games that have been finished for Monte Carlo mode */

public:
	qIOSheet(QWidget *parent, qIOTool *tool, const QString &name);
	qIOTool* getIOTool() {return ioTool;}
	const QString& getName() {return name;}

	QPtrList<struct release>& getReleases() {return releases;}

	/* recalculate doesn't attempt to update all references, so it can crash
	 * if you're not careful. DO NOT call recalculate right after UpdateAllDataTokens
	 * as the GUI messes with release structures (moving/deleting them) and will
	 * most likely crash if any releases have changed */

	virtual void setReleases(QPtrList<struct release> releases); /* Set releases from which data is used */

	virtual void setItem(int row, int col, qTableItem *item);
	/* Use a different setItem() for qIOInputCells so that we may connect signals */
	virtual void setItem(int row, int col, qIOInputCell *item);

	virtual int isDirty(); /* Whether any cell is dirty */

	/* Print out the sheet in tab-delimited form to the stream */
	virtual void exportAsText(FILE *f);
	virtual QString exportAsHTML();

	qIODragObject* copy(); /* Returns a drag object for copy and paste */
	void paste(QMimeSource* src); /* Pastes in a drag object */

	int getGameCount(); /* Get the number of Monte Carlo games that should have been completed */

protected:
	virtual void setup() = 0; /* Setup the spreadsheet */

	/* utility functions */
	virtual void clear(); /* Remove all cells */
	void adjustColumns(); /* Make columns as small as possible */

	/* Override createEditor so that the user can only edit cells that exist and not blank ones */
	QWidget* createEditor(int row, int col, bool initFromCell) const;

public slots:
	void cbZoomIn() {setFont(QFont(font().family(), font().pointSize()*4/3)); horizontalHeader()->adjustHeaderSize(); verticalHeader()->adjustHeaderSize(); for (int i=0; i < numRows(); i++) adjustRow(i); for (int j=0; j < numCols(); j++) adjustColumn(j);}
	void cbZoomOut() {setFont(QFont(font().family(), font().pointSize()*3/4)); horizontalHeader()->adjustHeaderSize(); verticalHeader()->adjustHeaderSize(); for (int i=0; i < numRows(); i++) adjustRow(i); for (int j=0; j < numCols(); j++) adjustColumn(j);}

	void cbResetGameCount() {gameCount = -1;}

	virtual void cbApply();
	virtual void cbReset();

protected slots:
	void cbValueChanged(int row, int col); /* Resize columns so that values always fit */
	void cbDoubleClicked(int row, int col, int mouseButton); /* Send double clicked signal */
	void cbFileOpening(); /* Remove all cells when a file is going to be opened (since pointers will probably change) */

signals:
	void calculate(); /* signal saying that cells should be calculated */
	void evApply();
	void evReset();
};

/* Base class for I/O sheets that center around information about dams, e.g. SIMPAS */
class qIODamSheet : public qIOSheet {
	Q_OBJECT

protected:
	int nonDamCols;
	/* pointers to dams along the paths of any selected release. This list of dams
	 * should be in order with the dam closest to the river mouth being the last
	 * entry in the list. */
	QPtrList<struct river_segment> dams;

	/* map from dam river segments to a list of rls_segs that pass through that dam.
	 * Note that these lists should be in the same order as qIOSheet::releases list
	 * so that when a function wants to obtain the release along with the rls_seg for
	 * a particular dam, all it needs to do is start and synchronize an iterator over
	 * each list. */
	QMap<struct river_segment*, QPtrList<struct rls_seg> > releaseSegs;

	struct river_segment *transport_destination; /* transport destination segment pointer */

public:
	qIODamSheet(QWidget *parent, qIOTool *tool, const QString& name, int rows, int nonDamCols);

	virtual void setReleases(QPtrList<struct release> releases); /* Set releases from which data is used */

	/* For accessing the releaseSegs map */
	const QPtrList<struct rls_seg>& damReleaseSegs(struct river_segment* seg);

	/* For accessing list of dams */
	const QPtrList<struct river_segment>& getDams() {return dams;}

protected:
	/* recursive function for determining the list of dams that are passed
	 * by the selected releases. Returns non-zero if any dam at seg should
	 * be added (if seg isn't a dam segment, then the value is propagated
	 * back up). */
	int setDamsList(QPtrList<struct river_segment>& damSegs, struct river_segment *seg);

	void setReleaseSegsMap(); /* set up releaseSegs map properly */
};

#endif
