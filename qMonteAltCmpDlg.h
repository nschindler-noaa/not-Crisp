/* qMonteAltCmpDlg.h
 * This class defines a dialog which performs monte alternative comparisons.
 *
 * This is a special-purpose dialog which will probably not be subclassed.
 */

#ifndef __qMONTEALTCMPDLG_H__
#define __qMONTEALTCMPDLG_H__

//#include <zapp.hpp>
#include "AlternComparisonBase.h"
#include "crisp.h"
#include <qcombobox.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qbuttongroup.h>


#include <qbuttongroup.h>
extern "C" {
#include "struct_at.h"
};

#define NUM_COMPS 2 	/* number of comparable alterns */

class qMonteAltCmpDlg : public AlternComparisonBase {
	Q_OBJECT

protected:


	static qMonteAltCmpDlg *instance_;

	void consCommon( void );
	void clearOutput( void );

	/* Child window controls */
	QComboBox *cmbAltern1_, *cmbAltern2_, *cmbYear1_, *cmbYear2_,
		              *cmbRelease1_, *cmbRelease2_, *cmbPassPoint_;

	QLabel *stOut1_, *stOut2_;

	QPushButton *bCompute_, *bDone_, *bHelp_;

	QRadioButton *rbSurvival_, *rbTravelTime_;
	
	QButtonGroup *bg; //holds the two radio buttons in a button group

protected slots:

	/* Event handlers. */
	void cbDone();
	void cbHelp();
	void cbAltern1(int pos);
	void cbAltern2(int pos);
	void cbYear1(int pos);
	void cbYear2(int pos);
	void cbRelease1(int pos);
	void cbRelease2(int pos);
	void cbPassPoint(int pos);
	//int cbSurvival();
	//int cbTravelTime();
	void cbCompute();

protected:


	/* Hopefully prevents UI notification recursion */
	int lock_;

	/* Contains data structures which describe alternatives being compared.
	 */
	struct at_disp alterns[NUM_COMPS];
	struct river_seg_info *cur_seg;
	struct river_seg_info *riv_seg_list;

	/* Local processing routines. */
	void set_release( struct at_disp *cur );
	struct river_seg_info *copy_seg( struct river_seg_info *seg );
	void make_panel_segments( struct at_disp *cur );
	void make_panel_releases( struct at_disp *cur );
	int compare_read_altern( const char *altern, struct at_disp *data );

public:

	static qMonteAltCmpDlg *instance( QWidget *w = NULL );

	qMonteAltCmpDlg( QWidget *w );
	~qMonteAltCmpDlg();

    // Monte Carlo tool may prod us when the alternatives change on disk.
    void altsUpdated( void );

};

#endif


