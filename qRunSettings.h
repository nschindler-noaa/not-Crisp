/* qRunSettings.h
 * This class defines a modal dialog which edits the runtime settings.
 *  The dialog is based on the IDD_RUNSETTINGS resource template.
 *
 * This class edits the global variable "struct run *current".
 *
 */

#ifndef __QRUNSETTINGS_H__
#define __QRUNSETTINGS_H__


#include "qEditGraphDataToken.h"
#include <qmainwindow.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qcheckbox.h>
#include <qbuttongroup.h>
#include <qpixmap.h>

class qRunSettings : public QMainWindow {

	Q_OBJECT

protected:
 	QCheckBox *VarSup_, *PredVol_, *AgeDep_;
    QRadioButton *PredGasMort_, *SimpleMort_;
	QButtonGroup *buttonGroup;

	/* These pointers are used to show/hide the dirtyness indicators.
	 */
	QLabel *bpDirty1_, *bpDirty2_, *bpDirty3_, *bpDirty4_;
	QPixmap  dirtyPM;

    /* These routines move settings between the dialog and the underlying
     *  data structures.
     */
    void getSettings( void );
    void applySettings( void );

    /* This routine shows or hides the red "dirty dots" to denote edit status.
     */
    void showDirty( int iDirty );

    /* This routine checks for a proper mortality class definition.
     */
    int checkMortClass( void );

	/* These pointers are used to enable/disable the OK and Apply buttons.
     */
    QPushButton *pbApply_, *pbOK_, *pbCancel, *pbReset, *pbHelp_;

	
	//static void _cbRunStatusChanged( void *msg );

public:
	qRunSettings( QWidget *w );
	~qRunSettings();
	
protected slots:
	/* Find out if a scenario or monte carlo run is being started/stopped.
	 *  If we are running, disable "OK" & "Apply".
	 */
	virtual void cbRunStatusChanged( void );	
	virtual void checkBoxChecked();
	virtual void PredGasMortChecked();
	virtual void SimpleMortChecked();
	virtual void okPressed();
	virtual void applyPressed();
	virtual void resetPressed();
	virtual void cancelPressed();
	virtual void helpPressed();



};

#endif

