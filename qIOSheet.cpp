#include "qIOSheet.h"
#include "qIOCell.h"
#include <qvaluevector.h>
#include "qGlobalEvents.h"
#include "log.h"
#include "qScenarioManager.h"
#include "qIOTool.h"
#include "qFileManager.h"
#include "run_mode.h"
#include <qpoint.h>
#include <qmime.h>

extern "C" {
#include "monte_output.h"
}

/* qIODragObject implementation */
#define	DRAG_ROW_SEPARATOR	"\n"
#define	DRAG_COL_SEPARATOR	"\t"

qIODragObject::qIODragObject(QTable *src, int _top, int _left, int _bottom, int _right)
	:	QDragObject(src),
		table(src), top(_top), left(_left), bottom(_bottom), right(_right)
{}
bool qIODragObject::provides(const char *mimeType) const {
	QString m(mimeType);

	if (m == "x-application/qtable-selection") return true;
	else if (m == "text/plain") return true;
	else return false;
}
const char* qIODragObject::format(int i) const {
	if (i ==0) return "x-application/qtable-selection";
	else if (i == 1) return "text/plain";
	else return NULL;
}
QByteArray qIODragObject::encodedData(const char *mimeType) const {
	QString m(mimeType);

	if (m == "x-application/qtable-selection") {
		QByteArray data(4);

		data[0] = (char) top;
		data[1] = (char) left;
		data[2] = (char) bottom;
		data[3] = (char) right;

		return data;
	}
	else if (m == "text/plain") {
		QString cells;

		for (int row=top; row <= bottom; row++) {
			for (int col=left; col <= right; col++) {
				QTableItem *cell = table->item(row, col);
				if (cell) {
					cells += cell->text();
					for (int k=1; k < cell->colSpan(); k++) {
						cells += "\t";
						col++;
					}
				}
				else cells += "\t";
				if (col < right) cells += DRAG_COL_SEPARATOR;
			}
			cells += DRAG_ROW_SEPARATOR;
		}

		QByteArray data(cells.length());
		const char *string = cells.ascii();
		for (int i=0; i < (int) cells.length(); i++) {
			data[i] = string[i];
		}
		return data;
	}
	else return QByteArray();
}

/* qIOSheet base class implementation */

qIOSheet::qIOSheet(QWidget *parent, qIOTool *tool, const QString &_name)
:	QTable(parent),
	ioTool(tool),
	gameCount(-1),
	name(_name)
{
	/* Connect signals */
	if (!connect(this, SIGNAL(valueChanged(int, int)), this, SLOT(cbValueChanged(int, int)))
		|| !connect(this, SIGNAL(doubleClicked(int, int, int, const QPoint &)), this, SLOT(cbDoubleClicked(int, int, int)))
		|| !connect(tool, SIGNAL(evApplyAll()), this, SLOT(cbApply()))
		|| !connect(tool, SIGNAL(evResetAll()), this, SLOT(cbReset()))
		|| !connect(tool, SIGNAL(evZoomIn()), this, SLOT(cbZoomIn()))
		|| !connect(tool, SIGNAL(evZoomOut()), this, SLOT(cbZoomOut()))
		|| !connect(&fileManager, SIGNAL(fileOpening()), this, SLOT(cbFileOpening()))
		|| !connect(&scenarioManager, SIGNAL(running()), this, SLOT(cbResetGameCount())))
		log_msg(L_ERROR, "Could not connect I/O sheet signal handlers\n");

	setSelectionMode(QTable::Single); /* Copy and paste relies on this only allowing a single range of cells to be selected */

	verticalHeader()->hide();
	setLeftMargin(0);
}

qIODragObject* qIOSheet::copy() {
	QTableSelection sel = selection(currentSelection());
	return new qIODragObject(this, sel.topRow(), sel.leftCol(), sel.bottomRow(), sel.rightCol());
}
void qIOSheet::paste(QMimeSource *src) {
	if (src->provides("x-application/qtable-selection")) {
		QByteArray data(src->encodedData("x-application/qtable-selection"));
		if (data.size() == 4) {
			int i, j;

			int top = data[0];
			int left = data[1];
			int bottom = data[2];
			int right = data[3];

			QTableSelection sel = selection(currentSelection());
			int pasteTop = sel.topRow();
			int pasteLeft = sel.leftCol();;
			int pasteBottom = pasteTop + bottom - top;
			int pasteRight = pasteLeft + right - left;

			/* Check to make sure we paste into the table */
			if (pasteLeft < 0 || pasteRight >= numCols()
				|| pasteTop < 0 || pasteBottom >= numRows()) {
				log_msg(L_ERROR, "Attempt to paste outside of table\n");
				return;
			}

			/* Check drag types because, e.g. pasting a dam mortality into a spill schedule cell would make no sense */
			for (i=top; i <= bottom; i++) {
				int pi = pasteTop + i - top;
				for (j=left; j <= right; j++) {
					int pj = pasteLeft + j - left;
					if (!item(i, j) || item(i, j)->rtti() != qIOInputCellRTTI
						|| !item(pi, pj) || item(pi, pj)->rtti() != qIOInputCellRTTI
						|| ((qIOInputCell*)item(i,j))->dragType() != ((qIOInputCell*)item(pi,pj))->dragType()) {
						log_msg(L_WARNING, "Attempt to paste into non-existent or mismatched cell(s)\n");
						goto skip;
					}
				}
			}

			/* Actually copy the cells here */
			for (i=top; i <= bottom; i++) {
				int pi = pasteTop + i - top;
				for (j=left; j <= right; j++) {
					int pj = pasteLeft + j - left;

					qIOInputCell* source = (qIOInputCell*) item(i, j);
					qIOInputCell* dest = (qIOInputCell*) item(pi, pj);

					dest->copy(source);
				}
			}

			return;
		}
	}
skip:
	if (src->provides("text/plain")) {
		int row = currentRow();
		int col = currentColumn();
		QString str(src->encodedData("text/plain"));

		if (!item(row, col) || item(row, col)->rtti() != qIOInputCellRTTI
			|| ((qIOInputCell*)item(row,col))->dragType() != DRAG_TEXT) {
			log_msg(L_ERROR, "Attempt to paste into non-existent or mismatched cell(s)\n");
			return;
		}

		int i, j;
		QStringList::Iterator ri, ci;
		QStringList rows = QStringList::split(DRAG_ROW_SEPARATOR, str, true);
		for (ri = rows.begin(), i = 0; ri != rows.end(); ++ri, i++) {
			QStringList cols = QStringList::split(DRAG_COL_SEPARATOR, *ri, true);
			for (ci = cols.begin(), j = 0; ci != cols.end(); ++ci, j++) {
				QString val = *ci;
				if (val != QString::null && val != "") {
					/* Check to make sure the user is pasting in a number */
					bool check;
					val.toDouble(&check);

					if (check) {
						qIOInputCell* dest = (qIOInputCell*) item(row+i, col+j);
						dest->setContent(val);
					}
					else log_msg(L_ERROR, "Attempt to paste in a non-number: %s\n", val.ascii());
				}
			}
		}
	}
}

void qIOSheet::setReleases(QPtrList<struct release> _releases) {
	clear();
	releases = _releases;
	setup();
	emit calculate();

	adjustColumns();
}

int qIOSheet::getGameCount() {
	if (run_mode.running || gameCount > 0) return gameCount;
	else if (gameCount < 0) {
		/* Recalculate */
		QPtrListIterator<struct release> it(releases);
		struct release *rel;
		int min = -1;

		if (it.current() == NULL) {
			gameCount = 0;
			return gameCount;
		}

		while (rel = it.current()) {
			struct rls_seg *rseg = rel->rls_seg;
			struct rls_seg *last_rseg_with_data = NULL;
			/* Find the farthest downstream segment with data, if there is one */
			while (rseg) {
				if (rseg->data) last_rseg_with_data = rseg;
				rseg = rseg->down;
			}

			if (last_rseg_with_data == NULL) {
				gameCount = 0;
				return gameCount;
			}

			int game;
			for (game = 0; last_rseg_with_data->data[game] != NULL && last_rseg_with_data->data[game]->stats.done; game++);
			if (min == -1 || game < min) min = game;

			++it;
		}

		gameCount = min;
	}

	return gameCount;
}

void qIOSheet::clear() {
	for (int i=0; i < numRows(); i++) {
		for (int j=0; j < numCols(); j++) {
			clearCell(i, j);
		}
	}
}

void qIOSheet::cbApply() {
	emit evApply();
}

void qIOSheet::cbReset() {
	emit evReset();
}

void qIOSheet::adjustColumns() {
	/* Adjust column widths */
	static const int extraWidth = 20;

	for (int i=0; i < numCols(); i++) {
		adjustColumn(i);
		setColumnWidth(i, columnWidth(i) + extraWidth);
	}
}

QWidget* qIOSheet::createEditor(int row, int col, bool initFromCell) const {
	if (item(row, col) != NULL) return QTable::createEditor(row, col, initFromCell);
	else return NULL;
}

void qIOSheet::cbFileOpening() {
	clear();
}

void qIOSheet::cbValueChanged(int row, int col) {
	/* Only grow the column; don't shrink it */
	int oldWidth = columnWidth(col);
	adjustColumn(col);
	if (columnWidth(col) < oldWidth) setColumnWidth(col, oldWidth);
}

void qIOSheet::cbDoubleClicked(int row, int col, int mouseButton) {
	if (mouseButton == Qt::LeftButton) {
		QTableItem *c = item(row, col);
		/* Check to make sure that the object has a cbDoubleClicked() slot.
		 * Note that we don't send this signal for input cells. This could be changed
		 * in the future, but since input cells are edited via a double-click, we're
		 * currently assuming that wouldn't be helpful. */
		if (c && (c->rtti() == qTableItemRTTI || c->rtti() == qIODamPassageCellRTTI || c->rtti() == qIODamWaterAverageCellRTTI /*|| c->rtti() == qIOInputCellRTTI*/)) {
			qTableItem* cell = (qTableItem*) c;
			if (cell->isValid()) cell->cbDoubleClicked();
		}
	}
}

void qIOSheet::setItem(int row, int col, qIOInputCell *cell) {
	/* Connect signals */
	if (!connect(this, SIGNAL(evApply()), cell, SLOT(cbApply()))
			|| !connect(this, SIGNAL(evReset()), cell, SLOT(cbReset()))
			|| !connect(cell, SIGNAL(evDirtyChanged()), ioTool, SLOT(updateDirty())))
		log_msg(L_ERROR, "Could not connect input cell signal handlers\n");

	/* Call QTable's setItem to do the actual setting */
	QTable::setItem(row, col, (qTableItem*)cell);
}
void qIOSheet::setItem(int row, int col, qTableItem *cell) {
	QTable::setItem(row, col, cell);
}

int qIOSheet::isDirty() {
	for (int i=0; i < numRows(); i++) {
		for (int j=0; j < numCols(); j++) {
			/* Check to make sure it's a qTableItem and not just Qt's generic one */
			QTableItem *c = item(i, j);
			if (c && (c->rtti() == qTableItemRTTI /*|| c->rtti() == qIODamPassageCellRTTI || c->rtti() == qIODamWaterAverageCellRTTI*/ || c->rtti() == qIOInputCellRTTI)) {
				qTableItem* cell = (qTableItem*) c;
				if (cell->isDirty()) return 1;
			}
		}
	}

	return 0;
}

void qIOSheet::exportAsText(FILE *f) {
	int col;

	/* Output horizontal headers */
	for (col = 0; col < numCols(); col++) {
		fprintf(f, "%s%s", col > 0 ? "\t" : "", horizontalHeader()->label(col).ascii());
	}
	fprintf(f, "\n");

	/* Output rows */
	int row;
	for (row = 0; row < numRows(); row++) {
		for (col = 0; col < numCols(); col++) {
			QTableItem *i = item(row, col);
			fprintf(f, "%s", col > 0 ? "\t" : "");
			if (i && i->colSpan() == 1 && i->text() != QString::null) {
				fprintf(f, "%s", i->text().ascii());
			}
		}

		fprintf(f, "\n");
	}

	fprintf(f, "\n");
}
QString qIOSheet:: exportAsHTML() {
	QString html = "<table border=\"1\">\n";
	int col;

	/* Output horizontal headers */
	html += "<tr>";
	for (col = 0; col < numCols(); col++) {
		html += "<th>" + horizontalHeader()->label(col) + "</th>";
	}
	html += "</tr>\n";

	/* Output rows */
	int row;
	for (row = 0; row < numRows(); row++) {
		html += "<tr>";

		QTableItem *i = NULL;
		for (col = 0; col < numCols(); col += i ? i->colSpan() : 1) {
			i = item(row, col);

			if (col == 0 && i) {
				/* Only consider cells spanning multiple columns for the first column */
				if (i->colSpan() > 1) {
					html += "<th colspan=\"" + QString::number(i->colSpan()) + "\"><b>";
				}
				else html += "<td>";

				html += (i->text() != QString::null ? i->text() : "");

				if (i->colSpan() > 1) html += "</b></th>";
				else html += "</td>";
			}
			else {
				html += "<td align=\"right\">";

				if (i) {
					html += (i->text() != QString::null ? i->text() : "");
				}

				html += "</td>";
			}
		}

		html += "</tr>\n";
	}

	html += "</table>\n";
	return html;
}


/* qIODamSheet implementation */
qIODamSheet::qIODamSheet(QWidget *parent, qIOTool *tool, const QString& name, int rows, int _nonDamCols)
:	qIOSheet(parent, tool, name),
	nonDamCols(_nonDamCols)
{
	setNumRows(rows);
}

void qIODamSheet::setReleases(QPtrList<struct release> _releases) {
	int powerhouses;

	clear();
	releases = _releases;

	/* Clear out data */
	dams.clear();
	releaseSegs.clear();

	/* Get the list of dams that are passed through by any selected release */
	setDamsList(dams, river_mouth);

	/* Power houses have their own columns, so count them here */
	{
		QPtrListIterator<struct river_segment> it(dams);
		struct river_segment *seg;

		powerhouses = 0;
		while (seg = it.current()) {
			struct river_segment *ph;
			/* Add first powerhouse */
			if (seg->current->dam.additional_pwrhs) powerhouses++;
			/* Add additional powerhouses */
			for (ph = seg->current->dam.additional_pwrhs; ph; ph = ph->current->dam.additional_pwrhs)
				powerhouses++;

			++it;
		}
	}

	/* Resize the table horizontally as appropriate */
	setNumCols(dams.count() + powerhouses + nonDamCols + 1);

	/* set up releaseSegs map */
	setReleaseSegsMap();

	setup();

	/* Compute statistics at dam segments that are needed during recalculation */
	QPtrListIterator<struct release> it(releases);
	struct release *rls;

	while (rls = it.current()) {
		struct rls_seg *rseg;
			
		for (rseg = rls->rls_seg; rseg; rseg = rseg->down) {
			if (rseg->river_segment->seg_type == DAM_SEG && rseg->data && rseg->data[0]) {
				compute_stats(rseg->data[0], rseg->river_segment,
					  rls->start_date, (int) rls->stats.total.mean);
			}
		}

		++it;
	}

	emit calculate();

	adjustColumns();
}

const QPtrList<struct rls_seg>& qIODamSheet::damReleaseSegs(struct river_segment* seg) {
	return releaseSegs[seg];
}

int qIODamSheet::setDamsList(QPtrList<struct river_segment>& damSegs, struct river_segment *seg) {
	int ret = 0;

	/* recursive calls to get to the furthest segments from the mouth.
	 * if a release site is located upstream of either the upstream segment or the
	 * fork, then dams on this path should be added. */
	if (seg->up) ret = setDamsList(damSegs, seg->up);
	if (seg->fork) ret |= setDamsList(damSegs, seg->fork);

	/* if we should add it and it's a dam, append it to the list */
	if (seg->seg_type == DAM_SEG) {
		if (ret) {
			damSegs.append(seg);
		}
	}
	else {
		/* if the segment's not a dam, check to see if it's a release segment, because
		 * then we want all downstream dams to be added to the list of dams. */
		QPtrListIterator<struct release> it(releases);
		struct release *rel;
		while (rel = it.current()) {
			if (rel->rls_seg->river_segment == seg) {
				ret = 1; /* add dams on this path */
				break;
			}

			++it;
		}
	}

	/* Note that transportation target sites are not really considered here since it's
	 * assumed that the target sites are downstream of the original release sites. */

	return ret;
}

void qIODamSheet::setReleaseSegsMap() {
	QPtrListIterator<struct release> it(releases);
	struct release *rls;

	/* Loop through releases */
	while (rls = it.current()) {
		struct rls_seg *rseg;

		/* Follow release segments downstream and add them to dams' list */
		for (rseg = rls->rls_seg; rseg; rseg = rseg->down) {
			struct river_segment *seg = rseg->river_segment;

			/* These will be used for calculations, so include the river_mouth as well */
			if (seg->seg_type == DAM_SEG || seg == river_mouth) {
				releaseSegs[seg].append(rseg);

				/* Here we hardwire the transport destination to the segment after the last dam.
				   This does add some unnecessary segments to releaseSegs[], but it's not a big deal. */
				if (seg != river_mouth) {
					transport_destination = seg->down;
					releaseSegs[transport_destination].append(rseg->down);
				}
			}
		}

		++it;
	}
}

