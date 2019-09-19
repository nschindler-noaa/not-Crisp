#include "qTableItem.h"
#include <qpainter.h>

void qTableItem::setValid(int i) {
	valid = i;

	setEnabled(i);
}

void qTableItem::paint(QPainter* p, const QColorGroup& colorGroup, const QRect& cr, bool selected) {
	if (header) {
		QColorGroup cg(colorGroup);
		cg.setColor(QColorGroup::Base, cg.button());
		QFont prev = p->font();
		QFont bold = QFont(prev);
		bold.setBold(TRUE);
		p->setFont(bold);
		QTableItem::paint(p, cg, cr, selected);
		p->setFont(prev);
	}
	else if (valid) {
		QColorGroup cg(colorGroup);

		/* Input cells are green */
		if (editType() != QTableItem::Never) {
			if (dirty) cg.setColor(QColorGroup::Base, Qt::red.light(195));
			else cg.setColor(QColorGroup::Base, Qt::green.light(195));
		}

		QTableItem::paint(p, cg, cr, selected);

		if (dirty) {
			p->setPen(Qt::red);
			p->drawRect(0, 0, cr.width() - 1, cr.height() - 1);
		}
	}
	else p->fillRect(0, 0, cr.width(), cr.height(), colorGroup.button());
}

qTableItem::qTableItem(QTable *table, EditType et, int _valid, int _header, const QString& _text)
:	QTableItem(table, et),
	valid(_valid),
	header(_header)
{
	dirty = FALSE;

	setText(_text);
}
