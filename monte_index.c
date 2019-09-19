/* monte_index.c
 *
 *	Source file for non-parsing functions related to the monte carlo
 * index files---indices of the Monte Carlo output data containing only
 * that information required to build the Monte report frame, to avoid
 * parsing the entire output file for alternatives that don't require it.
 */

#include <stdlib.h>
#include <string.h>
#include "crisp.h"
#include "struct_at.h"
#include "parseutil.h"
#include "writeutil.h"
#include "tokens_mindex.h"
#include "monte_index.h"

struct string_list {
    char *string;
    struct string_list *next;
};

#define MAKE_RELEASE_STRING(buffer, rls)  sprintf((buffer), "%s(%s)[%d]", \
((rls)->location), ((rls)->species), ((rls)->date))

/* Missing final ';' so that it can be added after the macro call */
#define ALLOC_ELEM(element) \
if (!(element = (struct string_list *) malloc(sizeof(struct string_list)))) { \
	log_msg(L_ERROR, "No memory to construct monte index"); \
} else element->next = 0

/*
 * void monte_index_free(struct monte_struct *index) -- Frees a monte_index
 *	struct and all elements of it.
 */
void monte_index_free(index)
	struct monte_index *index;
{
    int i;

    if (index) {
	if (index->releases) {
	    for (i = 0 ; i < index->release_count ; ++i) {
		if (index->releases[i])
		    free(index->releases[i]);
	    }
	    free(index->releases);
	}
	if (index->segments) {
	    for (i = 0 ; i < index->segment_count ; ++i) {
		if (index->segments[i])
		    free(index->segments[i]);
	    }
	    free(index->segments);
	}
	free(index);
    }
}

/*
 * char *monte_index_filename(char *altern) -- returns a pointer to a static
 *	buffer containing the name of the index file for the given alternative.
 */
char *monte_index_filename(altern)
	char *altern;
{
    static char index_buf[256];

    sprintf(index_buf, "%s/%s.%s", altern, altern, "ind");
    return index_buf;
}

#define MAX_RELEASE_SIZE 128

/*
 * void word_to_release(char *string) -- This function sucks, turning
 *	release characters into characters the parser won't explode on
 */
void word_to_release(string)
	char *string;
{
    while (*string) {
	if (*string == '(' || *string == ')') *string = '+';
	else if (*string == '[' || *string == ']') *string = '~';
	++string;
    }
}


/*
 * int make_monte_index_from_output(char *altern) -- Reads the monte output
 *	file into an at_struct and writes an index based on that data.
 *	Altern is the name of the alternative, not the name of the file.
 *
 *	A zero return value is normal; a return value of -1 indicates an
 *	error occurred.
 */
int make_monte_index_from_output(altern)
	char *altern;
{
    extern char *monte_output_name();
    extern struct at_struct *parse_at_input();
    extern void at_struct_free(); 

    char *monte_index_filename();

    struct at_struct *monte_data;
    int return_value;

    monte_data = parse_at_input(monte_output_name(altern));
    if (!monte_data)
	return -1;
    return_value = write_monte_index(monte_index_filename(altern), monte_data);
    at_struct_free(monte_data);
    return return_value;
}

/*
 * int list_include_string(struct string_list *list, char *string,
 *	int *counter) -- Adds a string_list element containing
 *	string to the end of list, if list does not already contain an
 *	equivalent element.  The counter reference is incremented if it
 *	is not NULL.
 *
 *	Returns 1 if string was added to the list, -1 if a match was found,
 *	and 0 if memory could not be allocated.
 */
static int list_include_string(list, string, counter)
	struct string_list *list;
	char *string;
	int *counter;
{
    struct string_list *step;
    int result;

    step = list;
    while((result = strcmp(step->string, string))) {
	if (!step->next) break;
	step = step->next;
    }
    if (result) {
	ALLOC_ELEM(step->next);
	if (!step->next)
	    return 0;
	step->next->string = string;
	if (counter)
	    ++(*counter);
	log_msg(L_DBGMSG, "String list element %s added!\n", string);
	return 1;
    }
    return -1;
}

/*
 * void free_string_list(struct string_list *list) -- Walks down a string list,
 *	freeing as it goes, until it encounters a null pointer.
 *	Assumes that list->string will NOT need to be freed.
 */
static void free_string_list(list)
	struct string_list *list;
{
    struct string_list *next;

    while (list) {
	next = list->next;
	free(list);
	list = next;
    }
}

/*
 * void free_dynamic_string_list(struct string_list *list) -- Walks down a
 *	string list, freeing as it goes, until it encounters a null pointer.
 *	Assumes that list->string WILL need to be freed.
 */
static void free_dynamic_string_list(list)
	struct string_list *list;
{
    struct string_list *next;

    while(list) {
	next = list->next;
	if (list->string)
	    free(list->string);
	free(list);
	list = next;
    }
}

/*
 * int create_release_and_segment_lists(struct at_struct *monte_data,
 *	int *seg_count, int *rls_count, struct string_list **releases,
 *	struct string_list **segments) -- Given an at_struct pointer,
 *	returns two lists: one containing all unique releases, and
 *	another containing all unique river segments encompassed by that
 *	at_struct, plus their lengths.
 */
static int create_release_and_segment_lists(monte_data, rls_count, seg_count,
		releases, segments)
	struct at_struct *monte_data;
	int *rls_count;
	int *seg_count;
	struct string_list **releases;
	struct string_list **segments;
{
    struct string_list *release_front;
    struct string_list *segment_front;
    struct river_seg_info *cur_seg;
    struct rls *cur_rls;
    char buf[MAX_RELEASE_SIZE];
    char *alloc_string;
    int result;
    int i;

    if (!(monte_data && monte_data->release_year
	  && monte_data->release_year[0].releases)) {
	log_msg(L_ERROR, "Bogus data for release lists!\n");
	return 0;
    }
    MAKE_RELEASE_STRING(buf, monte_data->release_year[0].releases);
    ALLOC_ELEM(release_front);
    if (!release_front) return 0;
    release_front->string = (char *) malloc(strlen(buf) + 1);
    if (!release_front->string) {
	log_msg(L_ERROR, "No memory for monte carlo index\n");
	free(release_front);
	return 0;
    }
    strcpy(release_front->string, buf);
    log_msg(L_DBGMSG, "Initializing release list with %s\n", buf);
    ALLOC_ELEM(segment_front);
    if (!segment_front) {
	free(release_front->string);
	free(release_front);
	return 0;
    }
    segment_front->string = monte_data->release_year[0].releases->river_segment
	->name;
    log_msg(L_DBGMSG, "Initializing segment list with %s\n",
	    segment_front->string);
    *rls_count = 1;
    *seg_count = 1;
    for (i = 0 ; i < monte_data->years ; ++i) {
	for (cur_rls = monte_data->release_year[i].releases ; cur_rls
	     ; cur_rls = cur_rls->next) {
	    for (cur_seg = cur_rls->river_segment ; cur_seg
		 ; cur_seg = cur_seg->next) {
		if (!list_include_string(segment_front, cur_seg->name,
			seg_count)) {
		    free_string_list(segment_front);
		    free_dynamic_string_list(release_front);
		    return 0;
		}
	    } /* for (segments) */
	    MAKE_RELEASE_STRING(buf, cur_rls);
	    alloc_string = (char *) malloc(strlen(buf) + 1);
	    if (!alloc_string) {
		free_string_list(segment_front);
		free_dynamic_string_list(release_front);
		return 0;
	    }
	    strcpy(alloc_string, buf);
	    if (!(result = list_include_string(release_front, alloc_string,
			rls_count))) {
		free_string_list(segment_front);
		free_dynamic_string_list(release_front);
		return 0;
	    }
	    if (result < 0)
		free(alloc_string);
	} /* for (releases) */
    } /* for (years) */
    *releases = release_front;
    *segments = segment_front;
    return 1;
}

/*
 * int write_monte_index(char *filename, struct at_struct *monte_data) --
 *	given a filename and an at_struct, writes an index file for use with
 *	the report tool.
 *
 *	A return value of 0 represents successful completion, whereas a
 *	return value of -1 indicates that an error occurred.
 */
int write_monte_index(filename, monte_data)
	char *filename;
	struct at_struct *monte_data;
{
    FILE *ind_file;
    struct string_list *releases;
    struct string_list *segments;
    struct string_list *next_ptr;
    char buf[MAX_RELEASE_SIZE];
    int release_count;
    int segment_count;

    if (!(monte_data && monte_data->release_year
	  && monte_data->release_year[0].releases)) {
	log_msg(L_WARNING,
		"No releases in monte carlo!  No data for index file!\n");
	return -1;
    }
    if (!(ind_file = buf_open(filename, "w"))) {
	log_msg(L_ERROR, "can't open file {%s} for writing\n", filename);
	return -1;
    }
    if (fputs("# Monte Carlo Index file generated automatically by COMPASS\n",
	      ind_file) == EOF) {
	perror_msg("index_output:");
	buf_close(ind_file);
	return -1;
    }

    if (fputs("# DO NOT EDIT; this file will be overwritten without warning\n",
	      ind_file) == EOF) {
	perror_msg("index_output:");
	buf_close(ind_file);
	return -1;
    }
    if (fprintf(ind_file, " %s %s\n", TKS_MI_ALTERN, monte_data->alternative)
	== EOF) {
	perror_msg("index_output:");
	buf_close(ind_file);
	return -1;
    }
    if (fprintf(ind_file, " %s %d\n", TKS_MI_YEARS, monte_data->years) == EOF){
	perror_msg("index_output:");
	buf_close(ind_file);
	return -1;
    }
    if (!create_release_and_segment_lists(monte_data,
		&release_count, &segment_count, &releases, &segments)) {
	return -1;
    }
    if (fprintf(ind_file, " %s %d\n", TKS_MI_NRELEASES, release_count)==EOF) {
	perror_msg("index_output:");
	free_dynamic_string_list(releases);
	free_string_list(segments);
	buf_close(ind_file);
	return -1;
    }
    for ( ; releases ; releases = next_ptr) {
	string_to_word(releases->string, buf);
	word_to_release(buf);
	if (fprintf(ind_file, "  %s %s\n", TKS_MI_RELEASE, buf)
	    == EOF) {
	    perror_msg("index_output:");
	    free_dynamic_string_list(releases);
	    free_string_list(segments);
	    buf_close(ind_file);
	    return -1;
	}
	next_ptr = releases->next;
	free(releases->string);
	free(releases);
    }
    if (fprintf(ind_file, " %s %d\n", TKS_MI_NSEGMENTS, segment_count)==EOF) {
	perror_msg("index_output:");
	free_string_list(segments);
	buf_close(ind_file);
	return -1;
    }
    for ( ; segments ; segments = next_ptr) {
	string_to_word(segments->string, buf);
	if (fprintf(ind_file, "  %s %s\n", TKS_MI_SEGMENT, buf)
	    == EOF) {
	    perror_msg("index_output:");
	    free_string_list(segments);
	    buf_close(ind_file);
	    return -1;
	}
	next_ptr = segments->next;
	free(segments);
    }
    if (fprintf(ind_file, "%s\n", TKS_MI_END) == EOF) {
	perror_msg("index_output:");
	buf_close(ind_file);
	return -1;
    }
    buf_close(ind_file);
    return 0;
}
