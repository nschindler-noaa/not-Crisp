/* qFileManager.h
 * qFileManager is a utility class that handles file operations. It abstracts
 * out what was once currently just code in Qcrisp.cpp since separating this
 * code allows other windows to be the "top-level" window for the entire
 * GUI. Thus, this class helps make it possible to have a qIOTool as the main
 * application window. */

#ifndef	__QFILEMANAGER_H__
#define	__QFILEMANAGER_H__

#include <qobject.h>

extern char writename[];

class qFileManager : public QObject {
	Q_OBJECT

protected:
	QWidget *topLevel; /* Top-level widget for the application */

public:
	qFileManager();

	void setTopLevel(QWidget *parent) {topLevel = parent;}

	/* This function actually loads the given file */
	void openFile(const QString& fileName);
	int saveFile(const QString& fileName, int warn = FALSE); /* Returns nonzero on error */

	void emitFileOpening() {emit fileOpening();}
	void emitFileOpened() {emit fileOpened();}

signals:
	void fileOpening(); /* When a file is about to be read in */
	void fileOpened(); /* After a file has been read in and all IDCs, etc. have been updated */

public slots:
    virtual void fileOpen(); /* Pop open Open File dialog and then run openFile() */
    virtual int fileSaveAs(); /* Returns nonzero on error */
};

extern qFileManager fileManager;

#endif
