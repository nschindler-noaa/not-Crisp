/* qReachClassTool.h
 * This class defines a modal dialog which edits the number and names
 *  of reach classes. It is based on the IDD_REACHCLASSNAMES 
 *  resource template.
 *
 * This class edits the global variables "num_reach_classes" and
 *  "reach_class_names[10]".
 *
 */

#ifndef __qReachClassTool_H__
#define __qReachClassTool_H__

#include "qlineedit.h"
#include <qmainwindow.h>
#include <qradiobutton.h>
#include "ReachClassNameBase.h"
#include <qbuttongroup.h>


class qReachClassTool : public ReachClassNameBase {

	Q_OBJECT

protected:
    QRadioButton *radioButton_[10];
    QLineEdit *editLine_[10];

	QButtonGroup *bGroup;  //button group holds radio buttons..

    /* These routines move settings between the dialog and the underlying
     *  data structures.
     */
    void getSettings( void );
    void applySettings( void );

    /* This routine shows or hides the red "dirty dots" to denote edit status.
     */
    void showDirty( int iDirty );

    /* Check for proper class names.
     */
    int checkClassNames( void );


public:
	qReachClassTool( QWidget *w );
	~qReachClassTool();

/* Find out if a scenario or monte carlo run is being started/stopped.
	 *  If we are running, disable "OK" & "Apply".
	 */
protected slots:
	virtual void cbRunStatusChanged();
	virtual void cbOK();
	virtual void cbReset();
	virtual void cbCancel();
	virtual void cbHelp();
	virtual void cbApply();
	virtual void radioClicked(int id); //accepts events from the radio buttons



};

#endif /* __qReachClassTool_H__ */

