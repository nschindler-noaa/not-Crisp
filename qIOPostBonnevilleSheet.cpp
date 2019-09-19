#include "crisp.h"
#include "qIOPostBonnevilleSheet.h"
#include "qIOCell.h"

qIOPostBonnevilleSheet::qIOPostBonnevilleSheet(QWidget *parent, qIOTool *tool, const QString& name)
:	qIODamSheet(parent, tool, name, 10, 0)
{
	hypothesisRow = 0;
	lastHypothesis = -1;

	if (!connect(&postBonnevilleHypothesisIDC, SIGNAL(invalidated()), this, SLOT(cbPostBonnevilleHypothesisChanged())))
		log_msg(L_ERROR, "Could not connect I/O sheet post-Bonneville signal handlers\n");
}

void qIOPostBonnevilleSheet::setup() {
	QPtrListIterator<struct river_segment> it(dams);
	struct river_segment *seg;
	int row = -1;
	unsigned int i;

	setItem(++row, 0, (qTableItem*) new qTableItem(this, QTableItem::Never, FALSE, TRUE, "INRIVER FISH"));
	item(row, 0)->setSpan(1, numCols());

	/* Proportion arriving */
	setItem(++row, 0, (qTableItem*) new qIOStaticCell("Proportion Arriving", this));
	setItem(row, 1, (qTableItem*) new qIOTotalInriverCell(transport_destination, this, FALSE, FALSE));

	++row; /* Inriver bypass number labels */
	/* Count collector dams */
	unsigned int collector_dams = 0;
	for (; seg = it.current(); ++it) {
		if (seg->current->dam.collector) collector_dams++;
	}

	/* Print a message if there were no collector dams */
	if (it.count() && collector_dams <= 0) log_msg(L_WARNING, "There were no collector dams found. Please use the most recent river description file.\n");

	/* Add labels and cells for number of bypasses */
	for (i=0; i <= collector_dams; i++) {
		const int bypasses = i;
		setItem(row, i + 1, (qTableItem*) new qTableItem(this, QTableItem::Never, FALSE, TRUE, QString("") + QString::number(bypasses) + " bypass" + (bypasses != 1 ? "es" : "")));
		setItem(row + 1, i + 1, (qTableItem*) new qIODamBypassesCell(transport_destination, this, FALSE, FALSE, bypasses));
	}
	it = QPtrListIterator<struct river_segment>(dams);
	setItem(++row, 0, (qTableItem*) new qIOStaticCell("Inriver Bypasses", this));

	setItem(++row, 0, (qTableItem*) new qTableItem(this, QTableItem::Never, FALSE, TRUE, "TRANSPORTED FISH"));
	item(row, 0)->setSpan(1, numCols());

	/* Proportion Destined */
	setItem(++row, 0, (qTableItem*) new qIOStaticCell("Proportion Destined", this));
	setItem(row, 1, (qTableItem*) new qIODamCollectionFractionCell(transport_destination, this, DAM_PASSAGE_REMAINDER_SUM, FALSE));

	/* Proportion arriving */
	setItem(++row, 0, (qTableItem*) new qIOStaticCell("Proportion Arriving", this));
	setItem(row, 1, (qTableItem*) new qIOTotalTransportedCell(transport_destination, this, FALSE, FALSE));

	/* TODO: Duplicate the dam labels so it's clear what the columns are here (and not as easily confused with the bypass number labels above here) */

	setItem(++row, 0, (qTableItem*) new qIOStaticCell("Transport Arrivals From...", this));

	setItem(++row, 0, (qTableItem*) new qTableItem(this, QTableItem::Never, FALSE, TRUE, "POST-BONNEVILLE SURVIVAL"));
	item(row, 0)->setSpan(1, numCols());

	/* Hypothesis row */
	setItem(hypothesisRow = ++row, 0, (qTableItem*) new qIOStaticCell("Hypothesis", this));
	setHypothesisItem(row, 1, new qIOPostBonnevilleHypothesisInputCell(this));

	/* Other headers */
	horizontalHeader()->setLabel(0, "");

	/* Add dam-specific cells and name the horizontal header */
	for (i=1; seg = it.current(); i++, ++it) {
		setDamCol(seg, &i);
	}

	/* The hypothesis-specific stuff is added from cbPostBonnevilleHypothesisChanged() */
}

void qIOPostBonnevilleSheet::setDamCol(struct river_segment *seg, unsigned int *col, struct river_segment *dam, int powerhouseNum) {
	/* Name header */
	if (dam && dam->abbrev) {
		horizontalHeader()->setLabel(*col, QString(dam->abbrev) + " PH" + QString::number(powerhouseNum));
	}
	else if (seg->abbrev) horizontalHeader()->setLabel(*col, QString(seg->abbrev));

	/* Add rows */
	int row = 0;
	int powerhouse = dam ? 1 : 0; /* If the cell is for a powerhouse */

	++row;
	++row;
	++row;
	++row;
	++row;
	++row;
	++row;
	if (seg->current->dam.collector) setItem(row, *col, (qTableItem*) new qIODamTransportArrivalCell(seg, this, FALSE, powerhouse));

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

void qIOPostBonnevilleSheet::cbPostBonnevilleHypothesisChanged() {
	if (hypothesisRow <= 0) return; /* The main part of the sheet hasn't been created yet */

	/* Changes should only be made if everything has been loaded and run (otherwise this will crash) */
	if (ioTool->isLoading()) return;

	if (lastHypothesis >= 0 && lastHypothesis == postBonnevilleHypothesisIDC.getData()) return;
	else lastHypothesis = postBonnevilleHypothesisIDC.getData();

	int row = hypothesisRow;

	int totalRows = numRows();
	for (int i=0; i < totalRows - (hypothesisRow + 1); i++) removeRow(hypothesisRow + 1);
	setNumRows(hypothesisRow + 1);

	switch (postBonnevilleHypothesisIDC.getData()) {
	case POST_BONNEVILLE_SAR_VS_DATE:
		setNumRows((hypothesisRow + 1) + 5);


		setItem(++row, 0, (qTableItem*) new qIOStaticCell("Inriver Return Rate", this));
		setItem(row, 1, (qTableItem*) new qIOInriverReturnRateCell(default_transport_target, this, FALSE, FALSE));

		setItem(++row, 0, (qTableItem*) new qIOStaticCell("Transport Return Rate", this));
		setItem(row, 1, (qTableItem*) new qIOTransportReturnRateCell(default_transport_target, this, FALSE, FALSE));
		
		setItem(++row, 0, (qTableItem*) new qIOStaticCell("Differential Return (D)", this));
		setItem(row, 1, (qTableItem*) new qIOSARVsDateDifferentialReturnCell(default_transport_target, this, FALSE, FALSE));
		
		setItem(++row, 0, (qTableItem*) new qIOStaticCell("Overall Return Rate", this));
		setItem(row, 1, (qTableItem*) new qIOOverallReturnRateCell(default_transport_target, this, FALSE, FALSE));
		
		setItem(++row, 0, (qTableItem*) new qIOStaticCell("Overall Survival", this));
		setItem(row, 1, (qTableItem*) new qIOSARVsDateOverallSurvivalCell(default_transport_target, this, FALSE, FALSE));
	break;
	case POST_BONNEVILLE_LATENT_MORTALITY:
		setNumRows((hypothesisRow + 1) + 4);


		setItem(++row, 0, (qTableItem*) new qIOStaticCell("Inriver Latent Mort.", this));
		setItem(row, 1, (qIOInputCell*) new qIOInriverLatentMortInputCell(this));

		setItem(++row, 0, (qTableItem*) new qIOStaticCell("Transport Latent Mort.", this));
		setItem(row, 1, (qIOInputCell*) new qIOTransportLatentMortInputCell(this));
		
		setItem(++row, 0, (qTableItem*) new qIOStaticCell("Differential Return (D)", this));
		setItem(row, 1, (qTableItem*) new qIOLatentMortalityDifferentialReturnCell(default_transport_target, this, FALSE, FALSE));
		
		setItem(++row, 0, (qTableItem*) new qIOStaticCell("Hydrosystem Survival", this));
		setItem(row, 1, (qTableItem*) new qIOLatentMortalityHydrosystemSurvivalCell(default_transport_target, this, FALSE, FALSE));
	break;
	case POST_BONNEVILLE_CONSTANT_D:
		setNumRows((hypothesisRow + 1) + 2);

		
		setItem(++row, 0, (qTableItem*) new qIOStaticCell("Differential Return (D)", this));
		setItem(row, 1, (qIOInputCell*) new qIODifferentialReturnInputCell(this));
		
		setItem(++row, 0, (qTableItem*) new qIOStaticCell("System \"Survival\"", this));
		setItem(row, 1, (qTableItem*) new qIOConstantDSystemSurvivalCell(default_transport_target, this, FALSE, FALSE));
	break;
	case POST_BONNEVILLE_S3_VS_WTT:
		setNumRows((hypothesisRow + 1) + 3);


		setItem(++row, 0, (qTableItem*) new qIOStaticCell("Inriver Latent Mort.", this));
		setItem(row, 1, (qTableItem*) new qIOInriverLatentMortalityCell(default_transport_target, this, FALSE, FALSE));
		
		setItem(++row, 0, (qTableItem*) new qIOStaticCell("Differential Return (D)", this));
		setItem(row, 1, (qIOInputCell*) new qIODifferentialReturnInputCell(this));
		
		setItem(++row, 0, (qTableItem*) new qIOStaticCell("System \"Survival\"", this));
		setItem(row, 1, (qTableItem*) new qIOS3VsWTTSystemSurvivalCell(default_transport_target, this, FALSE, FALSE));
	break;
	default:
		log_msg(L_ERROR, "Invalid or unsupported (in the I/O tool) post-Bonneville hypothesis: %d", current->post_bonneville_hypothesis);
	break;
	}

	emit calculate();
}

void qIOPostBonnevilleSheet::cbApply() {
	emit evApplyFirst();
	qIOSheet::cbApply();
}

void qIOPostBonnevilleSheet::cbReset() {
	qIOSheet::cbReset();
	emit evResetLast();
}

void qIOPostBonnevilleSheet::setHypothesisItem(int row, int col, qIOPostBonnevilleHypothesisInputCell *cell) {
	/* Connect signals */
	if (!connect(this, SIGNAL(evApplyFirst()), cell, SLOT(cbApply()))
			|| !connect(this, SIGNAL(evResetLast()), cell, SLOT(cbReset()))
			|| !connect(cell, SIGNAL(evDirtyChanged()), ioTool, SLOT(updateDirty())))
		log_msg(L_ERROR, "Could not connect input cell signal handlers\n");

	/* Call QTable's setItem to do the actual setting */
	QTable::setItem(row, col, (qTableItem*)cell);
}
