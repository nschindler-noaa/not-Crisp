/* \file
 *
 * \brief The structures, enumeration types and defines for the report tool.
 */
#include <stdio.h>
#include "crisp.h"

#ifdef __cplusplus
extern "C" {
#endif

#define REPORT_NUM_HEADER_LINES 5
#define REPORT_STRING_MAX_LEN 1024

#define REPORT_PREFIX_WIDTH	21
#define REPORT_PREFIX_FORMAT	"%21s"	/* right justify */

#define REPORT_TITLE_WIDTH	80
#define REPORT_TITLE_FORMAT	"%-80s"	/* left justify */

/* The maximum width of a report format */
#define MAX_REPORT_WIDTH REPORT_TITLE_WIDTH

#define REPORT_COLUMN_WIDTH 11
#define SPEC_STRING	"%11s"
#define SPEC_FLOAT	"%11.2f"
#define SPEC_INT       	"%11d"

/* definitions for the flags field of the report struct
 */

#define REP_FL_NONE	0x00		/* no flags set */
#define REP_FL_DYNAMIC 	0x01		/* dynamically changing data list */

/*
 * struct report_var is the structure which represents each variable
 * contained in a structure.  For ease of modification, addition, and
 * removal of variables in the report tool, each variable which can
 * be output must have a structure like this defined in the appropriate
 * data area (dams, reaches, releases, etc.) of which the variable is
 * a part.
 *
 * [Editor's note: Yes, it's an ugly solution, but it works.]
 */
struct report_var {
    int vartype;	/* The type of */
    long int offset;	/* The address offset from the base structure to
			 * the individual value we are interested in.
			 */
    char *name;		/* The string name of the variable. */
    int length;		/* The number of elements for arrays */
};

/*
 * struct output_array is a structure giving the output routine information
 * as to how many values from the given array variable to print.
 */
struct output_array {
    int varnum;
    int start;
    int end;
    int varnum2;
    int start2;
    int end2;
};

/*
 * This is the struct attached to the report frame which stores all the
 *	data which may need to be called up by any of the Xview items.
 */
struct report_info {
    char  *title;
    char  *help_hook;
    struct data_choice *choices_list;
    struct report_var **vars_list;
};

/*
 * A structure to represent a report list.  Each report frame list has
 *	one of these structures to keep track of which are set.
 */
struct report_struct {
    int type;
    int number;
    char *title;
    char **items;
    char *set;
};

/*
 * This struct is used to represent a single data category, with information
 *	on all the different parts required for its use.
 */
struct data_choice
{
    char * title;     	/* title to use on the report panel */
    int type;
    unsigned int flags; /* bit field */
    
    /* fcn to return the data pointer, given the location string */
    void *(*get_data_pointer)();

    /* fcn to return a list of strings (usually located in report_setup.c) */
    char **(*get_variables)();

    /* fcns to return report_structs for the menus */
    struct report_struct *(*get_locations)();
    struct report_struct *(*get_index)();
    struct report_struct *(*get_index2)();

    struct report_struct *variables;
};

/*
 * This is the enumeration type which contains all the possible values
 * for a struct report_var vartype--the type of a data variable that we
 * can write to a report.
 *
 * Note that RPT_EOD is just a place holder and does not represent a valid
 * variable type.
 */
enum { RPT_FLOAT, RPT_INT, RPT_DAM_SPEC_FLOAT, RPT_DAM_SPEC_INT,
       RPT_REACH_SPEC_FLOAT, RPT_PERIOD_LIST, RPT_ALT_FLOAT, 
       RPT_RC_FLOAT, RPT_RCEQN_FLOAT, RPT_RCEQN_INT, RPT_EOD };

/*
 * This is the enumeration type which contains all the possible data
 * areas that the report tool can give information for.
 *
 * The order of these is arbitrary, but must match the order of the
 * array of struct report_vars called report_var_list in report_init.c
 *
 * Note that REPORT_EOD is just a place holder and does not represent
 *	a type of report of its own.
 */
enum { REPORT_DAM_SPEC, REPORT_DAM, REPORT_SPECIES, REPORT_RELEASE,
	   REPORT_TRANSPORT, REPORT_HEADWATER, REPORT_REACH_SPEC,
       REPORT_REACH, REPORT_RUN, REPORT_STOCK, REPORT_EOD };

/*
 * This is the alternate enumeration type for use in the alternative
 * report tool.
 */
enum { REPORT_ALTERN };


/*
 * Interesting external definitions.
 */
extern int write_report_header( FILE *filep, struct report_var *var_list,
							char *choices, struct output_array *array_info );

extern int do_report( FILE *fp, struct report_var *var_list, char *choices,
					void *item, int index, struct output_array *array_info );

void altern_report_output_loop( FILE *fp, struct report_var *variables,
				char *vars_set, void *data_ptr, struct output_array *array_info);

#ifdef ROUTE_TRACKING
	extern int calculate_route_avgs( struct rls_stats *rls_data, int num, 
			float *sluiceway_avg, float *byp_avg, float *tur_avg, float *spi_avg, float *rsw_avg, float *tra_avg,
			float *premort_transport_total, float *postmort_transport_total,
			float *fpe, float *sluiceway_in_avg, float *byp_in_avg, float *tur_in_avg, 
			float *spi_in_avg, float *rsw_in_avg, float *pro_in_sum);
#endif



extern char * write_all_report_file( struct report_info *info );

/* report variable global structures */
extern struct report_var reach_spec_report_vars[];
extern struct report_var dam_spec_report_vars[];
extern struct report_var dam_report_vars[];
extern struct report_var species_report_vars[];
extern struct report_var stock_report_vars[];
extern struct report_var release_report_vars[];
extern struct report_var transport_report_vars[];
extern struct report_var headwater_report_vars[];
extern struct report_var reach_report_vars[];
extern struct report_var run_report_vars[];
extern struct report_var travel_report_vars[];

#ifdef __cplusplus
}
#endif

