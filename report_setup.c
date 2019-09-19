/** \file
 *
 * Routines to set up the report tools.  These functions create lists to
 * be used when the individual report tools are first brought up, creating
 * the lists of variable names to be used for the tools' list items.
 *
 * Note:Each of the get_xxx_varnames functions dynamically allocate memory
 *	*once* and never free it as long as the program is active.
 */
#include "report.h"
#include "log.h"
#include "strtools.h"
#include <stdlib.h>

/*
 * char **create_names_list(command_struct command_list[]) - Extracts
 *	the list of crisp variable names from the command_list passed
 *	in, dynamically allocating memory for the array of strings as
 *	it goes.
 */
static char **create_names_list(command_list)
	struct report_var command_list[];
{
    char **list;
    int list_size;
    int i;

    for ( list_size = 1 ; command_list[list_size-1].name ; list_size++)
	/* Do nothing */ ;
    list = (char **) malloc(list_size * sizeof(char *));
    if (!list) {
	log_msg(L_ERROR, "Ran out of virtual memory!\n");
	return (char **) 0;
    }
    bzero(list, list_size * sizeof(char *));
    for (i = 0
	 ; i < list_size && command_list[i].name
	 ; i++) {
	list[i /* -nuke_count */] = command_list[i].name;
    }
    list = (char **) realloc(list, (list_size /* - nuke_count */ )
	* sizeof(char *));
    return list;
}

/*
 * char **get_dam_varnames(void) -- Create a list of strings that contains
 *	the names of all of the dam variables (species independent).
 */
char **get_dam_varnames()
{
    extern struct report_var dam_report_vars[];
    static char **dam_varnames = 0;

    return (dam_varnames) ? dam_varnames
	: (dam_varnames = create_names_list(dam_report_vars));
}

/*
 * char **get_dam_spec_varnames(void) -- Create the list of all dam
 *	species variables.
 */
char **get_dam_spec_varnames()
{
    extern struct report_var dam_spec_report_vars[];
    static char **dam_spec_varnames = 0;

    return (dam_spec_varnames) ? dam_spec_varnames
	: (dam_spec_varnames = create_names_list(dam_spec_report_vars));
}

/*
 * char **get_species_varnames(void) -- Create the list of all species
 *	variables.
 */
char **get_species_varnames()
{
    extern struct report_var species_report_vars[];
    static char **species_varnames = 0;

    return (species_varnames) ? species_varnames
	: (species_varnames = create_names_list(species_report_vars));
}

/*
 * char **get_stock_varnames(void) -- Create the list of all stock
 *	variables.
 */
char **get_stock_varnames()
{
    extern struct report_var stock_report_vars[];
    static char **stock_varnames = 0;

    return (stock_varnames) ? stock_varnames
	: (stock_varnames = create_names_list(stock_report_vars));
}

/*
 * char **get_release_varnames(void) -- Create the list of all release
 *	variables.
 */
char **get_release_varnames()
{
    extern struct report_var release_report_vars[];
    static char **release_varnames = 0;

    return (release_varnames) ? release_varnames
	: (release_varnames = create_names_list(release_report_vars));
}

/*
 * char **get_transport_varnames(void) -- Create the list of all transport
 *	variables.
 */
char **get_transport_varnames()
{
    extern struct report_var transport_report_vars[];
    static char **transport_varnames = 0;

    return (transport_varnames) ? transport_varnames
	: (transport_varnames = create_names_list(transport_report_vars));
}

/*
 * char **get_headwater_varnames(void) -- Create the list of all headwater
 *	variables.
 */
char **get_headwater_varnames()
{
    extern struct report_var headwater_report_vars[];
    static char **headwater_varnames = 0;

    return (headwater_varnames) ? headwater_varnames
	: (headwater_varnames = create_names_list(headwater_report_vars));
}

/*
 * char **get_reach_varnames(void) -- Create the list of all reach variables.
 */
char **get_reach_varnames()
{
    extern struct report_var reach_report_vars[];
    static char **reach_varnames = 0;

    return (reach_varnames) ? reach_varnames
	: (reach_varnames = create_names_list(reach_report_vars));
}

/*
 * char **get_reach_spec_varnames(void) -- Create the list of all dam
 *	species variables.
 */
char **get_reach_spec_varnames()
{
    extern struct report_var reach_spec_report_vars[];
    static char **reach_spec_varnames = 0;

    return (reach_spec_varnames) ? reach_spec_varnames
	: (reach_spec_varnames = create_names_list(reach_spec_report_vars));
}

/*
 * char **get_run_varnames(void) -- Create the list of all run variables.
 */
char **get_run_varnames()
{
    extern struct report_var run_report_vars[];
    static char **run_varnames = 0;

    return (run_varnames) ? run_varnames
	: (run_varnames = create_names_list(run_report_vars));
}

/*
 * char **get_output_varnames(void) -- Create the list of all alternative
 *	report variables.
 */
char **get_output_varnames()
{
    extern struct report_var travel_report_vars[];
    static char **output_varnames = 0;

    return (output_varnames) ? output_varnames
	: (output_varnames = create_names_list(travel_report_vars));
}
