/* qInputReportTool.h
 * This class defines a modal dialog which edits transport structures
 *  via comboboxes, scrollbars, and edit boxes. The dialog is based on 
 *  the IDD_TRANSPORTTOOL resource template.
 *
 * This is a special-purpose dialog which will probably not be subclassed.
 */

#include "qInputReportTool.h"
#include "resource.h"
#include <stdlib.h>

/* These things are stolen from "report_tool.c" and define the choices to
 *  be offered to the user by this dialog.
 */

extern "C" {
	/* In report_tool.c */
	extern void *get_dam_segment( );
	extern void *get_reach_segment( );
	extern void *get_headwater_segment( );
	extern void *get_run( );
	extern void *get_species_by_name( );
	extern void *get_stock_by_name( );
	extern void *get_transport_segment( );

    extern struct report_struct *get_dam_spec_list ();
    extern struct report_struct *get_dam_list ();
    extern struct report_struct *get_headwater_list ();
    extern struct report_struct *get_species_list ();
    extern struct report_struct *get_species_index ();
    extern struct report_struct *get_stock_list ();
    extern struct report_struct *get_stock_index ();
    extern struct report_struct *get_reach_list ();
    extern struct report_struct *get_reach_spec_list ();
    extern struct report_struct *get_transport_list ();

    /* in report_setup.c */
    extern char ** get_dam_spec_varnames ();
    extern char ** get_dam_varnames ();
    extern char ** get_headwater_varnames ();
    extern char ** get_reach_varnames ();
    extern char ** get_reach_spec_varnames ();
    extern char ** get_release_varnames ();
    extern char ** get_run_varnames ();
    extern char ** get_species_varnames ();
    extern char ** get_stock_varnames ();
    extern char ** get_transport_varnames ();

    static struct data_choice choice_list[] =
	{
	    {
		"dam/spec",
		REPORT_DAM_SPEC,
		REP_FL_NONE,
		get_dam_segment,
		get_dam_spec_varnames,
		get_dam_spec_list,
		get_species_index,
		NULL,
		NULL},
	    {
		"dam",
		REPORT_DAM,
		REP_FL_NONE,
		get_dam_segment,
		get_dam_varnames,
		get_dam_list,
		NULL,
		NULL,
		NULL},
	    {
		"headwater",
		REPORT_HEADWATER,
		REP_FL_NONE,
		get_headwater_segment,
		get_headwater_varnames,
		get_headwater_list,
		NULL,
		NULL,
		NULL},
	    {
		"reservoir/spec",
		REPORT_REACH_SPEC,
		REP_FL_NONE,
		get_reach_segment,
		get_reach_spec_varnames,
		get_reach_spec_list,
		get_species_index,
		NULL,
		NULL},
	    {
		"reservoir",
		REPORT_REACH,
		REP_FL_NONE,
		get_reach_segment,
		get_reach_varnames,
		get_reach_list,
		NULL,
		NULL,
		NULL},
	    {
		"global",
		REPORT_RUN,
		REP_FL_NONE,
		get_run,
		get_run_varnames,
		NULL,
		NULL,
		NULL,
		NULL},
	    {
		"species",
		REPORT_SPECIES,
		REP_FL_NONE,
		get_species_by_name,
		get_species_varnames,
		get_species_list,
		NULL,
		NULL,
		NULL},
	    {
		"stock",
		REPORT_STOCK,
		REP_FL_NONE,
		get_stock_by_name,
		get_stock_varnames,
		get_stock_list,
		NULL,
		NULL,
		NULL},
	    {
		"transport",
		REPORT_TRANSPORT,
		REP_FL_DYNAMIC,
		get_transport_segment,
		get_transport_varnames,
		get_transport_list,
		NULL,
		NULL,
		NULL},
	    {
		NULL,
		0,
		0,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL},
	};
     
extern struct report_var dam_spec_report_vars[];
extern struct report_var dam_report_vars[];
extern struct report_var species_report_vars[];
extern struct report_var stock_report_vars[];
extern struct report_var release_report_vars[];
extern struct report_var transport_report_vars[];
extern struct report_var headwater_report_vars[];
extern struct report_var reach_report_vars[];
extern struct report_var reach_spec_report_vars[];
extern struct report_var run_report_vars[];

/* The order of this array is significant, and must match the order of
 * the enumeration type specifying possible report types.
 * (i.e. if REPORT_DAM is the first enumeration value, the dam_report_vars
 *  item must be the first value in this array.)
 */

    static struct report_var *report_var_list[] = {
	dam_spec_report_vars,
	dam_report_vars,
	species_report_vars,
	release_report_vars,
	transport_report_vars,
	headwater_report_vars,
	reach_spec_report_vars,
	reach_report_vars,
	run_report_vars,
    stock_report_vars,
	(struct report_var *) 0
    };

    static struct report_info rf_info = {
		"Input Report Tool",
		NULL,
		choice_list,
		report_var_list
	};

}

qInputReportTool::qInputReportTool( QWidget *w )
			: qDataReportTool ( w, &rf_info )
{
	setCaption( "Input Report Tool" );
	setHelpPage( 78 );
}


/* Pointer to currently active member of this class, if any.
 */
qInputReportTool *qInputReportTool::instance_ = NULL;

qInputReportTool::~qInputReportTool()
{
	instance_ = NULL;
}

qInputReportTool *qInputReportTool::instance( QWidget *w )
{
	if (instance_ == NULL)
		instance_ = new qInputReportTool( w );

	return instance_;
}

