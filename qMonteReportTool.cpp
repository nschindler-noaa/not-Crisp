/* qInputReportTool.h
 * This class defines a modal dialog which edits transport structures
 *  via comboboxes, scrollbars, and edit boxes. The dialog is based on 
 *  the qDataReportTool class
 *
 * This is a special-purpose dialog which will probably not be subclassed.
 */

#include "qMonteReportTool.h"
#include "resource.h"
#include <stdlib.h>

/* These things are stolen from "report_altern.c" and define the choices to
 *  be offered to the user by this dialog.
 */

extern "C" {
    extern char **get_output_varnames();

    extern struct report_struct *get_output_list ();
    extern struct report_struct *get_output_index ();
    extern struct report_struct *get_output_index2 ();
	extern void *get_altern_year( );

    static struct data_choice choice_list[] =
	{
	    {
		"alterns",
		REPORT_ALTERN,
		REP_FL_NONE,
		get_altern_year,
		get_output_varnames,
		get_output_list,
		get_output_index,
		get_output_index2,
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
     
    extern struct report_var travel_report_vars[];

    static struct report_var *report_var_list[] = {
	travel_report_vars,
	(struct report_var*) 0
    };

    static struct report_info rf_info = {
		"Monte Report Tool",
		NULL,
		choice_list,
		report_var_list
	};

}

qMonteReportTool::qMonteReportTool( QWidget *w )
			: qDataReportTool ( w, &rf_info, 1 )
{
	setCaption( "Monte Report Tool" );
	setHelpPage( 77 );
}


/* Pointer to currently active member of this class, if any.
 */
qMonteReportTool *qMonteReportTool::instance_ = NULL;

qMonteReportTool::~qMonteReportTool()
{
	instance_ = NULL;
}

qMonteReportTool *qMonteReportTool::instance( QWidget *w )
{
	if (instance_ == NULL && w != NULL)
		instance_ = new qMonteReportTool( w );

	return instance_;
}

// Monte Carlo tool may prod us when the alternatives change on disk.
void qMonteReportTool::altsUpdated( void ) {
    updateDialog();
}

