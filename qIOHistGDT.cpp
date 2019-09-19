#include "qIOHistGDT.h"
#include "qIOCell.h"
#include "crisp.h"
#include "log.h"
#include "mapConstants.h"

#define IO_HIST_GDT_BINS	20
#define	IO_HIST_GDT_MIN_BIN_SIZE	0.0001

qIODamPassageGDT::qIODamPassageGDT(qIODamPassageCell *_cell, const char *title, const char *xlabel, const char *ylabel)
:	qBasePassHistGDT((QString(title) + ": ").ascii(), _cell->getSeg()->seg_name, DAM, -10)
{
	sheet = _cell->getSheet();
	row = _cell->row();
	col = _cell->col();
	game = -1;

	insertLabel(title);	
	setGraphXAxisLabel(xlabel);
	setGraphYAxisLabel(ylabel);
	Fill(NULL, "");
	setShowDots(0);
}

/* Most of the following code is very similar to qBasePassHistGDT::FillHistogram()
 * and some of it was copied and pasted in. I try not to copy code where possible,
 * but the other GDTs would need to use QPtrLists to make code sharing feasible. */

float qIODamPassageGDT::HistData(rls_seg *rls_seg, int i) {
	QTableItem *item = sheet->item(row, col);
	if (game >= 0 && item && item->rtti() == qIODamPassageCellRTTI) {
		qIODamPassageCell *cell = (qIODamPassageCell*)item;

		if (rls_seg->data && rls_seg->data[0]) return cell->getHistData(rls_seg, game, i);
		else log_msg(L_ERROR, "Release segment data not found at %s\n", rls_seg->river_segment->seg_name);
	}

	return 0.0;
}

int qIODamPassageGDT::Fill(const char *feature, const char *species) {
	FillHistogram();
	return 1;
}

void qIODamPassageGDT::FillHistogram( ) {
	unsigned int i, j;
    struct release	*release;
    struct rls_seg	*rls_seg;
	float f, releaseTotal = 0, rlsTotal = 0, histTotal = 0;

	QTableItem *item = sheet->item(row, col);
	if (item && item->rtti() == qIODamPassageCellRTTI) {
		qIODamPassageCell *cell = (qIODamPassageCell*)item;
		if (isRunning()) return;

		/* Count the number of games */
		unsigned int games = cell->getSheet()->getGameCount();

		game = games - 1; /* Show the last game */
		if (games >= 1) {

			/* Scenario mode */

			/* Initialization. */
			setNumLines(1);
			setNumElementsPerLine(DAYS_IN_SEASON);
			dataArray_.resize(DAYS_IN_SEASON);
			while (dataArray_.size() < DAYS_IN_SEASON)
				dataArray_.append( 0.0 );
			for (i = 0; i < games * DAYS_IN_SEASON; i++)
				dataArray_[i] = 0.0;

			/* Go through release segments */
			QPtrListIterator<struct rls_seg> it(cell->getReleaseSegments());
			QPtrListIterator<struct release> rit(cell->getReleases());

			while ((rls_seg = it.current()) && (release = rit.current())) {
				releaseTotal += cell->getTotal(rls_seg, game);
				rlsTotal = 0;

				/* Sum the passage histogram for this species in this segment. */
				for (i = 0; i < STEPS_IN_RUN; i++) {
					/* Find destination slot in data token. */
					j = release->start_date + i / STEPS_PER_DAY;

					/* Get source data item and accumulate histogram. */
					f = HistData(rls_seg, i);
					if (j < DAYS_IN_SEASON)    // beware of overrun.
						dataArray_[j] += f;
					rlsTotal += f;
				}

				histTotal += rlsTotal;

				++it;
				++rit;
			}

			/* Get YMax for the data array. Forbid Ymin == Ymax. */
			setYBounds( 1 );
			char strBuf[50];
			sprintf( strBuf, "total passage %.0f = %.0f%%", histTotal,
										   100 * histTotal / releaseTotal );
			graphXAxisLabel_ = strBuf;
		}
	}
}

/* qIODamWaterAverageGDT */
qIODamWaterAverageGDT::qIODamWaterAverageGDT(qIODamWaterAverageCell *_cell)
:	qBasePassHistGDT((_cell->getName() + ": ").ascii(), _cell->getSeg()->seg_name, DAM, -10)
{
	sheet = _cell->getSheet();
	row = _cell->row();
	col = _cell->col();

	insertLabel(_cell->getName().ascii());
	setGraphYAxisLabel("KCFS");
	Fill(NULL, "");
	setShowDots(0);
}

/* Most of the following code is very similar to qBasePassHistGDT::FillHistogram()
 * and some of it was copied and pasted in. I try not to copy code where possible,
 * but the other GDTs would need to use QPtrLists to make code sharing feasible. */

float qIODamWaterAverageGDT::HistData(rls_seg *rls_seg, int i) {
	QTableItem *item = sheet->item(row, col);
	if (item && item->rtti() == qIODamWaterAverageCellRTTI) {
		qIODamWaterAverageCell *cell = (qIODamWaterAverageCell*)item;

		return cell->getKCFS(i);
	}

	return 0.0;
}

int qIODamWaterAverageGDT::Fill(const char *feature, const char *species) {
	FillHistogram();
	return 1;
}

void qIODamWaterAverageGDT::FillHistogram( ) {
	unsigned int i;

	QTableItem *item = sheet->item(row, col);
	if (item && item->rtti() == qIODamWaterAverageCellRTTI) {
		qIODamWaterAverageCell *cell = (qIODamWaterAverageCell*)item;
		if (isRunning()) return;

		/* Initialization. */
		setNumLines(1);
		setNumElementsPerLine(DAYS_IN_SEASON);
		dataArray_.resize(DAYS_IN_SEASON);
		while (dataArray_.size() < DAYS_IN_SEASON)
			dataArray_.append( 0.0 );
		for (i = 0; i < DAYS_IN_SEASON; i++)
			dataArray_[i] = 0.0;

		/* Sum the passage histogram for this species in this segment. */
		for (i = 0; i < DAYS_IN_SEASON; i++) {
			dataArray_[i] += HistData(NULL, i);
		}

		/* Get YMax for the data array. Forbid Ymin == Ymax. */
		setYBounds( 1 );
		graphXAxisLabel_ = "";
	}
}

/* Monte Carlo histograms */
qIOMonteCarloGDT::qIOMonteCarloGDT(qIOCell *_cell, const QString &title)
:	qBasePassHistGDT((QString(title) + ": ").ascii(), "", DAM, -10)
{
	sheet = _cell->getSheet();
	row = _cell->row();
	col = _cell->col();

	insertLabel((title + " (Monte Carlo)").ascii());
	Fill(NULL, "");
	setShowDots(0);
}

/* Most of the following code is very similar to qBasePassHistGDT::FillHistogram()
 * and some of it was copied and pasted in. I try not to copy code where possible,
 * but the other GDTs would need to use QPtrLists to make code sharing feasible. */

float qIOMonteCarloGDT::HistData(rls_seg *rls_seg, int i) {
	QTableItem *item = sheet->item(row, col);
	if (item && item->rtti() == qIODamPassageCellRTTI) {
		qIODamPassageCell *cell = (qIODamPassageCell*)item;

		return cell->value(i);
	}

	return 0.0;
}

int qIOMonteCarloGDT::Fill(const char *feature, const char *species) {
	FillHistogram();
	return 1;
}

void qIOMonteCarloGDT::FillHistogram( ) {
	unsigned int i;

	QTableItem *item = sheet->item(row, col);
	if (item) {
		qIOCell *cell = (qIOCell*)item;
		if (isRunning()) return;

		/* Count the number of games */
		unsigned int games = cell->getSheet()->getGameCount();

		if (games > 1) {
			/* Monte Carlo mode */
			
			/* Initialization. */
			setNumLines(1);
			setNumElementsPerLine(games);
			dataArray_.resize(games);
			while (dataArray_.size() < games)
				dataArray_.append( 0.0 );
			for (i = 0; i < games; i++)
				dataArray_[i] = 0.0;

			unsigned int game;

			for (game=0; game < games; game++) {
				dataArray_[game] = cell->value(game);
			}

			setYBounds(1);
		}
	}
}
