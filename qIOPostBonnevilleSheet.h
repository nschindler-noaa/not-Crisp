#ifndef	_QIOPOSTBONNEVILLESHEET_H
#define	_QIOPOSTBONNEVILLESHEET_H

#include "crisp.h"
#include "qIOTool.h"
#include "qIOSheet.h"

/* This I/O sheet is for the post-Bonneville part of the model */
class qIOPostBonnevilleSheet : public qIODamSheet {
	Q_OBJECT

protected:
	int hypothesisRow; /** The row that the hypothesis-choosing cell is on */
	int lastHypothesis; /** The last post-Bonneville hypothesis (so that the sheet isn't modified unnecessarily) */

public:
	qIOPostBonnevilleSheet(QWidget *parent, qIOTool *tool, const QString &name);

	/** The hypothesis cell must connect different signals because it modifies the sheet and thus must apply first and reset last. */
	virtual void setHypothesisItem(int row, int col, qIOPostBonnevilleHypothesisInputCell *item);

public slots:
	void cbPostBonnevilleHypothesisChanged();

	/** The post-Bonneville sheet overrides the default apply and reset behavior so that the hypothesis cell is applied first and reset last */
	virtual void cbApply();
	/** The post-Bonneville sheet overrides the default apply and reset behavior so that the hypothesis cell is applied first and reset last */
	virtual void cbReset();

protected:
	virtual void setup();
	virtual void clear() {
		lastHypothesis = -1; /* Need to redraw the hypothesis cells */
		qIOSheet::clear();
	}

	/* add dam-specific cells */
	void setDamCol(struct river_segment *seg, unsigned int *col, struct river_segment *dam = NULL, int powerHouseNum = 0);

signals:
	void evApplyFirst();
	void evResetLast();

};

#endif
