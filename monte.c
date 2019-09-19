/* Monte Carlo module. */

#include <stdio.h>

#include <string.h>
#include <stdlib.h>

#include "crisp.h"			/* includes period.h */
#include "monte.h"
#include "log.h"
#include "run_mode.h"
#include "batch.h"
#include "init.h"
#include "release.h"
#include "flow.h"
#include "realtime.h"
#include "token.h"
#include "monte_index.h"
#include "monte_output.h"
#include "spill.h"
#include "parseutil.h"
#include "parse.h"
#include "main.h"
#include "writeutil.h"

#ifndef MAXPATHLEN
#define MAXPATHLEN (1024)
#endif

/*
 *  Initiation: compass has been invoked and has come up in the normal
 *  fashion.  The user has indicated in some fashion that monte carlo mode
 *  is to be invoked.
 */

void free_game_spills(struct archive_spill **game_spills);

void free_ftells_alt(alt_off, nyears)
	long **alt_off;
	int nyears;
{
    if (!alt_off) return;
    for ( ; nyears > 0 ; nyears--) {
	if (alt_off[nyears-1]) free(alt_off[nyears-1]);
    }
    free(alt_off);
}

static void free_ftells(offsets, nalts, alts, monte_data)
    long ***offsets;
    int nalts;
    struct archive_header **alts;
    struct monte_carlo_data *monte_data;
{
    if (!offsets) return;
    for ( ; nalts > 0 ; nalts--) {
	if (offsets[nalts-1] && alts[nalts-1]
	    && !monte_data->alts[nalts-1].multi)
	    free_ftells_alt(offsets[nalts-1], alts[nalts-1]->nyears);
    }
    free(offsets);
}

/**
 * \brief Frees everything that needs to be
 *	freed in the case of an error unearthed in the get_and_check_archives
 *	routine.
 */
static void monte_check_error(alts, nalts, offsets, monte_data)
    struct archive_header **alts;
    int nalts;
    long ***offsets;
    struct monte_carlo_data *monte_data;
{
    free_ftells(offsets, nalts, alts, monte_data);
    free_archive_headers(alts, nalts);
}

/* routine to do the cleanup on a monte error */

static void monte_error_cleanup(archive_data, monte_data, game_spills,
				arch_fp, ftells)
    struct archive_header **archive_data;
    struct monte_carlo_data *monte_data;
    struct archive_spill **game_spills;
    FILE *arch_fp;
    long ***ftells;
{
    free_game_spills(game_spills);
    free_ftells(ftells, monte_data->nalternatives, archive_data, monte_data);
    free_archive_headers(archive_data, monte_data->nalternatives);
    buf_close(arch_fp);
    close_output_file();
	run_mode.running = FALSE;
}



/** \brief allocate storage for a game's worth of archive spill info.  an example
 * of accessing data in this array is
 * 	planned_spill =  game_spills[dam][period].planned;
 */

struct archive_spill **alloc_game_spills(ndams, nperiods)
    int ndams;
    int nperiods;
{
    int i;
    struct archive_spill **game_spills =
	(struct archive_spill **) malloc(ndams
					  * sizeof(struct archive_spill *));

    if (!game_spills)
    {
	log_msg(L_ERROR, "no mem for spills\n");
	return(game_spills);
    }

    if (! (game_spills[0] =
	(struct archive_spill *) malloc(ndams * nperiods
					 * sizeof(struct archive_spill))))
    {
	log_msg(L_ERROR, "o mem for spills\n");
	free(game_spills);
	game_spills = NULL;
	return(game_spills);
    }

    for (i=1; i < ndams; i++)
	game_spills[i] = game_spills[0] + i * nperiods;

    return(game_spills);
}


/** \brief routine to free game array of archive spill info */

void free_game_spills(game_spills)
    struct archive_spill **game_spills;
{
    if (game_spills)
    {
	free(game_spills[0]);
	free(game_spills);
    }
}

/**
 * \brief Returns the
 *	offsets pointer from the struct archive_map passed in, freeing
 *	everything else.
 */

static long **strip_offset_from_map(map)
	struct archive_map *map;
{
    long **ret_ptr;
    int i;

    ret_ptr = map->offsets;
    for (i = 0 ; i < map->ngames ; i++)
	free(map->gamenames[i]);
    free(map->gamenames);
    free(map);
    return ret_ptr;
}

/**
 * \brief Check flow archive files.
 *
 * \return a valid pointer to number of alternatives pointers to archive
 *	header structures if it could find and parse all the archive files
 *	appropriately, and returns a pointer to # of alternatives pointers
 *	to # of games in the appropriate alternative ftell pointers to
 *	locations of games in the archive through the ftells parameter,
 *	if successful,
 * Zero if unsuccessful.
 *
 * Note: some of these error messages should be made clearer.
 */

struct archive_header **get_and_check_archives(monte_data, ftells)
	struct monte_carlo_data *monte_data;
	long ****ftells;
{
    extern struct archive_header *parse_archive_header();
    extern struct archive_map *open_and_map_flow();

    struct archive_header **headers;
    struct archive_map *map;
    long ***ft;
    int alt;

    /* Freed in run_monte_carlo, or in this structure on an error. */
    headers = (struct archive_header **) malloc(monte_data->nalternatives*
		sizeof(struct archive_header *));
    if (headers) bzero(headers, monte_data->nalternatives *
		sizeof(struct archive_header *));
    ft= *ftells= (long ***) malloc(monte_data->nalternatives*sizeof(long **));
    if (*ftells) bzero(*ftells, monte_data->nalternatives*sizeof(long **));
    if (!(headers && *ftells)) {
	log_msg(L_ERROR, "Out of memory checking headers!\n");
	if (headers) free(headers);
	if (*ftells) free(*ftells);
	return (struct archive_header **) 0;
    }

    for (alt = 0; alt < monte_data->nalternatives; alt++, ft++)
    {
	if (monte_data->alts[alt].use_flow_from != ARCHIVE_FLOW)
	    continue;

	if (!(headers[alt] = parse_archive_header(alt, monte_data))) {
	    log_msg(L_ERROR, "Can't parse header for alternative %s\n",
		    monte_data->alts[alt].alternative_name);
	    monte_check_error(headers, alt, *ftells, monte_data);
	    return (struct archive_header **) 0;
	}
	if (monte_data->alts[alt].multi) {
	    if (headers[alt]->nyears != monte_data->alts[alt].multi->nyears) {
		log_msg(L_ERROR, "%s in alternative %s!\n",
			"Archive # of years not the same as multi # of years",
			monte_data->alts[alt].alternative_name);
		monte_check_error(headers, alt+1, *ftells, monte_data);
		return (struct archive_header **) 0;
	    }
	    if (headers[alt]->ngames != monte_data->alts[alt].multi->ngames) {
		log_msg(L_ERROR, "%s in alternative %s!\n",
			"Archive # of games not the same as multi # of games",
			monte_data->alts[alt].alternative_name);
		monte_check_error(headers, alt+1, *ftells, monte_data);
		return (struct archive_header **) 0;
	    }
	    if (!(*ft = monte_data->alts[alt].multi->ftell)) {
		if (!(map = open_and_map_flow(monte_data->alts + alt))) {
		    monte_check_error(headers, alt+1, *ftells, monte_data);
		    return (struct archive_header **) 0;
		}
		*ft = monte_data->alts[alt].multi->ftell =
		    strip_offset_from_map(map);
	    }
	} else {
	    if (headers[alt]->ngames < monte_data->alts[alt].ngames) {
		log_msg(L_ERROR, "%s alternative %s!\n",
			"Not enough games in flow archive to run",
			monte_data->alts[alt].alternative_name);
		monte_check_error(headers, alt+1, *ftells, monte_data);
		return (struct archive_header **) 0;
	    }
	    if (!(map = open_and_map_flow(monte_data->alts + alt))) {
		monte_check_error(headers, alt+1, *ftells, monte_data);
		return (struct archive_header **) 0;
	    }
	    *ft = strip_offset_from_map(map);
	}
    }
    return headers;
}


/**
 * \brief  The main
 *	shebang.  This function runs the monte carlo version of the model.
 *
 *	It cycles through the available alternatives, each alternative
 *	running the number of games and years contained in it.  Much more
 *	to it than that, but this is only a comment.
 */
int run_monte_carlo(monte_data)
    struct monte_carlo_data *monte_data;	/* all data specified above */
{
    int read_game;	/* The game number from the archive */
    int m_game;		/* multi game number */
    int m_count;	/* multi count per game */
    struct archive_header **archive_data = 0;
    struct archive_spill **game_spills = 0;
    char archive_path[MAXPATHLEN];
    FILE *arch_fp;
    long ***ftells = 0;
    int alt;
	int game = 0;
	int year = 0;

    if (!(archive_data = get_and_check_archives(monte_data, &ftells)))
	return(-1);
 
    if (!BATCH_MODE_ACTIVE) {
	set_monte_gauges(0, 0, 0, monte_data);
    }

	run_mode.running = TRUE;

    for (alt = 0; alt < monte_data->nalternatives; alt++) /* loop over alts */
    {
	/* use fixed flow instead of flow archive if requested */
	int fixed_flow = !(monte_data->alts[alt].use_flow_from == ARCHIVE_FLOW);
	
	if (new_outfile(alt, monte_data))
	    continue;			/* couldn't open output file */

	if (!fixed_flow)
	{
	    (void) tilde(monte_data->alts[alt].flow_archive, archive_path);
#ifdef WIN32
		if (archive_path[0] != '/' && archive_path[1] != ':') {
#else
		if (archive_path[0] != '/') {
#endif
		strcpy(archive_path, monte_data->alts[alt].load_directory);
		strcat(archive_path, "/");
		strcat(archive_path, monte_data->alts[alt].flow_archive);
	    }
	    if (!(arch_fp = buf_open(archive_path, "r"))) {
		close_output_file();
		continue;
	    }

	    /* get the #games and #years for which flow information
	       is available and verify that this is sufficient for those
	       requested by the user in monte_data.  Also allocate and fill
	       in archive_header data structure, including constructing dam
	       list and period list.  Return the new header struct.
	       */

	    if (! (game_spills = alloc_game_spills(
                    archive_data[alt]->ndams + archive_data[alt]->nreaches,
					archive_data[alt]->nperiods)))
	    {
		monte_error_cleanup(archive_data, monte_data, game_spills,
				    arch_fp, ftells);
		return(-1);		/* error */
	    }
	}
	else arch_fp=0;
	
	/* loop over years */
	for (year = 0; year < monte_data->alts[alt].nyears; year++)
	{
	    /* read in all compass data for this year using existing parser
	       after freeing all memory associated with existing releases and
	       transports */
	    if (monte_data->alts[alt].input_files[year].list)
	    {
		int count=0;
		while (monte_data->alts[alt].input_files[year].list[count] &&
		       *monte_data->alts[alt].input_files[year].list[count])
		{
		    if ( ! parse_datafile(monte_data->
				   alts[alt].input_files[year].list[count],
				   1, 0)) /* replace TRUE, refresh FALSE */
		    {
			monte_error_cleanup(archive_data, monte_data,
					    game_spills, arch_fp, ftells);
			return(-1);			/* error */
		    }

		}
	    }
	    else if (year == 0 ||
		 strcmp(monte_data->alts[alt].input_files[year].file_name,
			monte_data->alts[alt].input_files[year-1].file_name))
	    {
		if ( ! parse_datafile(monte_data->
			       alts[alt].input_files[year].file_name,
			       1, 0)) /* replace TRUE, refresh FALSE */
		{
		    monte_error_cleanup(archive_data, monte_data,
					game_spills, arch_fp, ftells);
		    return(-1);		/* error */
		}
	    }
	    set_planned_spill();
	    
	    if (simulation_init(MONTE))
	    {
		log_msg(L_ERROR, "aborting monte carlo simulation\n");
		monte_error_cleanup(archive_data, monte_data, game_spills,
				    arch_fp, ftells);
		return(-1);		/* error */
	    }

	    m_game  = -1;
	    m_count = 0;

	    /*loop over games*/
	    for (game = 0; game < monte_data->alts[alt].ngames ; game++) 
	    {
		if (fixed_flow || !monte_data->alts[alt].multi)
		    read_game = game;
		else {

		/* m_count stores the number of times the current game has
		   left to be used.  m_game stores the number of the active
		   game from the flow file. 				*/

		    while (!m_count) {
			m_game++;
			if (m_game >= monte_data->alts[alt].multi->ngames) {
			    log_msg(L_ERROR,
				    "Programmer error in monte!\n");
			    monte_error_cleanup(archive_data, monte_data,
						game_spills, arch_fp, ftells);
			    return(-1);
			}
			m_count=monte_data->alts[alt].multi->nruns[m_game];
		    }
		    m_count--;
		    read_game = m_game;
		}

		log_msg(L_MSG, "Monte: Running game #%d.\n", read_game);

		if (!fixed_flow)
		{
		    /* Put flow into existing dam structures, as
		       step function (all days in a period have equal values).
		       Put spill data for this game into archive period
		       dependent temporary structure.
		       */

		    read_flow_and_spill_from_archive(read_game, year,
		        archive_data[alt],
			game_spills, arch_fp,
			ftells[alt],
			monte_data->alts[alt].use_elev_from);

            propogate_elev_change( NULL, 0 );

		    /* Flow modulation.  Required parameter input data is year
		       of simulation (for day of week calculations).  All other
		       input is taken directly from the data in the dam structs
		       at this time (i.e. periods are deduced from the flow
		       arrays, active dams are determined by the
		       presence/absence of data, etc.)  In other words, the
		       modulation routine is entirely independent of SAM or
		       archived flow data.
		       */

		    /* standard year: Jan 1 falls on
		     * a Monday; non-leap-year
		     */
		    if (!(current->flags & VAR_SUPPRESS_FLAG))
			modulate_flows(archive_data[alt]->dam_list,
                                 archive_data[alt]->reach_list);

		    /* Fill in spill schedules based on data already in dam
		       structs as derived from spill schedule input file, and
		       archive spill data.  Note that the periods described in
		       these two separate data files must be compatible.
		       */
		    monte_compute_spill_schedules(archive_data[alt],
						  game_spills,
						  monte_data->
						    alts[alt].use_spill_from);

		    /* Propagate flow upstream, treating non-archive dams as
		       unregulated.  This will fill all the headwaters based
		       on flows at the archive dams.  The downstream flow
		       calculation which automatically occurs will fill in
		       the remaining dams along with all other segments.
		       */
		    monte_prop_upstream(archive_data[alt]->dam_list,
                                 archive_data[alt]->reach_list);
		}
		else
		{   /* fixed flow, compute spills scenario style */
		    set_planned_spill();
		}
		/* run this scenario using the existing routine. */
		if (run_the_model(game) != 0)
		{
		    monte_error_cleanup(archive_data, monte_data, game_spills,
					arch_fp, ftells);
		    return(-1);  /* error */
		}

		/* write the general output summary file if desired */
		if (monte_data->flags & SUMMARY) 	
		    write_output_summary(game, year, alt);

#ifdef REALTIME
		if (monte_data->flags & MF_REALTIME_MONTE)
		    realtime_output_datafile(current);
#endif

		/* prune the tree if not last game in simulation */
		calculate_stats_and_prune_tree(game, 
			(int) ! ((alt == (monte_data->nalternatives - 1))
			   && (year == (monte_data->alts[alt].nyears - 1))
			   && (game == (monte_data->alts[alt].ngames - 1))));
		if (!BATCH_MODE_ACTIVE) {
		    set_monte_gauges(game+1, year, alt, monte_data);
		}
	    }				/* end game loop */

	    /* stats are gathered for all the games in a year, then written to
	       an output file, for later detailed analysis.  
	       */
	    write_output_data(year);
	    if (!BATCH_MODE_ACTIVE) {
		set_monte_gauges(0, year+1, alt, monte_data);
	    }
	}				/* end year loop */
	if (!fixed_flow)
	{
	    free_game_spills(game_spills);
	    if (!monte_data->alts[alt].multi) {
		free_ftells_alt(ftells[alt], archive_data[alt]->nyears);
		ftells[alt] = 0;
	    }
	    free_archive_header(archive_data[alt]);
	    buf_close(arch_fp);
	}
	archive_data[alt] = 0;
	close_output_file();
	make_monte_index_from_output(monte_data->alts[alt].alternative_name);
	altern_report_account_for(monte_data->alts[alt].alternative_name);
	if (!BATCH_MODE_ACTIVE) {
	    set_monte_gauges(0, 0, alt+1, monte_data);
	}
    }					/* end alternative loop */
    free(ftells);
    free(archive_data);
    if (!BATCH_MODE_ACTIVE) {
	set_monte_gauges(game, year, alt, monte_data);
    }
    return(0);
}

