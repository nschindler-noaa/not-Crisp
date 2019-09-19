/* qIONotes.h
 * This file defines the notes dock window and its functionality. It is tied closely
 * to the qIdcNotes class. */

#ifndef __QIONOTES_H__
#define __QIONOTES_H__

#include <qdockwindow.h>
#include <qtextedit.h>
#include <qlabel.h>
#include <stdlib.h>
#include "qGlobalEvents.h"
#include "log.h"
#include "qIdcData.h"

extern qIdcDataString notesIDC;
extern qIdcDataString fileCreationDateIDC;
extern qIdcDataString fileCreatorIDC;
extern qIdcDataString fileModificationDateIDC;
extern qIdcDataString fileModifierIDC;

/* Notes dock */
class qIONotes : public QDockWindow {
	Q_OBJECT

protected:
	QLabel *fileCreationDate;
	QLabel *fileCreator;
	QLabel *fileModificationDate;
	QLabel *fileModifier;
	QTextEdit *notes;

	/* programmatically changing the text in a QTextEdit causes a textChanged() event,
	 * so we need some sort of lock to prevent this from happening */
	int lock;

public:
	qIONotes(QWidget *parent);

	int isDirty();

public slots:
	virtual void cbNotesTextChanged();
	virtual void cbInvalidated();

	virtual void cbApply();
	virtual void cbReset();
};

#endif
