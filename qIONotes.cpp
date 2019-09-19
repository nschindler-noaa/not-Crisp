#include "qIONotes.h"
#include "crisp.h"
#include "log.h"
#include <qvbox.h>
#include <qlabel.h>
#include <qlayout.h>

/* Notes IDC */
qIdcDataString notesIDC(&input_notes);
qIdcDataString fileCreationDateIDC(&file_creation_date);
qIdcDataString fileCreatorIDC(&file_creator);
qIdcDataString fileModificationDateIDC(&file_modification_date);
qIdcDataString fileModifierIDC(&file_modifier);

qIONotes::qIONotes(QWidget *parent)
:	QDockWindow(parent),
	lock(0)
{
	setCaption("Notes");
	setResizeEnabled(TRUE);
	setCloseMode(QDockWindow::Always);

	/* Notes section */
	QVBox *notesVBox = new QVBox(this);

	fileCreator = new QLabel("", notesVBox);
	fileCreationDate = new QLabel("", notesVBox);
	fileModifier = new QLabel("", notesVBox);
	fileModificationDate = new QLabel("", notesVBox);

	new QLabel("<b>Notes</b>:", notesVBox);
	notes = new QTextEdit(notesVBox);

	boxLayout()->addWidget(notesVBox);

	/* Connect signal handlers */
	if (!connect(notes, SIGNAL(textChanged()), SLOT(cbNotesTextChanged()))
		|| !connect(&notesIDC, SIGNAL(invalidated()), this, SLOT(cbInvalidated()))
		|| !connect(&fileCreationDateIDC, SIGNAL(invalidated()), this, SLOT(cbInvalidated()))
		|| !connect(&fileCreatorIDC, SIGNAL(invalidated()), this, SLOT(cbInvalidated()))
		|| !connect(&fileModificationDateIDC, SIGNAL(invalidated()), this, SLOT(cbInvalidated()))
		|| !connect(&fileModifierIDC, SIGNAL(invalidated()), this, SLOT(cbInvalidated())))
		log_msg(L_ERROR, "Could not connect notes signal handlers\n");
}

int qIONotes::isDirty() {
	return notesIDC.isDirty();
}

void qIONotes::cbNotesTextChanged() {
	/* Locking is necessary since textChanged() is called regardless of whether the
	 * user made the changes or the underlying code did */
	if (!lock) {
		lock = 1;
		notesIDC.setData(notes->text());
		lock = 0;
	}
}

void qIONotes::cbInvalidated() {
	if (!lock) {
		lock = 1;
		notes->setText(notesIDC.getData());

		if (fileCreationDateIDC.getData() != QString::null) {
			fileCreationDate->show();
			fileCreationDate->setText("<b>Creation Date</b>: " + fileCreationDateIDC.getData());
		}
		else fileCreationDate->hide();

		if (fileCreatorIDC.getData() != QString::null) {
			fileCreator->show();
			fileCreator->setText("<b>Author</b>: " + fileCreatorIDC.getData());
		}
		else fileCreator->hide();

		if (fileModificationDateIDC.getData() != QString::null) {
			fileModificationDate->show();
			fileModificationDate->setText("<b>Modification Date</b>: " + fileModificationDateIDC.getData());
		}
		else fileModificationDate->hide();

		if (fileModifierIDC.getData() != QString::null) {
			fileModifier->show();
			fileModifier->setText("<b>Modified by</b>: " + fileModifierIDC.getData());
		}
		else fileModifier->hide();

		lock = 0;
	}
}

void qIONotes::cbApply() {
	notesIDC.apply();
}
void qIONotes::cbReset() {
	notesIDC.reset();
}

