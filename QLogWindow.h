/****************************************************************************
** Form interface generated from reading ui file 'qlogwindow.ui'
**
** Created: Thu Aug 5 15:03:43 2004
**      by: The User Interface Compiler ($Id: QLogWindow.h,v 1.1.1.1.2.2 2006/08/25 18:54:19 jkrinke Exp $)
**
** Main crisp Window, holds map, menu, and controls the mesage pane
****************************************************************************/

#ifndef QLOGWINDOW_H
#define QLOGWINDOW_H

#include <qvariant.h>
#include <qpixmap.h>
#include <qmainwindow.h>
#include <qtextbrowser.h>
//#include <qtextedit.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QAction;
class QActionGroup;
class QPopupMenu;
class QTextEdit;

//Error level constants
#define IDM_ERRORS		0
#define IDM_WARNINGS	1
#define IDM_MESSAGES	2
#define IDM_RAWDUMP		3
#define IDM_DEBUG		4


class QLogWindow : public QMainWindow
{

	  Q_OBJECT

public:
    QLogWindow(const char* name = 0, WFlags fl = WType_TopLevel );
    ~QLogWindow();
	static QLogWindow *instance(int autoCreate);

	void addMsg(char *msg, int id);
	void static deleteLogFile( void );

    QMenuBar *MenuBar;
    QPopupMenu *fileMenu;
    QPopupMenu *editMenu;
    QPopupMenu *LoggingMenu;
    QPopupMenu *helpMenu;
    QAction* fileNewAction;
    QAction* fileSaveAsAction;
    QAction* filePrintAction;
    QAction* fileExitAction;
    QAction* editCopyAction;
    QAction* editSelect_AllAction;
    QAction* loggingWarningAction;
    QAction* loggingMessageAction;
    QAction* loggingDebugAction;
    QAction* loggingRawAction;

public slots:
    virtual void fileNew();
    virtual void fileSaveAs();
    virtual void filePrint();
    virtual void fileExit();
    virtual void editCopy();
	virtual void Raw(bool);
	virtual void Message(bool);
	virtual void Debug(bool);
	virtual void Warning(bool);
	virtual void editSelectAll();
	virtual void helpPressed();
     
protected:
	
	virtual void customEvent( QCustomEvent * e );
	static FILE *logFile_;
	static char logFileName_[30];
	static QLogWindow *m_pInstance;
	QTextEdit * qte;
	QString lastLine;
	int repCount;
	


protected slots:
    virtual void languageChange();

private:
    QPixmap image0;
    QPixmap image1;
    QPixmap image4;
    QPixmap image8;
	QString text;
 

};

#endif // QLOGWINDOW_H


