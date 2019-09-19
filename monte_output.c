#include <sys/types.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#ifdef WIN32
#include <stdlib.h>
#include <io.h>
#include <direct.h>
#else
#include <unistd.h>
#endif
#include <errno.h>
#include <time.h>
#include <sys/stat.h>

#include "crisp.h"
#include "monte.h"
#include "monte_output.h"
#include "log.h"
#include "command.h"
#include "tokens_at.h"
#include "writeutil.h"
#include "output_flags.h"
#include "crisp_time.h"

/* one output file may be open at a time.  this is the file pointer. */

static FILE *outfile = NULL;

/* routine to calculate the stats for a game, and to remove the output hist
 * info; parameters are game #, and bool indicating whether to prune
 */

void calculate_stats_and_prune_tree(game, prune)
    int game;
    int prune;
{
    struct release *rls = current->release_list;
    struct rls_seg *rls_seg;
    struct rls_seg_data *seg_data;
    int err;
    int i;
    
    while (rls)
    {
	rls_seg = rls->rls_seg;
	while (rls_seg)
	{
	    err = 0;
	    if (!rls_seg->data)
		err = 1;
	    else
	    {
		/* is this game in the null terminated list? */
		for (i = 0; i <= game && rls_seg->data[i]; i++)
		    ;
		if (i <= game
		    || (rls_seg->data[game] == (struct rls_seg_data *) -1)
		    || (!rls_seg->data[game]->output))
		    err = 1;
		else
		{
		    seg_data = rls_seg->data[game];

		    compute_stats(seg_data, rls_seg->river_segment,
				  rls->start_date, (int) rls->stats.total.mean);

		    if (prune)
		    {
			if (seg_data->output)
			    free(seg_data->output);
			if (seg_data->transport_in)
			    free(seg_data->transport_in);
			if (seg_data->transport_out)
			    free(seg_data->transport_out);
			seg_data->output = NULL;
			seg_data->transport_in = NULL;
			seg_data->transport_out = NULL;
#ifdef ROUTE_TRACKING
			if (seg_data->project_in)
			    free(seg_data->project_in);
			if (seg_data->turbine_in)
			    free(seg_data->turbine_in);
			if (seg_data->bypass_in)
			    free(seg_data->bypass_in);
			if (seg_data->sluiceway_in)
			    free(seg_data->sluiceway_in);
			if (seg_data->spillway_in)
			    free(seg_data->spillway_in);
			if (seg_data->rsw_in)
			    free(seg_data->rsw_in);

			if (seg_data->bypass_pass)
			    free(seg_data->bypass_pass);
			if (seg_data->sluiceway_pass)
			    free(seg_data->sluiceway_pass);
			if (seg_data->turbine_pass)
			    free(seg_data->turbine_pass);
			if (seg_data->spillway_pass)
			    free(seg_data->spillway_pass);
			if (seg_data->rsw_pass)
			    free(seg_data->rsw_pass);
			if (seg_data->transport_pass)
			    free(seg_data->transport_pass);
			if (seg_data->transport_arrived)
			    free(seg_data->transport_arrived);

			seg_data->project_in = NULL;
			seg_data->turbine_in = NULL;
			seg_data->sluiceway_in = NULL;
			seg_data->bypass_in = NULL;
			seg_data->spillway_in = NULL;
			seg_data->rsw_in = NULL;
			seg_data->sluiceway_pass = NULL;
			seg_data->bypass_pass = NULL;
			seg_data->turbine_pass = NULL;
			seg_data->spillway_pass = NULL;
			seg_data->rsw_pass = NULL;
			seg_data->transport_pass = NULL;
			seg_data->transport_arrived = NULL;
#endif
		    }
		}
	    }
	    if (err)
	    {
		log_msg(L_ERROR, "Unable to find data for release %s seg %s game %d; aborting!!\n",
			rls->site->name, rls_seg->river_segment->seg_name,
			game);
		break;
	    }
	    rls_seg = rls_seg->down;
	}

	rls = rls->next;
    }
}


/* summarize the output histogram for this segment in preparation
 * for output to file.
 */

void compute_stats(seg_data, seg, start_date, first_flow_day)
    struct rls_seg_data *seg_data;	/* rls data seg */
    struct river_segment *seg;		/* Just for the output_flags */
    int start_date;			/* season offset in days */
    int first_flow_day;			/* first day for flow calculation */
{
    int i = 0;

	double night_pass = 0.0;

    /* only compute the stats if not marked */
    if (seg_data->stats.done)
	return;
    
    bzero((char *) &seg_data->stats, sizeof(struct rls_stats));
    
    /* first pass */
    while (i < STEPS_IN_RUN &&
	    seg_data->output[i] == 0.0 &&
            (seg_data->project_in == NULL || seg_data->project_in[i] == 0.0))
	i++;
    if (i == STEPS_IN_RUN)
	return;
    
    while (i < STEPS_IN_RUN)
    {
#ifdef ROUTE_TRACKING
	if (TRACK_TRANSPORT(seg) || TRACK_ROUTING(seg))
	{
	    if (seg->seg_type == DAM_SEG || seg->seg_type == POWERHOUSE_SEG)
	    {
		if (seg_data->transport_pass)
		    seg_data->stats.postmort_transport_total
			+= seg_data->transport_pass[i];
		if (seg_data->transport_out)
		    seg_data->stats.premort_transport_total
			+= seg_data->transport_out[i];
	    }
		if (seg->seg_type == REACH_SEG)
		{
		if (seg_data->transport_in)
			seg_data->stats.transport_in_total
			+= seg_data->transport_in[i];
		}
	}
	if (seg->seg_type == REACH_SEG && TRACK_PASSAGE(seg))
	{
	    /* total input to the "project in" field, even though
	     * this is a reach.  passage input for dams is handled
	     * under "TRACK_ROUTING"
	     */
	    if (seg_data->project_in)
		seg_data->stats.project_in_total += seg_data->project_in[i];
	}
	if (TRACK_ROUTING(seg))
	{
	    if (seg_data->project_in)
		seg_data->stats.project_in_total += seg_data->project_in[i];
	    if (seg_data->turbine_in)
		seg_data->stats.turbine_in_total += seg_data->turbine_in[i];
	    if (seg_data->sluiceway_in)
		seg_data->stats.sluiceway_in_total += seg_data->sluiceway_in[i];
	    if (seg_data->bypass_in)
		seg_data->stats.bypass_in_total += seg_data->bypass_in[i];
	    if (seg_data->spillway_in)
		seg_data->stats.spillway_in_total += seg_data->spillway_in[i];
	    if (seg_data->rsw_in)
		seg_data->stats.rsw_in_total += seg_data->rsw_in[i];

	    if (seg_data->sluiceway_pass)
		seg_data->stats.sluiceway_total += seg_data->sluiceway_pass[i];
	    if (seg_data->bypass_pass)
		seg_data->stats.bypass_total += seg_data->bypass_pass[i];
	    if (seg_data->turbine_pass)
		seg_data->stats.turbine_total += seg_data->turbine_pass[i];
	    if (seg_data->spillway_pass)
		seg_data->stats.spillway_total += seg_data->spillway_pass[i];
	    if (seg_data->rsw_pass)
		seg_data->stats.rsw_total += seg_data->rsw_pass[i];
	}
#endif

	if (seg_data->output[i] != 0.0)
	{
		if (is_night(i % STEPS_PER_DAY)) night_pass += seg_data->output[i];
	    seg_data->stats.total.total += seg_data->output[i];
	}

	i++;
    }

	/* Calculate overall passage statistics */
	compute_passage_stats(&seg_data->stats.total, seg_data->output, STEPS_IN_RUN, start_date);

	/* Calculate inriver and transported statistics */
	if (seg_data->output_transport) {
		float inriver[STEPS_IN_RUN];
		int s;

		for (s = 0; s < STEPS_IN_RUN; s++) inriver[s] = seg_data->output[s] - seg_data->output_transport[s];

		compute_passage_stats(&seg_data->stats.inriver, inriver, STEPS_IN_RUN, start_date);
		compute_passage_stats(&seg_data->stats.transport, seg_data->output_transport, STEPS_IN_RUN, start_date);
	}

    /* we can return if passage is 0 */
    if (seg_data->stats.total.total == 0.0)
	return;
    
    /* compute average flow over mean passage at indicator seg, if it exists */
    if (current->flow_seg)
    {
	float *flow;
	float avg;
	int i;

	if (current->flow_seg->seg_type == DAM_SEG
	    || current->flow_seg->seg_type == POWERHOUSE_SEG)
	    flow = current->flow_seg->current->dam.flow;
	else if (current->flow_seg->seg_type == REACH_SEG)
	    flow = current->flow_seg->current->reach.flow;
	else
	{
	    log_msg(L_ERROR, "bad flow indicator seg %s\n",
		    current->flow_seg->seg_name);
	    return;
	}
	avg = 0;
	for (i = first_flow_day; i <= (int) seg_data->stats.total.mean; i++)
	    avg += flow[i];
	avg = avg / (float) (((int)seg_data->stats.total.mean - first_flow_day) + 1);
	seg_data->stats.avg_flow = avg;
    }

	/* compute spill efficiency */
	if (TRACK_ROUTING(seg)) {
		float sum = 0.0;

		for (i=0; i < STEPS_IN_RUN; i++) {
			int j;
			const float fish = seg_data->spillway_in[i] + seg_data->rsw_in[i];
			float avg_spill_fraction = 0.0; /* Dam time slices are more fine-grained than model steps */

			if (fish <= 0.0f) continue;

			/* Get average spill fraction over the time step */
			for (j=0; j < SLICES_IN_STEP; j++) avg_spill_fraction += seg->current->dam.spill[start_date*DAM_TIME_SLC + i*SLICES_IN_STEP + j];
			avg_spill_fraction /= SLICES_IN_STEP;

			if (avg_spill_fraction <= 0.0f) continue;

			sum += seg_data->project_in[i] * (fish / seg_data->project_in[i]) / avg_spill_fraction;
		}

		seg_data->stats.spill_efficiency = sum / seg_data->stats.project_in_total;
	}

	/* Put the delayed mortality table into this segment's stats structure */
	if (seg_data->d) {
		seg_data->stats.d = seg_data->d;
	}

	/* Copy post-Bonneville stuff into this segment's stats structure */
	seg_data->stats.inriver_latent_mort = seg_data->inriver_latent_mort;
	seg_data->stats.transport_latent_mort = seg_data->transport_latent_mort;
	seg_data->stats.differential_return = seg_data->differential_return;
	seg_data->stats.inriver_return = seg_data->inriver_return;
	seg_data->stats.transport_return = seg_data->transport_return;
	seg_data->stats.overall_return = seg_data->overall_return;
	seg_data->stats.post_bonneville_total = seg_data->post_bonneville_total;


	/* Compute night passage (diel passage) */
	seg_data->stats.night_passage = night_pass / seg_data->stats.total.total;
    
    seg_data->stats.done = TRUE;
}


/* computes the initial stats for all the releases */

void init_release_stats()
{
    struct release *rls;
    struct rls_seg_data rls_data;

    bzero((char *) &rls_data, sizeof(rls_data));
    for (rls = current->release_list; rls; rls = rls->next)
    {
	rls_data.output = (float *) rls->number;
	rls_data.stats.done = FALSE; /* mark as not done */
	compute_stats(&rls_data, rls->rls_seg->river_segment,
		      rls->start_date, 0);
	rls->stats = rls_data.stats;	/* structure copy */
    }
}


/* opens an output file associated with the desired alternative
 * in the monte structure.  one file can be opened at a time, maintained
 * in a static file pointer var.  also writes the header to the file.
 * returns 0 on success, non-zero on failure.
 */

int
new_outfile(alt, monte_data)
    int alt;
    struct monte_carlo_data *monte_data;
{
    time_t clock;
    char path[256];
    struct stat sbuf;
    extern int errno;

    /* close current output file if still open */
    if (outfile)
	close_output_file();
    
    strcpy(path, monte_data->alts[alt].alternative_name);

    if (stat(path, &sbuf) < 0) {
	if (errno == ENOENT) {
#ifdef WIN32
		if (mkdir(path) < 0) {
#else
		if (mkdir(path, 0777) < 0) {
#endif
		log_msg(L_ERROR, "Unable to create alternative directory %s",
			path);
		return 1;
	    }
	} else {
	    log_msg(L_ERROR, "Unable to access the current directory!");
	    return 1;
	}
    } else {
#ifndef WIN32
/* TODO Win32 should have a similar test */
	if (!S_ISDIR(sbuf.st_mode)) {
	    log_msg(L_ERROR,
		    "%s is not a directory!  I can't save my output.", path);
	    return 1;
	}
#endif
    }

    strcat(path, "/");
    strcat(path, monte_data->alts[alt].alternative_name);
    strcat(path, ".out");

#ifndef WIN32
    if (access(path, F_OK) == 0)
	log_msg(L_DBGMSG, "Warning: Overwriting existing output file %s.\n",
		path);
#endif

    if (! (outfile = buf_open(path, "w")))
    {
	log_msg(L_ERROR, "unable to open output file %s.\n", path);
	return(1);
    }

    /* write header */
    time(&clock);
    OUTPUT_STRING("%s%s\n","# COMPASS Output Data File v1.0    ",ctime(&clock));
    OUTPUT_STRING("%s%s\n", "#\t(flow input from ",
		  (monte_data->alts[alt].use_flow_from == ARCHIVE_FLOW) ?
		   "archive data)" : "user data)" );
    OUTPUT_STRING("%s%s\n", "#\t(planned spill input from ",
		  (monte_data->alts[alt].use_flow_from == ARCHIVE_FLOW
		   && monte_data->alts[alt].use_spill_from == ARCHIVE_SPILL) ?
		   "archive data)" : "user data)" );
    OUTPUT_STRING("%s%s\n", "#\t(elevation input from ",
		  (monte_data->alts[alt].use_flow_from == ARCHIVE_FLOW
		   && monte_data->alts[alt].use_elev_from == ARCHIVE_ELEV) ?
		  "archive data)" : "user data)" );
    OUTPUT_STRING("%s%s\n", "#\tstats: first, last, total out, mean, median, mode, std.dev, avg.flow\n","");

#ifdef ROUTE_TRACKING    
    OUTPUT_STRING("%s%s\n", "#\t\tbypass out, spillway out, turbine out, transport out", "");
    OUTPUT_STRING("%s%s\n", "#\t\ttotal in, bypass in, spillway in, turbine in, transport in", "");
#endif
    OUTPUT_NEWLINE;
    
    OUTPUT_STRING("%s %s\n",TKS_ALT,monte_data->alts[alt].alternative_name);
    OUTPUT_ITEM("%s %d\n", TKS_YEARS, monte_data->alts[alt].nyears);
    OUTPUT_ITEM("%s %d\n", TKS_GAMES,
		(monte_data->alts[alt].use_flow_from == ARCHIVE_FLOW
		 && monte_data->alts[alt].multi) ?
		monte_data->alts[alt].multi->gamesum :
		monte_data->alts[alt].ngames);
    OUTPUT_NEWLINE;

    return(0);
}


void close_output_file()
{
    buf_close(outfile);
    outfile = NULL;
}


/* construct a display list by copying the default list, then
 * adding all segment with monte output flags.  return the list
 * if successfull, else null.  Called in monte mode only.  Maybe
 * some day this should avoid adding one seg to the list multiple
 * times.
 */

LIST *construct_display_list()
{
    LIST *display_list, *listp, *dfltp;
    struct river_segment *seg;

    display_list = listp = NULL;
    
    /* add segments with monte_flags specified */
    for (seg = river_mouth; seg; seg = seg->next)
    {
        if (seg->output_flags)
        {
            if ( ! (listp = make_list_entry()))
            {
                free_list(display_list, NULL);
                return(NULL);
            }
            listp->item = (void *) seg;
            listp->prev = NULL;
            listp->next = display_list;
            if (display_list)
                display_list->prev = listp;
            display_list = listp;
        }
    }

    /* Make sure list is not empty (various parts of the Monte Carlo
     *  code assume that the list is not empty - this is wrong, but too
     *  complicated to fix right now. So we make sure that this list is
     *  never empty. */
    if (display_list == NULL) {
        /* copy default list, order doesn't matter */
        for (dfltp = default_segment_list; dfltp; dfltp = dfltp->next)
        {
            if ( ! (listp = make_list_entry()))
            {
                free_list(display_list, NULL);
                return(NULL);
            }
            listp->item = dfltp->item;
            listp->prev = NULL;
            listp->next = display_list;
            if (display_list)
                display_list->prev = listp;
            display_list = listp;
        }
    }

    return(display_list);
}


/* write the ouput data for one year.  assumes that this is the current
 * data in memory.  returns 0 on success, non-zero on error.
 */

int write_output_data(year)
    int year;
{
    struct release *rls = current->release_list;
    struct rls_seg *rls_seg;
    int game, did_some_output;
    LIST *dsp_list;
    char scratch[NAME_LEN]; 

    dsp_list = construct_display_list();

    OUTPUT_ITEM("%s %d\n", TKS_YEAR, year);
    while (rls)
    {
		/* write general release info */
		/* format: "release species site date" */
		OUTPUT_STRING("%s %s ", TKS_REL,
			  string_to_word(rls->species->name, scratch));
		OUTPUT_ITEM("%s %d\n",
			string_to_word(rls->site->name, scratch),
			rls->start_date);
		OUTPUT_ITEM ("\t%s %d", TKS_SMOLT_ON,
			 rls->smolt_onset);
		
		/* write hist summary for release input array */
		write_seg_stats(outfile, &rls->stats, FALSE);
		if (current->flow_seg)
		OUTPUT_STRING("\t%s %s\n", TKS_FLOW_SEG,
			  string_to_word(current->flow_seg->seg_name,
					 scratch));
				  
		OUTPUT_NEWLINE;

	rls_seg = rls->rls_seg;
    did_some_output = 0;
	while (rls_seg)
	{
	    /* process this seg if in the default list, or if it is the
         *  last segment in the release list and we have not output
         *  any data for this release yet. *.out files must have some
         *  data for every release - other parts of the program assume
         *  this. */
	    
	    if ((item_in_list(dsp_list, (void *) rls_seg->river_segment)
             || (rls_seg->down == NULL && !did_some_output))
		        && rls_seg->data)
	    {
            did_some_output = 1;
		    OUTPUT_STRING("\t%s %s\n", TKS_RIV_SEG, 
			          string_to_word(rls_seg->river_segment->seg_name,
					         scratch));
		    game = 0;
		    while (rls_seg->data[game])
		    {
		        if (rls_seg->data[game] != (struct rls_seg_data *) -1)
		        {
			    OUTPUT_ITEM("\t%s %d", TKS_GAME, game);
			    write_seg_stats(outfile, &rls_seg->data[game]->stats, FALSE);
		        }
		        game++;
		    }
		    OUTPUT_END("\t", TKS_RIV_SEG,
			       string_to_word(rls_seg->river_segment->seg_name,
					      scratch));
	    }
	    rls_seg = rls_seg->down;
	}


    OUTPUT_END("", TKS_REL, string_to_word(rls->site->name, scratch));
	
	rls = rls->next;
    }
    free_list(dsp_list, NULL);
    return(0);
}


/* routines to write statistics and stat column header */

void write_stat_col_hdr(outfile, flow_seg)
    FILE *outfile;
    struct river_segment *flow_seg;
{
    if (flow_seg) {
	fprintf(outfile, "%s was used as the average flow indicator.\n",
		flow_seg->seg_name);
	log_msg(L_MSG, "%s was used as the average flow indicator.\n",
		flow_seg->seg_name);
    } else {
	fprintf(outfile, "No average flows were calculated for this simulation.\n");
	log_msg(L_MSG, "No average flows were calculated for this simulation.\n");
    }
    fprintf(outfile, "\nStatistics fields: first day, last day, total passage, mean day\n");
    fprintf(outfile, "\tmedian day, mode day, std.dev day, avg.flow\n");
#ifdef ROUTE_TRACKING
    fprintf(outfile,
	    "\tsluiceway out, bypass out, spillway out, RSW out, turbine out, transport out\n");
    fprintf(outfile,
	    "\ttotal in, sluiceway in, bypass in, spillway in, RSW in, turbine in, transport in\n");
#endif    
    fprintf(outfile, "\n");

    log_msg(L_MSG, "\nStatistics fields: first day, last day, total passage, mean day\n");
    log_msg(L_MSG, "\tmedian day, mode day, std.dev day, avg.flow\n");
#ifdef ROUTE_TRACKING
    log_msg(L_MSG, "\tbypass out, spillway out, turbine out, transport out\n");
    log_msg(L_MSG,
	    "\ttotal in, bypass in, spillway in, turbine in, transport in\n");
#endif    
    log_msg(L_MSG, "\n");
}

int write_seg_stats(outfile, stats, print_summary)
    FILE *outfile;
    struct rls_stats *stats;
    int print_summary;
{
    OUTPUT_ITEM("\t%s %3d", TKS_STATS, stats->total.first);
    OUTPUT_ITEM("%s %3d", "", stats->total.last);
    OUTPUT_ITEM("%s\t%.2lf", "", stats->total.total);
    OUTPUT_ITEM("%s\t%.2f", "", stats->total.mean);
    OUTPUT_ITEM("%s\t%.2f", "", stats->total.median);
    OUTPUT_ITEM("%s\t%.2f", "", stats->total.mode);
    OUTPUT_ITEM("%s\t%.2f", "", stats->total.std_dev);
    OUTPUT_ITEM("%s\t%.2f", "", stats->avg_flow);
#ifdef ROUTE_TRACKING
    OUTPUT_ITEM("%s\t%.2f", "", stats->sluiceway_total);
    OUTPUT_ITEM("%s\t%.2f", "", stats->bypass_total);
    OUTPUT_ITEM("%s\t%.2f", "", stats->spillway_total);
    OUTPUT_ITEM("%s\t%.2f", "", stats->rsw_total);
    OUTPUT_ITEM("%s\t%.2f", "", stats->turbine_total);
    OUTPUT_ITEM("%s\t%.2f", "", stats->postmort_transport_total);
    OUTPUT_ITEM("%s\t%.2lf", "", stats->project_in_total);
    OUTPUT_ITEM("%s\t%.2f", "", stats->sluiceway_in_total);
    OUTPUT_ITEM("%s\t%.2f", "", stats->bypass_in_total);
    OUTPUT_ITEM("%s\t%.2f", "", stats->spillway_in_total);
    OUTPUT_ITEM("%s\t%.2f", "", stats->rsw_in_total);
    OUTPUT_ITEM("%s\t%.2f", "", stats->turbine_in_total);
    OUTPUT_ITEM("%s\t%.2f", "", stats->premort_transport_total);
#endif
    OUTPUT_NEWLINE;
	if (stats->transport.total > 0) {
		OUTPUT_ITEM("\t%s %3d", TKS_STATS_TRANSPORT " " TKS_STATS, stats->transport.first);
		OUTPUT_ITEM("%s %3d", "", stats->transport.last);
		OUTPUT_ITEM("%s\t%.2lf", "", stats->transport.total);
		OUTPUT_ITEM("%s\t%.2f", "", stats->transport.mean);
		OUTPUT_ITEM("%s\t%.2f", "", stats->transport.median);
		OUTPUT_ITEM("%s\t%.2f", "", stats->transport.mode);
		OUTPUT_ITEM("%s\t%.2f", "", stats->transport.std_dev);
	    OUTPUT_NEWLINE;
		OUTPUT_ITEM("\t%s %3d", TKS_STATS_INRIVER " " TKS_STATS, stats->inriver.first);
		OUTPUT_ITEM("%s %3d", "", stats->inriver.last);
		OUTPUT_ITEM("%s\t%.2lf", "", stats->inriver.total);
		OUTPUT_ITEM("%s\t%.2f", "", stats->inriver.mean);
		OUTPUT_ITEM("%s\t%.2f", "", stats->inriver.median);
		OUTPUT_ITEM("%s\t%.2f", "", stats->inriver.mode);
		OUTPUT_ITEM("%s\t%.2f", "", stats->inriver.std_dev);
	    OUTPUT_NEWLINE;
	}

    if (print_summary) {
	log_msg(L_MSG, "\t%s: %3d", TKS_STATS, stats->total.first);
	log_msg(L_MSG, "%s %3d", "", stats->total.last);
	log_msg(L_MSG, "%s\t%.2lf", "", stats->total.total);
	log_msg(L_MSG, "%s\t%.2f", "", stats->total.mean);
	log_msg(L_MSG, "%s\t%.2f", "", stats->total.median);
	log_msg(L_MSG, "%s\t%.2f", "", stats->total.mode);
	log_msg(L_MSG, "%s\t%.2f", "", stats->total.std_dev);
	log_msg(L_MSG, "%s\t%.2f", "", stats->avg_flow);
#ifdef ROUTE_TRACKING
	log_msg(L_MSG, "%s\t%.2f", "", stats->sluiceway_total);
	log_msg(L_MSG, "%s\t%.2f", "", stats->bypass_total);
	log_msg(L_MSG, "%s\t%.2f", "", stats->spillway_total);
	log_msg(L_MSG, "%s\t%.2f", "", stats->rsw_total);
	log_msg(L_MSG, "%s\t%.2f", "", stats->turbine_total);
	log_msg(L_MSG, "%s\t%.2f", "", stats->postmort_transport_total);
	log_msg(L_MSG, "%s\t%.2lf", "", stats->project_in_total);
	log_msg(L_MSG, "%s\t%.2f", "", stats->sluiceway_in_total);
	log_msg(L_MSG, "%s\t%.2f", "", stats->bypass_in_total);
	log_msg(L_MSG, "%s\t%.2f", "", stats->spillway_in_total);
	log_msg(L_MSG, "%s\t%.2f", "", stats->rsw_in_total);
	log_msg(L_MSG, "%s\t%.2f", "", stats->turbine_in_total);
	log_msg(L_MSG, "%s\t%.2f", "", stats->premort_transport_total);
#endif
	log_msg(L_MSG, "\n");
    }
	return 0;
}

void compute_passage_stats(struct passage_stats *stats, float *data, int length, int start_date) {
	int i;
	float median_cutoff;
	float median_sum = 0;
	int mode_count = 0;
	float max = -1;

	/* First calculate first, last, mode, and total */
	stats->first = -1;
	stats->total = 0;

	for (i=0; i < length; i++) {
		/* First */
		if (stats->first == -1 && data[i] >= 1) stats->first = i;

		/* Total */
		stats->total += data[i];

		/* Last */
		if (data[i] >= 1) stats->last = i;

		/* Mode */
		if (data[i] > max) {
			max = data[i];
			stats->mode = i;
			mode_count = 1;
		}
		else if (data[i] == max) {
			mode_count++;
			stats->mode = (stats->mode * (mode_count - 1) + i)/mode_count;
		}
	}

	/* Now calculate mean, median, and standard deviation */
	stats->median = 0;
	stats->mean = 0;
	stats->std_dev = 0;

    i = 0;
	median_cutoff = stats->total / 2.0;
    while (i < length)
    {
		float freq = data[i] / stats->total;
		stats->mean += i * freq;
		stats->std_dev += i * i * freq;
		if (stats->median == 0.0) /* median not yet set */
		{
			if (median_sum + data[i] > median_cutoff)
				stats->median = i + (median_cutoff - median_sum) 
													/ data[i];

			median_sum += data[i];
		}
		i++;
    }

    /* adjust median to be "sensible" for a point release - i.e. that
     *  would get a calculated median at the middle of the timestep, which
     *  is moved back to the beginning of the timestep. Non-point releases
     *  are adjusted similarly to make the median function seem smooth.
     */
    stats->median -= 0.5;

    /* finish some calculations */
    stats->std_dev = sqrt(stats->std_dev - stats->mean * stats->mean);

	/* Convert from time steps to days */
    stats->first = start_date + stats->first/STEPS_PER_DAY;
    stats->last = start_date + stats->last/STEPS_PER_DAY;
    stats->mean = start_date + stats->mean/STEPS_PER_DAY;
    stats->median = start_date + stats->median/STEPS_PER_DAY;
    stats->mode = start_date + stats->mode/STEPS_PER_DAY;
    stats->std_dev = stats->std_dev/STEPS_PER_DAY;
}
