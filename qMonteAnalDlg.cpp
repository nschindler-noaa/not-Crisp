/* qMonteAnalDlg.h
 * This class defines a dialog which performs monte alternative analysis.
 *
 * This is a special-purpose dialog which will probably not be subclassed.
 *
 * The code for this dialog was stolen from at_frame.c (the XView version)
 *  with minimal understanding.
 */


#include "qMonteAnalDlg.h"
#include "qTextOutDialog.h"
#include "qGraphMedDrawLines.h"
#include "qGraphMedZoom.h"
#include "qGraphMedPosition.h"
#include "qGraphMedDrawAxisYD.h"
#include "qGraphMedDrawAxisXI.h"
#include "qHelpManager.h"
#include <qmessagebox.h>
#include "resource.h"
#include "Qcrisp.h"
#include "altern_list.h"
#include "dir.h"
#include "log.h"
#include "report.h"

#include <stdlib.h>
#ifdef SOLARIS2
#include <unistd.h>
#endif

#ifdef WIN32
#include <io.h>
#endif

#ifdef WIN32
/* we need this constant and don't know where to get it.
 *  TODO - get it from a header file somewhere.
 */
#define R_OK  04
#endif

#define MIN_NUM_BINS		1
#define MAX_NUM_BINS		200
#define DEFAULT_NUM_BINS	50
#define NEAR_ZERO		0.000001

extern "C" {
    extern void at_struct_free( struct at_struct *at_data );
	extern struct at_struct *parse_at_input ( char *input_file );
	extern struct summary *get_data( int games, struct rls *cur_rls,
							    char *seg_name, int flags, 
								float *total, int *num );
	extern float *create_summary( struct rls_stats *rls_data, float *total, 
						int num, float *mean_day, float *first_day, 
						float *last_day, float *median_day, float *mode_day, 
						float *day_dev, float *avg_flow, int flags );
	extern void kstwo( float *data1, int n1, float *data2, int n2,
							float *d, float *prob );
	extern int split_data( struct summary *in_ptr, int size,
					struct rls_stats **out_ptr,	float **out_alt );
	extern void moment( float *data, int n, float *ave, float *sdev,
						float *svar, float *skew, float *curt );
	extern float compute_weighted_average( float *travel_times, 
							struct rls_stats *rls_data, int num );
};



qMonteAnalDlg::qMonteAnalDlg( QWidget *w )
: MonteAnalToolBase ( NULL ) 
	
{
	//Set up the widget refrences
	cmbAltern_ = AlternativeCB;
	cmbYear_ = YearCB;
	cmbRelease_ = ReleaseCB;
	cmbPassPoint_ = PassagePointCB;
	stHistMean_ = HistMeanLabel;
	stHistStdDev_ = HistStdDevLabel;
	stHistCoeffVar_ = HistCoeffLabel;
	stHistSkew_ = HistSkewLabel;
	stHistKurtosis_ = HistKurtosisLabel;
	stHistNGames_ = HistNumGamesLabel;
	stPassMean_ = DOPMeanLabel;
	stPassMedian_ = DOPMedianLabel;
	stPassMode_ = DOPModeLabel;
	stPassFirst_ = DOPFirstLabel;
	stPassLast_ = DOPLastLabel;
	stPassStdDev_ = DOPStdDevLabel;
	stRouteBypass_ = RouteBypassLabel;
	stRouteTurbine_ = RouteTurbineLabel;
	stRouteSpillway_ = RouteSpillwayLabel;
	stRouteTransport_ = RouteTransportLabel;
	stRouteFPE_ = RouteFPELabel;
	stRoutePremort_ = RoutePreMortLabel;
	stRoutePostmort_ = RoutePostMortLabel;
	bCompute_ = PlotButton;
	bPrint_ = PrintButton;
	bDone_ = DoneButton;
	bHelp_ = HelpButton;
	rbSurvival_ = SurvivalRadio;
	rbTravelTime_ = TravelTimeRadio;
	rbFrequency_ = FrequencyRadio;
	rbCumulative_ = CumulativeRadio;
	edBinSize_ =BinSizeLine;

	consCommon( );
}

void qMonteAnalDlg::consCommon( void ) {

	/* Hook up child window controls.
	 */
	lock_ = 1;
	flagGraphRemoved_ = 0;
	flagSelfDeleted_ = 0;

	setWFlags(getWFlags() | WDestructiveClose);


	connect(cmbAltern_, SIGNAL(activated(int)), SLOT(cbAltern()));

	connect(cmbYear_, SIGNAL(activated(int)), SLOT(cbYear()));

	connect(cmbRelease_, SIGNAL(activated(int)), SLOT(cbRelease()));

	connect(cmbPassPoint_, SIGNAL(activated(int)), SLOT(cbPassPoint()));

	rbSurvival_->setChecked( TRUE );
	connect(rbSurvival_,SIGNAL(clicked ()), SLOT(cbSurvival()));

	rbTravelTime_->setChecked( FALSE );
	connect(rbTravelTime_,SIGNAL(clicked ()), SLOT(cbTravelTime()));

	QButtonGroup* bg = new QButtonGroup();
	bg->insert (rbTravelTime_);
	bg->insert (rbSurvival_);


	rbFrequency_->setChecked( TRUE );
	connect(rbFrequency_,SIGNAL(clicked ()), SLOT(cbFrequency()));

	rbCumulative_->setChecked( FALSE );
	connect(rbCumulative_,SIGNAL(clicked ()), SLOT(cbCumulative()));
	
	QButtonGroup* bg2 = new QButtonGroup();
	bg2->insert (rbFrequency_);
	bg2->insert (rbCumulative_);
	
	edBinSize_->setText( "0.02" );

	
	connect(bCompute_,SIGNAL(clicked ()), SLOT(cbCompute()));

	connect(bPrint_,SIGNAL(clicked ()), SLOT(cbPrint()));

	connect(bDone_,SIGNAL(clicked ()), SLOT(cbDone()));
	
	connect(bHelp_,SIGNAL(clicked ()), SLOT(cbHelp()));

	lock_ = 0;
	CreateGraphWindow();

    // This will set up the alternatives combo box and clear out the others.
    altsUpdated();
}

/* Pointer to currently active member of this class, if any.
 */
qMonteAnalDlg *qMonteAnalDlg::instance_ = NULL;

qMonteAnalDlg::~qMonteAnalDlg()
{
	flagSelfDeleted_ = 1;

	/* Discard dynamic data structures.
	 */
	if (cur_alt)
		at_struct_free(cur_alt);

    while (riv_seg_list) {
		struct river_seg_info *item;

		item = riv_seg_list;
		riv_seg_list = riv_seg_list->next;
		free(item);
    }

	instance_ = NULL;

	DestroyGraphWindow();
}

qMonteAnalDlg *qMonteAnalDlg::instance( QWidget *w )
{
	if (instance_ == NULL)
		instance_ = new qMonteAnalDlg( w );

	return instance_;
}

/* Receive notification that graph dialog has been closed. Use flag
 *  to prevent double-deletion of graph dialog.
 */
void qMonteAnalDlg::cbGraphRemoved(){
	flagGraphRemoved_ = 1;
	if (!flagSelfDeleted_) {
		cbDone();
	}
}

/* Routines to create and destroy the histogram graphing window, data
 *  token, and mediator. */
void qMonteAnalDlg::CreateGraphWindow() {
	/* First the graph data token.
	 */
	dt_ = new qGraphDataToken( "Analysis Tool", "", 0, 0, GR_POSITIVE, GRT_DOUBLE );
	dt_->setBlockUpdate( 1 );
	dt_->setShowDots( 0 );
	dt_->setNumLines( 1 );
	dt_->setNumElementsPerLine( DEFAULT_NUM_BINS );
	for (int i=0; i < DEFAULT_NUM_BINS; i++)
		dt_->insert(0.0);
	dt_->insertLabel("percent");
	dt_->setYBounds( 1 );
	dt_->setHelpPage( 81 );

	/* Then the graph dialog.
	 */
	gd_ = new qGraphDialog( NULL, 0, Qt::WType_Dialog | WDestructiveClose,
								dt_->getGraphTitle().ascii() );
   
	connect(gd_, SIGNAL(evRemove_()), SLOT( cbGraphRemoved() ) );
	/* Then the graph mediator and submediators.
	 */
	gm_ = new qGraphMediator( gd_, dt_ );
	qGraphPane *gp = gd_->getGraphPane();
	gp->drawMethod( HOLLOWBARS );
	gm_->insertSubMediator( new qGraphMedDrawLines( gp ) );
	gm_->insertSubMediator( new qGraphMedDrawAxisYD( gp ) );
	gm_->insertSubMediator( new qGraphMedDrawAxisXI( gp ) );
	gm_->insertSubMediator( new qGraphMedZoom( gp, gd_ ) );
	gm_->insertSubMediator( new qGraphMedPosition( gp, gd_ ) );
}

void qMonteAnalDlg::showEvent(QShowEvent* e) {
	if (gd_ != NULL) gd_->show();
}

void qMonteAnalDlg::DestroyGraphWindow() {
	if (!flagGraphRemoved_)
		delete gd_;
}

/* Internal routines.
 */
void qMonteAnalDlg::setTextValues( void ) {
	int i;

	if (passValid) {
		stHistMean_->setText(QString("%1").arg(p_ave));
		stHistStdDev_->setText(QString("%1").arg(p_sdev));
		stHistCoeffVar_->setText(QString("%1").arg(p_sdev / p_ave));
		stHistSkew_->setText(QString("%1").arg(p_skew));
		stHistKurtosis_->setText(QString("%1").arg(p_curt));
		stHistNGames_->setText(QString("%1").arg(numgames));
	
		stPassMean_->setText(QString("%1").arg(m_day));
		stPassMedian_->setText(QString("%1").arg(m_median));
		stPassMode_->setText(QString("%1").arg(m_mode));
		stPassFirst_->setText(QString("%1").arg(m_first));
		stPassLast_->setText(QString("%1").arg(m_last));
		stPassStdDev_->setText(QString("%1").arg(m_dev));
	
	}
	else {
		stHistMean_->setText( "" );
		stHistStdDev_->setText( "" );
		stHistCoeffVar_->setText( "" );
		stHistSkew_->setText( "" );
		stHistKurtosis_->setText( "" );
		stHistNGames_->setText( "" );

		stPassMean_->setText( "" );
		stPassMedian_->setText( "" );
		stPassMode_->setText( "" );
		stPassFirst_->setText( "" );
		stPassLast_->setText( "" );
		stPassStdDev_->setText( "" );
	}
	
	if (trackValid) {
		stRouteBypass_->setText(QString("%1").arg(byp_avg));
		stRouteTurbine_->setText(QString("%1").arg(tur_avg));
		stRouteSpillway_->setText(QString("%1").arg(spi_avg));
		stRouteTransport_->setText(QString("%1").arg(tra_avg));
		stRouteFPE_->setText(QString("%1").arg(fpe));
		stRoutePremort_->setText(QString("%1").arg(premort_transport_total));
		stRoutePostmort_->setText(QString("%1").arg(postmort_transport_total));

	}
	else {
		stRouteBypass_->setText( "" );
		stRouteTurbine_->setText( "" );
		stRouteSpillway_->setText( "" );
		stRouteTransport_->setText( "" );
		stRouteFPE_->setText( "" );
		stRoutePremort_->setText( "" );
		stRoutePostmort_->setText( "" );
	}

	if (!graphValid) {
		dt_->dataArray_.clear();
		dt_->setNumElementsPerLine( MIN_NUM_BINS );
		for (i = 0; i < MIN_NUM_BINS; i++)
			dt_->insert( 0.0, 0 );
		dt_->setYBounds( 1 );
		dt_->setXMax( 1.0 );
		dt_->setDirty();
		//gd_->getGraphPane()->setDirty();  ??
	}
}

/*
 * species_name - Gives the name of the currently selected species, or
 *	all, if that is the case.
 *
 *	Note: This will need to change when we allow for a more sophisticated
 *	selection process.
 */

char *qMonteAnalDlg::species_name( void ) {
    return (cur_rls->next) ? (char*)"all species" : (char*)cur_rls->species;
}

/*
 * release_name - Gives the name of the currently selected release, or
 *	all, if that is the case.
 *
 *	Note: This will need to change when we allow for a more sophisticated
 *	selection process.
 */
char *qMonteAnalDlg::release_name( void ) {
    return (cur_rls->next) ? (char*)"all releases" : (char*)cur_rls->location;
}

/*
 * set_release - Sets cur_rls to a single release (as opposed to a list).
 *	Necessary because cur_rls is a list item.
 */
void qMonteAnalDlg::set_release(struct rls *rls) {

    bcopy(rls, &current_rls, sizeof(struct rls));
    current_rls.next = 0;
    cur_rls = &current_rls;
}

/*
 * copy_seg - Returns a pointer to a malloc'ed bcopied river_seg_info
 *	structure, identical to the one passed in, except with the next
 *	field nulled.
 */
struct river_seg_info *qMonteAnalDlg::copy_seg( 
									struct river_seg_info *seg ) {
    struct river_seg_info *new_seg;

    if (!(seg && (new_seg = (struct river_seg_info *)
								malloc(sizeof(struct river_seg_info)))))
		return NULL;
    bcopy(seg, new_seg, sizeof(struct river_seg_info));
    new_seg->next = NULL;
    return new_seg;
}

void qMonteAnalDlg::setTextValue( QLabel *st, float val ) {
	char cBuf[25];

	sprintf( cBuf, "%.2f", val );
	st->setText( cBuf );
}

void qMonteAnalDlg::setTextValue( QLabel *st, int val ) {
	char cBuf[25];

	sprintf( cBuf, "%d", val );
	st->setText( cBuf );
}

/* This actually makes a histogram graph (including filling in output fields
 *  in qMonteAnalDlg and also creating/updating histogram window).
 */
void qMonteAnalDlg::make_analysis_hist( rls_stats *rls_data, float total, 
				int num, char *species_name, char *release_name,
				char *segment_name, char *flow_seg_name, int flag_mask, 
				float *o_data, char *name) {
	/* */
	float *passage;
	float dummy;
	float xmax, binsize;
	QString cstr, title, label;
	int nbins;
	float ftemp[MAX_NUM_BINS];
	int i, j;

	trackValid = passValid = graphValid = 0;

	for (i = 0; i < MAX_NUM_BINS; i++)
		ftemp[i] = 0.0;

	/* Summary statistical values.
	*/
	numgames = num;
    passage=create_summary(rls_data, &total, num, &m_day, &m_first,
			   &m_last, &m_median, &m_mode, &m_dev, &avg_flow, flag_mask);

    if ((flag_mask & AT_TRAVEL_FLAG)
						&& 	passage != 0) {
		/* passage != 0 is a error flag for travel time, only!
		 * pretty lame, really.
		 */
		passage = NULL;
		goto escape;
    }

#ifdef ROUTE_TRACKING
    trackValid = calculate_route_avgs(rls_data, num, &sluiceway_avg, &byp_avg,
						&tur_avg, &spi_avg, &rsw_avg, &tra_avg,
						&premort_transport_total, &postmort_transport_total,
						&fpe, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy);
#endif

    if (flag_mask & AT_TRAVEL_FLAG) {
		passage = o_data;
		}
	else {
		if (!passage) {
			QMessageBox:: warning (this, "Alternative Analysis error","Unable to create histogram.", 
							 QMessageBox::Ok,  QMessageBox::NoButton );		
			goto escape;
		}
    }

    if (num < 2) {
		p_ave = *passage;
		p_sdev = p_svar = p_skew = p_curt = 0.0;
    } else {
		moment(passage, num, &p_ave, &p_sdev, &p_svar, &p_skew, &p_curt);
		if (flag_mask & AT_TRAVEL_FLAG)
			p_ave = compute_weighted_average(o_data, rls_data, num);
    }

	if (p_ave < NEAR_ZERO) {
		QMessageBox:: warning (this, "Alternative Analysis error","Insufficient passage to perform analysis", 
							 QMessageBox::Ok,  QMessageBox::NoButton );
		goto escape;
	}

    if (!(flag_mask & AT_TRAVEL_FLAG)) {
		p_ave *= 100.0;
		p_sdev *= 100.0;
    }
	passValid = 1;

	/* update histogram window - first get # bins and max value.
	 */
    if (flag_mask & AT_TRAVEL_FLAG) {
        dt_->setHistogramMode( 1 );
		xmax = ((int) o_data[num-1] + 1);
    }
    else {
        dt_->setHistogramMode( 0 );
		xmax = 1.0;
    }
	cstr = edBinSize_->text();
	binsize = atof( cstr.ascii() );
	if (binsize > xmax)
		binsize = xmax;
	if (binsize <= 0.0)
		binsize = 0.02;
	nbins = (int)(0.5 + xmax / binsize);
	if (nbins > MAX_NUM_BINS)
		nbins = MAX_NUM_BINS;
	if (nbins < MIN_NUM_BINS)
		nbins = MIN_NUM_BINS;

	binsize = xmax / nbins;

	/* Then fill a temporary array with the values to be plotted.
	 */
	if (num > 0) {
		for (i = 0; i < num; i++) {
			/* NOTE this is a different, and to me more natural, way of
			 *  assigning values into histogram bins. My way results in
			 *  intervals which are closed at the left and open at the right,
			 *  whereas the old way led to the reverse. This will produce
			 *  visually different results when many of the values fall
			 *  exactly in bin boundaries. To achieve the visual results
			 *  of the old program, use this line to calculate "j".
			 *  j = (int)(0.999999 * passage[i]) / binsize;
			 */
			j = (int)(passage[i] / binsize);
			if (j < 0)
				j = 0;
			if (j >= nbins)
				j = nbins - 1;
			ftemp[j] += 100.0 / num;
		}
	}
	if (flag_mask & AT_CUMUL_FLAG) {
		for (i = 1; i < nbins; i++)
			ftemp[i] += ftemp[i-1];
		if (ftemp[nbins-1] > 0.0)
			for (i = 0; i < nbins; i++)
				ftemp[i] *= 100.0 / ftemp[nbins-1];
	}

	/* Then update the graph data token. Try to be informative, since this
     *  may be printed or photocopied. 
	*/
    title = cmbAltern_->text( cmbAltern_->currentItem() ) +
               ", year " + cmbYear_->text( cmbYear_->currentItem() ) + ", "
               + ((flag_mask & AT_TRAVEL_FLAG)
									? "Travel Time" : "Survival Analysis" );
    dt_->setGraphTitle( title.ascii() );
    gd_->setTitleLabel( title.ascii() );
    label = cmbRelease_->text( cmbRelease_->currentItem() ) + " at " +
             cmbPassPoint_->text( cmbPassPoint_->currentItem() );
    dt_->setGraphXAxisLabel( label.ascii() );

    dt_->setGraphYAxisLabel( "percent" );
	dt_->dataArray_.clear();
	dt_->setNumElementsPerLine( nbins );
	for (i = 0; i < nbins; i++)
		dt_->insert( ftemp[i], 0 );
	dt_->setYBounds( 1 );
	dt_->setXMax( xmax );
	graphValid = 1;

	gd_->getGraphPane()->repaint();  //repaint the output graph
	
	dt_->setDirty();


escape:
	if (passage)
		free( passage );
	free( rls_data );
	if (o_data != passage)
		free( o_data );
	return;
}

/*
 * make_panel_segments -- Set all the values of the segment_item choice stack
 *	for the given release.  The code for creating the river segment list
 *	based on multiple active releases is slow.  Improvement could be
 *	made through a number of methods, including hashing the segments.
 *
 *	we only make segements for the first of the pair of alternatives.
 */

void qMonteAnalDlg::make_panel_segments( ) {
    struct rls *release;
    struct river_seg_info *ptr;
    int i;
    QString old_string;;
    int new_choice = 0;

	if ((i = cmbPassPoint_->currentItem()) >= 0)
		old_string = cmbPassPoint_->text( i );
	cmbPassPoint_->clear();

    while (riv_seg_list) {
		struct river_seg_info *item;

		item = riv_seg_list;
		riv_seg_list = riv_seg_list->next;
		free(item);
    }

    if (!(release = cur_rls)) {
		cur_seg = 0;
		return;
    }

    if (release->next) {	/* Major nasty */
		struct river_seg_info *rls_list;
		struct river_seg_info *o_list;
		struct river_seg_info *item;
		struct river_seg_info *tail;

		rls_list = release->river_segment;
		tail = riv_seg_list = copy_seg(rls_list);
		rls_list = rls_list->next;
		while (rls_list) {
			item = copy_seg(rls_list);
			tail->next = item;
			tail = item;
			rls_list = rls_list->next;
		}
		release = release->next;
		while(release) {
			for (rls_list=release->river_segment;
			 rls_list;
			 rls_list=rls_list->next) {
			for (o_list = riv_seg_list;o_list;o_list=o_list->next) {
				if (!strcmp(o_list->name, rls_list->name)) break;
			}
			if (o_list) continue;
			item = copy_seg(rls_list);
			tail->next = item;
			tail = item;
			}
			release = release->next;
		}
		cur_seg = ptr = riv_seg_list;
    } else 
		cur_seg = ptr = release->river_segment;

    for (i=0 ; ptr ; i++, ptr=ptr->next){
		cmbPassPoint_->insertItem( ptr->name, cmbPassPoint_->count() );
		if(ptr->name == old_string)
			cmbPassPoint_->setCurrentItem(i); //try to keep the original selection
	}

}

/* 
 * make_panel_releases -- Set all the values of the release_item choice
 *	stack for the given year.  Automatically resets cur->rls and cur->seg.
 */
void qMonteAnalDlg::make_panel_releases(){
    struct rls *ptr;
    char buf[80];
    char loc_buf[80];
    char spec_buf[80];
    int i;
    int new_active;

	QString last_release;

	if ((i = cmbRelease_->currentItem()) >= 0 && cmbRelease_->count()){
		last_release = cmbRelease_->text( i );
		sscanf(last_release.ascii(), "%s (%[^)])", loc_buf, spec_buf);
	}

	cmbRelease_->clear();
    if (!cur_year) {
		cur_rls = NULL;
		make_panel_segments();
		return;
    }

	cmbRelease_->insertItem( "All (All)", 0 );
    new_active = 0;

    for (i = 1, cur_rls = ptr = cur_year->releases ;
								ptr ; i++, ptr = ptr->next) {
		sprintf(buf, "%s (%s)", ptr->location, ptr->species);
		cmbRelease_->insertItem( buf, cmbRelease_->count() );

		if (!(strcmp(ptr->location, loc_buf)
				  || strcmp(ptr->species, spec_buf)))
			new_active = i;
    }

	if (!new_active) {
		new_active = (strcmp(loc_buf, "All") || strcmp(spec_buf, "All"))
		? 1 : 0;
	}

    if (cur_rls) {
		set_release( cur_rls );
		cmbRelease_->setCurrentItem( new_active );
    }
    make_panel_segments();
}

/*
 * int read_altern( char *altern ) - Read an
 *	alternative from directory file altern into the cur_alt pointer.
 */
int qMonteAnalDlg::read_altern( const char *altern ) {
	int i;
    char fname[MAXPATHLEN];

    if (cur_alt) {
		at_struct_free(cur_alt);
        cur_alt = NULL;
    }
    cur_year = NULL;
	cmbYear_->clear();

    /* check to see if the file/folder exists */
	/* since it is a directory, check the file inside */
	sprintf( fname, "%s/%s.out", altern, altern );
	if (access(fname, R_OK) == -1)
	{
		QString cstr = "Cannot find alternative file ";
		cstr += fname;
		QMessageBox:: warning (this, "Alternative Analysis error",cstr, 
							 QMessageBox::Ok,  QMessageBox::NoButton );
		make_panel_releases();
		return 0;
    }

    if (!(cur_alt = parse_at_input(fname))) {
		cur_year = NULL;
		make_panel_releases();
		return 0;
    }

	for (i = 1; i <= cur_alt->years; i++) {
		sprintf( fname, "%d", i );
		cmbYear_->insertItem( fname, cmbYear_->count() );
		if(cur_alt->release_year->year == (unsigned)i)      //try to keep the same selection
			cmbYear_->setCurrentItem( cmbYear_->count()-1 );
	}
    cur_year = cur_alt->release_year;

	make_panel_releases();
    return 1;
}

/* Event reponse routines.
 */
void qMonteAnalDlg::cbAltern(  ) {
	if (lock_)
		return;  /* event "handled" */
	lock_ = 1;

	QString cstr;
	int sel;

	if ((sel = cmbAltern_->currentItem()) >= 0) {
		cstr = cmbAltern_->text( sel );
		read_altern( cstr.ascii() );

		passValid = trackValid = graphValid = 0;
		setTextValues();
	}

	lock_ = 0;
	return ;  /* event handled */
}

void qMonteAnalDlg::cbYear(  ) {
	if (lock_)
		return ;  /* event "handled" */
	lock_ = 1;

	int value;

    if (cur_alt && (value = cmbYear_->currentItem()) >= 0) {
		if (value  > cur_alt->years)
			cur_year = 0;
		else
			cur_year = cur_alt->release_year + value;
		make_panel_releases();

		passValid = trackValid = graphValid = 0;
		setTextValues();
	}

	lock_ = 0;
	return ;  /* event handled */
}

void qMonteAnalDlg::cbRelease(  ) {
	if (lock_)
		return ;  /* event "handled" */
	lock_ = 1;

	int value = cmbRelease_->currentItem();

    if (!cur_year) {
		QMessageBox:: warning (this, "Alternative Analysis","No active year!", 
							 QMessageBox::Ok,  QMessageBox::NoButton );
		goto escape;
    }

    if (!value) {	/* Value 0 corresponds to All Releases. */
		cur_rls = cur_year->releases;
		make_panel_segments();	/* We need a better method than this */
		goto escape;
    }
    value--;	/* Because 0 is all */
    for (cur_rls = cur_year->releases ; value && cur_rls ; value--)
		cur_rls = cur_rls->next;
    if (!cur_rls)
		QMessageBox:: warning (this, "Alternative Analysis","No release found!", 
					 QMessageBox::Ok,  QMessageBox::NoButton );		
    else 
		set_release(cur_rls);
    make_panel_segments();

escape:
	passValid = trackValid = graphValid = 0;
	setTextValues();
	lock_ = 0;
	return ;  /* event handled */
}

void qMonteAnalDlg::cbPassPoint( ) {
	if (lock_)
		return ;  /* event "handled" */
	lock_ = 1;

	int value = cmbPassPoint_->currentItem();

    if (!cur_rls) {
		QMessageBox:: warning (this, "Alternative Analysis","No active release!", 
				 QMessageBox::Ok,  QMessageBox::NoButton );				
		goto escape;
    }

    if (cur_rls->next)	/* All releases selected */
		for (cur_seg = riv_seg_list ; value && cur_seg ; value--)
			cur_seg = cur_seg->next;
    else
		for (cur_seg = cur_rls->river_segment ;
			 value && cur_seg ; value--) {
	    
			cur_seg = cur_seg->next;
		}

	if (!cur_seg) 
		QMessageBox:: warning (this, "Alternative Analysis","No current segment!", 
				 QMessageBox::Ok,  QMessageBox::NoButton );					

escape:
	passValid = trackValid = graphValid = 0;
	setTextValues();
 	lock_ = 0;
	return ;  /* event handled */
}

void qMonteAnalDlg::cbSurvival(  ) {
	if (lock_)
		return ;  /* event "handled" */
	lock_ = 1;

	edBinSize_->setText( "0.02" );
    edBinSize_->setEnabled(true);

	passValid = trackValid = graphValid = 0;
	setTextValues();
	lock_ = 0;
	return ;  /* event handled */
}

void qMonteAnalDlg::cbTravelTime(  ) {
	if (lock_)
		return ;  /* event "handled" */
	lock_ = 1;

	edBinSize_->setText( "1" );
    edBinSize_->setEnabled(false);

	passValid = trackValid = graphValid = 0;
	setTextValues();
	lock_ = 0;
	return ;  /* event handled */
}

void qMonteAnalDlg::cbCumulative(  ) {
	if (lock_)
		return ;  /* event "handled" */
	lock_ = 1;

	passValid = trackValid = graphValid = 0;
	setTextValues();
	lock_ = 0;
	return ;  /* event handled */
}

void qMonteAnalDlg::cbFrequency(  ) {
	if (lock_)
		return ;  /* event "handled" */
	lock_ = 1;

	passValid = trackValid = graphValid = 0;
	setTextValues();
	lock_ = 0;
	return ;  /* event handled */
}

void qMonteAnalDlg::cbPrint(  ) {
	qTextOutDialog *tld;
	char cBuf[80];
	QString cstr;

	if (lock_)
		return ;  /* event "handled" */

	if (passValid || trackValid) {
		/* Create a text logging window of current statistics.
		 */
		lock_ = 1;
		tld = new qTextOutDialog( NULL );
		
		cstr = cmbAltern_->text( cmbAltern_->currentItem() );
		sprintf( cBuf, "Alternative name: %s\n", cstr.ascii() );
		tld->addString( cBuf );
		sprintf( cBuf, "Year: %d\n", cmbYear_->currentItem() + 1 );
		tld->addString( cBuf );
		cstr = cmbRelease_->text( cmbRelease_->currentItem() );
		sprintf( cBuf, "Release: %s\n", cstr.ascii() );
		tld->addString( cBuf );
		cstr = cmbPassPoint_->text( cmbPassPoint_->currentItem() );
		sprintf( cBuf, "Passage point: %s\n", cstr.ascii() );
		tld->addString( cBuf );
		if (rbTravelTime_->isOn())
			tld->addString( "\nCalculating Travel Time\n\n" );
		else
			tld->addString( "\nCalculating Passage\n\n" );


		if (passValid) {
			sprintf( cBuf, "\tMean:  %f\n", p_ave);
			tld->addString( cBuf );
			sprintf( cBuf, "\tStandard Deviaton:  %f\n", p_sdev);
			tld->addString( cBuf );
			sprintf( cBuf, "\tCoefficient of Variance:  %f\n",
										p_ave == 0 ? 0 : p_sdev / p_ave);
			tld->addString( cBuf );
			sprintf( cBuf, "\tSkew:  %f\n", p_skew);
			tld->addString( cBuf );
			sprintf( cBuf, "\tKurtosis:  %f\n", p_curt);
			tld->addString( cBuf );
			sprintf( cBuf, "\tGames:  %d\n", numgames);
			tld->addString( cBuf );

			sprintf( cBuf, "\tDay of passage mean:  %f\n", m_day);
			tld->addString( cBuf );
			sprintf( cBuf, "\tDay of passage median:  %f\n", m_median);
			tld->addString( cBuf );
			sprintf( cBuf, "\tDay of passage mode:  %f\n", m_mode);
			tld->addString( cBuf );
			sprintf( cBuf, "\tDay of passage first:  %f\n", m_first);
			tld->addString( cBuf );
			sprintf( cBuf, "\tDay of passage last:  %f\n", m_last);
			tld->addString( cBuf );
			sprintf( cBuf, "\tDay of passage standard deviation:  %f\n", m_dev);
			tld->addString( cBuf );
		}

		if (trackValid && (byp_avg > 0.0 || tur_avg > 0.0 ||
							spi_avg > 0.0 || tra_avg > 0.0))
		{
			sprintf( cBuf, "\tPassage via bypass:  %f\n", byp_avg);
			tld->addString( cBuf );
			sprintf( cBuf, "\tPassage via turbine:  %f\n", tur_avg);
			tld->addString( cBuf );
			sprintf( cBuf, "\tPassage via spillway:  %f\n", spi_avg);
			tld->addString( cBuf );
			sprintf( cBuf, "\tPassage via transport:  %f\n", tra_avg);
			tld->addString( cBuf );
			sprintf( cBuf, "\tFPE:  %f\n", fpe);
			tld->addString( cBuf );
		}
		if (premort_transport_total > 0.0 || postmort_transport_total > 0.0)
		{
			sprintf( cBuf, "\tPremort Transport:  %f\n",
											premort_transport_total);
			tld->addString( cBuf );
			sprintf( cBuf, "\tPostmort Transport:  %f\n",	
											postmort_transport_total);
			tld->addString( cBuf );
		}

		lock_ = 0;
	}

	return ;  /* event handled */
}

void qMonteAnalDlg::cbCompute(  ) {
	if (lock_)
		return ;  /* event "handled" */
	lock_ = 1;

    struct summary *ptr;
    struct rls_stats *data = 0;
    float total;
    int num;
	int flags = 0;
    int i;
    float *travel = 0;

    /* get and split the summary data for the two alternatives */

	passValid = trackValid = graphValid = 0;
	if (rbTravelTime_->isOn())
		flags = AT_TRAVEL_FLAG;
	if (rbCumulative_->isOn())
		flags += AT_CUMUL_FLAG;

    if (!(cur_seg && cur_alt)) {
		QMessageBox:: warning (this, "Alternative Analysis","No data available!", 
				 QMessageBox::Ok,  QMessageBox::NoButton );	
		goto escape;
	}

	if (!(ptr = get_data(cur_alt->games, cur_rls,
								cur_seg->name, flags, &total, &num))) {
		QMessageBox:: warning (this, "Alternative Analysis","Unable to create histogram!", 
				 QMessageBox::Ok,  QMessageBox::NoButton );	
		goto escape;
    }

    if (!split_data(ptr, num, &data, &travel)) {
		QMessageBox:: warning (this, "Alternative Analysis","Unable to create histogram arrays!", 
				 QMessageBox::Ok,  QMessageBox::NoButton );	
		goto escape;
	}

	for(i = 0; i < num; ++i)
		free(ptr[i].info);
	free(ptr);

    make_analysis_hist(data, total, num, species_name(), release_name(),
		       cur_seg->name, cur_rls->flow_seg, flags,
		       travel, cur_alt->alternative);

escape:
	setTextValues();
	lock_ = 0;
	return ;  /* event handled */
}

void qMonteAnalDlg::cbDone(  ) {
	if (lock_)
		return ;  /* event "handled" */
	lock_ = 1;
	
	close(TRUE);

	lock_ = 0;
	return ;  /* event handled */
}

void qMonteAnalDlg::cbHelp( ) {
	helpManager->showHelp( QString("80.html") );
	return ;  /* event handled */
}

// Monte Carlo tool may prod us when the alternatives change on disk.
void qMonteAnalDlg::altsUpdated( void ) {
	/* Initialize alternatives to compare.
	 */
	cur_seg = NULL;
	riv_seg_list = NULL;
	cur_alt = NULL;
	cur_year = NULL;
	cur_rls = NULL;
	trackValid = passValid = graphValid = 0;

	cmbAltern_->clear();
    cmbYear_->clear();
    cmbRelease_->clear();
    cmbPassPoint_->clear();

    // It may happen that the altern list has not been initialized yet.
    if (get_altern_list_size() == 0)
        altern_update_directory();

    for (reset_altern_list() ; get_current_list_altern(); inc_altern_list()) {
		cmbAltern_->insertItem( get_current_list_altern(), cmbAltern_->count() );
	}

	setTextValues();

	/* initialize selections by calling event handlers */
	if (get_altern_list_size() > 0) {
		cbAltern();
		cbYear();
		cbRelease();
		cbPassPoint();
	}
}



