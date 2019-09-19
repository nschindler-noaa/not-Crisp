/* qHelpWindow is a simple help viewer that replaces Qt Assistant
 * (for a variety of reasons).
 *
 * It is based on the example help viewer code provided by
 * Trolltech, which states that the code may be modified and used
 * without limitation */

#ifndef HELPWINDOW_H
#define HELPWINDOW_H

#include <qmainwindow.h>
#include <qtextbrowser.h>
#include <qstringlist.h>
#include <qmap.h>
#include <qdir.h>
#include <qlistview.h>
#include <qxml.h>
#include <qptrstack.h>
#include <qdialog.h>

class QComboBox;
class QPopupMenu;

class qHelpWindow : public QMainWindow
{
    Q_OBJECT
public:
    qHelpWindow( const QString& home_, QWidget* parent = 0, const char *name=0 );
	void showPage(QString& page);
    ~qHelpWindow();

private slots:
    void setBackwardAvailable( bool );
    void setForwardAvailable( bool );

    void sourceChanged( const QString& );
    void print();

	void treeCurrentChanged(QListViewItem* section);
	void browserSourceChanged(const QString & src);

	void search();

private:
    QTextBrowser *browser;
    int backwardId, forwardId;
	QListView *tree;

	QMap<QString, QString> fileToSection;
	QMap<QString, QString> sectionToFile;

	QMimeSourceFactory *mime; // Contains the read-in help files

	friend class qHelpContentsReader;
	friend class qHelpSearchDialog;
};

/* Searching */
class qHelpSearchDialog : public QDialog {
	Q_OBJECT
public:
	qHelpSearchDialog(qHelpWindow* parent); // Create dialog
	QString getSelectedTopic(); // Return selected topic
private:
	qHelpWindow* parent;
	QListView* list;
	QLineEdit* lineEdit;
private slots:
	void textChanged(const QString &); // Adjust list
};

#endif

