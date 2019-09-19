/* qMonteAltCmpDlg.h
 * This class defines a dialog which performs monte alternative comparisons.
 *
 * This is a special-purpose dialog which will probably not be subclassed.
 *
 * The code for this dialog was stolen from at_compare.c (the XView version)
 *  with minimal understanding.
 */


#include "qMonteAltCmpDlg.h"
#include "resource.h"
#include "altern_list.h"

#include "Qcrisp.h"
#include "dir.h"
#include "log.h"
#include <stdlib.h>
#ifdef SOLARIS2
#include <unistd.h>
#endif /* SOLARIS2*/
#include <stdlib.h>
#include <stdio.h>
#include <qmessagebox.h>
#include "qHelpManager.h"

#ifdef WIN32
#include <io.h>
#endif


#ifdef WIN32
/* we need this constant and don't know where to get it.
 *  TODO - get it from a header file somewhere.
 */
#define R_OK  04
#endif

extern "C" {
    extern void at_struct_free( struct at_struct *at_data );
	extern struct at_struct *parse_at_input ( char *input_file );
	extern void at_struct_free( struct at_struct *at_data );
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
};



qMonteAltCmpDlg::qMonteAltCmpDlg( QWidget *w )
	: AlternComparisonBase(NULL, 0, FALSE, WType_TopLevel | WStyle_SysMenu | WStyle_Minimize | WStyle_Dialog)
{

    //Set the names from the base class to this child class
	stOut1_ = TopMessageLabel;
	stOut2_ = BottomMessageLabel;
	bCompute_ = ComputeButton;
	bDone_ = DoneButton;
	bHelp_ = HelpButton;
	rbSurvival_ = SurvivalButton; 
	rbTravelTime_ = TravelTimeButton;
	cmbAltern1_ = AlternCombo1;
	cmbAltern1_->setEditable ( false);
	cmbAltern2_ = AlternCombo2;
	cmbAltern2_->setEditable ( false);
	cmbYear1_ = YearCombo1;
	cmbYear1_->setEditable ( false);
	cmbYear2_ = YearCombo2;
	cmbYear2_->setEditable ( false);
	cmbRelease1_ = ReleaseCombo1;
	cmbRelease1_->setEditable ( false);
	cmbRelease2_ = ReleaseCombo2; 
	cmbRelease2_->setEditable ( false);
	cmbPassPoint_ = PassagePointCombo;
	cmbPassPoint_->setEditable ( false);
	
	//set the 
	consCommon();
	
}

void qMonteAltCmpDlg::consCommon( void ) {
	/* Hook up child window controls.
	 */
	lock_ = 1;

	connect(cmbAltern1_, SIGNAL(activated(int)), SLOT(cbAltern1(int)));
	connect(cmbYear1_, SIGNAL(activated(int)), SLOT(cbYear1(int)));
	connect(cmbRelease1_, SIGNAL(activated(int)), SLOT(cbRelease1(int)));
	connect(cmbPassPoint_, SIGNAL(activated(int)), SLOT(cbPassPoint(int)));
	connect(cmbAltern2_, SIGNAL(activated(int)), SLOT(cbAltern2(int)));
	connect(cmbYear2_, SIGNAL(activated(int)), SLOT(cbYear2(int)));
	connect(cmbRelease2_, SIGNAL(activated(int)), SLOT(cbRelease2(int)));
	
	clearOutput();

	if(!rbSurvival_->isOn())
		rbSurvival_->toggle();
	bg = new QButtonGroup(0);
	bg->setExclusive ( true);
	bg->insert(rbSurvival_);
	bg->insert(rbTravelTime_);

	connect(bCompute_, SIGNAL(clicked()), SLOT(cbCompute()));

	connect(bHelp_, SIGNAL(clicked()), SLOT(cbHelp()));

	connect(bDone_, SIGNAL(clicked()), SLOT(cbDone()));

	/* Initialize alternatives to compare.
	 */
	bzero( alterns, sizeof( struct at_disp ) * NUM_COMPS );
    altsUpdated();

	lock_ = 0;
}

/* Pointer to currently active member of this class, if any.
 */
qMonteAltCmpDlg *qMonteAltCmpDlg::instance_ = NULL;

qMonteAltCmpDlg::~qMonteAltCmpDlg()
{
	int i;

	/* Discard dynamic data structures.
	 */
	for (i = 0; i < NUM_COMPS; i++) {
		if (alterns[i].alt)
			at_struct_free(alterns[i].alt);
	}
    while (riv_seg_list) {
		struct river_seg_info *item;

		item = riv_seg_list;
		riv_seg_list = riv_seg_list->next;
		free(item);
    }

	instance_ = NULL;
}

qMonteAltCmpDlg *qMonteAltCmpDlg::instance( QWidget *w )
{
	if (instance_ == NULL)
		instance_ = new qMonteAltCmpDlg( w );

	return instance_;
}

/* Internal routines.
 */
void qMonteAltCmpDlg::clearOutput( void ) {
	stOut1_->setText( "" );
	stOut2_->setText( "" );
}

/*
 * set_release - Sets cur->rls to a single release (as opposed to a list),
 *      the data for which is in the display struct.
 *	Necessary because cur->rls is a list item.
 */
void qMonteAltCmpDlg::set_release( struct at_disp *cur ) {
    bcopy(cur->rls, &cur->one_release, sizeof(struct rls));
    cur->one_release.next = 0;
    cur->rls = &cur->one_release;
}

/*
 * copy_seg - Returns a pointer to a malloc'ed bcopied river_seg_info
 *	structure, identical to the one passed in, except with the next
 *	field nulled.
 */
struct river_seg_info *qMonteAltCmpDlg::copy_seg( 
									struct river_seg_info *seg ) {
    struct river_seg_info *new_seg;

    if (!(seg && (new_seg = (struct river_seg_info *)
								malloc(sizeof(struct river_seg_info)))))
		return (struct river_seg_info *) 0;
    bcopy(seg, new_seg, sizeof(struct river_seg_info));
    new_seg->next = 0;
    return new_seg;
}

/*
 * make_panel_segments -- Set all the values of the segment_item choice stack
 *	for the given release.  The code for creating the river segment list
 *	based on multiple active releases is slow.  Improvement could be
 *	made through a number of methods, including hashing the segments.
 *
 *	we only make segements for the first of the pair of alternatives.
 */

void qMonteAltCmpDlg::make_panel_segments( struct at_disp *cur ) {
    struct rls *release;
    struct river_seg_info *ptr;
    int i;
    QString old_string;
    int new_choice = 0;

    if (cur != &alterns[0])
		return;

	if ((i = cmbPassPoint_->currentItem()) >= 0)
		old_string = cmbPassPoint_->text( i );
	cmbPassPoint_->clear();

    while (riv_seg_list) {
		struct river_seg_info *item;

		item = riv_seg_list;
		riv_seg_list = riv_seg_list->next;
		free(item);
    }

    if (!cur || !(release = cur->rls)) {
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
		//not necccesary, but try to keep the users selection:
		if(old_string == QString(ptr->name))
			cmbPassPoint_->setCurrentItem( i );
	}
	
}

/* 
 * make_panel_releases -- Set all the values of the release_item choice
 *	stack for the given year.  Automatically resets cur->rls and cur->seg.
 */
void qMonteAltCmpDlg::make_panel_releases( struct at_disp *cur ) {
    struct rls *ptr;
    char buf[80];
    char loc_buf[80];
    char spec_buf[80];
    int i;
    int flag = 0;
    int new_active;
    QComboBox *cmb = (cur == &alterns[0]) ? cmbRelease1_ : cmbRelease2_;

	QString last_release;

    loc_buf[0] = 0;
    spec_buf[0] = 0;

    if (cmb->currentText() != NULL) {
		last_release = cmb->currentText();		
		sscanf(last_release.ascii(), "%s (%[^)])", loc_buf, spec_buf);
    }

	cmb->clear();
    if (!cur->year) {
		cur->rls = 0;
		make_panel_segments(cur);
		return;
    }

	cmb->insertItem( "All (All)", 0 );
    new_active = 0;

    for (i = 1, cur->rls = ptr = cur->year->releases;
								ptr ; i++, ptr = ptr->next) {
		sprintf(buf, "%s (%s)", ptr->location, ptr->species);
		cmb->insertItem( buf, cmb->count() );

		if (!flag && !(strcmp(ptr->location, loc_buf)
			|| strcmp(ptr->species, spec_buf))) {
			new_active = i;
			flag = 1;
		}
    }

    if (!flag)
		new_active = -1;
    else
		if (!new_active) {
			new_active = (strcmp(loc_buf, "All") || strcmp(spec_buf, "All"))
			? -1 : 0;
	}

    if (cur->rls) {
		set_release(cur);
		cmb->setCurrentItem( new_active );
    }
    make_panel_segments(cur);
}

/*
 * int compare_read_altern(char *altern, struct at_disp *data) - Read an
 *	alternative from directory file altern into the side pointed to
 *	by data.
 */
int qMonteAltCmpDlg::compare_read_altern(
					const char *altern, struct at_disp *data ) {
	int i;
    char fname[MAXPATHLEN];


    QComboBox *cmb = (data == &alterns[0]) ? cmbYear1_ : cmbYear2_;

    clearOutput();

    if (data->alt) {
		at_struct_free(data->alt);
        data->alt = NULL;
    }
	cmb->clear();
	data->year = 0;

    /* check to see if the file/folder exists */
	/* since it is a directory, check the file inside */
	sprintf( fname, "%s/%s.out", altern, altern );
	if (access(fname, R_OK) == -1)	
	{
		QString cstr = "Cannot find alternative file ";
		cstr += fname;
		QMessageBox::warning(this, cstr, "Alternative Comparison error",QMessageBox::NoButton,
																		QMessageBox::Ok );
		make_panel_releases( data );
		return 0;
    }

    if (!(data->alt = parse_at_input(fname))) {
		data->year = 0;
		make_panel_releases(data);
		return 0;
    }

	for (i = 1; i <= data->alt->years; i++) {
		sprintf( fname, "%d", i );
		cmb->insertItem( fname, cmb->count() );
	}
    data->year = data->alt->release_year;
    make_panel_releases(data);
    return 1;
}

/* Event reponse routines.
 */
void qMonteAltCmpDlg::cbAltern1(int pos) {
	if (lock_)
		return ;  /* event "handled" */
	lock_ = 1;

	QString cstr;
	int sel;

	if ((sel = cmbAltern1_->currentItem()) >= 0) {
		cstr = cmbAltern1_->text( sel );
		compare_read_altern( cstr.ascii(), &(alterns[0]) );
	}

	lock_ = 0;
	return ;  /* event handled */
}

void qMonteAltCmpDlg::cbYear1(int pos) {
	if (lock_)
		return ;  /* event "handled" */
	lock_ = 1;

	int value = pos;
    struct at_disp *cur = &(alterns[0]);

    if (cur->alt && value >= 0) {
		if (value  > cur->alt->years)
			cur->year = 0;
		else
			cur->year = cur->alt->release_year + value;
		make_panel_releases(cur);

		/* clear the output stuff */
		clearOutput();
	}

	lock_ = 0;
	return ;  /* event handled */
}

void qMonteAltCmpDlg::cbRelease1( int pos ) {
	if (lock_)
		return;  /* event "handled" */
	lock_ = 1;

    struct at_disp *cur = &(alterns[0]);
	//int value = cmbRelease1_-> currentItem();
	int value = pos;

    if (!cur->year) {
		QMessageBox::warning(this, "No active year!","Alternative Comparison",QMessageBox::NoButton,
																		QMessageBox::Ok );	
    }

    /* clear the output stuff */
    clearOutput();
    
    if (!value) {	/* Value 0 corresponds to All Releases. */
		cur->rls = cur->year->releases;
		make_panel_segments(cur);	/* We need a better method than this */
		goto escape;
    }
    value--;	/* Because 0 is all */
    for (cur->rls = cur->year->releases ; value && cur->rls ; value--)
		cur->rls = cur->rls->next;
    if (!cur->rls)
		QMessageBox::warning(this, "No release found!","Alternative Comparison",QMessageBox::NoButton,
																		QMessageBox::Ok );		
    else 
		set_release(cur);
    make_panel_segments(cur);

escape:
	lock_ = 0;
	return;  /* event handled */
}

void qMonteAltCmpDlg::cbPassPoint( int pos ) {
	if (lock_)
		return;  /* event "handled" */
	lock_ = 1;

	//int value = cmbPassPoint_->selection();
	int value = pos;

    if (!alterns[0].rls) {
		QMessageBox::warning(this, "No active release!","Alternative Comparison",QMessageBox::NoButton,
																		QMessageBox::Ok );		
		goto escape;
    }

    /* clear the output stuff */
    clearOutput();

    if (alterns[0].rls->next)	/* All releases selected */
		for (cur_seg = riv_seg_list ; value && cur_seg ; value--)
			cur_seg = cur_seg->next;
    else
		for (cur_seg = alterns[0].rls->river_segment ;
			 value && cur_seg ; value--) {
	    
			cur_seg = cur_seg->next;
		}

	if (!cur_seg) 
		QMessageBox::warning(this, "No current segment!","Alternative Comparison",QMessageBox::NoButton,
																		QMessageBox::Ok );
escape:
 	lock_ = 0;
	return;  /* event handled */
}

void qMonteAltCmpDlg::cbAltern2( int pos ) {
	if (lock_)
		return;  /* event "handled" */
	lock_ = 1;

	QString cstr;
	int sel = pos;

	if (sel  >= 0) {
		cstr = cmbAltern2_->text( sel );
		compare_read_altern( cstr.ascii(), &(alterns[1]) );
	}

	lock_ = 0;
	return;  /* event handled */
}

void qMonteAltCmpDlg::cbYear2( int pos ) {
	if (lock_)
		return;  /* event "handled" */
	lock_ = 1;

	int value = pos;
    struct at_disp *cur = &(alterns[1]);

    if (cur->alt && value >= 0) {
		if (value  > cur->alt->years)
			cur->year = 0;
		else
			cur->year = cur->alt->release_year + value;
		make_panel_releases(cur);

		/* clear the output stuff */
		clearOutput();
	}

	lock_ = 0;
	return;  /* event handled */
}

void qMonteAltCmpDlg::cbRelease2( int pos ) {
	if (lock_)
		return;  /* event "handled" */
	lock_ = 1;

    struct at_disp *cur = &(alterns[1]);
	int value = cmbRelease2_->currentItem();

    if (!cur->year) {
			QMessageBox::warning(this, "No active year!","Alternative Comparison",QMessageBox::NoButton,
																		QMessageBox::Ok );	
    }

    /* clear the output stuff */
    clearOutput();
    
    if (!value) {	/* Value 0 corresponds to All Releases. */
		cur->rls = cur->year->releases;
		make_panel_segments(cur);	/* We need a better method than this */
		goto escape;
    }
    value--;	/* Because 0 is all */
    for (cur->rls = cur->year->releases ; value && cur->rls ; value--)
		cur->rls = cur->rls->next;
    if (!cur->rls)
		QMessageBox::warning(this, "No release found!","Alternative Comparison",QMessageBox::NoButton,
																		QMessageBox::Ok );			
    else 
		set_release(cur);
    make_panel_segments(cur);

escape:
	lock_ = 0;
	return;  /* event handled */
}


void qMonteAltCmpDlg::cbCompute(  ) {
	if (lock_)
		return ;  /* event "handled" */
	lock_ = 1;

    struct summary *ptr;
    struct rls_stats *pass0 = 0;
    struct rls_stats *pass1 = 0;
    float total0, total1;
    int num0, num1, flags;
    int i;
    float *travel0 = 0;
    float *travel1 = 0;
    float *data0, *data1;
    float x;
    float d, prob;
    char line2[256];

    /* make sure that the cur_seg is present in both alternatives. */
    /* GROT */

    /* get and split the summary data for the two alternatives */

    flags = rbSurvival_->isOn() ? AT_CUMUL_FLAG : AT_TRAVEL_FLAG;

    if (!(cur_seg && alterns[0].alt 
	  && (ptr = get_data(alterns[0].alt->games, alterns[0].rls,
	      cur_seg->name, flags, &total0, &num0)))) {

		stOut1_->setText( "Unable to compare alternatives." );
		stOut2_->setText( "No data available." );
		goto escape;
    }

    if (!split_data(ptr, num0, &pass0, &travel0)) {
		for(--num0 ; num0 >= 0 ; --num0)
			free(ptr[num0].info);
		free(ptr);
		stOut1_->setText( "Unable to create comparison arrays!" );
		goto escape;
    }
    for (i = 0 ; i < num0 ; ++i)
		free(ptr[i].info);
    free(ptr);

    if (!(cur_seg && alterns[1].alt
	  && (ptr = get_data(alterns[1].alt->games, alterns[1].rls,
	      cur_seg->name, flags, &total1, &num1)))) {

		stOut1_->setText( "Unable to compare alternatives." );
		stOut2_->setText(  "No data available." );
		free(pass0);
		free(travel0);
		goto escape;
    }

    if (!split_data(ptr, num1, &pass1, &travel1)) {
		stOut1_->setText( "Unable to create histogram arrays!" );
		for (--num1 ; num1 >= 0 ; --num1)
			free(ptr[num1].info);
		free(ptr);
		free(travel0);
		free(pass0);
		goto escape;
    }
    for (i = 0; i < num1 ; ++i)
		free(ptr[i].info);
    free(ptr);

    /* compare num0 and num1 to make sure they are the same */
    if (num1 != num0)
    {
		/* GROT */   // <- ?? what does that mean
	//	log_msg(L_MSG,
	//		"num0 and num1 don't match.  I don't know what that means.\n");
    }
    
    if (flags & AT_TRAVEL_FLAG) {
		data0 = travel0;
		data1 = travel1;
    }
    else {
		free(travel0);
		free(travel1);
		data0 = create_summary(pass0, &total0, num0,
					  &x, &x, &x, &x, &x, &x, &x, flags);
		data1 = create_summary(pass1, &total1, num1,
					  &x, &x, &x, &x, &x, &x, &x, flags);
    }
    free(pass0);
    free(pass1);

    kstwo(data0, num0, data1, num1, &d, &prob);

    /* output the results to the message window */
//    log_msg(L_MSG, "KS test: prob: %f  d: %f\n", prob, d);

    /* write the results to the panel */
	stOut1_->setText(  "Kolmogorov - Smirnov test");
    sprintf(line2, "    probability: %f   d: %f\n", prob, d);
	stOut2_->setText( line2 );

    free(data0);
    free(data1);

escape:
	lock_ = 0;
	return ;  /* event handled */
}

void qMonteAltCmpDlg::cbDone(  ) {
	if (lock_)
		return ;  /* event "handled" */
	lock_ = 1;

	close();

	lock_ = 0;
	return ;  /* event handled */
}

void qMonteAltCmpDlg::cbHelp() {

	helpManager->showHelp( QString("79.html") );
	return;  /* event "handled" */
}

// Monte Carlo tool may prod us when the alternatives change on disk.
void qMonteAltCmpDlg::altsUpdated( void ) {
	cur_seg = NULL;
	riv_seg_list = NULL;

    cmbAltern1_->clear();
    cmbAltern2_->clear();
	cmbYear1_->clear();
	cmbYear2_->clear();
	cmbRelease1_->clear();
	cmbRelease2_->clear();
	cmbPassPoint_->clear();

    // It may happen that the altern list has not been initialized yet.
    if (get_altern_list_size() == 0){
        altern_update_directory();
		//log_msg(0, "alternlist update directory");
	}


    for (reset_altern_list() ; get_current_list_altern(); inc_altern_list()) {
		cmbAltern1_->insertItem( get_current_list_altern(), cmbAltern1_->count() );
		cmbAltern2_->insertItem( get_current_list_altern(), cmbAltern2_->count() );
	}

	clearOutput();

	//set the first two alternatives to be selected:
	if (cmbAltern1_->count() >= 1) cmbAltern1_->setCurrentItem(0);
	if (cmbAltern2_->count() >= 2) cmbAltern2_->setCurrentItem(1);    

	/* initialize selections by calling the event handlers */
	if (cmbAltern1_->count() >= 1) cbAltern1(cmbAltern1_->currentItem());
	if (cmbAltern2_->count() >= 1) cbAltern2(cmbAltern2_->currentItem());
	if (cmbYear1_->count() >= 1) cbYear1(cmbYear1_->currentItem());
	if (cmbRelease1_->count() >= 1) cbRelease1(cmbRelease1_->currentItem());
	if (cmbYear2_->count() >= 1) cbYear2(cmbYear2_->currentItem());
	if (cmbRelease2_->count() >= 1) cbRelease2(cmbRelease2_->currentItem());
	if (cmbPassPoint_->count() >= 1) cbPassPoint(cmbPassPoint_->currentItem());
}

