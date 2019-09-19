#ifndef qHelpManager_H
#define qHelpManager_H

#include <qwidget.h>
#include <qmessagebox.h>
#include "qHelpWindow.h"

class qHelpManager : public QObject
{
    Q_OBJECT

public:
    qHelpManager( QWidget *parent , const char *name );
    ~qHelpManager();

	void showHelp( QString page );
	void shutdown();

public slots:
	void about(); /* Display the About dialog */

private slots:
    void displayPage();

private:    
	qHelpWindow *helpWindow;
   

};

/* global pointer to the help manager (set it main(), which is actually Qmain() in Qmain.cpp) */
extern qHelpManager *helpManager;

#endif

