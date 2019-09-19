#ifndef	_QIOSIMPASSHEET_H
#define	_QIOSIMPASSHEET_H

#include "crisp.h"
#include "qIOSheet.h"
#include "qIOTool.h"

/* This is the SIMPAS-like I/O sheet. */
class qIOSIMPASSheet : public qIODamSheet {
	Q_OBJECT
protected:
	int averageFlowRow;
	int averageSpillRow;
	int daySpillRow;
	int nightSpillRow;

public:
	qIOSIMPASSheet(QWidget *parent, qIOTool *tool, const QString &name);

protected:
	virtual void setup(); /* Setup the spreadsheet */

	/* add dam-specific cells */
	void setDamCol(struct river_segment *seg, unsigned int *col, struct river_segment *dam = NULL, int powerHouseNum = 0);

public slots:
	/* Update cell labels that depend on the spill averaging period */
	void cbAverageChanged();

protected slots:
	/* This is needed to ensure that proper output settings are used, i.e. route tracking
	 * (NOTE: other sheets don't do this so at least one of these should be present!) */
	virtual void cbRunningScenario();
};


#endif

