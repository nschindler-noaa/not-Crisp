/* qIOCell.h
 * This file defines the different types of cells that may be placed
 * in a qIOSheet. All such cells are defined and implemented in these
 * two files. */

#ifndef __qIOCELL_H__
#define __qIOCELL_H__

#include "crisp.h"
#include <qtable.h>
#include "qIOSheet.h"
#include "qIOTool.h"
#include "qIOHistGDT.h"
#include "qTableItem.h"
#include "qIdcSelectBase.h"
#include "qIdcSelect.h"
#include "qIdcListBase.h"
#include "qIdcList.h"
#include "qIdcScheduleBase.h"
#include "qMediatorBuilder.h"
#include "unused.h"
#include "crisp_time.h"
#include "qIdcData.h"

#define	RESOLUTION	1

enum {
	qIOInputCellRTTI=856000,
	qIODamPassageCellRTTI,
	qIODamWaterAverageCellRTTI
};

#define	IO_CELL_PRECISION	3

/* Options for which cell value to display (if >= 0, it specifies the game
 * number, otherwise it is a special value as listed here). */
#define	IO_CELL_VALUE_MONTE_CARLO_MEAN		-1
#define	IO_CELL_VALUE_MONTE_CARLO_STDDEV	-2

/* This is the base class for I/O sheet cells to define the interface
 * and group common code. */
class qIOCell : public QObject, public qTableItem {
	Q_OBJECT

protected:
	qIOSheet *sheet;
	qGraphDialog *monteCarloHistogram;

public:
	qIOCell(qIOSheet *table, int colspan = 1, EditType et = Never);

	qIOSheet* getSheet() {return sheet;}

	virtual QString getName() = 0; /* Return the name of this cell (this is used as a title for histograms) */
	virtual QString getXAxisLabel() {return "";}; /* For histograms */
	virtual QString getYAxisLabel() {return "";}; /* For histograms */

	/* These functions simply convert to and from formatting in the cell.
	 * They may be overridden to, for example, convert mortalities to survivals */
	virtual QString valueToText(double value) const {return QString::number(value, 'f', IO_CELL_PRECISION);}
	virtual double textToValue(const QString& text) const {return text.toDouble();}

	/* This function returns the value of the cell calculated for the given game.
	 * Note that this really only makes sense for output cells since input cells don't change between games. */
	virtual double value(int game) = 0;
	/* Calculate the mean or standard deviation of all games */
	virtual double mean();
	virtual double stddev();

	/* Cells default to showing a Monte Carlo histogram in Monte Carlo mode */
	/* TODO: title?? */
	virtual void cbDoubleClicked() {if (sheet->getGameCount() > 1) showMonteCarloHistogram();}

protected:
	void showMonteCarloHistogram();

public slots:
	/* Calculate the value */
	virtual void calculate();

	void monteCarloHistogramDestroyed();
};

/* Interface and data for cells that are specific to a single dam (e.g. dam
 * survival or turbine mortality). */
class qIODamSpecific {
protected:
	qIODamSheet *damSheet;
	struct river_segment *seg;
	int powerhouse; /* If the cell is a powerhouse cell */
public:
	qIODamSpecific(struct river_segment *seg, qIODamSheet *damSheet, int powerhouse);

	struct river_segment* getSeg() {return seg;}

	const QPtrList<struct rls_seg>& getReleaseSegments(struct river_segment *_seg) {return damSheet->damReleaseSegs(_seg);}
	const QPtrList<struct rls_seg>& getReleaseSegments() {return getReleaseSegments(seg);}
	QPtrList<struct release>& getReleases() {return damSheet->getReleases();}
	const QPtrList<struct release>& getReleases() const {return damSheet->getReleases();}
};

/* This is a special class for non-editable, static cells that only
 * have text in them. */
class qIOStaticCell : public qIOCell {
public:
	qIOStaticCell(const QString& text, qIOSheet *table, int colspan = 1);

	virtual double value(int game) {return 0.0;} /* this makes no sense for static cells */
	virtual QString getName() {return "";}
};

/* Base class for displaying flow/spill data that is averaged (in KCFS) */
class qIODamWaterAverageCell : public qIOCell, public qIODamSpecific {
	Q_OBJECT

protected:
	qGraphDialog *gd; /* Pointer to this cell's histogram if one exists */

public:
	qIODamWaterAverageCell(struct river_segment *seg, qIODamSheet *table, int powerhouse, int colspan = 1);

	virtual int rtti() const {return qIODamWaterAverageCellRTTI;}
	virtual QString valueToText(double value) const {return QString::number(value, 'g', 3);}
	virtual double value(int game);
	virtual double getKCFS(int day) = 0;

public slots:
	virtual void calculate();
	virtual void cbDoubleClicked();
	virtual void graphDestroyed();
};

/* Dam average flow cell */
class qIODamFlowCell : public qIODamWaterAverageCell {
public:
	qIODamFlowCell(struct river_segment *seg, qIODamSheet *table, int powerhouse, int colspan = 1)
		:	qIODamWaterAverageCell(seg, table, powerhouse, colspan) {}

	virtual QString getName() {return "Dam Flow (KCFS)";}
	virtual double getKCFS(int day) {return seg->current->dam.flow[day];}
};

/* Dam average spill cell */
class qIODamSpillCell : public qIODamWaterAverageCell {
public:
	qIODamSpillCell(struct river_segment *seg, qIODamSheet *table, int powerhouse, int colspan = 1)
		:	qIODamWaterAverageCell(seg, table, powerhouse, colspan) {}

	virtual QString getName() {return "Dam Spill (KCFS)";}
	virtual QString getYAxisLabel() {return "KCFS";}
	virtual double getKCFS(int day) {
		/* Average over a day */
		double sum = 0.0;
		for (int i=day*DAM_TIME_SLC; i < day*DAM_TIME_SLC + DAM_TIME_SLC; i++) sum += seg->current->dam.spill[i];
		return (sum / DAM_TIME_SLC) * seg->current->dam.flow[day];
	}
};

/* Base class for adding up numbers of fish at a dam and displaying
 * the proportion out of a total. This class is subclassed a lot to
 * provide most of the cells on the SIMPAS I/O sheet. */
/* Different options for this cell */

/* This is necessary so that we can determine if a given QTableItem is a dam passage cell */

/** These are the different algorithms for combining multiple I/O values together (e.g. for averaging or summing). */
enum {
	DAM_PASSAGE_AVERAGE = 1, /** Average values */
	DAM_PASSAGE_CUMULATIVE, /** Multiply the upstream values */
	DAM_PASSAGE_SUM, /** Sum the upstream values */
	DAM_PASSAGE_REMAINDER_SUM /** Sum of the upstream dam values multiplied by product of the complements of upstream dams */
};

enum {
	MONTE_CARLO_STAT_MEAN,
	MONTE_CARLO_STAT_STDDEV
};

/** Generic cell for displaying some passage-related information at a specific dam. */
class qIODamPassageCell : public qIOCell, public qIODamSpecific {
	Q_OBJECT

	friend class qIOHistGDT;

protected:
	int monte_carlo_stat;	/* Which Monte Carlo statistic should be displayed (e.g. mean or standard deviation) */
	int average; /* Whether this cell shows the average, cumulative, or neither (0) */
	int complement; /* Whether we actually want the complement to be shown */
	QString name; /* Name for the cell */

	qGraphDialog *gd; /* Pointer to this cell's histogram if one exists */

	/* Show either a passage vs. time graph or the standard Monte Carlo histogram */
	void showGraph();
	void showOnlyMonteCarloGraph() {if (sheet->getGameCount() > 1) showMonteCarloHistogram();}

	/* Calculate survival for a dam, given that dam's struct river_segment */
	float calculatePassage(struct river_segment *seg, int game, int *success);

	/* Get the number of fish that followed the path we're counting (e.g. the
	 * total number that survived passing through the dam), defaults to total
	 * passed through the dam. */
	virtual float getPassage(struct rls_seg *rseg, int game)  {return rseg->data[game]->stats.total.total;}

protected slots:
	void graphDestroyed(); /* Managing histograms */

public:
	qIODamPassageCell(const QString &name, struct river_segment *seg, qIODamSheet *table, int monte_carlo_stat, int average, int complement, int powerhouse, int colspan);

	virtual QString getName();

	/** Get the cell's value for a specific game.
	 *
	 * Note that for different values of average the upstream dam cells are combined in different ways.
	 *
	 * - For DAM_PASSAGE_AVERAGE, average the values, as in \f$x_i' = \frac{1}{i}\sum_{j=1}^i x_j \f$
	 * - For DAM_PASSAGE_CUMULATIVE, multiply the values, as in \f$x_i' = \prod_{j=1}^i x_j \f$
	 * - For DAM_PASSAGE_SUM, add the values, as in \f$x_i' = \sum_{j=1}^i x_j \f$
	 * - For DAM_PASSAGE_REMAINDER_SUM, \f$x_i' = \sum_{j=1}^i x_j \prod_{k=1}^{j-1} (1 - x_k) \f$
	 */
	double value(int game);

	/* Get the total that this statistic is out of, defaults to the total number
	 * of fish entering the project. */
	virtual float getTotal(struct rls_seg *rseg, int game) {return rseg->data[game]->stats.project_in_total;}

	virtual void calculate();

	virtual int rtti() const {return qIODamPassageCellRTTI;}

	/* This function retrieves passage on a given day. */
	virtual float getHistData(struct rls_seg *UNUSED(rseg), int UNUSED(game), int UNUSED(i)) {log_msg(L_ERROR, "Attempted to get histogram data that is undefined\n"); return 0.0f;}
	virtual void cbDoubleClicked() {showGraph();}
};

/* Dam FPE cell */
class qIODamFPECell : public qIODamPassageCell {
protected:
	virtual float getPassage(struct rls_seg *rseg, int game) {return rseg->data[game]->stats.turbine_in_total;}
public:
	qIODamFPECell(struct river_segment *seg, qIODamSheet *table, int average, int powerhouse, int colspan = 1)
		:	qIODamPassageCell("Fish Passage Efficiency", seg, table, MONTE_CARLO_STAT_MEAN, average, TRUE, powerhouse, colspan) {}
	virtual void cbDoubleClicked() {showOnlyMonteCarloGraph();}
};

/* Dam survival cell */
class qIODamSurvivalCell : public qIODamPassageCell {
protected:
	virtual float getPassage(struct rls_seg *rseg, int game) {
		float num = 1;
		num *= rseg->data[game]->stats.project_in_total; /* forebay */
		num *= rseg->data[game]->stats.turbine_total + rseg->data[game]->stats.sluiceway_total + rseg->data[game]->stats.bypass_total + rseg->data[game]->stats.spillway_total + rseg->data[game]->stats.rsw_total; /* dam */
		num *= rseg->data[game]->stats.total.total; /* tailrace */
		return num;
	}
public:
	qIODamSurvivalCell(struct river_segment *seg, qIODamSheet *table, int average, int powerhouse, int colspan = 1)
		:	qIODamPassageCell("Passage", seg, table, MONTE_CARLO_STAT_MEAN, average, FALSE, powerhouse, colspan) {}
	virtual float getTotal(struct rls_seg *rseg, int game) {
		float denom = 1;
		denom *= rseg->up->data[game]->stats.total.total + rseg->data[game]->stats.transport_in_total; /* forebay */
		denom *= rseg->data[game]->stats.project_in_total - rseg->data[game]->stats.premort_transport_total - rseg->data[game]->nopass; /* dam */
		denom *= rseg->data[game]->stats.turbine_total + rseg->data[game]->stats.sluiceway_total + rseg->data[game]->stats.bypass_total + rseg->data[game]->stats.spillway_total + rseg->data[game]->stats.rsw_total; /* tailrace */
		return denom;
	}
	virtual float getHistData(struct rls_seg *rseg, int game, int i)  {return rseg->data[game]->output[i];}
};

/* Pool survival cell */
class qIODamPoolSurvivalCell : public qIODamPassageCell {
protected:
	virtual float getPassage(struct rls_seg *rseg, int game) {
		float num = 1;
		for (struct rls_seg *top = rseg->up; top && top->river_segment->seg_type != DAM_SEG; top = top->up) {
			num *= top->data[game]->stats.total.total;
		}
		return num;
	}
public:
	qIODamPoolSurvivalCell(struct river_segment *seg, qIODamSheet *table, int average, int powerhouse, int colspan = 1)
		:	qIODamPassageCell("Pool Passage", seg, table, MONTE_CARLO_STAT_MEAN, average, FALSE, powerhouse, colspan) {}
	virtual float getTotal(struct rls_seg *rseg, int game) {
		float denom = 1;
		for (struct rls_seg *top = rseg->up; top && top->river_segment->seg_type != DAM_SEG; top = top->up) {
			denom *= top->data[game]->stats.project_in_total - top->data[game]->nopass;
		}
		return denom;
	}
	virtual float getHistData(struct rls_seg *rseg, int game, int i)  {return rseg->up->data[game]->output[i];}
};

/* Dam plus pool inriver survival cell */
class qIODamPlusPoolInriverSurvivalCell : public qIODamPassageCell {
protected:
	virtual float getPassage(struct rls_seg *rseg, int game) {
		float num = 1;
		for (struct rls_seg *top = rseg->up; top && top->river_segment->seg_type != DAM_SEG; top = top->up) {
			num *= top->data[game]->stats.total.total;
		}
		num *= rseg->data[game]->stats.project_in_total; /* forebay */
		num *= rseg->river_segment->seg_type == REACH_SEG ? rseg->data[game]->stats.total.total : rseg->data[game]->stats.turbine_total + rseg->data[game]->stats.sluiceway_total + rseg->data[game]->stats.bypass_total + rseg->data[game]->stats.spillway_total + rseg->data[game]->stats.rsw_total; /* dam */
		num *= rseg->data[game]->stats.total.total; /* tailrace */
		return num;
	}
public:
	qIODamPlusPoolInriverSurvivalCell(struct river_segment *seg, qIODamSheet *table, int average, int powerhouse, int colspan = 1)
		:	qIODamPassageCell("Dam and Pool Passage", seg, table, MONTE_CARLO_STAT_MEAN, average, FALSE, powerhouse, colspan) {}
	virtual float getTotal(struct rls_seg *rseg, int game) {
		float denom = 1;
		for (struct rls_seg *top = rseg->up; top && top->river_segment->seg_type != DAM_SEG; top = top->up) {
			denom *= top->data[game]->stats.project_in_total - top->data[game]->nopass;
		}
		denom *= rseg->up->data[game]->stats.total.total + rseg->data[game]->stats.transport_in_total; /* forebay */
		denom *= rseg->data[game]->stats.project_in_total - rseg->data[game]->stats.premort_transport_total - rseg->data[game]->nopass; /* dam */
		denom *= rseg->river_segment->seg_type == REACH_SEG ? rseg->data[game]->stats.total.total : rseg->data[game]->stats.turbine_total + rseg->data[game]->stats.sluiceway_total + rseg->data[game]->stats.bypass_total + rseg->data[game]->stats.spillway_total + rseg->data[game]->stats.rsw_total; /* tailrace */
		return denom;
	}
	virtual float getHistData(struct rls_seg *rseg, int game, int i)  {return rseg->data[game]->output[i];}
};

/* Dam plus pool survival cell */
class qIODamPlusPoolSurvivalCell : public qIODamPassageCell {
protected:
	virtual float getPassage(struct rls_seg *rseg, int game) {
		float num = 1;
		for (struct rls_seg *top = rseg->up; top && top->river_segment->seg_type != DAM_SEG; top = top->up) {
			num *= top->data[game]->stats.total.total;
		}
		num *= rseg->data[game]->stats.total.total;
		return num;
	}
public:
	qIODamPlusPoolSurvivalCell(struct river_segment *seg, qIODamSheet *table, int average, int powerhouse, int colspan = 1)
		:	qIODamPassageCell("Dam and Pool Passage", seg, table, MONTE_CARLO_STAT_MEAN, average, FALSE, powerhouse, colspan) {}
	virtual float getTotal(struct rls_seg *rseg, int game) {
		float denom = 1;
		for (struct rls_seg *top = rseg->up; top && top->river_segment->seg_type != DAM_SEG; top = top->up) {
			denom *= top->data[game]->stats.project_in_total - top->data[game]->stats.transport_in_total - top->data[game]->nopass;
		}
		denom *= rseg->data[game]->stats.project_in_total - rseg->data[game]->stats.transport_in_total - rseg->data[game]->nopass;
		return denom;
	}
	virtual float getHistData(struct rls_seg *rseg, int game, int i)  {return rseg->data[game]->output[i];}
};

/* Cumulative survival cells */
/* Cumulative inriver survival cell */
class qIOCumulativeInriverSurvivalCell : public qIODamPassageCell {
protected:
	virtual float getPassage(struct rls_seg *rseg, int game) {return rseg->data[game]->stats.total.total;}
public:
	qIOCumulativeInriverSurvivalCell(struct river_segment *seg, qIODamSheet *table, int powerhouse, int colspan = 1)
		:	qIODamPassageCell("Cumulative Inriver Survival", seg, table, MONTE_CARLO_STAT_MEAN, FALSE, FALSE, powerhouse, colspan) {}
	virtual float getTotal(struct rls_seg *rseg, int game) {
		float transported = 0;
		struct rls_seg *start; /* The start of the release */
		for (struct rls_seg *top = rseg; top; top = top->up) {
			if (top->river_segment->seg_type == DAM_SEG) transported += top->data[game]->stats.premort_transport_total;
			start = top;
		}
		return start->data[game]->stats.project_in_total - transported;
	}
};
/* Cumulative total survival cell */
class qIOCumulativeSurvivalCell : public qIODamPassageCell {
protected:
	virtual float getPassage(struct rls_seg *rseg, int game) {return getSegmentPassage(rseg, game);}
public:
	qIOCumulativeSurvivalCell(struct river_segment *seg, qIODamSheet *table, int powerhouse, int monte_carlo_stat = MONTE_CARLO_STAT_MEAN, int colspan = 1)
		:	qIODamPassageCell("Cumulative Total Survival", seg, table, monte_carlo_stat, FALSE, FALSE, powerhouse, colspan) {}
	static float getSegmentPassage(struct rls_seg *rseg, int game) {
		return rseg->data[game]->stats.total.total;
	}
	virtual float getTotal(struct rls_seg *rseg, int game) {return getSegmentTotal(rseg, game);}
	static float getSegmentTotal(struct rls_seg *rseg, int game) {
		struct rls_seg *top;
		for (top = rseg; top->up; top = top->up);
		return top->data[game]->stats.project_in_total;
	}
	virtual float getHistData(struct rls_seg *rseg, int game, int i)  {return rseg->data[game]->output[i];}
};

/* "Turbine Mortality at Individual Dams" cell */
class qIODamTurbineMortalityCell : public qIODamPassageCell {
protected:
	virtual float getPassage(struct rls_seg *rseg, int game) {if (rseg->data[game]->stats.turbine_in_total == 0.0) return 0.0; else return rseg->data[game]->stats.turbine_in_total*(1 - rseg->data[game]->stats.turbine_total/rseg->data[game]->stats.turbine_in_total);}
public:
	qIODamTurbineMortalityCell(struct river_segment *seg, qIODamSheet *table, int average, int powerhouse, int colspan = 1)
		:	qIODamPassageCell("Turbine Mortality", seg, table, MONTE_CARLO_STAT_MEAN, average, FALSE, powerhouse, colspan) {}
	virtual float getHistData(struct rls_seg *rseg, int game, int i)  {return (rseg->data[game]->turbine_in ? rseg->data[game]->turbine_in[i] : 0.0f) - (rseg->data[game]->turbine_pass ? rseg->data[game]->turbine_pass[i] : 0.0f);}
};

/* "Population Arriving" cell */
class qIODamPopulationArrivingCell : public qIODamPassageCell {
protected:
	virtual float getPassage(struct rls_seg *rseg, int game) {return rseg->data[game]->stats.project_in_total;}
public:
	qIODamPopulationArrivingCell(struct river_segment *seg, qIODamSheet *table, int average, int powerhouse, int colspan = 1)
		:	qIODamPassageCell("Arrival", seg, table, MONTE_CARLO_STAT_MEAN, average, FALSE, powerhouse, colspan) {}
	virtual float getHistData(struct rls_seg *rseg, int game, int i)  {return rseg->data[game]->project_in ? rseg->data[game]->project_in[i] : 0.0f;}
	virtual float getTotal(struct rls_seg *rseg, int game) {
		struct rls_seg *top;
		for (top = rseg; top->up; top = top->up);
		return top->data[game]->stats.project_in_total;
	}
};

/* "Collection Fraction" cell */
class qIODamCollectionFractionCell : public qIODamPassageCell {
protected:
	virtual float getPassage(struct rls_seg *rseg, int game) {return rseg->data[game]->stats.premort_transport_total;}
public:
	qIODamCollectionFractionCell(struct river_segment *seg, qIODamSheet *table, int average, int powerhouse, int colspan = 1)
		:	qIODamPassageCell("Transport Out", seg, table, MONTE_CARLO_STAT_MEAN, average, FALSE, powerhouse, colspan) {}
	virtual float getHistData(struct rls_seg *rseg, int game, int i)  {return rseg->data[game]->transport_out ? rseg->data[game]->transport_out[i] : 0.0f;}
};

/* "Population Collected" cell */
class qIODamPopulationCollectedCell : public qIODamPassageCell {
protected:
	virtual float getPassage(struct rls_seg *rseg, int game) {return rseg->data[game]->stats.premort_transport_total;}
public:
	qIODamPopulationCollectedCell(struct river_segment *seg, qIODamSheet *table, int average, int powerhouse, int colspan = 1)
		:	qIODamPassageCell("", seg, table, MONTE_CARLO_STAT_MEAN, average, FALSE, powerhouse, colspan) {}
	virtual float getTotal(struct rls_seg *rseg, int game) {
		struct rls_seg *top;
		for (top = rseg; top->up; top = top->up);
		return top->data[game]->stats.project_in_total;
	}
	virtual void cbDoubleClicked() {showOnlyMonteCarloGraph();}
};

/* Transport Arrival */
class qIODamTransportArrivalCell : public qIODamPassageCell {
protected:
	virtual float getPassage(struct rls_seg *rseg, int game);
public:
	qIODamTransportArrivalCell(struct river_segment *seg, qIODamSheet *table, int average, int powerhouse, int colspan = 1);
	virtual float getHistData(struct rls_seg *rseg, int game, int i);
	virtual void cbDoubleClicked();
	virtual float getTotal(struct rls_seg *rseg, int game);
};

/* Dam turbine passage cell */
class qIODamTurbinePassageCell : public qIODamPassageCell {
protected:
	virtual float getPassage(struct rls_seg *rseg, int game) {return rseg->data[game]->stats.turbine_in_total;}
public:
	qIODamTurbinePassageCell(struct river_segment *seg, qIODamSheet *table, int average, int powerhouse, int colspan = 1)
		:	qIODamPassageCell("Turbine Passage", seg, table, MONTE_CARLO_STAT_MEAN, average, FALSE, powerhouse, colspan) {}
	virtual float getHistData(struct rls_seg *rseg, int game, int i)  {return rseg->data[game]->turbine_in[i];}
};

/* Dam bypass passage cell */
class qIODamBypassPassageCell : public qIODamPassageCell {
protected:
	virtual float getPassage(struct rls_seg *rseg, int game) {return rseg->data[game]->stats.bypass_in_total;}
public:
	qIODamBypassPassageCell(struct river_segment *seg, qIODamSheet *table, int average, int powerhouse, int colspan = 1)
		:	qIODamPassageCell("Bypass Passage", seg, table, MONTE_CARLO_STAT_MEAN, average, FALSE, powerhouse, colspan) {}
	virtual float getHistData(struct rls_seg *rseg, int game, int i)  {return rseg->data[game]->bypass_in[i];}
};

/* Dam sluiceway passage cell */
class qIODamSluicewayPassageCell : public qIODamPassageCell {
protected:
	virtual float getPassage(struct rls_seg *rseg, int game) {return rseg->data[game]->stats.sluiceway_in_total;}
public:
	qIODamSluicewayPassageCell(struct river_segment *seg, qIODamSheet *table, int average, int powerhouse, int colspan = 1)
		:	qIODamPassageCell("Sluiceway Passage", seg, table, MONTE_CARLO_STAT_MEAN, average, FALSE, powerhouse, colspan) {}
	virtual float getHistData(struct rls_seg *rseg, int game, int i)  {return rseg->data[game]->sluiceway_in[i];}
};

/* Dam spill passage cell */
class qIODamSpillwayPassageCell : public qIODamPassageCell {
protected:
	virtual float getPassage(struct rls_seg *rseg, int game) {return rseg->data[game]->stats.spillway_in_total;}
public:
	qIODamSpillwayPassageCell(struct river_segment *seg, qIODamSheet *table, int average, int powerhouse, int colspan = 1)
		:	qIODamPassageCell("Conventional Spillway Passage", seg, table, MONTE_CARLO_STAT_MEAN, average, FALSE, powerhouse, colspan) {}
	virtual float getHistData(struct rls_seg *rseg, int game, int i)  {return rseg->data[game]->spillway_in[i];}
};

/* Dam RSW passage cell */
class qIODamRSWPassageCell : public qIODamPassageCell {
protected:
	virtual float getPassage(struct rls_seg *rseg, int game) {return rseg->data[game]->stats.rsw_in_total;}
public:
	qIODamRSWPassageCell(struct river_segment *seg, qIODamSheet *table, int average, int powerhouse, int colspan = 1)
		:	qIODamPassageCell("RSW Passage", seg, table, MONTE_CARLO_STAT_MEAN, average, FALSE, powerhouse, colspan) {}
	virtual float getHistData(struct rls_seg *rseg, int game, int i)  {return rseg->data[game]->rsw_in[i];}
};

/* Dam spill efficiency cell */
class qIODamSpillEfficiencyCell : public qIODamPassageCell {
protected:
	virtual float getPassage(struct rls_seg *rseg, int game) {return rseg->data[game]->stats.spill_efficiency;}
public:
	qIODamSpillEfficiencyCell(struct river_segment *seg, qIODamSheet *table, int average, int powerhouse, int colspan = 1)
		:	qIODamPassageCell("Spill Efficiency", seg, table, MONTE_CARLO_STAT_MEAN, average, FALSE, powerhouse, colspan) {}
	virtual float getTotal(struct rls_seg *rseg, int game) {return 1;}
	virtual void cbDoubleClicked() {showOnlyMonteCarloGraph();}
};

/* Dam diel passage cell */
class qIODamDielPassageCell : public qIODamPassageCell {
protected:
	virtual float getPassage(struct rls_seg *rseg, int game) {return rseg->data[game]->stats.night_passage;}
public:
	qIODamDielPassageCell(struct river_segment *seg, qIODamSheet *table, int average, int powerhouse, int colspan = 1)
		:	qIODamPassageCell("Diel Passage", seg, table, MONTE_CARLO_STAT_MEAN, average, FALSE, powerhouse, colspan) {}
	virtual float getTotal(struct rls_seg *rseg, int game) {return 1;}
	virtual float getHistData(struct rls_seg *rseg, int game, int i)  {
		double day = 0.0;
		double night = 0.0;
		const int start_step = (i / STEPS_PER_DAY) * STEPS_PER_DAY;

		for (int j=0; j < STEPS_PER_DAY; j++) {
			if (is_night(j)) night += rseg->data[game]->output[start_step + j];
			else day += rseg->data[game]->output[start_step + j];
		}
		if (day >= RESOLUTION || night >= RESOLUTION) return night / (day + night) / STEPS_PER_DAY;
		else return 0.0f;
	}
};

/* Transport arrivals vs inriver arrivals cell */
class qIOTotalTransportedCell : public qIODamPassageCell {
protected:
	virtual float getPassage(struct rls_seg *rseg, int game) {return rseg->data[game]->stats.transport_in_total;}
public:
	qIOTotalTransportedCell(struct river_segment *seg, qIODamSheet *table, int average, int powerhouse, int colspan = 1)
		:	qIODamPassageCell("Transported", seg, table, MONTE_CARLO_STAT_MEAN, average, FALSE, powerhouse, colspan) {}
	virtual float getHistData(struct rls_seg *rseg, int game, int i)  {return (rseg->data[game]->transport_in ? rseg->data[game]->transport_in[i] : 0.0);}
};
class qIOTotalInriverCell : public qIODamPassageCell {
protected:
	virtual float getPassage(struct rls_seg *rseg, int game) {return rseg->data[game]->stats.transport_in_total;}
public:
	qIOTotalInriverCell(struct river_segment *seg, qIODamSheet *table, int average, int powerhouse, int colspan = 1)
		:	qIODamPassageCell("Inriver", seg, table, MONTE_CARLO_STAT_MEAN, average, TRUE, powerhouse, colspan) {}
	virtual float getHistData(struct rls_seg *rseg, int game, int i)  {return rseg->data[game]->project_in[i] - (rseg->data[game]->transport_in ? rseg->data[game]->transport_in[i] : 0.0);}
};

/* Number of bypasses */
class qIODamBypassesCell : public qIODamPassageCell {
protected:
	int bypasses;
	virtual float getPassage(struct rls_seg *rseg, int game) {
		if (rseg->data[game]->stats.d) return rseg->data[game]->stats.d->prob_num_bypasses[bypasses];
		else setValid(FALSE);
		return 0.0;
	}
public:
	qIODamBypassesCell(struct river_segment *seg, qIODamSheet *table, int average, int powerhouse, int _bypasses, int colspan = 1)
		:	qIODamPassageCell("Fraction Inriver through " + QString::number(_bypasses) + " bypass" + (_bypasses != 1 ? "es" : ""), seg, table, MONTE_CARLO_STAT_MEAN, average, FALSE, powerhouse, colspan), bypasses(_bypasses) {}
	virtual float getTotal(struct rls_seg *rseg, int game) {return 1;}
	virtual void cbDoubleClicked() {showOnlyMonteCarloGraph();}
};

/* Return rates */
class qIOInriverReturnRateCell : public qIODamPassageCell {
protected:
	virtual float getPassage(struct rls_seg *rseg, int game) {return rseg->data[game]->stats.inriver_return;}
public:
	qIOInriverReturnRateCell(struct river_segment *seg, qIODamSheet *table, int average, int powerhouse, int colspan = 1)
		:	qIODamPassageCell("Inriver Return Rate", seg, table, MONTE_CARLO_STAT_MEAN, average, FALSE, powerhouse, colspan) {}
	virtual float getTotal(struct rls_seg *rseg, int game) {return 1;}
	virtual void cbDoubleClicked() {showOnlyMonteCarloGraph();}
};
class qIOTransportReturnRateCell : public qIODamPassageCell {
protected:
	virtual float getPassage(struct rls_seg *rseg, int game) {return rseg->data[game]->stats.transport_return;}
public:
	qIOTransportReturnRateCell(struct river_segment *seg, qIODamSheet *table, int average, int powerhouse, int colspan = 1)
		:	qIODamPassageCell("Inriver Return Rate", seg, table, MONTE_CARLO_STAT_MEAN, average, FALSE, powerhouse, colspan) {}
	virtual float getTotal(struct rls_seg *rseg, int game) {return 1;}
	virtual void cbDoubleClicked() {showOnlyMonteCarloGraph();}
};
class qIOOverallReturnRateCell : public qIODamPassageCell {
protected:
	virtual float getPassage(struct rls_seg *rseg, int game) {return getSegmentPassage(rseg, game);}
public:
	qIOOverallReturnRateCell(struct river_segment *seg, qIODamSheet *table, int average, int powerhouse, int colspan = 1)
		:	qIODamPassageCell("Overall Return Rate", seg, table, MONTE_CARLO_STAT_MEAN, average, FALSE, powerhouse, colspan) {}
	static float getSegmentPassage(struct rls_seg *rseg, int game) {return rseg->data[game]->stats.overall_return;}
	virtual float getTotal(struct rls_seg *rseg, int game) {return getSegmentTotal(rseg, game);}
	static float getSegmentTotal(struct rls_seg *rseg, int game) {return 1;}
	virtual void cbDoubleClicked() {showOnlyMonteCarloGraph();}
};
class qIOSARVsDateDifferentialReturnCell : public qIODamPassageCell {
protected:
	virtual float getPassage(struct rls_seg *rseg, int game) {return rseg->data[game]->stats.transport_return / rseg->data[game]->stats.inriver_return;}
public:
	qIOSARVsDateDifferentialReturnCell(struct river_segment *seg, qIODamSheet *table, int average, int powerhouse, int colspan = 1)
		:	qIODamPassageCell("Overall Return Rate", seg, table, MONTE_CARLO_STAT_MEAN, average, FALSE, powerhouse, colspan) {}
	virtual float getTotal(struct rls_seg *rseg, int game) {return 1;}
	virtual void cbDoubleClicked() {showOnlyMonteCarloGraph();}
};
class qIOSARVsDateOverallSurvivalCell : public qIODamPassageCell {
protected:
	virtual float getPassage(struct rls_seg *rseg, int game) {return qIOCumulativeSurvivalCell::getSegmentPassage(rseg, game) * qIOOverallReturnRateCell::getSegmentPassage(rseg, game);}
public:
	qIOSARVsDateOverallSurvivalCell(struct river_segment *seg, qIODamSheet *table, int average, int powerhouse, int colspan = 1)
		:	qIODamPassageCell("Overall Return Rate", seg, table, MONTE_CARLO_STAT_MEAN, average, FALSE, powerhouse, colspan) {}
	virtual float getTotal(struct rls_seg *rseg, int game) {return qIOCumulativeSurvivalCell::getSegmentTotal(rseg, game) * qIOOverallReturnRateCell::getSegmentTotal(rseg, game);}
	virtual void cbDoubleClicked() {showOnlyMonteCarloGraph();}
};
class qIOLatentMortalityDifferentialReturnCell : public qIODamPassageCell {
protected:
	virtual float getPassage(struct rls_seg *rseg, int game) {return rseg->data[game]->stats.transport_latent_mort / rseg->data[game]->stats.inriver_latent_mort;}
public:
	qIOLatentMortalityDifferentialReturnCell(struct river_segment *seg, qIODamSheet *table, int average, int powerhouse, int colspan = 1)
		:	qIODamPassageCell("Overall Return Rate", seg, table, MONTE_CARLO_STAT_MEAN, average, FALSE, powerhouse, colspan) {}
	virtual float getTotal(struct rls_seg *rseg, int game) {return 1;}
	virtual void cbDoubleClicked() {showOnlyMonteCarloGraph();}
};
class qIOLatentMortalityHydrosystemSurvivalCell : public qIODamPassageCell {
protected:
	virtual float getPassage(struct rls_seg *rseg, int game) {
		float total_in = rseg->data[game]->stats.project_in_total;
		float transport_in = rseg->data[game]->stats.transport_in_total;
		float inriver_in = total_in - transport_in;
		return qIOCumulativeSurvivalCell::getSegmentPassage(rseg, game) * (inriver_in/total_in * (1.0 - rseg->data[game]->stats.inriver_latent_mort) + transport_in/total_in *(1.0 - rseg->data[game]->stats.transport_latent_mort));
	}
public:
	qIOLatentMortalityHydrosystemSurvivalCell(struct river_segment *seg, qIODamSheet *table, int average, int powerhouse, int colspan = 1)
		:	qIODamPassageCell("Overall Return Rate", seg, table, MONTE_CARLO_STAT_MEAN, average, FALSE, powerhouse, colspan) {}
	virtual float getTotal(struct rls_seg *rseg, int game) {return qIOCumulativeSurvivalCell::getSegmentTotal(rseg, game);}
	virtual void cbDoubleClicked() {showOnlyMonteCarloGraph();}
};
class qIOConstantDSystemSurvivalCell : public qIODamPassageCell {
protected:
	virtual float getPassage(struct rls_seg *rseg, int game) {
		float total_in = rseg->data[game]->stats.project_in_total;
		float transport_in = rseg->data[game]->stats.transport_in_total;
		float inriver_in = total_in - transport_in;
		return qIOCumulativeSurvivalCell::getSegmentPassage(rseg, game) * (inriver_in/total_in + transport_in/total_in * rseg->data[game]->stats.differential_return);
	}
public:
	qIOConstantDSystemSurvivalCell(struct river_segment *seg, qIODamSheet *table, int average, int powerhouse, int colspan = 1)
		:	qIODamPassageCell("Overall Return Rate", seg, table, MONTE_CARLO_STAT_MEAN, average, FALSE, powerhouse, colspan) {}
	virtual float getTotal(struct rls_seg *rseg, int game) {return qIOCumulativeSurvivalCell::getSegmentTotal(rseg, game);}
	virtual void cbDoubleClicked() {showOnlyMonteCarloGraph();}
};
class qIOInriverLatentMortalityCell : public qIODamPassageCell {
protected:
	virtual float getPassage(struct rls_seg *rseg, int game) {return rseg->data[game]->stats.inriver_latent_mort;}
public:
	qIOInriverLatentMortalityCell(struct river_segment *seg, qIODamSheet *table, int average, int powerhouse, int colspan = 1)
		:	qIODamPassageCell("Overall Return Rate", seg, table, MONTE_CARLO_STAT_MEAN, average, FALSE, powerhouse, colspan) {}
	virtual float getTotal(struct rls_seg *rseg, int game) {return 1;}
	virtual void cbDoubleClicked() {showOnlyMonteCarloGraph();}
};
class qIOS3VsWTTSystemSurvivalCell : public qIODamPassageCell {
protected:
	virtual float getPassage(struct rls_seg *rseg, int game) {
		float total_in = rseg->data[game]->stats.project_in_total;
		float transport_in = rseg->data[game]->stats.transport_in_total;
		float inriver_in = total_in - transport_in;
		return qIOCumulativeSurvivalCell::getSegmentPassage(rseg, game) * (inriver_in/total_in + transport_in/total_in * rseg->data[game]->stats.differential_return) * (1.0 - rseg->data[game]->stats.inriver_latent_mort);
	}
public:
	qIOS3VsWTTSystemSurvivalCell(struct river_segment *seg, qIODamSheet *table, int average, int powerhouse, int colspan = 1)
		:	qIODamPassageCell("Overall Return Rate", seg, table, MONTE_CARLO_STAT_MEAN, average, FALSE, powerhouse, colspan) {}
	virtual float getTotal(struct rls_seg *rseg, int game) {return qIOCumulativeSurvivalCell::getSegmentTotal(rseg, game);}
	virtual void cbDoubleClicked() {showOnlyMonteCarloGraph();}
};


/* Cell that can be edited (and should therefore be tied to an interface
 * data control object, e.g. qIdcDamMortality. It is assumed that input
 * cells are floating point values only (this could be abstracted out and
 * is only specific to createEditor()).
 *
 * There is no signal when the cell is changed since we just use
 * QTable::valueChanged(row, col) */

/* Drag and drop types for input cells */
enum {
	DRAG_TEXT = 1, /* Simply copy the number between the cells */
	DRAG_SPILL_SCHEDULE /* Special case for spill schedule cells */
};

class qIOInputCell : public qIOCell {
	Q_OBJECT

public:
	qIOInputCell(qIOSheet* table, QTableItem::EditType et, int colspan = 1);

	/* Create the cell editor (since values can only be floating point
	 * numbers, a validator is used) */
	virtual QWidget* createEditor() const;

	/* Set the cell contents to the specified string. By default, this does nothing */
	virtual void setContent(const QString& str) {}

	virtual int rtti() const {return qIOInputCellRTTI;}

	/* Returns the type for dragging (this is because spill schedules and such
	 * don't fit into a single cell, so copying the text would make no sense). */
	virtual int dragType() {return DRAG_TEXT;}
	/* This method should be different for different drag types and should copy the
	 * contents from a like cell into this cell. */
	virtual void copy(qIOInputCell* source) {setContent(source->text());}

	virtual void cbDoubleClicked() {} /* Input cells don't show a histogram on double click (they probably become editable) */
	virtual QString getName() {return "Input Cell";} /* Not really used */

public slots:
	virtual void cbApply() = 0; /* Apply IDC cached data to data structure */
	virtual void cbReset() = 0; /* Reset IDC cache */
	virtual void calculate() = 0; /* Update the cell from the IDC */

signals:
	virtual void evDirtyChanged() = 0; /* If the cell's dirty status changes */
};

/* Max number of decimal places the validator will accept */
#define	IO_INPUT_DECIMAL_PLACES	10

/* This class is a utility class that takes care of a lot of details for
 * input cells that just modify a species parameter that is a broken
 * stick distribution in the old GUI. */
class qIOSpeciesParameterInputCell : public qIOInputCell {
	Q_OBJECT

protected:
	qIOSheet *ioSheet;
	qIdcListBase *idc; /* Pointer to the IDC */
	/* If the value that is being edited has a mean, low, and high value */
	int dist;

public:
	qIOSpeciesParameterInputCell(qIOSheet *ioSheet,
		qIdcListBase *idc, int dist, int colspan = 1);

	/* Create the cell editor (since values can only be floating point
	 * numbers in a fixed range defined by the IDC, a validator is used) */
	virtual QWidget* createEditor() const;

	/* Instead of copying the value from the editor, write to the interface
	 * data controller's cache for live updating and whatnot */
	virtual void setContentFromEditor(QWidget *w);
	virtual void setContent(const QString& str);

	virtual double value(int game);

protected:
	void updateDirty(); /* Update dirty flag from various qDatContainers */

public slots:
	virtual void cbApply();
	virtual void cbReset();
	virtual void calculate();

signals:
	virtual void evDirtyChanged();
};

/* Inriver Latent Mort. */
class qIOInriverLatentMortInputCell : public qIOSpeciesParameterInputCell {
public:
	qIOInriverLatentMortInputCell(qIOSheet *ioSheet, int colspan = 1)
		:	qIOSpeciesParameterInputCell(ioSheet, &qIdcInriverLatentMort_, FALSE, colspan) {}
};

/* Transport Latent Mort. */
class qIOTransportLatentMortInputCell : public qIOSpeciesParameterInputCell {
public:
	qIOTransportLatentMortInputCell(qIOSheet *ioSheet, int colspan = 1)
		:	qIOSpeciesParameterInputCell(ioSheet, &qIdcTransportLatentMort_, FALSE, colspan) {}
};

/* Differential return (Constant D) */
class qIODifferentialReturnInputCell : public qIOSpeciesParameterInputCell {
public:
	qIODifferentialReturnInputCell(qIOSheet *ioSheet, int colspan = 1)
		:	qIOSpeciesParameterInputCell(ioSheet, &qIdcDifferentialReturn_, FALSE, colspan) {}
};

/* This class is a utility class that takes care of a lot of details for
 * input cells that just modify a dam species parameter that is a broken
 * stick distribution in the old GUI. */
class qIODamParameterInputCell : public qIOInputCell, public qIODamSpecific {
	Q_OBJECT

protected:
	qIdcSelectBase *idc; /* Pointer to the IDC */
	int idcOffset; /* Offset to beginning of distribution in the IDC */
	/* If this cell should only be for dam cells without powerhouse cells or
	 * powerhouse cells. You can think of this as meaning it's actually a
	 * property of the powerhouse, but when the powerhouses are not listed
	 * separately, it is shown in a dam cell (e.g. FGE) */
	int forPowerhouse;
	/* If the qDatContainer is indexed by feature only (and not species) */
	int notPerSpecies;
	/* If the value that is being edited has a mean, low, and high value */
	int dist;

public:
	qIODamParameterInputCell(struct river_segment *seg, qIODamSheet *damSheet,
		qIdcSelectBase *idc, int idcOffset, int dist, int forPowerhouse, int notPerSpecies, int powerhouse, int colspan = 1);

	/* Create the cell editor (since values can only be floating point
	 * numbers in a fixed range defined by the IDC, a validator is used) */
	virtual QWidget* createEditor() const;

	/* Instead of copying the value from the editor, write to the interface
	 * data controller's cache for live updating and whatnot */
	virtual void setContentFromEditor(QWidget *w);
	virtual void setContent(const QString& str);

	virtual double value(int game);

protected:
	void updateDirty(); /* Update dirty flag from various qDatContainers */

public slots:
	virtual void cbApply();
	virtual void cbReset();
	virtual void calculate();

signals:
	virtual void evDirtyChanged();
};

/* Dam transport start */
class qIODamTransportStartInputCell : public qIODamParameterInputCell {
public:
	qIODamTransportStartInputCell(struct river_segment *seg, qIODamSheet *damSheet, int powerhouse, int colspan = 1)
		:	qIODamParameterInputCell(seg, damSheet, &qIdcTransportDates_, IDC_TRANSPORT_DATES_START, FALSE, 0, 1, powerhouse, colspan) {}
	virtual QString valueToText(double value) const {return QString::number((int)value);}
};
/* Dam transport stop */
class qIODamTransportStopInputCell : public qIODamParameterInputCell {
public:
	qIODamTransportStopInputCell(struct river_segment *seg, qIODamSheet *damSheet, int powerhouse, int colspan = 1)
		:	qIODamParameterInputCell(seg, damSheet, &qIdcTransportDates_, IDC_TRANSPORT_DATES_STOP, FALSE, 0, 1, powerhouse, colspan) {}
	virtual QString valueToText(double value) const {return QString::number((int)value);}
};

/* Dam FGE */
class qIODamDayFGEInputCell : public qIODamParameterInputCell {
public:
	qIODamDayFGEInputCell(struct river_segment *seg, qIODamSheet *damSheet, int powerhouse, int colspan = 1)
		:	qIODamParameterInputCell(seg, damSheet, &qIdcFGE_, IDC_DAM_FGE_DAY*3, TRUE, 1, 0, powerhouse, colspan) {}
};
class qIODamNightFGEInputCell : public qIODamParameterInputCell {
public:
	qIODamNightFGEInputCell(struct river_segment *seg, qIODamSheet *damSheet, int powerhouse, int colspan = 1)
		:	qIODamParameterInputCell(seg, damSheet, &qIdcFGE_, IDC_DAM_FGE_NIGHT*3, TRUE, 1, 0, powerhouse, colspan) {}
};

/* Dam Sluiceway/SBC */
class qIODamSluicewayPropInputCell : public qIODamParameterInputCell {
public:
	qIODamSluicewayPropInputCell(struct river_segment *seg, qIODamSheet *damSheet, int powerhouse, int colspan = 1)
		:	qIODamParameterInputCell(seg, damSheet, &qIdcSluicewayProp_, 0, TRUE, 1, 0, powerhouse, colspan) {}
};

/* Dam survival input cells */
/* Dam survival cell */
class qIODamSurvivalInputCell : public qIODamParameterInputCell {
public:
	qIODamSurvivalInputCell(struct river_segment *seg, qIODamSheet *damSheet,
			qIdcSelectBase *idc, int idcOffset, int forPowerhouse, int powerhouse, int colspan = 1)
			: qIODamParameterInputCell(seg, damSheet, idc, idcOffset, TRUE, forPowerhouse, FALSE, powerhouse, colspan) {}
	/* Convert from mortalities to survivals */
	virtual QString valueToText(double value) const {return QString::number(1.0 - value, 'f', IO_CELL_PRECISION);}
	virtual double textToValue(const QString& text) const {return 1.0 - text.toDouble();}

};
/* Dam species turbine survival input cell */
class qIODamTurbineSurvivalInputCell : public qIODamSurvivalInputCell {
public:
	qIODamTurbineSurvivalInputCell(struct river_segment *seg, qIODamSheet *damSheet, int powerhouse, int colspan = 1)
		:	qIODamSurvivalInputCell(seg, damSheet, &qIdcDamMortality_, IDC_DAM_MORT_OFF_TURBINE*3, 1, powerhouse, colspan) {}
};
/* Dam species spillway survival input cell */
class qIODamSpillwaySurvivalInputCell : public qIODamSurvivalInputCell {
public:
	qIODamSpillwaySurvivalInputCell(struct river_segment *seg, qIODamSheet *damSheet, int powerhouse, int colspan = 1)
		:	qIODamSurvivalInputCell(seg, damSheet, &qIdcDamMortality_, IDC_DAM_MORT_OFF_SPILL*3, 0, powerhouse, colspan) {}
};
/* Dam species RSW survival input cell */
class qIODamRSWSurvivalInputCell : public qIODamSurvivalInputCell {
public:
	qIODamRSWSurvivalInputCell(struct river_segment *seg, qIODamSheet *damSheet, int powerhouse, int colspan = 1)
		:	qIODamSurvivalInputCell(seg, damSheet, &qIdcDamMortality_, IDC_DAM_MORT_OFF_RSW*3, 0, powerhouse, colspan) {}
};
/* Dam species bypass survival input cell */
class qIODamBypassSurvivalInputCell : public qIODamSurvivalInputCell {
public:
	qIODamBypassSurvivalInputCell(struct river_segment *seg, qIODamSheet *damSheet, int powerhouse, int colspan = 1)
		:	qIODamSurvivalInputCell(seg, damSheet, &qIdcDamMortality_, IDC_DAM_MORT_OFF_BYPASS*3, 1, powerhouse, colspan) {}
};
/* Dam species sluiceway survival input cell */
class qIODamSluicewaySurvivalInputCell : public qIODamSurvivalInputCell {
public:
	qIODamSluicewaySurvivalInputCell(struct river_segment *seg, qIODamSheet *damSheet, int powerhouse, int colspan = 1)
		:	qIODamSurvivalInputCell(seg, damSheet, &qIdcDamMortality_, IDC_DAM_MORT_OFF_SLUICEWAY*3, 1, powerhouse, colspan) {}
};
/* Dam species transport survival input cell */
class qIODamTransportSurvivalInputCell : public qIODamSurvivalInputCell {
public:
	qIODamTransportSurvivalInputCell(struct river_segment *seg, qIODamSheet *damSheet, int powerhouse, int colspan = 1)
		:	qIODamSurvivalInputCell(seg, damSheet, &qIdcDamMortality_, IDC_DAM_MORT_OFF_TRANSPORT*3, 0, powerhouse, colspan) {}
};

/* Base (and concrete) class for dam spill averages/schedules. */
enum {
	DAM_AVG_SPILL_DAY = 1,
	DAM_AVG_SPILL_NIGHT,
	DAM_AVG_SPILL_BOTH
};

class qIODamSpillInputCell : public qIOInputCell, public qIODamSpecific {
	Q_OBJECT

protected:
	int periods; /* Should contain a value from the above enumeration */
	qIdcScheduleBase *idc;

public:
	qIODamSpillInputCell(struct river_segment *seg, qIODamSheet *damSheet, int periods, int powerhouse, int colspan = 1);

	/* Open up spill schedule window */
	virtual QWidget* createEditor() const;
	/* Don't do anything on setConentFromEditor() */
	virtual void setContentFromEditor(QWidget *w);

	virtual int dragType() {return DRAG_SPILL_SCHEDULE;}
	virtual void copy(qIOInputCell* source); /* Copy the spill schedule */

	/* Used to implement copy and paste between spill schedule cells */
	const char* getSegName() {return seg->seg_name;}
	int getPeriods() {return periods;}

	virtual double value(int game) {return 0.0;}

protected:
	void updateDirty(); /* Update dirty flag from various qDatContainers */

public slots:
	virtual void cbApply();
	virtual void cbReset();
	virtual void calculate();

signals:
	virtual void evDirtyChanged();
};

/* Powerhouse priority input cell. This probably only works well with 2 powerhouse
 * (i.e. three or more can't be input with one cell easily. */
class qIODamPowerhousePriorityInputCell : public qIOInputCell, public qIODamSpecific {
	Q_OBJECT

public:
	qIODamPowerhousePriorityInputCell(struct river_segment *seg, qIODamSheet *damSheet, int powerhouse, int colspan = 1);

	/* Use a combo box to edit the cell */
	virtual QWidget* createEditor() const;
	/* Write to the interface data controller's cache based on selection */
	virtual void setContentFromEditor(QWidget *w);

	virtual double value(int game) {return 0.0;}

protected:
	int getIdcRow(); /* Get the row in the IDC for this dam */
	void updateDirty(); /* Update dirty flag from various qDatContainers */

public slots:
	virtual void cbApply();
	virtual void cbReset();
	virtual void calculate();

signals:
	virtual void evDirtyChanged();
};

/* Diel passage cell. Note that since diel passage is not set directly in
 * this model, we need a bit of GUI glue in order to fake it. This cell
 * provides a drop-down box with some diel passage values that set the
 * actual parameters appropriately.
 * Also note that this cell modifies the diel passage for ALL species. */

class qIODamDielPassageInputCell : public qIOInputCell, public qIODamSpecific {
	Q_OBJECT

public:
	qIODamDielPassageInputCell(struct river_segment *seg, qIODamSheet *damSheet, int powerhouse, int colspan = 1);

	/* Use a combo box to edit the cell */
	virtual QWidget* createEditor() const;
	/* Write to the interface data controller's cache based on selection */
	virtual void setContentFromEditor(QWidget *w);

	virtual double value(int game) {return 0.0;}

protected:
	void updateDirty(); /* Update dirty flag from various qDatContainers */

public slots:
	virtual void cbApply();
	virtual void cbReset();
	virtual void calculate();

signals:
	virtual void evDirtyChanged();
};

/* Drop-down cell for selecting integer global variables from a list of strings.
 * This is used, for example, to select post-Bonneville hypotheses. */

class qIOGlobalDropDownInputCell : public qIOInputCell {
	Q_OBJECT

private:
	qIdcDataInt *idc;
	int *setting;
	QStringList options;

public:
	qIOGlobalDropDownInputCell(qIdcDataInt *idc, int *setting, const char **options, int num_options, qIOSheet* sheet, int colspan = 1);

	/* Use a combo box to edit the cell */
	virtual QWidget* createEditor() const;
	/* Write to the interface data controller's cache based on selection */
	virtual void setContentFromEditor(QWidget *w);

	virtual double value(int game) {return 0.0;}

protected:
	void updateDirty(); /* Update dirty flag from various qDatContainers */

public slots:
	virtual void cbApply();
	virtual void cbReset();
	virtual void calculate();
	void clear(); /** This is used to make the changed take effect immediately */

signals:
	virtual void evDirtyChanged();
};

class qIOPostBonnevilleHypothesisInputCell : public qIOGlobalDropDownInputCell {
public:
	qIOPostBonnevilleHypothesisInputCell(qIOSheet *sheet, int colspan = 1)
		: qIOGlobalDropDownInputCell(&postBonnevilleHypothesisIDC, &run[0].post_bonneville_hypothesis, post_bonneville_hypothesis_names_formatted, NUM_POST_BONNEVILLE, sheet)
	{}
};

#endif
