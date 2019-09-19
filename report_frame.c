/** \file
 *
 * GUI functions have been moved to ZatDataReportTool.cpp - some utility
 *  functions remain here.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef WIN32
#include <direct.h>
#include <io.h>
#else
#include <strings.h>
#include <sys/types.h>
#include <sys/stat.h>
#endif

#include "crisp.h"
#include "log.h"
#include "report.h"
#include "writeutil.h"
#include "river.h"

/* The number of rows in each list item */
#define REPORT_NUM_ROWS		17

/*-------------------------------------------------------------*/

/*=============================================================*/
/* Support and setup routines 
 */
/*=============================================================*/
struct report_struct *get_var_list(type, get_proc)
    int type;
    char ** (*get_proc)();
{
    struct report_struct *var_list=0;
    int count;

    var_list =
	(struct report_struct *) malloc(sizeof(struct report_struct));
    bzero(var_list, sizeof(struct report_struct));

    var_list->type = type;
    var_list->items = (*get_proc)();
    for (count = 0; var_list->items[count] ; count ++);

    var_list->set = (char *) malloc(sizeof(char) * count);
    bzero(var_list->set, sizeof(char) * count);

    var_list->number=count;
    
    return(var_list);
}

/*=============================================================*/
/* output routines
 */
/*=============================================================*/

/*
 * struct report_struct *get_next_var_list (struct report_struct *list,
 *	struct output_array *array_info) -- This function returns a [static]
 *	struct report_struct pointer containing the next variables from list
 *	that will fit on a line of a length set in the line length panel item.
 *	list is the incoming report struct that we are incrementally working
 *	on.  Array_info is an output variable that we write to when one of
 *	our array variables has to be broken into parts.
 *
 *	Note that array_info must be preserved between calls to this function,
 *	as the old values are used to determine the new values in some cases.
 *
 *	It returns NULL if all the variables in list have been "used" --
 *	that is if a structure has been returned containing all active
 *	variables in list.
 *
 *	Note that this function is uglified, because we have to deal with
 *	some variables that actually represent arrays.
 */
/* KLUDGE - pathological data connection for passing in desired line length
 */
int g_report_line_length = 80;

struct report_struct *get_next_var_list(info, list, array_info)
	struct report_info *info;
	struct report_struct *list;
	struct output_array *array_info;
{
    /* Under Unix, statics are guaranteed to be zeroed! */
    static struct report_struct last_list;

	/* TODO */
	int line_length = g_report_line_length;

    /* const */ int num_vars = (line_length - REPORT_PREFIX_WIDTH)
	/ REPORT_COLUMN_WIDTH;

    int count;
    int i;

    if (!last_list.set) {

	/* We are starting a new list */
	bcopy(list, &last_list, sizeof(struct report_struct));

	/* last_list.set is Freed at the end of this function, on a call
	 * in which we run out of variables.  (Guaranteed to occur by the
	 * calling function.)
         */
	last_list.set = (char *) malloc(last_list.number * sizeof(char));
	if (!last_list.set) {
	    log_msg(L_ERROR, "Out of virtual memory creating report!\n");
	    return 0;
	}
	bzero(last_list.set, last_list.number * sizeof(char));
	count = 0;	/* Start at the beginning to check for set vars. */

    } else {

	/* Check to see what we set last time.
	 * count will store the number *after* the last var set last time
	 * this function was called for the current list.
	 */
	for (count = -1, i = 0 ; i < last_list.number ; i++ ) {
	    if (last_list.set[i]) {
		count = i + 1;
		if ((i == array_info->varnum && array_info->end
		     != info->vars_list[list->type][i].length)
		    || (i == array_info->varnum2 && array_info->end2
			!= info->vars_list[list->type][i].length)) {
		    count = i;
		}
	    }
	    /* Found one unset after we've set at least one */
	    else if (count > -1 && list->set[i]) {
		break;
	    }
	}
	bzero(last_list.set, last_list.number * sizeof(char));
    }

    /* Count should now store the first number we have to check for */
    i = count;
    count = 0;
    for ( ; i < last_list.number && count < num_vars ; i++) {
	if (list->set[i]) {
        /* First, hack the length of arrays whose length varies at
         *  run time.
         */
        if (info->vars_list[list->type][i].vartype == RPT_RC_FLOAT ||
                info->vars_list[list->type][i].vartype == RPT_RCEQN_FLOAT ||
                info->vars_list[list->type][i].vartype == RPT_RCEQN_INT)
            info->vars_list[list->type][i].length = num_reach_classes;

        /* Then, decide how many items to put on each line.
         */
	    if (info->vars_list[list->type][i].length > 1) {
		if (i == array_info->varnum) {
		    array_info->start = array_info->end;
		    array_info->end = (info->vars_list[list->type][i].length
			- array_info->start > num_vars)
			? array_info->start + num_vars
			: info->vars_list[list->type][i].length;
		    count += array_info->end - array_info->start;
		} else if (i == array_info->varnum2) { /* i != ai->varnum */
		    array_info->varnum = array_info->varnum2;
		    array_info->varnum2 = -1;
		    array_info->start = array_info->end2;
		    array_info->end = (info->vars_list[list->type][i].length
			- array_info->start > num_vars)
			? array_info->start + num_vars
		        : info->vars_list[list->type][i].length;
		    count += array_info->end - array_info->start;
		} else { /* i != ai->varnum or varnum2 */
		    if (num_vars - count
			< info->vars_list[list->type][i].length) {
			if (array_info->varnum >= 0
			    && last_list.set[array_info->varnum]) {
			    array_info->varnum2 = i;
			    array_info->start2 = 0;
			    array_info->end2 = num_vars - count;
			} else {
			    array_info->varnum = i;
			    array_info->start = 0;
			    array_info->end = num_vars - count;
			}
			count = num_vars;
		    } else {
			count += info->vars_list[list->type][i].length;
		    }
		}
	    } else {		/* !list->set[i] */
		if (info->vars_list[list->type][i].length == 1) ++count;
		else {		/* length must == 0 */
		    if (count) break;	/* If any variables are set, wait */
		    else count = num_vars;	/* Only this variable */
		}
	    }
	    last_list.set[i] = TRUE;
	}
    }

    /* If we set any variables, return the list, else return NULL */
    if (count) return &last_list;
    else {
	/* last_list.set == 0 is the signal that we are starting over */
	free(last_list.set);
	last_list.set = 0;
	return 0;
    }
}

/*
 * void write_data(FILE *outfile, struct report_info *info, int choice) -
 *	This function has gotten to be too big, and could use paring.
 *	The function that handles looping over all selected variables
 *	for output.
 */
static void write_data(outfile, info, choice)
	FILE *outfile;
	struct report_info *info;
	int choice;
{
    extern void free_at_struct_results();

    int offset, count;
    char title[MAX_REPORT_WIDTH+1];
    struct output_array array_info;

    /* The original variable list, taken from choice_list[choice] */
    struct report_struct *save_vars;

    /* The current (incremental) variable list, length less than
     * the current line_length */
    struct report_struct *var_list;
    struct report_struct *loc_list, *index_list, *index2_list;

    /* get the lists for the choice, They are already created at this
     * point. */
    if (info->choices_list[choice].variables)
	save_vars = info->choices_list[choice].variables;
    else
    {
	/* if this doesn't exist, then the user hasn't set anything for
	 * this list of variables */
	log_msg(L_MSG, "No variables set!\n");
	return;
    }

	if (info->choices_list[choice].get_locations &&
		!(*info->choices_list[choice].get_locations)()) {
		log_msg( L_MSG, "No locations set!\n" );
		return;
	}

    /* This initialization is required for get_next_var_list */
    array_info.varnum = array_info.varnum2 = -1;

    /* Cycle through the list incrementally, one line at a time. */
    for (var_list = get_next_var_list(info, save_vars, &array_info)
	 ; var_list
	 ; var_list = get_next_var_list(info, save_vars, &array_info)) {

	if (!write_report_header(outfile, info->vars_list[var_list->type],
		var_list->set, &array_info)) {
	    log_msg(L_WARNING, "report tool (report_header).  %s.\n",
		    "Something might be wrong");
	}

	if (!info->choices_list[choice].get_locations) {
	    /* call the report fcn with the known data */
	    fprintf(outfile, REPORT_PREFIX_FORMAT,
		    info->choices_list[choice].title);
	    do_report(outfile,
		    info->vars_list[var_list->type],
		    var_list->set,
		    (*info->choices_list[choice].get_data_pointer)(""),
		    /* offset */ 0,
		    &array_info);
	} else {
	    loc_list = (*info->choices_list[choice].get_locations)();

	    if (info->choices_list[choice].get_index) {
		index_list = (*info->choices_list[choice].get_index)();

		if (info->choices_list[choice].get_index2)
		    index2_list = (*info->choices_list[choice].get_index2)();
		else index2_list = 0;
	    }
	    else index2_list = index_list = 0;

	    /* add a blank line */
	    putc('\n', outfile);

	    /* call output routine */
	    for (count=0; count < loc_list->number; count++) {
		if (loc_list->set[count]) {
		    /* Just in case the item is too wide */
		    strncpy(title, loc_list->items[count], (index_list)
			    ? REPORT_TITLE_WIDTH : REPORT_PREFIX_WIDTH);
		    title[(index_list) ? REPORT_TITLE_WIDTH
			  : REPORT_PREFIX_WIDTH]='\0';
		    (void) fprintf(outfile, (index_list) ? REPORT_TITLE_FORMAT
				   : REPORT_PREFIX_FORMAT, title);

		    if (index_list) {
			(void) putc('\n', outfile);

			if (index2_list) {
			    /* Not a general solution! if more 4 index
			     * lists are added, a better solution will
			     * need to be used!  This solution is only
			     * effective for the alternative report tool,
			     * hence the function call.
 			     */

			    altern_report_output_loop(outfile,
				info->vars_list[var_list->type],
				var_list->set,
				(*info->choices_list[choice]
				 .get_data_pointer) (loc_list->items[count]),
				&array_info);
			    loc_list =
				(*info->choices_list[choice].get_locations)();
			} else {
			    for(offset=0;offset < index_list->number;++offset)
			    	if (index_list->set[offset]) {
				    /* This stuff in case name is too wide */
				    /* Size is guaranteed to be REPORT_PREFIX
				     * because of the enclosing else statement
				     * only.
				     */
				    strncpy(title, index_list->items[offset],
					    REPORT_PREFIX_WIDTH);
				    title[REPORT_PREFIX_WIDTH] = '\0';
				    (void) fprintf(outfile,
						   REPORT_PREFIX_FORMAT,
						   title);

				    do_report(outfile,
				            info->vars_list[var_list->type],
				            var_list->set,
				            (*info->choices_list[choice]
					     .get_data_pointer)
					    (loc_list->items[count]),
					    offset, &array_info);
				}
			}
		    } else do_report(outfile,
			info->vars_list[var_list->type],
			var_list->set,
			(*info->choices_list[choice].get_data_pointer)
				(loc_list->items[count]),
				/* offset */0, &array_info);
		    }
	    }
	}
    }
    free_at_struct_results();
}

#define REPORT_FILE_MODE 0644
/*
 * char *write_current_report_file(struct report_info *info) - Writes
 *	the report for the current category. NOTE that this is being hijacked
 *  to write reports for the Windows version as well.
 */
char * write_current_report_file(info, current_choice)
	struct report_info *info;
	int current_choice;
{
    FILE *outfile;
    char *templ;

    /* figure out which record we are dealing with */
#ifndef WIN32
    int fd=0;
    struct stat sbuf;
    int mask;
#endif

    /* open a temporary file */
    templ = (char *) malloc (sizeof(char) * 30);
#ifndef WIN32
    strcpy (templ, "/tmp/crispXXXXXX");
    if ((fd = mkstemp(templ)) == -1)
#else
    strcpy (templ, "crispXXXXXX");
    if (mktemp(templ) == NULL)
#endif
    {
	log_msg(L_ERROR,
		"Couldn't open a temporary file in the report tool.\n");
	return (char *) NULL;
    }

#ifndef WIN32
    mask = umask(-1);
    (void) umask(mask);

    if (fstat(fd, &sbuf) == 0 && sbuf.st_mode != (REPORT_FILE_MODE & ~mask))
	fchmod(fd, REPORT_FILE_MODE & ~mask);

    outfile = fdopen (fd, "w+");
#else
    outfile = fopen (templ, "w+");
#endif

    /* print a REAL header */
    write_header(outfile, "Report Data", "none" );

    /* output data */
    write_data(outfile, info, current_choice);

    fclose(outfile);
    return (templ);
}


/*=============================================================*/
/* frame management routines
 */
/*=============================================================*/

/*
 * int report_variables_set(struct report_info *info, int choice) -- Returns
 *	1 if the variables report_struct indexed in choice_list by choice has
 *	*any* variables set, or 0 if no variables are set for the given
 *	report_struct.
 */

static int report_variables_set(info, choice)
	struct report_info *info;
	int choice;
{
    struct report_struct *vars;
    int i;

    if (!(vars = info->choices_list[choice].variables)) return 0;
    for ( i = 0 ; i < vars->number ; i++ ) {
	if (vars->set[i]) break;
    }
    return (i != vars->number);
}

/*
 * report_categories_set(struct report_info *info, int choice) -- Same as
 *	above, only checks categories, and a data category which requires
 *	no categories returns -1;
 */
static int report_categories_set(info, choice)
	struct report_info *info;
	int choice;
{
    struct report_struct *(*cat_func)();
    struct report_struct *cats;
    int i;

    /* If cat_func is NULL, data category doesn't require locations */
    if (!(cat_func = info->choices_list[choice].get_locations)) return -1;
    if (!(cats = (*cat_func)())) return 0;
    for ( i = 0 ; i < cats->number ; i++) {
	if (cats->set[i]) break;
    }
    return (i != cats->number);
}

/*
 * char *write_all_report_file(struct report_info *info) - Goes through all
 *	categories, writing output for each in turn.
 */
char * write_all_report_file(info)
	struct report_info *info;
{
    int fd=0;
	int count;
    FILE *outfile;
    char *templ;

    /* open a temporary file */
    templ = (char *) malloc (sizeof(char) * 30);
#ifndef WIN32
    strcpy (templ, "/tmp/crispXXXXXX");
    if ((fd = mkstemp(templ)) == -1)
#else
    strcpy (templ, "crispXXXXXX");
    if (_mktemp(templ) == NULL)
#endif
	{
	log_msg(L_ERROR,
		"Couldn't open a temporary file in the report stuff.\n");
	return (char *) NULL;
    }
#ifdef SOLARIS2
    outfile = fdopen (fd, "w+");
#else
   outfile = fopen (templ, "w+");
#endif

    /* print a REAL header */
    write_header(outfile, "Report Data", "none");

    /* output data */
    for (count=0; info->choices_list[count].title; count++)
	if (report_variables_set(info, count)
	    && report_categories_set(info, count))
	    write_data(outfile, info, count);

    fclose(outfile);
    return (templ);
}

