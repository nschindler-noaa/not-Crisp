#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifndef WIN32
#include <unistd.h>
#endif

#include "crisp.h"
#include "parseutil.h"
#include "struct_at.h"
#include "tokens_at.h"

void parse_hist_sum (infile, hist_sums, at_data, at_list)
	FILE *infile;
	struct rls_stats *hist_sums;
	struct at_struct *at_data;
	COMMAND_LIST at_list;	
{
    char token[MAX_COMMAND_LEN+1], scratch[MAX_COMMAND_LEN+1];
    struct rls_stats *cur_hist_sum;
    int  gameno;

    cur_hist_sum = hist_sums;
    while (pop_token(infile, token, "archive command -> ") != EOF) {
	/* token  based case stmt */
	switch (com_reference(at_list, lowercase(token, scratch))) {
	  case TK_GAME:
	    if (!read_int (infile, &gameno, "game ->"))
	    {
		p_warning(infile,
			  "Error parsing game number.  Line discarded.\n");
		reset_input(infile);
	    }
	    if (gameno >= at_data->games) {
		log_msg(L_ERROR, "Illegal game read from datafile!\n");
		log_msg(L_ERROR,
			"Data file claims %d games, read game number %d\n",
			at_data->games, gameno+1);
		cur_hist_sum = (struct rls_stats *) 0;
	    } else
		cur_hist_sum = hist_sums + gameno;
	    break;

	  case TK_STATS:
	    if (!cur_hist_sum)
	    {
		p_warning(infile, 
		    "Release stats without game number.  Line discarded.\n");
		reset_input(infile);
	    }
	    if (!read_int   (infile, &(cur_hist_sum->total.first),   "first ->")  ||
		!read_int   (infile, &(cur_hist_sum->total.last),    "last ->") )
	    {
		p_warning(infile, 
		    "Error parsing release.  Line discarded.\n");
		reset_input(infile);
	    }

	    /* this is a bit odd, but it's this way for backwards
	       compatability with the old analysis files.  In the
	       old ones, there is no avg_flow.  For new models, we
	       can assume that avg_flow is 0 */
	    if (!read_float(infile, &(cur_hist_sum->total.total),    "total ->")  ||
		!read_float(infile, &(cur_hist_sum->total.mean),     "mean ->")   ||
		!read_float(infile, &(cur_hist_sum->total.median),   "median ->") ||
		!read_float(infile, &(cur_hist_sum->total.mode),     "mode ->")   ||
		!read_float(infile, &(cur_hist_sum->total.std_dev),  "std_dev ->")||
		!read_float(infile, &(cur_hist_sum->avg_flow), "avg_flow ->"))
		p_warning(infile, "Error parsing release data.\n");
	    
#ifdef ROUTE_TRACKING
	    /* If another float is available and ifdef ROUTE_TRACKING, we
	       expect six floats or it's an error condition. */
	    pop_token(infile, token, "data ->");
	    push_last_token(infile);
	    if (is_float(token))
	    {
		if (!read_float(infile, &cur_hist_sum->sluiceway_total, "sluiceway ->") ||
		    !read_float(infile, &cur_hist_sum->bypass_total, "bypass ->") ||
		    !read_float(infile, &cur_hist_sum->spillway_total, "spillway ->") ||
		    !read_float(infile, &cur_hist_sum->rsw_total, "rsw ->") ||
		    !read_float(infile, &cur_hist_sum->turbine_total, "turbine ->") ||
		    !read_float(infile, &cur_hist_sum->postmort_transport_total, "postmort transport ->") ||
		    !read_float(infile, &cur_hist_sum->project_in_total, "project in ->") ||
			!read_float(infile, &cur_hist_sum->sluiceway_in_total, "sluiceway in ->") ||
		    !read_float(infile, &cur_hist_sum->bypass_in_total, "bypass in ->") ||
		    !read_float(infile, &cur_hist_sum->spillway_in_total, "spillway in ->") ||
		    !read_float(infile, &cur_hist_sum->rsw_in_total, "rsw in ->") ||
		    !read_float(infile, &cur_hist_sum->turbine_in_total, "turbine in ->") ||
		    !read_float(infile, &cur_hist_sum->premort_transport_total, "premort transport ->")
		    )
		    p_error(infile, "Error parsing release data.\n");
	    }
#else
	    /* If more floats are available and ifndef ROUTE_TRACKING
	       we have to throw them out. */
	    skip_all_numbers(infile);
#endif	
	    break;

	  case TK_RIV_SEG:
	    push_last_token(infile);
	    return;
	    break;

	  case TK_FLOW_SEG:
	    push_last_token(infile);
	    return;
	    break;

	  case TK_END:
	    reset_input(infile);
	    return;
	    break;

	  default:
	    /* default - output some error message */
	    UNKNOWN_COMMAND_ERROR(infile, token);
	}
    }
    return;
}

void parse_rls(infile, at_data, cur_release_year, at_list)
	FILE *infile;
	struct at_struct *at_data;
	struct rls_years *cur_release_year;
	COMMAND_LIST at_list;
{
    char token[MAX_COMMAND_LEN+1], scratch[MAX_COMMAND_LEN+1];
    struct river_seg_info *cur_riv_seg;
    struct rls *cur_rls;
    int temp_int;

    if (!cur_release_year)
    {
	log_msg(L_WARNING, "Invalid data for parse_rls!\n");
	return;
    }

    if (!(cur_rls = (struct rls *) malloc(sizeof(struct rls))))
    {
	log_msg(L_ERROR, "No memory for release\n");
	return;
    }

    bzero((struct rls *) cur_rls, sizeof(struct rls));
    cur_rls->next = cur_release_year->releases;
    cur_release_year->releases = cur_rls;

    if (pop_token(infile, token, "release species -> ") == EOF)
    {
	p_warning(infile, "Unexpected EOF parsing release species.\n");
	cur_release_year->releases = cur_rls->next;
	free(cur_rls);
	return;
    }

    if ((cur_rls->species = (char *) malloc((strlen(token) +1) * sizeof (char))))
	strcpy(cur_rls->species, unders_to_spaces(token));
    else
    {
	log_msg(L_ERROR, "No memory for species name.\n");
	cur_release_year->releases = cur_rls->next;
	free(cur_rls);
	return;
    }

    if (pop_token(infile, token, "release location -> ") == EOF)
    {
	p_warning(infile, "Unexpected EOF parsing release location.\n");
	return;
    }

    if ((cur_rls->location = (char *) malloc((strlen(token) +1) * sizeof(char))))
	strcpy(cur_rls->location, unders_to_spaces(token));
    else
    {
	log_msg(L_ERROR, "No memory for release location name.\n");
	return;
    }
	
    if (!read_int(infile, &temp_int, "release date ->"))
    {
	p_warning(infile,
		  "Error parsing release date in {%s}. Line discarded.\n",
		  "Output Data File",
		  at_data->input_file);
	reset_input(infile);
    }
    else if (temp_int < 0)
    {
	p_warning(infile,
		  "signed value %d read for unsigned release date! %s\n",
		  temp_int, "Discarded");
	cur_rls->date = 0;
    }
    else
	cur_rls->date = (unsigned) temp_int;

    if (pop_token(infile, token, "smolt onset -> ") == EOF)
	p_warning(infile, "Unexpected EOF parsing smolt onset.\n");

    if (strcmp(token, TKS_AGE) && strcmp(token, TKS_SMOLT_ON))
	p_warning(infile, "Species smolt onset token not found.\n");
    
    if (!read_float (infile, &(cur_rls->smolt_onset), "smolt onset ->"))
    {
	p_warning(infile, "Error parsing smolt onset.  Line discarded.\n");
	cur_rls->smolt_onset = 0.0;
    }
    
    parse_hist_sum(infile, &(cur_rls->hist_summary), at_data, at_list);

    while (pop_token(infile, token, "archive command -> ") != EOF)
    {
	/* token  based case stmt */
	switch (com_reference(at_list, lowercase(token, scratch))) {

	  case TK_FLOW_SEG:
	    if (pop_token(infile, token, "flow indicator -> ") == EOF)
	    {
		p_error(infile, "Unexpected EOF parsing release location.\n");
		return;
	    }
	    if ((cur_rls->flow_seg =
		(char *) malloc((strlen(token) +1) * sizeof (char))))
		strcpy(cur_rls->flow_seg, unders_to_spaces(token));
	    else
	    {
		log_msg(L_ERROR, "No memory for flow segment name.\n");
		return;
	    }
	    break;

	  case TK_RIV_SEG:
	    if (!(cur_riv_seg = (struct river_seg_info *)
		  malloc(sizeof(struct river_seg_info))))
	    {
		log_msg(L_ERROR, "No memory for river segment\n");
		return;
	    }
	    bzero((struct river_seg_info *) cur_riv_seg,
		  sizeof(struct river_seg_info));

	    cur_riv_seg->next = cur_rls->river_segment;
	    cur_rls->river_segment = cur_riv_seg;

	    if (pop_token(infile, token, "river segment -> ") == EOF)
	    {
		p_warning(infile,  "Unexpected EOF parsing river segment.\n");
		return;
	    }

	    if ((cur_riv_seg->name =
		(char *) malloc((strlen(token) +1) * sizeof(char))))
		strcpy(cur_riv_seg->name, unders_to_spaces(token));
	    else
	    {
		log_msg(L_ERROR, "No memory for river segment name.\n");
		return;
	    }

	    cur_riv_seg->summaries = (struct rls_stats *) 
		malloc(sizeof(struct rls_stats) * (at_data->games));
	    bzero((struct rls_stats *) cur_riv_seg->summaries,
		  sizeof(struct rls_stats) * (at_data->games));
	    parse_hist_sum(infile, cur_riv_seg->summaries, at_data, at_list);
	    break;

	  case TK_END:
	    if(pop_token(infile, token, "") == EOF)
	    {
		p_warning(infile, "Unexpected EOF parsing end release.\n");
		return;
	    }
	    switch (com_reference(at_list, lowercase(token, scratch))) {
	      case TK_RIV_SEG:
		reset_input(infile);
	      case TK_REL:
		reset_input(infile);
		return;
	      default:
		reset_input(infile);
	    }
	    break;

	  default:
	    /* default - output some error message */
	    UNKNOWN_COMMAND_ERROR(infile, token);
	}
    }
    return;
}


struct at_struct *parse_at_input (input_file)
	char *input_file;
{
    void at_struct_free(); 

    FILE *infile;
    char token[MAX_COMMAND_LEN+1], scratch[MAX_COMMAND_LEN+1];
    struct at_struct *at_data;
    struct rls_years *cur_release_year = 0;
    static COMMAND_LIST at_list = 0;
    int years = 0;
    struct stat s_buf;

    if ((at_data = (struct at_struct *)	malloc (sizeof(struct at_struct))))
	bzero((struct at_struct *)at_data, sizeof(struct at_struct));
    else
    {
	log_msg(L_ERROR, "No memory for at_data\n");
	return((struct at_struct *)0);
    }

    /* open file using buf_open */
    if (!(infile = buf_open (input_file, "r")) )
    {
	log_msg(L_MSG, "can't open file {%s}\n", input_file);
	free(at_data);
	return((struct at_struct *)0);
    }

    if (stat(input_file, &s_buf) < 0) {
	free(at_data);
	buf_close(infile);
	return (struct at_struct *) 0;
    }

    if (s_buf.st_size == 0) {
	log_msg(L_WARNING, "Empty file {%s}\n", input_file);
	free(at_data);
	buf_close(infile);
	if (unlink(input_file) < 0) {
	    log_msg(L_MSG, "Can't remove empty file!\n");
	}
	return (struct at_struct *) 0;
    }

    if ((at_data->input_file =
	(char *) malloc((strlen(input_file) + 1) * sizeof (char))))
	strcpy(at_data->input_file, input_file);
    else
    {
	log_msg(L_ERROR, "No memory for analysis data!\n");
	free(at_data);
	buf_close(infile);
	return (struct at_struct *) 0;
    }

    if (!at_list)
	at_list = com_create_list(at_list, at_tokens);

    while (pop_token(infile, token, "archive command -> ") != EOF)
    {
	/* token  based case stmt */
	switch (com_reference(at_list, lowercase(token, scratch)))
	{
	  case TK_ALT:
	    if (pop_token(infile, token, "archive command -> ") == EOF)
	    {
		p_error(infile,	"Unexpected EOF.\n");
		at_struct_free(at_data);
		buf_close(infile);
		return((struct at_struct *)0);
	    }

	    if ((at_data->alternative =
		(char *) malloc((strlen(token) +1) * sizeof(char))))
		strcpy(at_data->alternative, token);
	    else
	    {
		log_msg(L_ERROR, "No memory for alternative name.\n");
		at_struct_free(at_data);
		buf_close(infile);
		return((struct at_struct *)0);
	    }
	    break;

	  case TK_YEARS:
	    {
		int temp_int;

		/* # years */
		if (!read_int (infile, &temp_int, "number of years ->"))
		{
		    p_error(infile,
			    "Error parsing # of years in.  Aborting.\n");
		    at_struct_free(at_data);
		    buf_close(infile);
		    return((struct at_struct *)0);
		    break;
		}
		else if (temp_int < 0)
		{
		    p_error(infile,
			    "Signed value read for unsigned variable %s\n",
			    "# of years!  Discarding.");
		    at_data->years = 0;
		}
		else
		    at_data->years = temp_int;

		at_data->release_year = (struct rls_years *) 
		    malloc(at_data->years * sizeof(struct rls_years));
		if (!at_data->release_year)
		{
		    log_msg(L_ERROR, "No memory for release years.\n");
		    at_struct_free(at_data);
		    buf_close(infile);
		    return((struct at_struct *)0);
		}
		bzero((struct rls_years *)at_data->release_year,
		      at_data->years * sizeof(struct rls_years));
	    }
	    break;

	  case TK_GAMES:
	    /* # games */
	    if (!read_int (infile, &(at_data->games), "number of games ->"))
	    {
		p_error(infile, "Error parsing # of games.  Aborting.\n");
		at_struct_free(at_data);
		buf_close(infile);
		return((struct at_struct *)0);
	    }
	    break;

	  case TK_YEAR:
	    {
		int temp_int;

		years += 1;
		if (years > at_data->years)
		{
		    p_warning(infile, "Discarding extras years in input.\n");
		    buf_close(infile);
		    return (at_data);
		}

		cur_release_year = (struct rls_years *)
		    (at_data->release_year + years - 1);
		if (!read_int (infile, &temp_int, "release year ->"))
		{
		    p_warning(infile,
			      "Error parsing release year. Line discarded.\n");
		    reset_input(infile);
		}
		else if (temp_int < 0)
		{
		    p_warning(infile,
			      "Signed value %d read for unsigned %s!  %s\n",
			      temp_int, "variable release year", "Discarding");
		    cur_release_year->year = 0;
		}
		else
		    cur_release_year->year = temp_int;
	    }
	    break;

	  case TK_REL:
	    parse_rls(infile, at_data, cur_release_year, at_list);
	    break;

	  default:
	    /* default - output some error message */
	    UNKNOWN_COMMAND_ERROR(infile, token);
	}
    }
    buf_close(infile);
    return (at_data);
}


void free_segs(a_seg)
	struct river_seg_info *a_seg;
{
    struct river_seg_info *cur_seg;
    struct river_seg_info *last=0;

    for (cur_seg = a_seg; cur_seg; cur_seg = cur_seg->next) {
#ifndef DONT_CHECK_FREES
	if (cur_seg->name)
#endif
	    free(cur_seg->name);   
#ifndef DONT_CHECK_FREES
	if (cur_seg->summaries)
#endif
	    free(cur_seg->summaries);
	if (last)
	    free(last);
	last = cur_seg;
    }
    if (last)
	free(last);
}

void free_releases(an_rls) 
	struct rls	*an_rls;
{
    struct rls *cur_rls;
    struct rls *last = 0;

    for (cur_rls = an_rls; cur_rls; cur_rls = cur_rls->next) {
	free_segs(cur_rls->river_segment);
#ifndef DONT_CHECK_FREES
	if (cur_rls->species)
#endif
	    free(cur_rls->species);
#ifndef DONT_CHECK_FREES
	if (cur_rls->location)
#endif
	    free(cur_rls->location);
	if (cur_rls->flow_seg)
	    free(cur_rls->flow_seg);
	if (last)
	    free(last);
	last = cur_rls;
    }
    if (last)
	free(last);
}

void at_struct_free(at_data)
	struct at_struct *at_data;
{
    int i;

    if (!at_data) return;

    for (i=0; i<at_data->years; ++i)
	free_releases(at_data->release_year[i].releases);
#ifndef DONT_CHECK_FREES
    if (at_data->release_year)
#endif
	free(at_data->release_year);
#ifndef DONT_CHECK_FREES
    if (at_data->input_file)
#endif
	free(at_data->input_file);
#ifndef DONT_CHECK_FREES
    if (at_data->alternative)
#endif
	free(at_data->alternative);
    free(at_data);
}
