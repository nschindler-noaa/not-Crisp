#ifndef QTEXTOUTDIALOG_H
#define QTEXTOUTDIALOG_H

#include <qtextbrowser.h>
#include <qmainwindow.h>


class qTextOutDialog : QMainWindow{

	Q_OBJECT

	QTextBrowser *lp_;

	/* Help file support. */
	int helpPage_;

public :
	qTextOutDialog( QWidget *w,	char *szLabel = "Output Window", int compress = 0 );
	~qTextOutDialog();

	void addString(char *str);
	void addFile( char *fname );
	void setFont(QFont f){ lp_->setCurrentFont(f); }
	void setFamily(const QString & f){ lp_->setFamily(f); }

	void setHelpPage( int i ) { helpPage_ = i; }
	int getHelpPage( void ) { return helpPage_; }
	
	void bringForward( void );
public slots:
	virtual void cbHelp();
	virtual void cbCopy();
	virtual void cbSaveAs();
	virtual void cbPrint();
	virtual void cbClose();
	virtual void cbSelectAll();
	

};

#endif /* QTEXTOUTDIALOG_H */

