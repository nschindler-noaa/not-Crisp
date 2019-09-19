/** \file
 * \brief Report functions specific to the alternative report tool.
 *
 * This module
 * uses the genericized report frame to create a tabular report of monte
 * carlo alternative output data for specified alternatives living in the
 * current directory.  The main incompatabilities between the alternative
 * report and the generic report tool are that calculations must be made
 * at the time of report generation, so multiple additions must be made
 * when a variable is added, and the addition of a fourth list item.
 *
 *	When adding a new alternative report variable, an entry must be
 * added to the struct alt_data (in alt_report.h) and since we never
 * output these variables as we do input data, the text name of each
 * variable must also be added there; the usual entry in "report_init.c"
 * must be added; and the appropriate calculation routines must be called
 * in the function accumulate_data() in this file.
 *
 *	Information on the generic report frame on which this tool is built
 * can be found in report_frame.c
 *
 *	Because there is a great deal of calculation involved in both the
 * initialization (reading and parsing all available alternatives found in
 * the current directory) and run (summarizing the output data for all
 * selected alternatives) phases of the alternative report, effort is taken
 * to ensure that recalculation is performed only when necessary.
 *
 *	Although some care is taken in this regard, there is yet room for
 * improvement.  The initialization of the alternative report tool causes
 * the tool to read the current directory, looking for directories which
 * contain a file with the name directoryname.out.  These files are assumed
 * to contain monte carlo alternative output data, and the monte index file
 * "directoryname.ind" in each directory is read and parsed into a static
 * pointer to a dynamically allocated array of at_struct pointers.  As it
 * does this, it stores the last changed time of the
 * current directory, which it later checks against to determine if a
 * reinitialization is required.  (Maximum safety would maintain a list of
 * the change time of all alternative output data files.)
 *
 *	Various stages of initialization require the creation of lists of
 * viable alternative-years, releases, and segments from these structures.
 * These lists are also kept around, and freed and reinitialized when the
 * list_alternatives() function demands an update.  These functions all
 * maintain static pointers which are freed and zeroed by the
 * clear_report_alternative_statics() function, which is called in
 * list_alternatives() when a reinitialization is performed.
 *
 *	The data tabulation is done in process_year(), called in the
 * wrapper function get_altern_year(), which maintains a static pointer
 * to the last altern-year data, reusing it if applicable, freeing the
 * pointer and calculating the new data if not.  This function is called
 * whenever the report frame requires data for a specific altern-year,
 * which may, unfortunately require some recalculation for short line
 * lengths, as the report frame tends to labor under the assumption that
 * the data values returned are those that linger, rather than those
 * which are calculated.
 */

#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#ifdef WIN32
#include <stdio.h>
#include <time.h>
#include <io.h>
#else
#include <sys/times.h>
#include <unistd.h>
#endif
#include <errno.h>
#include "crisp.h"
#include "log.h"
#include "report.h"
#include "struct_at.h"
#include "alt_report.h"
#include "monte_index.h"
#include "tree.h"

#ifndef R_OK
#define R_OK  4   /* needed by PC */
#endif

int do_report( FILE *fp, struct report_var *var_list,
    char *choices, void *item, int index,
    struct output_array *array_info );
extern int split_data(struct summary *in_ptr,
	int size, struct rls_stats **out_ptr, float **out_alt );


int altern_exists( char *altern );

/** A guess at the initial number of alternatives to be found in the
 * current directory
 */
#define LIST_SIZE_GUESS 10

/** The maximum length of an alternative name.  Alternatives with longer
 * names than this will have undefined consequences (and will likely
 * cause the tool to dump core).
 */
#define ALTERN_MAX_SIZE 124

/** The number of characters by which the release species follows the
 * release location in the release_list stings.
 */
#define RLS_STRING_ADD	1

/* Variables which will need to be updated any time our list of
 * valid alternatives changes (dealt with in list_monte_indices)
 *
 * We keep these around because they are expensive to calculate.
 */
static struct monte_index **report_list = 0;
static struct at_struct **alt_list = 0;
static struct report_struct *altern_output_list = 0;
static struct report_struct *altern_output_index = 0;
static struct report_struct *altern_output_index2 = 0;
static char **altern_releases = 0;
static char **altern_segments = 0;

/**
 * \brief Free any data stored in
 *	the statics we keep around to avoid massive recalculation.
 *	alt_list requires that report_list be freed after the call
 *	to this function. 
 */
static void clear_report_alternative_statics()
{
    extern void monte_index_free();

    if (alt_list) {
	int i;

	for (i = 0 ; report_list[i] ; ++i) {
	    if (alt_list[i])
		free(alt_list[i]);
	}
	free(alt_list);
	alt_list = 0;
    }

    if (report_list) {
	int i;

	for (i = 0 ; report_list[i] ; ++i)
	    monte_index_free(report_list[i]);
	free(report_list);
	report_list = 0;
    }

    if (altern_output_list) {
	free(altern_output_list->items);
	free(altern_output_list->set);
	free(altern_output_list);
	altern_output_list = 0;
    }
    if (altern_output_index) {
	/* items is not freed here because it is actually just
	 * the altern_releases pointer below
	 */
	free(altern_output_index->set);
	free(altern_output_index);
	altern_output_index = 0;
    }
    if (altern_output_index2) {
	/* items is not freed here because it is actually just
	 * the altern_segments pointer below
	 */
	free(altern_output_index2->set);
	free(altern_output_index2);
	altern_output_index2 = 0;
    }
    if (altern_releases) {
	int i;

	for (i = 0 ; altern_releases[i] ; ++i)
	    free(altern_releases[i]);
	free(altern_releases);
	altern_releases = 0;
    }
    if (altern_segments) {
	int i;

	for (i = 0 ; altern_segments[i] ; ++i)
	    free(altern_segments[i]);
	free(altern_segments);
	altern_segments = 0;
    }
}

/**
 * \brief Updates the report_list
 *	for the named alternative.
 *
 * Returns 1 on successful completion, 0 if a memory allocation
 * fails, and -1 if it can't read the index file.
 */
int report_update_altern(name)
	char *name;
{
    extern struct monte_index *parse_monte_index();
    extern void set_current_altern_list_flag();
    extern char *monte_index_filename();
    extern void at_struct_free();
    extern void monte_index_free();

    int altern_index;
    struct monte_index *read_struct;

    log_msg(L_DBGMSG, "Report tool: Updating alternative %s\n", name);

    if (access(monte_index_filename(name), R_OK) < 0) {
	if (errno == ENOENT || errno == EACCES) {
	    log_msg(L_MSG, "Creating index file for alternative %s\n",
		    name);
	    if (make_monte_index_from_output(name) != 0) {
		log_msg(L_MSG, "Can't create monte index file for %s\n",
			name);
	    }
	} else {
	    log_msg(L_WARNING,
		    "Error reading index file for alternative %s, error %s\n",
		    name, strerror(errno));
	}
    }
    if (!((read_struct = parse_monte_index(monte_index_filename(name)))
	  && read_struct->name)) {

	/* Can't read the index */

	log_msg(L_MSG,
		"Alternative %s %s, removing from list.\n", name,
		read_struct ? "contains no recognizable data"
		: "not accessible");

	if (read_struct)
	    monte_index_free(read_struct);

	if (altern_exists(name)) {
	    /* remove from the report list */

	    int altern_index;

	    altern_index = get_current_list_item_number();

	    if (report_list && report_list[altern_index])
		monte_index_free(report_list[altern_index]);

	    if (alt_list && alt_list[altern_index])
		at_struct_free(alt_list[altern_index]);

	    if (altern_list_remove(name) && report_list) {
		struct monte_index **new_list;
		struct at_struct **new_at_list;
		int size;

		size = get_altern_list_size();
		new_list = (struct monte_index **) malloc((size+1)
			* sizeof(struct monte_index *));
		new_at_list = (struct at_struct **) malloc((size+1)
			* sizeof(struct at_struct *));

		if (!(new_list && new_at_list)) {
		    log_msg(L_ERROR, "Out of memory!\n");
		    if (new_list)
			free(new_list);
		    return 0;
		}

		if (altern_index > 0) {
		    bcopy(report_list, new_list, altern_index
			  * sizeof(struct monte_index *));
		    bcopy(alt_list, new_at_list, altern_index
			  * sizeof(struct at_struct *));
		}

		if (altern_index < size) {
		    bcopy(&report_list[altern_index+1],
			  &new_list[altern_index],
			  (size-altern_index) * sizeof(struct monte_index *));
		    bcopy(&alt_list[altern_index+1],
			  &new_at_list[altern_index],
			  (size-altern_index) * sizeof(struct monte_index *));
		}

		new_list[size] = 0;
		new_at_list[size] = 0;
		free(report_list);
		free(alt_list);
		report_list = new_list;
		alt_list = new_at_list;
	    }
	}
	return -1;
    }

    if (!report_list) {
	report_list = (struct monte_index **) malloc((get_altern_list_size()+1)
		* sizeof(struct monte_index *));
	alt_list = (struct at_struct **) malloc((get_altern_list_size()+1)
		* sizeof(struct at_struct *));
	if (!(report_list && alt_list)) {
	    log_msg(L_ERROR, "Out of memory!\n");
	    if (report_list) free(report_list);
	    return 0;
	}
	bzero(report_list, (get_altern_list_size() + 1)
	      * sizeof(struct monte_index *));
	bzero(alt_list, (get_altern_list_size() + 1)
	      * sizeof(struct at_struct *));
    }

    if (!altern_exists(name)) {
	struct monte_index **new_list;
	struct at_struct **new_at_list;

	if (!altern_list_add(name)) {
	    monte_index_free(read_struct);
	    return 0;
	}

	altern_index = get_current_list_item_number();

	new_list = (struct monte_index **) malloc((get_altern_list_size() + 1)
            * sizeof(struct monte_index *));
	new_at_list = (struct at_struct **) malloc((get_altern_list_size()+1)
	    * sizeof(struct at_struct *));

	if (!(new_list && new_at_list)) {
	    log_msg(L_ERROR, "Out of memory!\n");
	    if (new_list) free(new_list);
	    return 0;
	}

	if (altern_index) {
	    bcopy(report_list, new_list, altern_index
		  * sizeof(struct monte_index *));
	    bcopy(alt_list, new_at_list, altern_index
		  * sizeof(struct at_struct *));
	}

	/* new_list [altern_index] is handled after this section */

	if (altern_index < get_altern_list_size() - 1) {
	    bcopy(&report_list[altern_index], &new_list[altern_index + 1],
		  (get_altern_list_size() - altern_index) /* copy the NULL */
		  * sizeof(struct monte_index *));
	    bcopy(&alt_list[altern_index], &new_at_list[altern_index + 1],
		  (get_altern_list_size() - altern_index)
		  * sizeof(struct at_struct *));
	}

	free(report_list);
	report_list = new_list;
	free(alt_list);
	alt_list = new_at_list;
    } else {
	altern_index = get_current_list_item_number();
	if (report_list[altern_index])
	    monte_index_free(report_list[altern_index]);
	if (alt_list[altern_index])
	    at_struct_free(alt_list[altern_index]);
    }

    set_current_altern_list_flag(0);
    report_list[altern_index] = read_struct;
    alt_list[altern_index] = (struct at_struct *) 0;
    return 1;
}

/**
 * \brief Updates the alternatives in the
 *	current directory, parsing alternatives found there.
 */
int report_update_directory()
{
    extern char *get_current_list_altern();
    extern void reset_altern_list();
    extern void inc_altern_list();
    extern void empty_altern_list();

    char *altern_name;
    char *last_altern;
    int result;

    log_msg(L_DBGMSG,"Report tool: updating all files in current directory\n");

    if (report_list)
	clear_report_alternative_statics();

    if (!altern_update_directory()) return 0;

    if (get_altern_list_size() == 0) {
	log_msg(L_WARNING,"No alternatives found in current directory!\n");
	return 0;
    }

    last_altern = 0;
    for (reset_altern_list() ; (altern_name = get_current_list_altern()) != NULL
	 ; inc_altern_list()) {
	if ((result = report_update_altern(altern_name)) == 0)
	    return 0;
	else if (result == -1) {
	    /* Current item has been deleted... reset list pointer */
	    if (!(last_altern
		  && altern_exists(last_altern)))
		/* altern_exists sets the current pointer, if it exists
		 * otherwise, we have to reset.
		 */
		reset_altern_list();
	} else
	    last_altern = altern_name;
    }

    return 1;
}

static time_t dir_update = 0;
static time_t file_change_time = 0;

/**
 * \brief List the Monte Carlo indexes 
 */
static struct monte_index **list_monte_indices()
{
    char *altern_name;
    clock_t initial_time;
    clock_t compare_time;
#ifdef WIN32
	initial_time = clock();
#else
    long clock_tick;
    struct tms process_initial_time;
    struct tms process_compare_time;

    initial_time = times(&process_initial_time);
#endif
    if (working_directory_modified(&dir_update) || !report_list) {
	if (!report_update_directory()) return 0;

	set_altern_updated_flag(&file_change_time);
    } else {
	int result;

	if ((result = altern_files_modified(&file_change_time)) > 0) {
	    for (reset_altern_list()
		 ; (altern_name = get_current_list_altern()) != NULL
		 ; inc_altern_list()) {
		if (get_current_altern_list_flag())
		    report_update_altern(altern_name);
	    }
	}
	else if (result == -1) {
	    if (!report_update_directory())
		return 0;

	    set_altern_updated_flag(&file_change_time);
	}
    }
#ifdef WIN32
	compare_time = clock();
	log_msg(L_DBGMSG, "%s: %s: %.2f sec\n",
		"Index read", "Process time",
		(float) (compare_time - initial_time) / CLOCKS_PER_SEC);
#else
    compare_time = times(&process_compare_time);
    if ((clock_tick = sysconf(_SC_CLK_TCK)) > 0) {
	log_msg(L_DBGMSG, "%s: %s: %.2f sec, %s: %.2f sec, %s: %.2f sec\n",
		"Index read", "Wall clock time",
		(float) (compare_time - initial_time) / clock_tick,
		"User time", (float) (process_compare_time.tms_utime
			- process_initial_time.tms_utime) / clock_tick,
		"Sys time", (float) (process_compare_time.tms_stime
			- process_initial_time.tms_stime) / clock_tick);
    }
#endif
    return report_list;
}

/**
 * \brief A function to turn the
 *	altern list generated by list_monte_indices() into a dynamically
 *	allocated report struct containing as its string list items of
 *	the form "alternativename, year n", where alternativename is the
 *	name of the alternative, the ', year' represents a character
 *	string, and n is an integer.
 */
static struct report_struct *alternyear_list()
{
/* This define assumes that the maximum length of a year is 3 digits. */
#define CONST_SIZE 18

    struct report_struct *alloc_struct;
    struct monte_index **index_list;
    int i, j, count;

    if (!(index_list = list_monte_indices()))
	return 0;

    /* Freed in list_monte_indices if necessary, otherwise hangs around
     * to save cycles.
     */
    alloc_struct = (struct report_struct *)
	malloc(sizeof(struct report_struct));
    if (!alloc_struct) {
	log_msg(L_ERROR, "Out of memory in alternyear_list!\n");
	return 0;
    }

    alloc_struct->title = "Altern Years";
    alloc_struct->type = REPORT_ALTERN;
    alloc_struct->number = 0;

    for (i = 0 ; index_list[i] ; ++i)
	alloc_struct->number += index_list[i]->years;

    alloc_struct->items = (char **) malloc((alloc_struct->number + 1)
	* sizeof(char *));
    alloc_struct->set = (char *) malloc(alloc_struct->number);
    if (!(alloc_struct->items && alloc_struct->set)) {
	log_msg(L_ERROR, "Out of memory in alternyear_list!\n");
	free(alloc_struct);
	return 0;
    }
    bzero(alloc_struct->set, alloc_struct->number);
    for (count = i = 0 ; index_list[i] ; ++i) {
	int size;

	size = strlen(index_list[i]->name) + CONST_SIZE;

	for ( j = 1 ; j <= index_list[i]->years ; ++j, ++count ) {
	    alloc_struct->items[count] = (char *) malloc(size);
	    if (!alloc_struct->items[count]) {
		for (i = 0 ; i < count ; ++i) {
		    free(alloc_struct->items[i]);
		}
		free(alloc_struct->items);
		free(alloc_struct);
		log_msg(L_ERROR, "Out of memory in alternyear_list!\n");
		return 0;
	    }
	    sprintf(alloc_struct->items[count], "%s, year %d",
		    index_list[i]->name, j);
	}
    }
    alloc_struct->items[count] = 0;
    return alloc_struct;
}

/**
 * \brief A simple wrapper function
 *	to ensure that we don't have two copies of the list.  The pointer
 *	altern_output_list is a static global.
 */
struct report_struct *get_output_list()
{
    if (!altern_output_list) {
	altern_output_list = alternyear_list();
    }
    return altern_output_list;
}

/**
 * \brief Given a list
 *	of alternatives (struct at_structs), return a dynamically allocated
 *	array of strings, and the length of the list in size if the pointer
 *	is non-zero.
 */
static char **releases_list(ind_list, size)
	struct monte_index **ind_list;
	int *size;
{
    int list_size = 40;
    register int i, j, k, cur_count;
    char **rel_list;
    char **old_list;

    cur_count = 0;
    rel_list = (char **) malloc(list_size * sizeof(char *));
    if (!rel_list) {
	log_msg(L_ERROR, "Out of memory in releases_list!\n");
	return 0;
    }
    for (i = 0 ; ind_list[i] ; ++i) {
	for (j = 0 ; j < ind_list[i]->release_count ; ++j) {
	    for (k = 0 ; k < cur_count ; ++k) {
		if (!strcmp(rel_list[k], ind_list[i]->releases[j]))
		    break;
	    }

	    if (k == cur_count) {
		/* A new one! */
		rel_list[cur_count] = (char *)
		    malloc(strlen(ind_list[i]->releases[j]) + 1);
		if (!rel_list[cur_count]) {
		    log_msg(L_ERROR,
			    "Out of memory creating release list!\n");
		    for (i = 0 ; i < cur_count ; ++i)
			free(rel_list[i]);
		    free(rel_list);
		    return 0;
		}
		strcpy(rel_list[cur_count], ind_list[i]->releases[j]);

		if (++cur_count == list_size) {
		    old_list = rel_list;

		    rel_list = (char **) realloc(old_list, (list_size *= 2)
			* sizeof(char *));
		    if (!rel_list) {
			log_msg(L_ERROR,
				"Out of memory creating release list!\n");
			for (i = 0 ; i < cur_count ; ++i)
			    free(old_list[i]);
			free(old_list);
		    } /* if (bad realloc) */
		} /* if (a new release) */
	    }
	}
    }
    old_list = rel_list;
    rel_list = (char **) realloc(rel_list, (cur_count+1) * sizeof(char *));
    if (!rel_list) {
	log_msg(L_ERROR, "Out of memory creating release list!\n");
	for (i = 0 ; i < cur_count ; ++i)
	    free(old_list[i]);
	free(old_list);
	return 0;
    }
    rel_list[cur_count] = (char *) 0;
    if (size) *size = cur_count;
    return rel_list;
}

/**
 * \brief The wrapper function for releases_list.
 *	This function returns the static pointer altern_releases if it
 *	exists and creates it if not.
 */
static char **altern_releases_list(size)
	int *size;
{
    static int list_size;
    struct monte_index **index_list = list_monte_indices();
    /* list_monte_indices() frees and zeroes altern_releases if it
     * reinitializes itself.  It returns a pointer to a static
     * value, so does not need to be freed.
     */

    if (!altern_releases) {
	if (!index_list) {
	    if (size) *size = 0;
	    return 0;
	}
	altern_releases = releases_list(index_list, &list_size);
    }

    if (size) *size = list_size;
    return altern_releases;
}

/**
 * \brief Wrapper function for
 *	altern_releases_list, this function creates the report struct for
 *	the releases list.
 */
static struct report_struct *alternrelease_list()
{
    struct report_struct *report;

    report = (struct report_struct *) malloc(sizeof(struct report_struct));
    if (!report) {
	log_msg(L_ERROR, "Out of memory in alternrelease_list!\n");
	return 0;
    }
    report->title = "Releases";
    report->type = REPORT_ALTERN;
    report->items = altern_releases_list(&report->number);
    if (!report->items) {
	free(report);
	return 0;
    }
    report->set = (char *) malloc(report->number);
    if (!report->set) {
	free(report->items);
	free(report);
	return 0;
    }
    bzero(report->set, report->number);
    return report;
}

/**
 * \brief Wrapper function for
 *	alternrelease_list, uses a static pointer to avoid reinitialization.
 */
struct report_struct *get_output_index()
{
    if (!altern_output_index) {
	altern_output_index = alternrelease_list();
    }
    return altern_output_index;
}

/**
 * \brief returns an
 *	alloced array of character strings representing the segments pointed
 *	to by all releases in struct at_struct.
 *  NOTE this routine may be called only to count the number of segments
 *   (alloc_flag == 0) or to allocate an array of segment names (alloc_flag == 1)
 */
static char **segments_list(index_list, size, alloc_flag)
	struct monte_index **index_list;
        int *size;
		int alloc_flag;
{
    int list_size = 40;

    register int i, j, k;
    int cur_size;
    char **seg_list;
    char **old_list;

    cur_size = 0;
    seg_list = (char **) malloc(list_size * sizeof(char *));
    if (!seg_list) {
		log_msg(L_ERROR, "Out of memory in segments_list!\n");
		return 0;
    }
    for (i = 0 ; index_list[i] ; ++i) {
	for (j = 0 ; j < index_list[i]->segment_count ; ++j) {
	    for (k = 0 ; k < cur_size ; ++k) {
		if (!strcmp(seg_list[k], index_list[i]->segments[j]))
		    break;
	    }
	    if (k == cur_size) {
		/* A new one! */
		seg_list[cur_size] = (char *)
		    malloc(strlen(index_list[i]->segments[j]) + 1);
		if (!seg_list[cur_size]) {
		    log_msg(L_ERROR, "Out of memory making segment list!\n");
		    for (i = 0 ; i < cur_size ; ++i)
			free(seg_list[cur_size]);
		    free(seg_list);
		    return 0;
		}
		strcpy(seg_list[cur_size], index_list[i]->segments[j]);

		if (++cur_size == list_size) {
		    old_list = seg_list;
		    seg_list = (char **) realloc(seg_list, (list_size *= 2)
			* sizeof(char *));
		    if (!seg_list) {
			for (i = 0 ; i < cur_size ; ++i)
			    free(old_list[cur_size]);
			free(old_list);
			return 0;
		    }
		}
	    }
	}
    }
    old_list = seg_list;
	if (alloc_flag)
	    seg_list = (char **) realloc(seg_list, (cur_size+1) * sizeof(char *));
	/* If we are only counting segments, or if something went wrong, 
	 *  discard the list we have been building up
	 */
	if (!seg_list)
		log_msg(L_ERROR, "Out of memory in segments_list!\n");
    if (!seg_list || !alloc_flag) {
		for (i = 0 ; i < cur_size ; ++i)
			free(old_list[i]);
		free(old_list);
		if (!alloc_flag && size)
			*size = cur_size;
		return 0;
    }
    seg_list[cur_size] = (char *) 0;
    if (size) *size = cur_size;
    return seg_list;
}

/**
 * \brief This function uses the
 *	alternative list to build an array of segments found in all
 *	alternatives in the list.  It then passes this information
 *	to segments_list() which does all the work.
 */
static char **altern_segments_list(size)
	int *size;
{
    static int list_size;
    struct monte_index **index_list = list_monte_indices();
    /* list_monte_indices() frees and zeroes altern_segments if it
     * reinitializes itself.  It returns a pointer to a static
     * value, so does not need to be freed.
     */

    if (!altern_segments) {
	if (!index_list) {
	    log_msg(L_ERROR, "No alternatives for segments list!\n");
	    if (size) *size = 0;
	    return 0;
	}
	/* Allocate the actual list. */
	altern_segments = segments_list(index_list, &list_size, 1);
    }

    if (size) *size = list_size;
    return altern_segments;
}

/**
 * \brief A function to return a
 *	report_struct pointer for the passage segment list item.
 */
static struct report_struct *alternpassage_list()
{
    struct report_struct *report;

    report = (struct report_struct *) malloc(sizeof(struct report_struct));
    if (!report) {
	log_msg(L_ERROR, "Out of memory in alternpassage_list (1)!\n");
	return 0;
    }
    report->title = "Segments";
    report->type = REPORT_ALTERN;
    report->items = altern_segments_list(&report->number);
    if (!report->items) {
	free(report);
	return 0;
    }
    report->set = (char *) malloc(report->number);
    if (!report->set) {
	free(report->items);
	free(report);
	log_msg(L_ERROR, "Out of memory in alternpassage_list! (2)\n");
	return 0;
    }
    bzero(report->set, report->number);
    return report;
}

/**
 * \brief The function to return
 *	a pointer to the static global altern_output_index2.
 */
struct report_struct *get_output_index2()
{
    if (!altern_output_index2) {
	altern_output_index2 = alternpassage_list();
    }
    return altern_output_index2;
}

static struct alt_data **last_results = 0;
static int n_last_results = 0;  /*size of allocated array in last_results.*/

/**
 * \brief Accumulates all data
 *	required by the altern report tool for a given release and
 *	segment name, putting the result into the alt_data pointer
 *	provided.
 */
static int accumulate_data(result, games, release, seg_name)
	struct alt_data *result;
	int games;
	struct rls *release;
	char *seg_name;
{
    extern struct summary *get_data();
#ifdef WIN32
	extern float *create_summary( struct rls_stats *rls_data, float *total, 
						int num, float *mean_day, float *first_day, 
						float *last_day, float *median_day, float *mode_day, 
						float *day_dev, float *avg_flow, int flags );
#else
    extern float *create_summary();
#endif
    extern void moment();
    extern float compute_weighted_average();

    float total;
    int num, sum_num;
    int i;
    float bogus;
    float *info;
    float *travel;
    struct summary *sum_list;
    struct summary *travel_list;
    struct rls_stats *data = 0;

    sum_list = get_data(games, release, seg_name, 0 /* NO flags! */,
	&total, &sum_num);

    if (!sum_list) {
	log_msg(L_ERROR, "Error massaging data\n");
	return 0;
    }

    travel_list = get_data(games, release, seg_name, AT_TRAVEL_FLAG, &total,
	&num);
    /* total and num are the same for both types of data */

    if (!travel_list) {
	log_msg(L_ERROR, "Error massaging data\n");
	for (--sum_num ; sum_num >= 0 ; --sum_num)
	    free(sum_list[sum_num].info);
	free(sum_list);
	return 0;
    }

    if (!split_data(sum_list, num, &data, (float **) 0)) {
	log_msg(L_ERROR, "Can't split data array!\n");
	for (--sum_num ; sum_num >= 0 ; --sum_num)
	    free(sum_list[sum_num].info);
	free(sum_list);
	for (--num ; num >= 0 ; --num)
	    free(travel_list[num].info);
	free(travel_list);
	return 0;
    }

    for (--sum_num ; sum_num >= 0 ; --sum_num)
	free(sum_list[sum_num].info);
    free(sum_list);

    bzero(result, sizeof(struct alt_data));

    info = create_summary(data, &total, num, &bogus, &bogus, &bogus,
	&bogus, &bogus, &bogus, &result->avg_flow, 0 /* no flags */);
    /* info is now an array of passage data */

#ifdef ROUTE_TRACKING
    (void) calculate_route_avgs(data, num, &result->sluiceway_avg, &result->bypass_avg,
	&result->turbine_avg, &result->spill_avg, &result->rsw_avg, &result->transport_avg,
	&result->premort_transport_total, &result->postmort_transport_total,
	&result->fpe, &result->sluiceway_in_avg, &result->bypass_in_avg, &result->turbine_in_avg,
	&result->spillway_in_avg, &result->rsw_in_avg, &result->project_in_total);
#endif

    free(data);

    if (!info) {
	log_msg(L_ERROR, "Can't create summary!\n");
	for (--num ; num >= 0 ; --num)
	    free(travel_list[num].info);
	free(travel_list);
	return 0;
    }

    if (!split_data(travel_list, num, &data, &travel)) {
	log_msg(L_ERROR, "Can't split travel data!\n");
	for (--num ; num >= 0 ; --num)
	    free(travel_list[num].info);
	free(travel_list);
	return 0;
    }

    for (i = 0 ; i < num ; ++i) {
	free(travel_list[i].info);
    }
    free(travel_list);

    /* travel is now an array of travel time data */

    if (num < 2) {
	/* Check this stuff */
	result->mean_passage = *info * 100.0;
	result->mean_travel =  *travel;
    } else {
	moment(info, num, &result->mean_passage, &result->stddev_passage,
	       &result->var_passage, &result->skew_passage,
	       &result->kurt_passage);
	result->var_passage = (result->mean_passage == 0)
	    ? 0 : result->stddev_passage / result->mean_passage;
	result->mean_passage *= 100.0;
	result->stddev_passage *= 100.0;
	moment(travel, num,
	       &result->mean_travel, &result->stddev_travel,
	       &result->var_travel, &result->skew_travel,
	       &result->kurt_travel);
	result->mean_travel = compute_weighted_average(travel, data, num);
	result->var_travel = (result->mean_travel == 0)
	    ? 0 : result->stddev_travel / result->mean_travel;
    }
    free(data);
    free(info);
    free(travel);
    return 1;
}

/**
 * \brief Given a particular
 *	alternative, return an allocated array of allocated pointers
 *	to struct alt_data to be used by the output part of the
 *	alternative report.
 */
static struct alt_data **process_year(games, year, nr)
	int games;
	struct rls_years *year;
	int *nr;
{
    static struct rls release;
    char **releases;
    char **segments;
    register int i, j, count;
    int nrel, nseg;
    struct rls *cur_rls;
    struct alt_data **results;

    if (!year) {
	log_msg(L_ERROR, "Programmer error: no year provided to process!\n");
	return 0;
    }

    releases = altern_releases_list(&nrel);
    segments = altern_segments_list(&nseg);

    /* May change to a struct alt_data ** */
	*nr = 0;
    results = (struct alt_data **) malloc((nrel * nseg + 1)
	* sizeof(struct alt_data *));

    if (!results) {
	log_msg(L_ERROR, "Out of memory in process_year!\n");
	return 0;
    }

    if (!(releases && segments)) {
	free(results);
	return 0;
    }

    /* Loop over array of release names from the list item */
    for (count = 0, i = 0 ; releases[i] ; ++i) {

	/* set cur_rls, used below */
	for (cur_rls = year->releases ; cur_rls ; cur_rls = cur_rls->next) {
	    int loc_len = strlen(cur_rls->location);

	    if (strncmp(releases[i], cur_rls->location, loc_len)) continue;
	    if (strncmp(&releases[i][loc_len + RLS_STRING_ADD],
			cur_rls->species, strlen(cur_rls->species)))
		continue;
	    if (atoi(&releases[i][loc_len + RLS_STRING_ADD + 
		strlen(cur_rls->species) + 2]) == (int)(cur_rls->date)) break;
	}

	if (!cur_rls) {	/* This release doesn't exist for this year. */
	    bzero(&results[count], nseg * sizeof(struct alt_data *));
	    count += nseg;
	    continue;
	}

	log_msg(L_MSG, "%s: Calculating release %s\n", "monte report",
		releases[i]);

	bcopy(cur_rls, &release, sizeof(struct rls));
	release.next = 0;
	cur_rls = &release;

	/* Release exists, collect data for the segments. */
	for (j = 0 ; segments[j] ; ++j, ++count) {
	    struct river_seg_info *seg;

	    seg = cur_rls->river_segment;
	    for (seg = cur_rls->river_segment ; seg ; seg = seg->next) {
		if (!strcmp(seg->name, segments[j])) break;
	    }
	    if (!seg) {
		results[count] = 0;
		continue;
	    }

	    log_msg(L_MSG, "%s: segment %s\n",
		    "monte report", segments[j]);

	    results[count] = (struct alt_data *)
		malloc(sizeof(struct alt_data));
	    if (!results[count]) {
		log_msg(L_ERROR, "Out of memory in process_year\n");
		for (i = 0 ; i < count ; ++i)
		    free(results[i]);
		free(results);
		return 0;
	    }
	    if (!accumulate_data(results[count], games, cur_rls,
		segments[j])) {
		for (i = 0 ; i <= count ; ++i)
		    free(results[i]);
		free(results);
		return 0;
	    }
	}
    }
    results[count] = 0;
	*nr = nrel * nseg;
    return results;
}

/**
 * \brief Returns all calculable output
 *	data based on the given alternative.
 *
 *	The name contains the string
 *	representation of an alternative name followed by the constant
 *	string year and the year number (with 1 added to the actual
 *	index into the years array of the alternative), ie
 *	"altern1, year 1" represents data from the alternative named
 *	altern1, index 0 into the years array.
 *
 *	Output data is stored in a static variable which is freed every
 *	time this function is called, unless name matches the last name
 *	used.  (altern-year combination is equivalent.)
 */
void *get_altern_year(name)
	char *name;
{
    void free_at_struct_results();
    extern struct at_struct *parse_at_input();
    extern char *monte_output_name();

    static char last_altern[ALTERN_MAX_SIZE];
    static char altern[ALTERN_MAX_SIZE];
    static int last_year = -1;

    int year;
    struct monte_index **index_list;
    int i;

    sscanf(name, "%s year %d", altern, &year);

    /* Make year usable as an index */
    --year;
    /* remove the trailing comma. */
    altern[strlen(altern)-1] = '\0';

    /* check to see if this is the one we have in memory. */
    if (year == last_year && !strcmp(altern, last_altern) && last_results) {
	/* Return the existing results, saving expensive calculation */
	return (void *) last_results;
    }

    free_at_struct_results();

    index_list = list_monte_indices();
    if (!index_list) return NULL;

    for (i = 0 ; index_list[i] ; ++i) {
	if (!strcmp(index_list[i]->name, altern)) break;
    }
    if (!index_list[i]) {
	log_msg(L_ERROR, "Can't find alternative %s!\n", altern);
	return NULL;
    }

    /* Save last info, and calculate */
    last_year = year;
    strcpy(last_altern, altern);

    if (!(alt_list[i] || (alt_list[i]
		= parse_at_input(monte_output_name(altern)))))
	return (void *) 0;

    last_results = process_year(alt_list[i]->games,
		&alt_list[i]->release_year[year], &n_last_results);
    return (void *) last_results;
}

/**
 * \brief Frees the static copy of the last set
 *	of results calculated by get_altern_year.
 */
void free_at_struct_results()
{
    int i;

    if (last_results != NULL) {
		for (i = 0; i < n_last_results; i++) {
			if (last_results[i])
				free(last_results[i]);
		}
		free(last_results);
		last_results = NULL;
		n_last_results = 0;
    }
}

/**
 * \brief Make sure that the
 *	altern_list reflects the current alternative.
 */
void altern_report_account_for(name)
	char *name;
{
    if (name && dir_update) {
	/* Make sure the list of alternatives contains this one,
	 * updating if necessary.
	 *
	 * !dir_update is our way of telling that we have never
	 * created this frame.
	 */
	if (!altern_exists(name))
	    report_update_directory();
    }
}

/**
 * \brief Kludge to make alternative reports work better.
 *
 *	Ordinarily, this function is dealt with in the report_frame.c
 *	function write_data, but the report tool needed a kludge to make
 *	it work smoothly as the alternative report, and this function is
 *	the seam.  If the report tool needs to be reworked to be truly
 *	general for 4 list selections, then the functionality provided by
 *	this function will need to be integrated. (How, I have no idea.)
 *
 *	The whole problem with this function is the fact that we are using
 *	a single dimensional array to accumulate all the data for a single
 *	alternative-year (two dimensional has problems, too, but they are
 *	buried a bit deeper), and thus the offset for index_list is variable
 *	in length (the number of elements in index2), requiring a specialized
 *	output function.
 */
void altern_report_output_loop(fp, variables, vars_set, data_ptr, array_info)
	FILE *fp;
	struct report_var *variables;
	char *vars_set;
	void *data_ptr;
	struct output_array *array_info;
{
    struct report_struct *index_list;
    struct report_struct *index2_list;
    register int i, j;
    int heading_shown;
    int offset;
    int num_segs;
    char title[MAX_REPORT_WIDTH + 1];


    if (!data_ptr) {
	log_msg(L_WARNING, "No data pointer in alternative report!\n");
	return;
    }

    if (!report_list)
	(void) list_monte_indices();

	/* Find out how many segments there are but don't allocate the
	 *  actual list.
	 */
    (void) segments_list(report_list, &num_segs, 0);
    index_list = get_output_index();
    index2_list = get_output_index2();
    for (offset = 0, i = 0; i < index_list->number ; ++i) {

	heading_shown = 0;

	if (!index_list->set[i]) {
	    offset += num_segs * sizeof(struct alt_data *);
	    continue;
	}

	/* Print out the vars */
	for (j = 0 ; j < index2_list->number
	     ; ++j, offset += sizeof(struct alt_data *)) {
	    if (index2_list->set[j] &&
		(*(struct alt_data **)((char *)data_ptr + offset))) {

		if (!heading_shown) {
		    /* Output index heading */
		    strncpy(title, index_list->items[i], REPORT_TITLE_WIDTH);
		    title[REPORT_TITLE_WIDTH] = '\0';
		    fprintf(fp, REPORT_TITLE_FORMAT, title);
		    putc('\n', fp);
		    heading_shown = 1;
		}

		/* Output the index2 title */
		strncpy(title, index2_list->items[j], REPORT_PREFIX_WIDTH);
		title[REPORT_PREFIX_WIDTH] = '\0';
		fprintf(fp, REPORT_PREFIX_FORMAT, title);

		do_report(fp, variables, vars_set, data_ptr, offset,
			  array_info);
	    }
	}
    }
}

