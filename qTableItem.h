/* qTableItem.h
 * This class is an extension of Qt's QTableItem that contains
 * extra painting features for UI effects such as whether or not
 * the data in the cell is valid (e.g. for an output cell). */

#ifndef __QTABLEITEM_H__
#define __QTABLEITEM_H__

#include <qtable.h>

/* This is a number that is not the same as Qt's QTableItem and derived
 * classes. Its value is only significant in that it is hopefully unique. */
#define	qTableItemRTTI	856383

class qTableItem : public QTableItem {
private:
	 /* Whether the value in the cell is valid. If not, it's not drawn. */
	int valid;

	/* Whether the data has un-applied changes made to it. Note that this
	 * does not imply that the user changed the value via this table item
	 * since the IDC may have modified its cache elsewhere. */
	int dirty;

	int header; /* Whether this cell represents a header. */

public:
	qTableItem(QTable * table, EditType et, int valid = TRUE, int header = FALSE, const QString& text = "");

	void setValid(int i);
	int isValid() {return valid;}

	void setDirty(int i) {dirty = i;}
	int isDirty() {return dirty;}

	int isHeader() {return header;}

	virtual void paint(QPainter* p, const QColorGroup& cg, const QRect& cr, bool selected);

	/* In case special action should be taken on a double click, e.g. show graph window */
	/* Since qTableItem is not derived from QObject, this "signal handler" must be setup manually. */
	virtual void cbDoubleClicked() {}

	virtual int rtti() const {return qTableItemRTTI;}
};

#endif
