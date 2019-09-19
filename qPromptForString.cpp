/* qPromptForString.cpp, by Matt Moore, 4 Now 1997.
 * This class presents a simple, modal dailog which prompts for a string
 *  from the user. The string is left in an RWCString member variable, along
 *  with an OK/Cancel flag in another member variable.
 */

#include "qPromptForString.h"
#include <qvalidator.h>

void qPromptForString::consCommon(QWidget *parent, const char *titleStr, const char *promptStr) {
	QVBoxLayout *vbox = new QVBoxLayout(this, 6, 6);
	label = new QLabel(promptStr, this, "prompt_label");
	vbox->addWidget(label);

	lineEdit = new QLineEdit(this, "prompt_edit");
	vbox->addWidget(lineEdit);

	QHBoxLayout *hbox = new QHBoxLayout(6);
    vbox->addLayout(hbox, AlignRight);

    QPushButton *ok = new QPushButton("OK", this, "prompt_ok");
    ok->setDefault(TRUE);
    QPushButton *cancel = new QPushButton("Cancel", this, "prompt_cancel");

    QSize bs = ok->sizeHint().expandedTo(cancel->sizeHint());
    ok->setFixedSize(bs);
    cancel->setFixedSize(bs);

    hbox->addStretch();
    hbox->addWidget(cancel);
    hbox->addWidget(ok);

    connect(lineEdit, SIGNAL(returnPressed()),
	     this, SLOT(tryAccept()));
    connect(lineEdit, SIGNAL(textChanged(const QString&)),
	     this, SLOT(textChanged(const QString&)));

    connect(ok, SIGNAL(clicked()), this, SLOT( accept()));
    connect(cancel, SIGNAL(clicked()), this, SLOT( reject()));

	// Resize and focus
    QSize sh = sizeHint().expandedTo(QSize(400, 10));
    resize(sh.width(), hbox->heightForWidth(sh.width()));
	lineEdit->setFocus();
	
	// Set up dialog
    setCaption(titleStr);
}

qPromptForString::qPromptForString( QWidget *parent,
			   const char *titleStr, const char *promptStr,
			   const char *initVal)
			   : QDialog(parent, promptStr, TRUE)
{
	
/*	csText = QInputDialog::getText ( titleStr, promptStr,QLineEdit::Normal, 
		QString(initVal), &ok, parent, initVal ); // Wrong button order*/

	consCommon(parent, titleStr, promptStr);
    lineEdit->setText(initVal);

	// Show dialog
    QString result;
    bOK = exec() == QDialog::Accepted;
	csText = lineEdit->text();
}
qPromptForString::qPromptForString( QWidget *parent, const char *titleStr, 
		const char *promptStr, double value, double minValue,
		double maxValue, int decimals)
		: QDialog(parent, promptStr, TRUE) {
	consCommon(parent, titleStr, promptStr);

    lineEdit->setValidator(new QDoubleValidator(minValue, maxValue, decimals, lineEdit));
    lineEdit->setText(QString::number(value, 'f', decimals));
    lineEdit->selectAll();

	// Show dialog
    QString result;
    bOK = exec() == QDialog::Accepted;
	csText = lineEdit->text();
}


double qPromptForString::getDouble(const char* titleStr, const char* promptStr, double value,
		double minValue, double maxValue, int decimals, bool* ok, QWidget* parent,
		const char* name)
{
	qPromptForString pfs(parent, titleStr, promptStr, value, minValue, maxValue, decimals);

	*ok = pfs.bOK;
	return pfs.csText.toDouble();
}

QString qPromptForString::getString(const char* titleStr, const char* promptStr, const char* initVal,
		bool* ok, QWidget* parent, const char* name)
{
	qPromptForString pfs(parent, titleStr, promptStr, initVal);

	*ok = pfs.bOK;
	return pfs.csText;
}
