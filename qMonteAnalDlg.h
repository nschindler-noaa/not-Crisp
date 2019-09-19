/* qMonteAnalDlg.h
 * This class defines a dialog which performs monte alternative analysis.
 *
 * This is a special-purpose dialog which will probably not be subclassed.
 */

#ifndef __QMONTEANALDLG_H__
#define __QMONTEANALDLG_H__

//#include <zapp.hpp>
#include "crisp.h"
#include "qGraphMediator.h"
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qlineedit.h>
#include "MonteAnalToolBase.h"
#include "log.h"

extern "C" {
#include "struct_at.h"
};

class qMonteAnalDlg : public MonteAnalToolBase {
	Q_OBJECT
protected:

	static qMonteAnalDlg *instance_;

	void consCommon( void );

	/* Child window controls */
	QGroupBox *gb1_, *gb2_, *gb3_, *gb4_, *gb5_, *gb6_;

	QComboBox *cmbAltern_, *cmbYear_, *cmbRelease_, *cmbPassPoint_;

	QLabel *stHistMean_, *stHistStdDev_, *stHistCoeffVar_, *stHistSkew_,
				*stHistKurtosis_, *stHistNGames_;
	QLabel *stPassMean_, *stPassMedian_, *stPassMode_, *stPassFirst_,
				*stPassLast_, *stPassStdDev_;
	QLabel *stRouteBypass_ , *stRouteTurbine_, *stRouteSpillway_,
				*stRouteTransport_, *stRouteFPE_, *stRoutePremort_,
				*stRoutePostmort_;

	QPushButton *bCompute_, *bPrint_, *bDone_, *bHelp_;

	QRadioButton *rbSurvival_, *rbTravelTime_, *rbFrequency_, *rbCumulative_;

	QLineEdit *edBinSize_;

protected slots:
	/* Event handlers. */
	void cbAltern();
	void cbYear();
	void cbRelease();
	void cbPassPoint();

	void cbSurvival();
	void cbTravelTime();
	void cbFrequency();
	void cbCumulative();

	void cbDone();
	void cbHelp();
	void cbCompute();
	void cbPrint();

public slots:
	void cbGraphRemoved();

protected:
	/* Hopefully prevents UI notification recursion */
	int lock_;

	/* Contains data structures which describe alternatives being analyzed.
	 */
	struct at_struct *cur_alt;
	struct rls_years *cur_year;
	struct rls *cur_rls;
    struct rls current_rls;
	struct river_seg_info *cur_seg;
	struct river_seg_info *riv_seg_list;

	/* Contains summary statistical values and flags saying if they are valid.
	 */
    float p_ave, p_sdev, p_svar, p_skew, p_curt;	
    float m_day, m_first, m_last, m_mode, m_median, m_dev, avg_flow;
	int numgames;
	int passValid;  /* 1 = above stats valid, 0 = not */

    float sluiceway_avg, byp_avg, tur_avg, spi_avg, rsw_avg, tra_avg;
    float premort_transport_total, postmort_transport_total;
    float fpe;
	int trackValid; /* 1 = above stats valid, 0 = not */

	int graphValid; /* 1 = graph window valid, 0 = not. */

	/* Pointers to the histogram graphing window, data token, and mediator. */
	qGraphDataToken *dt_;
	qGraphMediator *gm_;
	qGraphDialog *gd_;

	/* Routines to create and destroy the histogram graphing window, data
	 *  token, and mediator. */
	void CreateGraphWindow( void );
	void DestroyGraphWindow( void );

	/* Local processing routines. */
	char *species_name( void );
	char *release_name( void );
	void set_release( struct rls *rls );
	struct river_seg_info *copy_seg( struct river_seg_info *seg );
	void make_panel_segments( );
	void make_panel_releases( );
	int read_altern( const char *altern );
	void make_analysis_hist( rls_stats *rls_data, float total, int num, 
				char *species_name, char *release_name,
			char *segment_name, char *flow_seg_name, int flag_mask, 
			float *o_data, char *name);

	void setTextValue( QLabel *st, float val );
	void setTextValue( QLabel *st, int val );
	void setTextValues( void );

public:

	static qMonteAnalDlg *instance( QWidget *w = NULL );

	qMonteAnalDlg( QWidget *w );
	~qMonteAnalDlg();

    // Monte Carlo tool may prod us when the alternatives change on disk.
    void altsUpdated( void );

	void showEvent( QShowEvent* );

	/* Receive notification that graph dialog has been closed. Use flag
	 *  to prevent double-deletion of graph dialog.
	 */
	int flagGraphRemoved_;
	int flagSelfDeleted_;

		
	//static void _cbGraphRemoved(void *msg);
};

#endif


