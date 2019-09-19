/* ZatPromptForString.h, by Matt Moore, 4 Now 1997.
 * This class presents a simple, modal dailog which prompts for a string
 *  from the user. The string is left in an RWCString member variable, along
 *  with an OK/Cancel flag in another member variable.
 */

//#include <zapp.hpp>
#include "resource.h"
//#include <rw/cstring.h>
#include <qstring.h>
#include <qwidget.h>
#include <qdialog.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>

class qPromptForString : QDialog {
protected:
	QLabel *label;
	QLineEdit *lineEdit;

	void consCommon(QWidget *parent, const char *titleStr, const char *promptStr);

public:
	qPromptForString( QWidget *parent, const char *titleStr, 
						const char *promptStr, const char *initVal = QString::null );
	qPromptForString( QWidget *parent, const char *titleStr, 
						const char *promptStr, double value, double minValue,
						double maxValue, int decimals = 1);
	~qPromptForString() {};

	/* Similar to QInputDialog::getDouble(), retrives a double from the user
	 * and sets ok to be if the user clicked the ok button */
	static double getDouble(const char* titleStr, const char* promptStr, double value,
		double minValue, double maxValue, int decimals = 1, bool* ok = NULL,
		QWidget* parent = NULL, const char* name = QString::null);

	static QString getString(const char* titleStr, const char* promptStr, const char* value,
		bool* ok = NULL, QWidget* parent = NULL, const char* name = QString::null);

	bool bOK;  /* TRUE if OK button pressed. */
	QString csText;  /* Text entered by user. */
};
