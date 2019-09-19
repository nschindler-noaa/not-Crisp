#include "qIOSIMPASSheet.h"
#include "qIOCell.h"
#include "qScenarioManager.h"

/* qIOSIMPASSheet SIMPAS I/O sheet implementation */
qIOSIMPASSheet::qIOSIMPASSheet(QWidget *parent, qIOTool *tool, const QString& name)
:	qIODamSheet(parent, tool, name, 38, 2)
{
	if (!connect(&scenarioManager, SIGNAL(running()), this, SLOT(cbRunningScenario()))
		|| !connect(ioTool, SIGNAL(evAverageChanged()), this, SLOT(cbAverageChanged())))
		log_msg(L_ERROR, "Could not connect SIMPAS I/O sheet signal handlers\n");
}

void qIOSIMPASSheet::setup() {
	unsigned int i;
	QPtrListIterator<struct river_segment> it(dams);
	struct river_segment *seg;
	int row = -1;

	int below_bon, std_dev, average;
	int monte_carlo = getGameCount() > 1;

	/* An Monte Carlo column if in Monte Carlo mode */
	if (monte_carlo) {
		setNumCols(numCols() + 1);
		below_bon = numCols() - 3;
		std_dev = numCols() - 2;
	}
	else {
		below_bon = numCols() - 2;
		std_dev = -1;
	}
	average = numCols() - 1;

	/* Extra horizontal headers */
	horizontalHeader()->setLabel(0, "");
	if (ioTool->getFullScenario()) {
		horizontalHeader()->setLabel(below_bon, "Below BON");
		if (monte_carlo) horizontalHeader()->setLabel(std_dev, "Std. Dev.");
		horizontalHeader()->setLabel(average, "Average");
	}

	/* Name vertical headers */
	if (ioTool->getFullScenario()) {
		setItem(++row, 0, (qTableItem*) new qTableItem(this, QTableItem::Never, FALSE, TRUE, "INRIVER SURVIVAL ESTIMATES"));
		item(row, 0)->setSpan(1, numCols());

		setItem(++row, 0, (qTableItem*) new qIOStaticCell("Pool Survival", this));
		setItem(++row, 0, (qTableItem*) new qIOStaticCell("Dam Survival", this));
		setItem(++row, 0, (qTableItem*) new qIOStaticCell("Dam Plus Pool Survival", this));
		setItem(row, average, (qTableItem*) new qIODamPlusPoolInriverSurvivalCell(NULL, this, DAM_PASSAGE_AVERAGE, FALSE));
		setItem(++row, 0, (qTableItem*) new qIOStaticCell("Cumulative Inriver Survival", this));
		setItem(row, below_bon, (qTableItem*) new qIODamPlusPoolInriverSurvivalCell(transport_destination, this, DAM_PASSAGE_CUMULATIVE, FALSE));
		setItem(++row, 0, (qTableItem*) new qIOStaticCell("Turbine Mortality", this));

		setItem(++row, 0, (qTableItem*) new qTableItem(this, QTableItem::Never, FALSE, TRUE, "TRANSPORT ESTIMATES"));
		item(row, 0)->setSpan(1, numCols());

		setItem(++row, 0, (qTableItem*) new qIOStaticCell("Population Arriving", this));
		setItem(++row, 0, (qTableItem*) new qIOStaticCell("Collection Fraction", this));
		setItem(++row, 0, (qTableItem*) new qIOStaticCell("Population Collected", this));
		setItem(++row, 0, (qTableItem*) new qIOStaticCell("Cumulative Pop. Collected", this));

		setItem(++row, 0, (qTableItem*) new qTableItem(this, QTableItem::Never, FALSE, TRUE, "OUTPUTS"));
		item(row, 0)->setSpan(1, numCols());

		setItem(++row, 0, (qTableItem*) new qIOStaticCell("Cumulative Total Survival", this));
		setItem(row, below_bon, (qTableItem*) new qIOCumulativeSurvivalCell(transport_destination, this, FALSE));
		if (getGameCount() > 1) setItem(row, std_dev, (qTableItem*) new qIOCumulativeSurvivalCell(transport_destination, this, FALSE, MONTE_CARLO_STAT_STDDEV));
		setItem(++row, 0, (qTableItem*) new qIOStaticCell("Fish Passage Efficiency", this));
		setItem(row, average, (qTableItem*) new qIODamFPECell(NULL, this, DAM_PASSAGE_AVERAGE, FALSE));
		setItem(++row, 0, (qTableItem*) new qIOStaticCell("Turbine Passage", this));
		setItem(row, average, (qTableItem*) new qIODamTurbinePassageCell(NULL, this, DAM_PASSAGE_AVERAGE, FALSE));
		setItem(++row, 0, (qTableItem*) new qIOStaticCell("Bypass Passage", this));
		setItem(row, average, (qTableItem*) new qIODamBypassPassageCell(NULL, this, DAM_PASSAGE_AVERAGE, FALSE));
		setItem(++row, 0, (qTableItem*) new qIOStaticCell("Sluiceway Passage", this));
		setItem(row, average, (qTableItem*) new qIODamSluicewayPassageCell(NULL, this, DAM_PASSAGE_AVERAGE, FALSE));
		setItem(++row, 0, (qTableItem*) new qIOStaticCell("Spill Passage (Normal Bays Only)", this));
		setItem(row, average, (qTableItem*) new qIODamSpillwayPassageCell(NULL, this, DAM_PASSAGE_AVERAGE, FALSE));
		setItem(++row, 0, (qTableItem*) new qIOStaticCell("RSW Passage", this));
		setItem(row, average, (qTableItem*) new qIODamRSWPassageCell(NULL, this, DAM_PASSAGE_AVERAGE, FALSE));
		setItem(++row, 0, (qTableItem*) new qIOStaticCell("Spill Efficiency", this));
		setItem(++row, 0, (qTableItem*) new qIOStaticCell("Diel Passage", this));

		setItem(++row, 0, (qTableItem*) new qTableItem(this, QTableItem::Never, FALSE, TRUE, "TRANSPORT INPUTS"));
		item(row, 0)->setSpan(1, numCols());

		setItem(++row, 0, (qTableItem*) new qIOStaticCell("Transport Start (Julian Day)", this));
		setItem(++row, 0, (qTableItem*) new qIOStaticCell("Transport Stop (Julian Day)", this));
		setItem(++row, 0, (qTableItem*) new qIOStaticCell("Transport Survival", this));
	}

	setItem(++row, 0, (qTableItem*) new qTableItem(this, QTableItem::Never, FALSE, TRUE, "FLOW INPUTS"));
	item(row, 0)->setSpan(1, numCols());
	setItem(averageFlowRow = ++row, 0, (qTableItem*) new qIOStaticCell("", this));
	setItem(averageSpillRow = ++row, 0, (qTableItem*) new qIOStaticCell("", this));
	setItem(daySpillRow = ++row, 0, (qTableItem*) new qIOStaticCell("", this));
	setItem(nightSpillRow = ++row, 0, (qTableItem*) new qIOStaticCell("", this));

	cbAverageChanged(); /* Set labels on average flow, day/night spill rows */

	if (ioTool->getFullScenario()) {
		setItem(++row, 0, (qTableItem*) new qTableItem(this, QTableItem::Never, FALSE, TRUE, "PASSAGE AND SURVIVAL INPUTS"));
		item(row, 0)->setSpan(1, numCols());

		//setItem(++row, 0, (qTableItem*) new qIOStaticCell("FGE (Day)", this));
		//setItem(++row, 0, (qTableItem*) new qIOStaticCell("FGE (Night)", this));
		setItem(++row, 0, (qTableItem*) new qIOStaticCell("Sluiceway/SBC Prop.", this));
		setItem(++row, 0, (qTableItem*) new qIOStaticCell("Powerhouse Priority", this));

		setItem(++row, 0, (qTableItem*) new qIOStaticCell("Turbine Survival", this));
		setItem(++row, 0, (qTableItem*) new qIOStaticCell("Spillway Survival", this));
		setItem(++row, 0, (qTableItem*) new qIOStaticCell("RSW Survival", this));
		setItem(++row, 0, (qTableItem*) new qIOStaticCell("Bypass Survival", this));
		setItem(++row, 0, (qTableItem*) new qIOStaticCell("Sluiceway/SBC Survival", this));

		/* Add dam-specific cells and name the horizontal header */
		for (i=1; seg = it.current(); i++, ++it) {
			setDamCol(seg, &i);
		}
	}
}

void qIOSIMPASSheet::setDamCol(struct river_segment *seg, unsigned int *col, struct river_segment *dam, int powerhouseNum) {
	/* Name header */
	if (dam && dam->abbrev) {
		horizontalHeader()->setLabel(*col, QString(dam->abbrev) + " PH" + QString::number(powerhouseNum));
	}
	else if (seg->abbrev) horizontalHeader()->setLabel(*col, QString(seg->abbrev));

	/* Add rows */
	int row = 0;
	int powerhouse = dam ? 1 : 0; /* If the cell is for a powerhouse */

	if (ioTool->getFullScenario()) {
		row++;
		setItem(row++, *col, (qTableItem*) new qIODamPoolSurvivalCell(seg, this, FALSE, powerhouse));
		setItem(row++, *col, (qTableItem*) new qIODamSurvivalCell(seg, this, FALSE, powerhouse));
		setItem(row++, *col, (qTableItem*) new qIODamPlusPoolInriverSurvivalCell(seg, this, FALSE, powerhouse));
		setItem(row++, *col, (qTableItem*) new qIODamPlusPoolInriverSurvivalCell(seg, this, DAM_PASSAGE_CUMULATIVE, powerhouse));
		setItem(row++, *col, (qTableItem*) new qIODamTurbineMortalityCell(seg, this, FALSE, powerhouse));

		row++;
		setItem(row++, *col, (qTableItem*) new qIODamPopulationArrivingCell(seg, this, FALSE, powerhouse));
		setItem(row++, *col, (qTableItem*) new qIODamCollectionFractionCell(seg, this, FALSE, powerhouse));
		setItem(row++, *col, (qTableItem*) new qIODamPopulationCollectedCell(seg, this, FALSE, powerhouse));
		setItem(row++, *col, (qTableItem*) new qIODamPopulationCollectedCell(seg, this, DAM_PASSAGE_SUM, powerhouse));

		row++;
		row++;/*setItem(row++, *col, (qTableItem*) new qIOCumulativeSurvivalCell(seg, this, powerhouse));*/
		setItem(row++, *col, (qTableItem*) new qIODamFPECell(seg, this, FALSE, powerhouse));
		setItem(row++, *col, (qTableItem*) new qIODamTurbinePassageCell(seg, this, FALSE, powerhouse));
		setItem(row++, *col, (qTableItem*) new qIODamBypassPassageCell(seg, this, FALSE, powerhouse));
		setItem(row++, *col, (qTableItem*) new qIODamSluicewayPassageCell(seg, this, FALSE, powerhouse));
		setItem(row++, *col, (qTableItem*) new qIODamSpillwayPassageCell(seg, this, FALSE, powerhouse));
		setItem(row++, *col, (qTableItem*) new qIODamRSWPassageCell(seg, this, FALSE, powerhouse));
		setItem(row++, *col, (qTableItem*) new qIODamSpillEfficiencyCell(seg, this, FALSE, powerhouse));
		setItem(row++, *col, (qTableItem*) new qIODamDielPassageCell(seg, this, FALSE, powerhouse));
	}

	row++;
	setItem(row++, *col, (qIOInputCell*) new qIODamTransportStartInputCell(seg, this, powerhouse));
	setItem(row++, *col, (qIOInputCell*) new qIODamTransportStopInputCell(seg, this, powerhouse));
	setItem(row++, *col, (qIOInputCell*) new qIODamTransportSurvivalInputCell(seg, this, powerhouse));

	row++;
	setItem(row++, *col, (qTableItem*) new qIODamFlowCell(seg, this, powerhouse));
	setItem(row++, *col, (qTableItem*) new qIODamSpillCell(seg, this, powerhouse));
	setItem(row++, *col, (qIOInputCell*) new qIODamSpillInputCell(seg, this, DAM_AVG_SPILL_DAY, powerhouse));
	setItem(row++, *col, (qIOInputCell*) new qIODamSpillInputCell(seg, this, DAM_AVG_SPILL_NIGHT, powerhouse));

	row++;
	//setItem(row++, *col, (qIOInputCell*) new qIODamDayFGEInputCell(seg, this, powerhouse));
	//setItem(row++, *col, (qIOInputCell*) new qIODamNightFGEInputCell(seg, this, powerhouse));
	setItem(row++, *col, (qIOInputCell*) new qIODamSluicewayPropInputCell(seg, this, powerhouse));
	setItem(row++, *col, (qIOInputCell*) new qIODamPowerhousePriorityInputCell(seg, this, powerhouse));

	setItem(row++, *col, (qIOInputCell*) new qIODamTurbineSurvivalInputCell(seg, this, powerhouse));
	setItem(row++, *col, (qIOInputCell*) new qIODamSpillwaySurvivalInputCell(seg, this, powerhouse));
	setItem(row++, *col, (qIOInputCell*) new qIODamRSWSurvivalInputCell(seg, this, powerhouse));
	setItem(row++, *col, (qIOInputCell*) new qIODamBypassSurvivalInputCell(seg, this, powerhouse));
	setItem(row++, *col, (qIOInputCell*) new qIODamSluicewaySurvivalInputCell(seg, this, powerhouse));

	row++;

	if (seg->current->dam.additional_pwrhs) {
		/* Add column for main powerhouse if we haven't done so already */
		if (!dam) {
			(*col)++;
			dam = seg;
			setDamCol(seg, col, dam, ++powerhouseNum);
		}
		/* Otherwise, just add the additional powerhouses */
		else {
			(*col)++;
			setDamCol(seg->current->dam.additional_pwrhs, col, dam ? dam : seg, powerhouseNum + 1);
		}
	}
}

void qIOSIMPASSheet::cbRunningScenario() {
	ioTool->enableTracking();
}

void qIOSIMPASSheet::cbAverageChanged() {
	item(averageFlowRow, 0)->setText(QString("Avg. Flow (KCFS) (") + ioTool->getAverageStartName() + " - " + ioTool->getAverageStopName() + ")");
	updateCell(averageFlowRow, 0);
	item(averageSpillRow, 0)->setText(QString("Avg. Spill (KCFS) (") + ioTool->getAverageStartName() + " - " + ioTool->getAverageStopName() + ")");
	updateCell(averageSpillRow, 0);
	item(daySpillRow, 0)->setText(QString("Avg. Day Spill Fraction (") + ioTool->getAverageStartName() + " - " + ioTool->getAverageStopName() + ")");
	updateCell(daySpillRow, 0);
	item(nightSpillRow, 0)->setText(QString("Avg. Night Spill Fraction (") + ioTool->getAverageStartName() + " - " + ioTool->getAverageStopName() + ")");
	updateCell(nightSpillRow, 0);
}
