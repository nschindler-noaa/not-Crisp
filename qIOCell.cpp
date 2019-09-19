#include "qIOCell.h"
#include "log.h"
#include "crisp.h"
#include "spill.h"
#include "unused.h"
#include "qIdcList.h"
#include "qIdcSchedule.h"
#include "qScheduleDialogMed.h"
#include "qIdcDamDelayEquation.h"
#include "qGraphMedDrawLines.h"
#include "qGraphMedDrawAxisYD.h"
#include "qGraphMedDrawAxisXI.h"
#include "qGraphMedZoom.h"
#include "qGraphMedPosition.h"
#include "qGraphMedDrawText.h"
#include "qScenarioManager.h"
#include "qHistogramGDT.h"
#include <qlineedit.h>
#include <qvalidator.h>
#include <qcombobox.h>
#include <qmap.h>
#include <qvaluelist.h>
extern "C" {
#include "crisp_time.h"
}

/* qIOCell implementation */

qIOCell::qIOCell(qIOSheet *table, int UNUSED(colspan), EditType et)
:	qTableItem(table, et),
	sheet(table),
	monteCarloHistogram(NULL)
{
	if (!connect(table, SIGNAL(calculate()), this , SLOT(calculate()))) {
		log_msg(L_ERROR, "Could not connect I/O cell event handler\n");
	}
}

void qIOCell::showMonteCarloHistogram() {
	if (monteCarloHistogram) {
		monteCarloHistogram->show();
		monteCarloHistogram->raise();
		return;
	}

	qGraphDataToken *token = new qHistogramGDT(new qIOMonteCarloGDT(this, getName()));
	qGraphMediator *gm;
	qGraphPane *gp;
	if(token){
		monteCarloHistogram = new qGraphDialog(NULL, SHOW_DATE, QObject::WDestructiveClose | QObject::WType_TopLevel, 
								token->getGraphTitle().ascii());
		gm = new qGraphMediator(monteCarloHistogram, token);
		gp = monteCarloHistogram->getGraphPane();
		gp->drawMethod(HOLLOWBARS);
		gm->insertSubMediator(new qGraphMedDrawLines(gp));
		gm->insertSubMediator(new qGraphMedDrawAxisYD(gp));
		gm->insertSubMediator(new qGraphMedDrawAxisXI(gp));
		gm->insertSubMediator(new qGraphMedZoom(gp,monteCarloHistogram));
		gm->insertSubMediator(new qGraphMedPosition(gp,monteCarloHistogram));

		if (!connect(monteCarloHistogram, SIGNAL(destroyed()), this, SLOT(monteCarloHistogramDestroyed()))) {
			log_msg(L_ERROR, "Could not connect I/O cell Monte Carlo histogram event handler\n");
		}

		sheet->getIOTool()->addGraphDialog(monteCarloHistogram);
		monteCarloHistogram->show(); // show the dialog
	}
}

void qIOCell::monteCarloHistogramDestroyed() {
	monteCarloHistogram = NULL;
}

/* Do nothing in default case */
void qIOCell::calculate() {
}

double qIOCell::mean() {
	int game;
	double sum = 0;

	if (sheet->getGameCount() <= 0) {
		setValid(FALSE);
		return 0.0;
	}

	for (game = 0; game < sheet->getGameCount(); game++) {
		sum += value(game);
	}

	return sum / sheet->getGameCount();
}
double qIOCell::stddev() {
	double mean_value = mean();
	if (isValid()) {
		int game;
		float sum = 0;

		if (sheet->getGameCount() <= 1) {
			setValid(FALSE);
			return 0.0;
		}

		for (game = 0; game < sheet->getGameCount(); game++) {
			double val = value(game);
			sum += (val - mean_value) * (val - mean_value);
		}

		sum /= (game - 1);
		return sqrt(sum);
	}
	else return 0.0;
}

/* qIODamSpecific implementation */
qIODamSpecific::qIODamSpecific(struct river_segment *_seg, qIODamSheet *_damSheet, int _powerhouse)
:	damSheet(_damSheet),
	seg(_seg),
	powerhouse(_powerhouse)
{
}

/* qIOStaticCell implementation */

qIOStaticCell::qIOStaticCell(const QString &text, qIOSheet *table, int colspan)
:	qIOCell(table, colspan)
{
	setText(text);
}

/* qIODamWaterAverageCell implementation */
qIODamWaterAverageCell::qIODamWaterAverageCell(struct river_segment *seg, qIODamSheet *table, int powerhouse, int colspan)
	:	qIOCell(table, colspan),
		qIODamSpecific(seg, table, powerhouse),
		gd(NULL)
{
	if (!connect(sheet->getIOTool(), SIGNAL(evAverageChanged()), this , SLOT(calculate()))) {
		log_msg(L_ERROR, "Could not connect I/O cell (flow avg.) event handler\n");
	}
}

void qIODamWaterAverageCell::cbDoubleClicked() {
	if (gd) {
		gd->show();
		gd->raise();
		return;
	}

	qGraphDataToken *token = new qIODamWaterAverageGDT(this);
	qGraphMediator *gm;
	qGraphPane *gp;
	if(token){
		gd = new qGraphDialog(NULL, SHOW_DATE, QObject::WDestructiveClose | QObject::WType_TopLevel, 
								token->getGraphTitle().ascii());
		gm = new qGraphMediator(gd, token);
		gp = gd->getGraphPane();
		gm->insertSubMediator(new qGraphMedDrawText(gp));
		gm->insertSubMediator(new qGraphMedDrawLines(gp));
		gm->insertSubMediator(new qGraphMedDrawAxisYD(gp));
		gm->insertSubMediator(new qGraphMedDrawAxisXI(gp));
		gm->insertSubMediator(new qGraphMedZoom(gp,gd));
		gm->insertSubMediator(new qGraphMedPosition(gp,gd));

		if (!connect(gd, SIGNAL(destroyed()), this, SLOT(graphDestroyed()))) {
			log_msg(L_ERROR, "Could not connect I/O cell graph event handler\n");
		}

		sheet->getIOTool()->addGraphDialog(gd);
		gd->show(); // show the dialog
	}
}
void qIODamWaterAverageCell::graphDestroyed() {
	gd = NULL;
}
double qIODamWaterAverageCell::value(int game) {
	double total = 0;
	int count = 0;

	int start_day = sheet->getIOTool()->getAverageStart();
	int stop_day = sheet->getIOTool()->getAverageStop();

	for (int day=start_day; day <= stop_day; day++) {
		total += getKCFS(day);
		count++;
	}

	return total / count;
}
void qIODamWaterAverageCell::calculate() {
	/* Doesn't make sense for powerhouses as far as SIMPAS is concerned */
	if (powerhouse) {
		setText("");
		setEnabled(FALSE);
		return;
	}

	setText(valueToText(value(0)));
	setValid(TRUE);

	/* repaint */
	table()->updateCell(row(), col());
}

/* qIODamPassageCell implementation */
qIODamPassageCell::qIODamPassageCell(const QString &_name, river_segment *seg, qIODamSheet *table, int _monte_carlo_stat, int _average, int _complement, int powerhouse, int colspan)
:	qIOCell(table, colspan, QTableItem::Never),
	qIODamSpecific(seg, table, powerhouse),
	name(_name),
	monte_carlo_stat(_monte_carlo_stat),
	average(_average),
	complement(_complement),
	gd(NULL)
{
}

QString qIODamPassageCell::getName() {
	QString n = name;
	if (sheet->getGameCount() > 1) {
		if (monte_carlo_stat == MONTE_CARLO_STAT_STDDEV) n += " (std dev)";
	}
	return n;
}

float qIODamPassageCell::calculatePassage(struct river_segment *_seg, int game, int *success) {
	struct rls_seg *rseg;
	float total_in = 0;
	float total_out = 0;
	float value = 0;

	QPtrListIterator<struct rls_seg> it(getReleaseSegments(_seg));

	while (rseg = it.current()) {
		if (rseg->data && rseg->data[game] && rseg->data[game]->stats.done) {
			total_in += getTotal(rseg, game);
			total_out += getPassage(rseg, game);
		}
		else {
			*success = 0;
			return complement ? 1.0 : 0.0;
		}
		++it;
	}
	if (total_in > 0) {
		value += total_out/total_in;
	}
	else {
		*success = 0;
		return complement ? 1.0 : 0.0;
	}

	*success = 1;
	printf("%s, %s, %f\n", _seg->seg_name, name.ascii(), (complement ? 1.0 - value : value));
	return complement ? 1.0 - value : value;
}

double qIODamPassageCell::value(int game) {
	if (game == IO_CELL_VALUE_MONTE_CARLO_MEAN) return mean();
	else if (game == IO_CELL_VALUE_MONTE_CARLO_STDDEV) return stddev();

	int success;
	float value;
	float complement_product; /* This is used in DAM_PASSAGE_REMAINDER_SUM, it's the product of all upstream complements */
	/* Add all dam passages up and divide by the number of dams */
	if (average) {
		struct river_segment *s;
		QPtrListIterator<struct river_segment> it(damSheet->getDams());
		int count = 0;
		/* Average adds them up, starting with an accumulator or 0.0, whereas
		 * cumulative multiplies them, starting with 1.0 */
		if (average == DAM_PASSAGE_AVERAGE || average == DAM_PASSAGE_SUM || average == DAM_PASSAGE_REMAINDER_SUM) value = 0.0;
		else value = 1.0;

		if (average == DAM_PASSAGE_REMAINDER_SUM) complement_product = 1.0;

		while (s = it.current()) {
			if (s->seg_type == DAM_SEG) {
				/* Add or multiply depending on if it's average or cumulative */
				if (average == DAM_PASSAGE_AVERAGE || average == DAM_PASSAGE_SUM) value += calculatePassage(s, game, &success);
				else if (average == DAM_PASSAGE_REMAINDER_SUM) {
					double x = calculatePassage(s, game, &success);
					value += complement_product * x;
					complement_product *= (1.0 - x);
				}
				else value *= calculatePassage(s, game, &success);
				if (success == 0) return 0.0;
				++count;

				/* Only accumulate up to this dam. Note that this requires the dam sheet to be
				 * in order with the dam closest to the river mouth being last. */
				if (seg && s == seg) break;
			}
			++it;
		}

		/* One last segment if this is beyond the last dam */
		if (seg && s != seg) {
			if (average == DAM_PASSAGE_AVERAGE || average == DAM_PASSAGE_SUM) value += calculatePassage(seg, game, &success);
			else if (average == DAM_PASSAGE_REMAINDER_SUM) {
				double x = calculatePassage(s, game, &success);
				value += complement_product * x;
				complement_product *= (1.0 - x);
			}
			else value *= calculatePassage(seg, game, &success);
			++count;
		}

		/* Only divide if we're taking the average */
		if (average == DAM_PASSAGE_AVERAGE) value /= count;
	}
	/* Or just calculate for a single dam */
	else {
		value = calculatePassage(seg, game, &success);
	}

	return value;
}

void qIODamPassageCell::calculate() {
	/* This is an output cell, so invalidate it if the run was not completed */
	if (scenarioManager.getCanceled()) {
		setValid(FALSE);
		return;
	}

	/* Only makes sense for dam cells */
	if (powerhouse) {
		setText("");
		setEnabled(FALSE);
		return;
	}
	
	/* If the cell is a standard deviation cell, calculate the standard deviation, otherwise calculate the mean/actual value */
	setText(valueToText(value(monte_carlo_stat == MONTE_CARLO_STAT_STDDEV ? IO_CELL_VALUE_MONTE_CARLO_STDDEV : IO_CELL_VALUE_MONTE_CARLO_MEAN)));
}

void qIODamPassageCell::showGraph() {
	if (!average && sheet->getGameCount() == 1) {
		if (gd) {
			gd->show();
			gd->raise();
			return;
		}

		qGraphDataToken *token = new qIODamPassageGDT(this, getName().ascii(), getXAxisLabel().ascii(), getYAxisLabel().ascii());
		qGraphMediator *gm;
		qGraphPane *gp;
		if(token){
			gd = new qGraphDialog(NULL, SHOW_DATE, QObject::WDestructiveClose | QObject::WType_TopLevel, 
									token->getGraphTitle().ascii());
			gm = new qGraphMediator(gd, token);
			gp = gd->getGraphPane();
			gm->insertSubMediator(new qGraphMedDrawText(gp));
			gm->insertSubMediator(new qGraphMedDrawLines(gp));
			gm->insertSubMediator(new qGraphMedDrawAxisYD(gp));
			gm->insertSubMediator(new qGraphMedDrawAxisXI(gp));
			gm->insertSubMediator(new qGraphMedZoom(gp,gd));
			gm->insertSubMediator(new qGraphMedPosition(gp,gd));

			if (!connect(gd, SIGNAL(destroyed()), this, SLOT(graphDestroyed()))) {
				log_msg(L_ERROR, "Could not connect I/O cell graph event handler\n");
			}

			sheet->getIOTool()->addGraphDialog(gd);
			gd->show(); // show the dialog
		}
	}
	else if (sheet->getGameCount() > 1 && monte_carlo_stat != MONTE_CARLO_STAT_STDDEV) {
		showMonteCarloHistogram();
	}
}

void qIODamPassageCell::graphDestroyed() {
	gd = NULL;
}

/* qIOInputCell implementation */
qIOInputCell::qIOInputCell(qIOSheet *table, QTableItem::EditType et, int colspan)
:	qIOCell(table, colspan, et)
{
}

QWidget* qIOInputCell::createEditor() const {
	QLineEdit *editor = new QLineEdit(text(), table()->viewport());
	editor->setValidator(new QDoubleValidator(editor));

	return editor;
}

/* qIOSpeciesParameterInputCell implementation */
qIOSpeciesParameterInputCell::qIOSpeciesParameterInputCell(qIOSheet *_ioSheet,
		qIdcListBase *_idc, int _dist, int colspan)
:	qIOInputCell(_ioSheet, QTableItem::OnTyping, colspan),
	ioSheet(_ioSheet),
	idc(_idc),
	dist(_dist)
{
	idc->Setup();

	if (!connect(idc, SIGNAL(evInvalidate_()), this , SLOT(calculate()))) {
		log_msg(L_ERROR, "Could not connect I/O cell event handler\n");
	}

	calculate();
}

QWidget* qIOSpeciesParameterInputCell::createEditor() const {
	QLineEdit *editor = new QLineEdit(text(), table()->viewport());
	editor->setValidator(new QDoubleValidator(idc->minVal(), idc->maxVal(), IO_INPUT_DECIMAL_PLACES, editor));

	return editor;
}

void qIOSpeciesParameterInputCell::setContent(const QString& str) {
	if (str != text()) {
		QPtrListIterator<struct release> it(ioSheet->getReleases());
		struct release *rel;
		double value = textToValue(str);

		/* Ensure the value is within the proper range */
		if (value > idc->maxVal()) value = idc->maxVal();
		if (value < idc->minVal()) value = idc->minVal();

		while (rel = it.current()) {
			/* Mean or value */
			idc->ModifyCache(rel->species->index*3, value);

			/* Set low and high if this is a distribution */
			if (dist) {
				/* Set low = high = 0.0 */
				idc->ModifyCache(rel->species->index*3, 0.0);
				idc->ModifyCache(rel->species->index*3, 0.0);
			}

			++it;
		}
	}
}
void qIOSpeciesParameterInputCell::setContentFromEditor(QWidget *w) {
	QLineEdit *lineEdit = (QLineEdit*)w;
	setContent(lineEdit->text());
}

void qIOSpeciesParameterInputCell::updateDirty() {
	QPtrListIterator<struct release> it(ioSheet->getReleases());
	struct release *rel;
	int oldDirty = isDirty();

	setDirty(FALSE);
	while (rel = it.current()) {
		qDatContainer *dat = idc->GetData();
		if (dat) {
			if (dat->isDirty()) {
				setDirty(TRUE);
				break;
			}
		}
		++it;
	}

	if (isDirty() != oldDirty) emit evDirtyChanged();
}

double qIOSpeciesParameterInputCell::value(int UNUSED(game)) {
	QPtrListIterator<struct release> it(ioSheet->getReleases());
	struct release *rel;
	double lastValue = 0.0; /* This default shouldn't matter since there's always a release */
	int firstLoop = 1;
	int valid = 1;

	while (rel = it.current()) {
		qDatContainer *dat = idc->GetData();
		if (dat) {
			double value = dat->getDouble(rel->species->index*3);

			if (firstLoop) {
				firstLoop = 0;
			}
			else if (value != lastValue) {
				valid = 0;
				break;
			}

			lastValue = value;
		}
		++it;
	}

	setValid(valid);
	return lastValue;
}
void qIOSpeciesParameterInputCell::calculate() {
	if (isValid()) setText(valueToText(value(0)));

	updateDirty();
	/* repaint */
	table()->updateCell(row(), col());
}

void qIOSpeciesParameterInputCell::cbApply() {
	QPtrListIterator<struct release> it(ioSheet->getReleases());
	struct release *rel;

	while (rel = it.current()) {
		qDatContainer *dat = idc->GetData();
		if (dat && dat->isDirty()) {
			idc->ApplyData(dat);
			idc->invalidate();
		}
		++it;
	}
}
void qIOSpeciesParameterInputCell::cbReset() {
	QPtrListIterator<struct release> it(ioSheet->getReleases());
	struct release *rel;

	while (rel = it.current()) {
		qDatContainer *dat = idc->GetData();
		if (dat && dat->isDirty()) {
			idc->ClearCache(dat);
		}
		++it;
	}
}

/* qIODamParameterInputCell implementation */
qIODamParameterInputCell::qIODamParameterInputCell(struct river_segment *seg, qIODamSheet *damSheet,
		qIdcSelectBase *_idc, int _idcOffset, int _dist, int _forPowerhouse, int _notPerSpecies, int powerhouse, int colspan)
:	qIOInputCell(damSheet, QTableItem::OnTyping, colspan),
	qIODamSpecific(seg, damSheet, powerhouse),
	idc(_idc),
	idcOffset(_idcOffset),
	forPowerhouse(_forPowerhouse),
	notPerSpecies(_notPerSpecies),
	dist(_dist)
{
	idc->Setup();

	if (!connect(idc, SIGNAL(evInvalidate_()), this , SLOT(calculate()))) {
		log_msg(L_ERROR, "Could not connect I/O cell event handler\n");
	}

	calculate();
}

QWidget* qIODamParameterInputCell::createEditor() const {
	QLineEdit *editor = new QLineEdit(text(), table()->viewport());
	editor->setValidator(new QDoubleValidator(idc->minVal(), idc->maxVal(), IO_INPUT_DECIMAL_PLACES, editor));

	return editor;
}

void qIODamParameterInputCell::setContent(const QString& str) {
	if (str != text()) {
		QPtrListIterator<struct release> it(getReleases());
		struct release *rel;
		double value = textToValue(str);

		/* Ensure the value is within the proper range */
		if (value > idc->maxVal()) value = idc->maxVal();
		if (value < idc->minVal()) value = idc->minVal();

		while (rel = it.current()) {
			char *species_name = notPerSpecies ? NULL : rel->species->name;
			/* Mean or value */
			idc->ModifyCache(seg->seg_name, species_name, idcOffset, value, 0, 0);

			/* Set low and high if this is a distribution */
			if (dist) {
				/* Set low = high = 0.0 */
				idc->ModifyCache(seg->seg_name, species_name, idcOffset+1, 0.0, 0, 0);
				idc->ModifyCache(seg->seg_name, species_name, idcOffset+2, 0.0, 0, 0);
			}

			++it;
		}
	}
}
void qIODamParameterInputCell::setContentFromEditor(QWidget *w) {
	QLineEdit *lineEdit = (QLineEdit*)w;
	setContent(lineEdit->text());
}

void qIODamParameterInputCell::updateDirty() {
	QPtrListIterator<struct release> it(getReleases());
	struct release *rel;
	int oldDirty = isDirty();

	setDirty(FALSE);
	while (rel = it.current()) {
		char *species_name = notPerSpecies ? NULL : rel->species->name;
		qDatContainer *dat = idc->GetData(seg->seg_name, species_name);
		if (dat) {
			if (dat->isDirty()) {
				setDirty(TRUE);
				break;
			}
		}
		++it;
	}

	if (isDirty() != oldDirty) emit evDirtyChanged();
}

double qIODamParameterInputCell::value(int UNUSED(game)) {
	QPtrListIterator<struct release> it(getReleases());
	struct release *rel;
	double lastValue = 0.0; /* This default shouldn't matter since there's always a release */
	int firstLoop = 1;
	int valid = 1;

	while (rel = it.current()) {
		char *species_name = notPerSpecies ? NULL : rel->species->name;
		qDatContainer *dat = idc->GetData(seg->seg_name, species_name);
		if (dat) {
			double value = dat->getDouble(idcOffset);

			if (firstLoop) {
				firstLoop = 0;
			}
			else if (value != lastValue) {
				valid = 0;
				break;
			}

			lastValue = value;
		}
		++it;
	}

	setValid(valid);
	return lastValue;
}
void qIODamParameterInputCell::calculate() {
	/* forPowerhouse: Doesn't make sense for dam cells if there are powerhouse
	 * cells for that dam. */
	if ((forPowerhouse && !powerhouse && seg->current->dam.additional_pwrhs)
			|| (!forPowerhouse && powerhouse)) {
		setText("");
		setEnabled(FALSE);
		return;
	}

	if (isValid()) setText(valueToText(value(0)));

	updateDirty();
	/* repaint */
	table()->updateCell(row(), col());
}

void qIODamParameterInputCell::cbApply() {
	QPtrListIterator<struct release> it(getReleases());
	struct release *rel;

	while (rel = it.current()) {
		char *species_name = notPerSpecies ? NULL : rel->species->name;
		qDatContainer *dat = idc->GetData(seg->seg_name, species_name);
		if (dat && dat->isDirty()) {
			idc->ApplyData(dat);
			idc->invalidate();
		}
		++it;
	}
}
void qIODamParameterInputCell::cbReset() {
	QPtrListIterator<struct release> it(getReleases());
	struct release *rel;

	while (rel = it.current()) {
		char *species_name = notPerSpecies ? NULL : rel->species->name;
		qDatContainer *dat = idc->GetData(seg->seg_name, species_name);
		if (dat && dat->isDirty()) {
			idc->ClearCache(dat);
		}
		++it;
	}
}

/* qIODamSpillInputCell implementation */
qIODamSpillInputCell::qIODamSpillInputCell(struct river_segment *seg, qIODamSheet *damSheet, int _periods, int powerhouse, int colspan)
:	qIOInputCell(damSheet, QTableItem::OnTyping, colspan),
	qIODamSpecific(seg, damSheet, powerhouse),
	periods(_periods)
{
	if (periods == DAM_AVG_SPILL_DAY) idc = &qIdcPlannedSpillDay_;
	else if (periods == DAM_AVG_SPILL_NIGHT) idc = &qIdcPlannedSpillNight_;
	else idc = NULL;

	if (idc) {
		idc->Setup();

		if (!connect(idc, SIGNAL(evInvalidate_()), this , SLOT(calculate()))
			|| !connect(sheet->getIOTool(), SIGNAL(evAverageChanged()), this , SLOT(calculate()))) {
			log_msg(L_ERROR, "Could not connect I/O cell event handler\n");
		}
	}

	calculate();
}

QWidget* qIODamSpillInputCell::createEditor() const {
	if (periods == DAM_AVG_SPILL_DAY || periods == DAM_AVG_SPILL_BOTH) new qSDMPlannedSpillDay(sheet->getIOTool(), seg->seg_name);
	if (periods == DAM_AVG_SPILL_NIGHT || periods == DAM_AVG_SPILL_BOTH) new qSDMPlannedSpillNight(sheet->getIOTool(), seg->seg_name);

	return NULL;
}

void qIODamSpillInputCell::setContentFromEditor(QWidget * UNUSED(w)) {
	/* Do nothing intentionally, since this input isn't in a cell */
}

void qIODamSpillInputCell::copy(qIOInputCell* src) {
	qIODamSpillInputCell* source = (qIODamSpillInputCell*) src;

	/* Use static data so we don't have to duplicate code */
	int copyPeriods[] = {DAM_AVG_SPILL_DAY, DAM_AVG_SPILL_NIGHT, -1};
	int p;

	for (p = 0; copyPeriods[p] >= 0; p++) {
		int copyPeriod = copyPeriods[p];
		/* Copy only between the same cell types */
		if (periods == copyPeriod || periods == DAM_AVG_SPILL_BOTH)
		{
			int sourceType = source->getPeriods() == DAM_AVG_SPILL_BOTH ? copyPeriod : source->getPeriods();
			int destType = periods == DAM_AVG_SPILL_BOTH ? copyPeriod : periods;
			qIdcScheduleBase *sourceIdc = sourceType == DAM_AVG_SPILL_DAY ? (qIdcScheduleBase*) &qIdcPlannedSpillDay_ : (qIdcScheduleBase*) &qIdcPlannedSpillNight_;
			qIdcScheduleBase *idc = destType == DAM_AVG_SPILL_DAY ? (qIdcScheduleBase*) &qIdcPlannedSpillDay_ : (qIdcScheduleBase*) &qIdcPlannedSpillNight_;
			qDatContainer *dat = idc->GetData(seg->seg_name, "all");
			qDatContainer *sourceDat = sourceIdc->GetData(source->getSegName(), "all");

			for (int i=0; i < DAYS_IN_SEASON; i++) {
				dat->setInt(i, sourceDat->getInt(i));
				dat->setDouble(i, sourceDat->getDouble(i));
			}

			dat->setDirty();
			updateDirty();
			/* repaint */
			table()->updateCell(row(), col());
		}
	}
}

void qIODamSpillInputCell::updateDirty() {
	if (idc) {
		int oldDirty = isDirty();
		qDatContainer *dat = idc->GetData(seg->seg_name, "all");
		if (dat) {
			setDirty(dat->isDirty());
			if (isDirty() != oldDirty) emit evDirtyChanged();
		}
	}
}

void qIODamSpillInputCell::calculate() {
	/* Doesn't make sense for powerhouses as far as SIMPAS is concerned */
	if (powerhouse) {
		setText("");
		setEnabled(FALSE);
		return;
	}

	double total;
	int count;
	int i;

	int start_day = sheet->getIOTool()->getAverageStart();
	int stop_day = sheet->getIOTool()->getAverageStop();

	/* Determine spill proportion by creating a phony dam segment and filling in its spill schedule */
	struct river_segment temp_seg;
	struct river_segment *tseg = &temp_seg;

	/* Copy data from the actual dam segment */
	bcopy(seg, tseg, sizeof(struct river_segment));

	/* Use spill.c functions to determine the actual spill schedule */
	set_dam_planned_spill(tseg);
	for (int day=start_day; day <= stop_day; day++) set_dam_flow_routing(tseg, day);

	for (i=start_day*DAM_TIME_SLC, count=0, total=0.0; i <= stop_day*DAM_TIME_SLC; i++) {
		int night = is_night(i % DAM_TIME_SLC);
		/* Only consider some slices, as defined by the mask */
		if (periods == DAM_AVG_SPILL_BOTH
			|| (periods == DAM_AVG_SPILL_DAY && !night)
			|| (periods == DAM_AVG_SPILL_NIGHT && night))
		{
			total += (double) tseg->current->dam.spill[i];
			count++;
		}
	}

	setText(valueToText(total/count));
	setValid(TRUE);

	updateDirty();
	/* repaint */
	table()->updateCell(row(), col());
}

void qIODamSpillInputCell::cbApply() {
	if (idc) {
		qDatContainer *dat = idc->GetData(seg->seg_name, "all");
		if (dat && dat->isDirty()) {
			idc->ApplyData(dat);
			idc->invalidate();
		}
	}
}
void qIODamSpillInputCell::cbReset() {
	if (idc) {
		qDatContainer *dat = idc->GetData(seg->seg_name, "all");
		if (dat && dat->isDirty()) {
			idc->ClearCache(dat);
		}
	}
}

/* qIODamPowerhousePriorityInputCell implementation */
qIODamPowerhousePriorityInputCell::qIODamPowerhousePriorityInputCell(struct river_segment *seg, qIODamSheet *damSheet, int powerhouse, int colspan)
:	qIOInputCell(damSheet, QTableItem::WhenCurrent, colspan),
	qIODamSpecific(seg, damSheet, powerhouse)
{
	qIdcPhousePriority_.Setup();

	if (!connect(&qIdcPhousePriority_, SIGNAL(evInvalidate_()), this , SLOT(calculate()))) {
		log_msg(L_ERROR, "Could not connect I/O cell event handler\n");
	}

	calculate();
}

QWidget* qIODamPowerhousePriorityInputCell::createEditor() const {
	QStringList powerhouses;
	struct river_segment *ph;
	int i;

	for (i = 1, ph = seg; ph; ph = ph->current->dam.additional_pwrhs, i++) {
		powerhouses.append(QString("PH") + QString::number(i));
	}

	QComboBox *editor = new QComboBox(table()->viewport());
	editor->insertStringList(powerhouses);

	/* Select current item from cell */
	editor->setCurrentText(text());

	return editor;
}

void qIODamPowerhousePriorityInputCell::setContentFromEditor(QWidget *w) {
	QComboBox *cb = (QComboBox*)w;
	if (cb->currentText() != text()) {
		int r = getIdcRow();
		if (r >= 0) {
			QString t = cb->currentText();
			int selectedHouse = t.remove("PH").toInt() - 1;
			qIdcPhousePriority_.ModifyCache(r*2, selectedHouse == 0 ? 1 : 0);
			qIdcPhousePriority_.ModifyCache(r*2+1, selectedHouse == 1 ? 1 : 0);
		}
	}
}

int qIODamPowerhousePriorityInputCell::getIdcRow() {
	struct river_segment *dam;
	int r;
	for (r=0, dam = river_mouth; dam != NULL; dam = dam->next) {
		if (dam->seg_type == DAM_SEG && dam->current->dam.additional_pwrhs != NULL)  {
			if (dam == seg) break;
			else r++;
		}
	}

	if (dam != seg) return -1;
	else return r;
}

void qIODamPowerhousePriorityInputCell::updateDirty() {
	int oldDirty = isDirty();
	qDatContainer *dat = qIdcPhousePriority_.GetData();
	if (dat) {
		setDirty(dat->isDirty());
		if (isDirty() != oldDirty) emit evDirtyChanged();
	}
}

void qIODamPowerhousePriorityInputCell::calculate() {
	/* Not needed if there aren't multiple powerhouses or if this is a powerhouse cell */
	if (!seg->current->dam.additional_pwrhs || powerhouse) {
		setText("");
		setEnabled(FALSE);
		return;
	}

	int r = getIdcRow();
	qDatContainer *dat = qIdcPhousePriority_.GetData();
	if (r < 0 || !dat) {
		setValid(FALSE);
		return;
	}

	setValid(TRUE);
	setText(QString("PH") + QString::number((dat->getDouble(r*2) == 1.0) ? 1 : 2));

	updateDirty();
	/* repaint */
	table()->updateCell(row(), col());
}

void qIODamPowerhousePriorityInputCell::cbApply() {
	qDatContainer *dat = qIdcPhousePriority_.GetData();
	if (dat && dat->isDirty()) {
		qIdcPhousePriority_.ApplyData(dat);
		qIdcPhousePriority_.invalidate();
	}
}
void qIODamPowerhousePriorityInputCell::cbReset() {
	qDatContainer *dat = qIdcPhousePriority_.GetData();
	if (dat && dat->isDirty()) {
		qIdcPhousePriority_.ClearCache(dat);
	}
}

/* qIODamDielPassageInputCell implementation */
/* Since diel passage is implemented as selecting a species and a diel passage fraction,
 * we need some static data which is defined here. */

/* Dam delay parameters are set from a table, these are the cells */
typedef struct {
	float diel;
	float aday;
	float anight;
} DielPassage;

/* Chinook 0 diel passage parameters */
static DielPassage ch0Diel[] = {
	{0.83,	1.1,	20},
	{0.68,	2.9,	20},
	{0.5,	5.5,	5.5},
	{0,0,0}
};
/* Chinook 1 diel passage parameters */
static DielPassage ch1Diel[] = {
	{0.83,	1.1,	20},
	{0.68,	2.75,	16},
	{0.5,	5.5,	5.5},
	{0.8,	1.39,	20},
	{0,0,0}
};
/* Steelhead diel passage parameters */
static DielPassage shDiel[] = {
	{0.83,	1.1,	20},
	{0.76,	1.8,	20},
	{0.5,	5.5,	5.5},
	{0,0,0}
};

/* Here we select which table to use based on the species name (if species names
 * ever change, then this selection will need to change as well!). */
static DielPassage* getDielTable(const char* species_name) {
	if (QString("Chinook 0") == species_name) return &ch0Diel[0];
	if (QString("Chinook 1") == species_name) return &ch1Diel[0];
	if (QString("Steelhead") == species_name) return &shDiel[0];
	else log_msg(L_ERROR, "No diel passage table for species %s\n", species_name);
	return NULL;
}

qIODamDielPassageInputCell::qIODamDielPassageInputCell(struct river_segment *seg, qIODamSheet *damSheet, int powerhouse, int colspan)
:	qIOInputCell(damSheet, QTableItem::WhenCurrent, colspan),
	qIODamSpecific(seg, damSheet, powerhouse)
{
//	qIdcDamDelayEquation_.Setup(); /* qIdcDamDelayEquation doesn't have Setup() */

	if (!connect(&qIdcDamDelayEquation_, SIGNAL(evInvalidate_()), this , SLOT(calculate()))) {
		log_msg(L_ERROR, "Could not connect I/O cell event handler\n");
	}

	calculate();
}

QWidget* qIODamDielPassageInputCell::createEditor() const {
	QStringList diels;
	int i;

	DielPassage *dielTable = getDielTable(getReleases().getFirst()->species->name);
	for (i = 0; dielTable[i].diel != 0 && dielTable[i].aday != 0 && dielTable[i].anight != 0; i++) {
		diels.append(valueToText(dielTable[i].diel));
	}

	QComboBox *editor = new QComboBox(table()->viewport());
	editor->insertStringList(diels);

	/* Select current item from cell */
	editor->setCurrentText(text());

	return editor;
}

void qIODamDielPassageInputCell::setContentFromEditor(QWidget *w) {
	QComboBox *cb = (QComboBox*)w;

	if (cb->currentText() != text()) {
		QPtrListIterator<struct release> it(getReleases());
		struct release *rel;

		while (rel = it.current()) {
			DielPassage *dielTable = getDielTable(rel->species->name);
			for (int i = 0; dielTable[i].diel != 0 && dielTable[i].aday != 0 && dielTable[i].anight != 0; i++) {
				if (cb->currentText() == valueToText(dielTable[i].diel)) {
					/* Note that ModifyCache(index, ...) actually modifies the dat container's getDouble(index*3)! */
					qIdcDamDelayEquation_.ModifyCache(0, FALSE, FALSE, dielTable[i].aday, seg->seg_name, rel->species->name, FALSE);
					qIdcDamDelayEquation_.ModifyCache(1, FALSE, FALSE, dielTable[i].anight, seg->seg_name, rel->species->name, FALSE);

					/* Set all other parameters to zero */
					for (int p=2; p < qIdcDamDelayEquation_.maxParams(); p++) qIdcDamDelayEquation_.ModifyCache(p, FALSE, FALSE, 0.0, seg->seg_name, rel->species->name, FALSE);
					break;
				}
			}
			++it;
		}

	}
}

void qIODamDielPassageInputCell::updateDirty() {
	QPtrListIterator<struct release> it(getReleases());
	struct release *rel;
	int oldDirty = isDirty();

	setDirty(FALSE);
	while (rel = it.current()) {
		char *species_name = rel->species->name;
		qDatContainer *dat = qIdcDamDelayEquation_.GetData(seg->seg_name, species_name);
		if (dat) {
			if (dat->isDirty()) {
				setDirty(TRUE);
				break;
			}
		}
		++it;
	}

	if (isDirty() != oldDirty) emit evDirtyChanged();
}

void qIODamDielPassageInputCell::calculate() {
	/* Not for powerhouse cells */
	if (powerhouse) {
		setText("");
		setEnabled(FALSE);
		return;
	}

	setValid(FALSE);

	/* only look at species of first release */
	qDatContainer *dat = qIdcDamDelayEquation_.GetData(seg->seg_name, getReleases().first()->species->name);
	if (!dat) return;

	/* Find the diel value for the given parameters */
	DielPassage *dielTable = getDielTable(getReleases().first()->species->name);
	double aday = dat->getDouble(0);
	double anight = dat->getDouble(1*3);

	if (dielTable) {
		for (int i = 0; dielTable[i].diel != 0 && dielTable[i].aday != 0 && dielTable[i].anight != 0; i++) {
			if (dielTable[i].aday == aday && dielTable[i].anight == anight) {
				/* Check to make sure other parameters are set to zero as well */
				int zeroed = TRUE;
				int whatitam = qIdcDamDelayEquation_.maxParams();
				for (int p=2; p < qIdcDamDelayEquation_.maxParams(); p++) {
					double whatitis = dat->getDouble(p*3);
					if (dat->getDouble(p*3) != 0.0) {
						zeroed = FALSE;
						break;
					}
				}
				if (zeroed != TRUE) continue;

				/* This matches an entry in the diel table */
				setValid(TRUE);
				setText(valueToText(dielTable[i].diel));
				break;
			}
		}
	}

	updateDirty();
	/* repaint */
	table()->updateCell(row(), col());
}

void qIODamDielPassageInputCell::cbApply() {
	QPtrListIterator<struct release> it(getReleases());
	struct release *rel;

	while (rel = it.current()) {
		char *species_name = rel->species->name;
		qDatContainer *dat = qIdcDamDelayEquation_.GetData(seg->seg_name, species_name);
		if (dat && dat->isDirty()) {
			qIdcDamDelayEquation_.ApplyData(dat);
			qIdcDamDelayEquation_.invalidate();
		}
		++it;
	}
}
void qIODamDielPassageInputCell::cbReset() {
	QPtrListIterator<struct release> it(getReleases());
	struct release *rel;

	while (rel = it.current()) {
		char *species_name = rel->species->name;
		qDatContainer *dat = qIdcDamDelayEquation_.GetData(seg->seg_name, species_name);
		if (dat && dat->isDirty()) {
			qIdcDamDelayEquation_.ClearCache(dat);
		}
		++it;
	}
}

/* Delayed mortality GUI */
/** Search for the release that contains this rls_seg */
inline struct release* get_release(struct rls_seg *rseg) {
	struct release *r;
	for (r = current->release_list; r != NULL; r = r->next) {
		struct rls_seg *s;
		for (s = r->rls_seg; s != NULL; s = s->down) {
			if (s == rseg) return r;
		}
	}
	return NULL;
}
/** Find the delayed_mortality_table for this release from the release segment by looking downstream */
inline delayed_mortality_table* get_table(struct rls_seg *rseg, int game) {
	struct rls_seg *r;
	for (r = rseg; r != NULL; r = r->down) {
		if (r->data[game]->stats.d != NULL) return r->data[game]->stats.d;
	}
	return NULL;
}
/** Figure out what the index of this dam is within the delayed_mortality_table */
inline int d_index(struct release *release, struct river_segment *seg) {
	struct rls_seg *rseg;
	int i;
	for (i=0, rseg = release->rls_seg; rseg; rseg = rseg->down) {
		if (rseg->river_segment->seg_type == DAM_SEG && rseg->river_segment->current->dam.collector) {
			if (rseg->river_segment == seg) return i;
			else i++;
		}
	}
	return -1;
}
/* TODO: Is this really what the transport arrivals cell should do? */
#if 0
qIODamTransportArrivalCell::qIODamTransportArrivalCell(struct river_segment *seg, qIODamSheet *table, int average, int powerhouse, int colspan)
		:	qIODamPassageCell("Transport Arrival", seg, table, MONTE_CARLO_STAT_MEAN, average, FALSE, powerhouse, colspan)
{}
float qIODamTransportArrivalCell::getPassage(struct rls_seg *rseg, int game) {
	struct release *release = get_release(rseg);
	delayed_mortality_table *d = get_table(rseg, game);

	if (release == NULL || d == NULL) return 0.0f;
	int index = d_index(release, rseg->river_segment);
	if (index == -1) return 0.0f;

	float value = 0.0;
	int day;
	for (day=0; day < DAYS_IN_SEASON; day++) {
		value += d->columns[index].prob[day];
	}
	value /= DAYS_IN_SEASON;

	return value;
}
float qIODamTransportArrivalCell::getHistData(struct rls_seg *rseg, int game, int i)  {
	return rseg->data[game]->transport_arrived ? rseg->data[game]->transport_arrived[i] : 0.0f;
}
void qIODamTransportArrivalCell::cbDoubleClicked() {
	if (!average) showGraph();
}
float qIODamTransportArrivalCell::getTotal(struct rls_seg *rseg, int game) {
	struct release *release = get_release(rseg);

	if (release == NULL) return 0.0f;

	int index = d_index(release, rseg->river_segment);
	return index == -1 ? 0.0f : 1.0f;
}
#else
qIODamTransportArrivalCell::qIODamTransportArrivalCell(struct river_segment *seg, qIODamSheet *table, int average, int powerhouse, int colspan)
		:	qIODamPassageCell("Transport Arrival", seg, table, MONTE_CARLO_STAT_MEAN, average, FALSE, powerhouse, colspan)
{}
float qIODamTransportArrivalCell::getPassage(struct rls_seg *rseg, int game) {
	return rseg->data[game]->stats.postmort_transport_total;
}
float qIODamTransportArrivalCell::getHistData(struct rls_seg *rseg, int game, int i)  {
	return rseg->data[game]->transport_arrived ? rseg->data[game]->transport_arrived[i] : 0.0f;
}
void qIODamTransportArrivalCell::cbDoubleClicked() {
	if (!average) showGraph();
}
float qIODamTransportArrivalCell::getTotal(struct rls_seg *rseg, int game) {
	struct rls_seg *r;
	for (r = rseg; r->river_segment != default_transport_target && r != NULL; r = r->down);
	if (r != NULL) return r->data[game]->stats.transport_in_total;
	else return 0.0f;
}
#endif

/* qIOGlobalDropDownInputCell implementation */
qIOGlobalDropDownInputCell::qIOGlobalDropDownInputCell(qIdcDataInt *_idc, int *_setting, const char **_options, int num_options, qIOSheet* sheet, int colspan)
:	qIOInputCell(sheet, QTableItem::WhenCurrent, colspan),
	idc(_idc),
	setting(_setting)
{
	/* Check for NULL pointers */
	if (!idc || !setting || !_options) {
		log_msg(L_ERROR, "NULL pointer for qIOGlobalDropDownInputCell!\n");
		return;
	}

	/* Connect invalidate signal */
	if (!connect(idc, SIGNAL(invalidated()), this , SLOT(calculate()))) {
		log_msg(L_ERROR, "Could not connect I/O cell event handler\n");
	}

	/* Construct string list */
	for (int i=0; i < num_options; i++) {
		options.append(_options[i]);
	}

	/* Dispaly the contents */
	calculate();
}

QWidget* qIOGlobalDropDownInputCell::createEditor() const {
	QComboBox *editor = new QComboBox(table()->viewport());
	if (!connect(editor, SIGNAL(activated(int)), this, SLOT(clear()))) {
		log_msg(L_ERROR, "Could not connect drop-down cell event handler\n");
	}
	editor->insertStringList(options);

	/* Select current item from cell */
	editor->setCurrentText(text());

	return editor;
}
void qIOGlobalDropDownInputCell::clear() {
	/* Unselect and select the cell to force the changes to take place.
	 * Note that just calling setContentFromEditor() doesn't appear to work (it crashes) */
	sheet->setCurrentCell(row() - 1, col());
	sheet->setCurrentCell(row(), col());
}

void qIOGlobalDropDownInputCell::setContentFromEditor(QWidget *w) {
	QComboBox *cb = (QComboBox*)w;
	if (cb->currentText() != text()) {
		int index = options.findIndex(cb->currentText());
		if (index >= 0) {
			idc->setData(index);
		}
		else {
			log_msg(L_ERROR, "Unknown drop-down selection %s\n", cb->currentText().ascii());
		}
	}
}

void qIOGlobalDropDownInputCell::updateDirty() {
	int oldDirty = isDirty();
	setDirty(idc->isDirty());
	if (isDirty() != oldDirty) emit evDirtyChanged();
}

void qIOGlobalDropDownInputCell::calculate() {
	/* Check to make sure the selection is within range */
	if (idc->getData() < 0 || ((unsigned int) idc->getData()) >= options.size()) {
		setValid(FALSE);
		return;
	}

	setValid(TRUE);
	setText(options[idc->getData()]);

	updateDirty();
	/* repaint */
	table()->updateCell(row(), col());
}

void qIOGlobalDropDownInputCell::cbApply() {
	idc->apply();
}
void qIOGlobalDropDownInputCell::cbReset() {
	idc->reset();
}
