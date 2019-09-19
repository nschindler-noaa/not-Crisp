#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#ifdef WIN32
#include <malloc.h>
#else
#include <unistd.h>
#include <strings.h>
#include <malloc.h>
#include <sys/param.h>
#endif //WIN32

#include "crisp.h"
#include "parseutil.h"
#include "tokens_archive.h"
#include "river.h"
#include "monte.h"

#ifndef MAXPATHLEN
#define MAXPATHLEN (1024)
#endif

static COMMAND_LIST arc_head_list = 0;	/* The list of archive_tokens */

/* This routine parses the section of the archive header listing the dams,
 * creates a dam_list, and returns it, for assignment to archive_data->dam_list.
 * This dam list is a mapping containing the addresses of the actual river
 * segments which correspond to the dams found in the archive header.
 * The dam_list returned is null-terminated, has ndams+1 entries (1 extra for
 * the null terminator) and has the value -1 for any dam which is not found
 * in the river as currently configured.
 */

static struct river_segment **parse_archive_dam(buffer, ndams)
	int ndams;
	FILE *buffer;
{
    struct river_segment **dams = 0;
    char token[MAX_COMMAND_LEN+1];
    int count;
    
    /* Freed ??? */
    if (!(dams = (struct river_segment **)
	  malloc(sizeof(struct river_segment *) * (unsigned)(ndams+1) )))
    {
	log_msg(L_FATAL, "No memory for dam structure");
	return ((struct river_segment **)0);
    }
    bzero((char *)dams, sizeof(struct river_segment *) * (ndams+1));

    for (count=0; count < ndams; count++)
    {
	if (pop_token(buffer, token, "dam abbreviation ->") == EOF)
	{
	    p_error(buffer, "Unexpected EOF while parseing the dams.%s\n","");
	    return(0);
	}
	if (!( *(dams+count) = find_seg_by_abbrev(river_mouth, token) ))
	{
	    p_warning(buffer,
		    "Dam abbreviation not found.  The Dam {%s} can't be used.\n",
		    token);
	    *(dams+count) = (struct river_segment *) -1;
	}
    }

    return (dams);
}

/* Same as above, but for reaches.
 */
static struct river_segment **parse_archive_reach(buffer, nreaches)
	int nreaches;
	FILE *buffer;
{
    struct river_segment **reaches = 0;
    char token[MAX_COMMAND_LEN+1];
    int count;
    
    /* Freed ??? */
    if (!(reaches = (struct river_segment **)
	  malloc(sizeof(struct river_segment *) * (unsigned)(nreaches+1) )))
    {
	    log_msg(L_FATAL, "No memory for reach structure");
	    return ((struct river_segment **)0);
    }
    bzero((char *)reaches, sizeof(struct river_segment *) * (nreaches+1));

    for (count=0; count < nreaches; count++)
    {
	    if (pop_token(buffer, token, "reach abbreviation ->") == EOF)
	    {
	        p_error(buffer, "Unexpected EOF while parseing the reaches.%s\n","");
	        return(0);
	    }
	    if (!( *(reaches+count) = find_seg_by_abbrev(river_mouth, token) ))
	    {
	        p_warning(buffer,
		        "Reach abbreviation not found.  The Reach {%s} can't be used.\n",
		        token);
	        *(reaches+count) = (struct river_segment *) -1;
	    }
    }

    return (reaches);
}

/*
 * static void free_archive_header(struct archive_header *header) - Frees the
 *	struct archive_header passed in, with all doodads contained within.
 *
 * Note: Does not check to see if it's passed a null pointer.  (Although
 *	it does check the doodads for nulls.
 */
void free_archive_header(header)
	struct archive_header *header;
{
    PERIOD_LIST *per;

    /* This is a list of pointers, but the sub-pointers are not freed here.*/
    if (header->dam_list)
	    free((char *) header->dam_list);
    if (header->reach_list)
	    free((char *) header->reach_list);
    while ((per = header->periods)) {
	header->periods = per->next;
	free((char *) per);
    }
    free((char *) header);
}

/*
 * free_archive_headers (struct archive_header **archive_data, int nheaders) -
 *	Frees an array of nheaders pointers to struct archive_header.
 */

void free_archive_headers (archive_data, nheaders) 
	struct archive_header **archive_data;
	int nheaders;
{
    int i;

    if (!archive_data) return;
    for ( i = 0 ; i < nheaders ; i++ ) {
	if (archive_data[i]) free_archive_header(archive_data[i]);
    }
    free ((char *)archive_data);
    return;
}

/*
 * struct archive_header *parse_arc_header(char *archive_file) - Parses
 *	the header only of the flow archive file passed in, returning the
 *	structure into which the parsed data is stored.
 */
static struct archive_header *parse_arc_header(archive_file)
	char *archive_file;
{
    FILE *fp;
    char token[MAX_COMMAND_LEN+1], scratch[MAX_COMMAND_LEN+1];

    /* Freed in this routine, on error.  Otherwise ??? */
    struct archive_header *archive_data = (struct archive_header *)
	malloc (sizeof(struct archive_header));

    if (!archive_file) return 0;

    if (!archive_data)
    {
	log_msg(L_FATAL, "No memory for archive_header\n");
	return((struct archive_header *)0);
    }
    bzero((char *)archive_data, sizeof(struct archive_header));

    /* set up the menu */
    if (!arc_head_list)
    {
	arc_head_list = com_create_list(arc_head_list, arc_head_tokens);
    }

    /* open file using buf_open */
    if (!(fp = buf_open (archive_file, "r")) )
    {
	log_msg(L_ERROR, "Can't open file {%s}\n", archive_file);
	free(archive_data);
	return((struct archive_header *)0);
    }

    /* parse identification line */
    if (pop_token(fp, token, "header -> ") == EOF	||
	sscanf(token, "V%d.%d", &archive_data->maj_vers,
	       &archive_data->min_vers) != 2 ||
	pop_token(fp, token, "header -> ") == EOF	||
	strcmp( token, TKS_CRISP_ID) 				||
	pop_token(fp, token, "header -> ") == EOF	||
	strcmp( token, TKS_FLOW_ID) 				||
	pop_token(fp, token, "header -> ") == EOF	||
	strcmp( token, TKS_ARCHIVE_ID) 			          )
    {
	p_error(fp, "File {%s} is not a %s %s %s.\n",
		archive_file, TKS_CRISP_ID, TKS_FLOW_ID, TKS_ARCHIVE_ID );
	buf_close(fp);
	free(archive_data);
	return((struct archive_header *)0);
    }
    reset_input(fp);

    while (pop_token(fp, token, "archive command -> ") != EOF)
    {
       /* token  based case stmt */
	switch (com_reference(arc_head_list, lowercase(token, scratch)))
	{

	case TK_DAM:
	    /* dams */
	    if (!read_int (fp, &archive_data->ndams, "number of dams ->"))
	    {
		p_error(fp,
			"Error parsing # of dams in archive file {%s}. Line discarded.\n",
			archive_file);
		reset_input(fp);
		break;
	    }
	    archive_data->dam_list=parse_archive_dam(fp, archive_data->ndams);
	    break;

	case TK_REACH:
	    /* dams */
	    if (!read_int (fp, &archive_data->nreaches, "number of reaches ->"))
	    {
		p_error(fp,
			"Error parsing # of reaches in archive file {%s}. Line discarded.\n",
			archive_file);
		reset_input(fp);
		break;
	    }
	    archive_data->reach_list=parse_archive_reach(fp, archive_data->nreaches);
	    break;

	case TK_GAME:
	    /* # games */
	    if (!read_int (fp, &archive_data->ngames, "number of games ->"))
	    {
		p_error(fp,
			"Parsing # of games in archive file {%s}. Line discarded.\n",
			archive_file);
		reset_input(fp);
		break;
	    }
	    break;

	case TK_YEAR:
	    /* # years */
	    if (!read_int (fp, &archive_data->nyears, "number of years ->"))
	    {
		p_error(fp,
			"Parsing # of years in archive file {%s}. Line discarded.\n",
			archive_file);
		reset_input(fp);
		break;
	    }
	    break;

	case TK_PERIOD:
	    /* periods */
	    if (!read_int(fp, &archive_data->nperiods, "number of periods ->"))
	    {
		p_error(fp,
			"Parsing # of periods in archive file {%s}. Line discarded.\n",
			archive_file);
		reset_input(fp);
		break;
	    }
	    archive_data->periods = parse_periods(fp);
	    break;

	case TK_BEGIN_DATA:
	    buf_close(fp);

        /* Special handling: reach_list may or may not have been defined
         *  in flow archive. If not, zero it out here.
         */
        if (archive_data->reach_list == NULL) {
            archive_data->reach_list = (struct river_segment **)
	                                malloc(sizeof(struct river_segment *) );
            archive_data->reach_list[0] = NULL;
        }

	    return (archive_data);

	default:
	    /* default - output some error message */
	    UNKNOWN_COMMAND_ERROR(fp, token);
	}
    }

    /* Special handling: reach_list may or may not have been defined
     *  in flow archive. If not, zero it out here.
     */
    if (archive_data->reach_list == NULL) {
        archive_data->reach_list = (struct river_segment **)
	                            malloc(sizeof(struct river_segment *) );
        archive_data->reach_list[0] = NULL;
    }

    buf_close(fp);
    return (archive_data);
}

/*
 * struct archive_header *parse_archive_header_in_alternative(struct
 *	altern_data *altern) -- This particular function in the series
 *	just uses the altern.
 */
struct archive_header *parse_archive_header_in_alternative(altern)
	struct altern_data *altern;
{
    char archive_path[MAXPATHLEN];

    if (!altern->flow_archive)
	return 0;

    (void) tilde(altern->flow_archive, archive_path);
#ifdef WIN32
		if (archive_path[0] != '/' && archive_path[1] != ':') {
#else
		if (archive_path[0] != '/') {
#endif
	strcpy(archive_path, altern->load_directory);
	strcat(archive_path, "/");
	strcat(archive_path, altern->flow_archive);
    }
    return parse_arc_header(archive_path);
}

/*
 * struct archive_header *parse_archive_header(int alt,
 *	struct monte_carlo_data *users_data) - Grabs the alternative, ngames,
 *	and nyears items as appropriate from users_data and calls
 *	parse_arc_header.
 */
struct archive_header *parse_archive_header (alt, users_data)
    int alt;
    struct monte_carlo_data *users_data;
{
    if (alt >= users_data->nalternatives)
    {
	log_msg(L_ERROR,
	  "alt is >= users_data->nalternatives in parse_archive_header()\n");
	return((struct archive_header *)0);
    }
    return parse_archive_header_in_alternative(&users_data->alts[alt]);
}

int skip_lines (buffer, num_lines) /* put in parseutil.c ^..^ */
    FILE *buffer;
    int num_lines;
{
    int count=0;
    while (count < num_lines && !feof(buffer))
	if (getc(buffer) == '\n') count++;
    return (feof(buffer) ? EOF : 0);
}

/*
 * void map_error(struct archive_header *header, struct archive_map *archive,
 *	int number) - The routine called when open_and_map_flow exits with an
 *	error.  Frees all of the appropriate variables.
 */

static void map_error(header, archive, number)
	struct archive_header *header;
	struct archive_map *archive;
	int number;
{
    int nyears, i;

    nyears = header->nyears;
    if (header) {
	free_archive_header(header);
    }
    if (archive) {
	if (archive->offsets) {
	    for (i = 0 ; i < nyears ; i++) {
		if (archive->offsets[i]) free(archive->offsets[i]);
		else break;
	    }
	    free(archive->offsets);
	}
	if (archive->gamenames) {
	    for (i = 0 ; i < number ; i++)
		free(archive->gamenames[i]);
	    free(archive->gamenames);
	}
	free(archive);
    }
}

/*
 * int open_and_map_flow(char *filename, struct flow_archive *archive) -
 *	This routine (WILL) calls parse_arc_header, and stores ftell pointers
 *	to each game in the archive, making a list of the names as we go.
 *	It puts all the data into archive, passed in.
 */
struct archive_map *open_and_map_flow(alt)
	struct altern_data *alt;
{
    extern char *tilde();

    FILE *fp;
    struct archive_header *head = 0;
    struct archive_map *archive = 0;
    char token[MAX_COMMAND_LEN + 1], scratch[MAX_COMMAND_LEN+1];
    char buf[256];
    char archive_path[MAXPATHLEN];
    int i;
    int game_c = 0;
	int year_c;

    if (!alt) return 0;
    if (!(head = parse_archive_header_in_alternative(alt))) return 0;
	if (head->nyears <= 0) return 0;
    /* Freed ??? */

    archive = (struct archive_map *) malloc(sizeof(struct archive_map));
    if (archive) {
	bzero((char *)archive, sizeof(struct archive_map));
	archive->offsets = (long **) malloc(head->nyears * sizeof(long *));
	if (archive->offsets) {
	    bzero((char *)archive->offsets, head->nyears * sizeof(long *));
	    archive->gamenames = (char **) malloc(head->ngames*sizeof(char *));
	    if (archive->gamenames) bzero((char *)archive->gamenames,
			head->ngames*sizeof(char *));
	    for (i = 0 ; i < head->nyears ; i++) {
		archive->offsets[i]=(long *) malloc(head->ngames*sizeof(long));
		if (!archive->offsets[i]) {
		    /* A hack so the next check will work */
		    free(archive->gamenames);
		    archive->gamenames = 0;
		    break;
		}
	    }
	}
    }
    if (!(archive && archive->offsets && archive->gamenames)) {
	log_msg(L_ERROR, "Unable to allocate memory for archive map!\n");
	map_error(head, archive, 0);
	return (struct archive_map *) 0;
    }
    (void) tilde(alt->flow_archive, archive_path);
#ifdef WIN32
		if (archive_path[0] != '/' && archive_path[1] != ':') {
#else
		if (archive_path[0] != '/') {
#endif
	strcpy(archive_path, alt->load_directory);
	strcat(archive_path, "/");
	strcat(archive_path, alt->flow_archive);
    }
    if (!(fp = buf_open(archive_path, "r"))) {
	map_error(head, archive, 0);
	return (struct archive_map *) 0;
    }
    if (!arc_head_list) {
	arc_head_list = com_create_list(arc_head_list, arc_head_tokens);
    }

    /* Set the pointer to the beginning of data */
    do {
	if (pop_token(fp, token, "data ->") == EOF) {
	    log_msg(L_ERROR, "Couldn't find a start of data token!\n");
	    buf_close(fp);
	    map_error(head, archive, 0);
	    return (struct archive_map *) 0;
	}
    } while(com_reference(arc_head_list, lowercase(token, scratch)) !=
	    TK_BEGIN_DATA);
    for (year_c = 0 ; year_c < head->nyears ; year_c++) {
	for (/* game_c already initialized */; game_c < head->ngames ; game_c++) {
	    archive->offsets[year_c][game_c] = ftell(fp);

	    /* Used fgets, rather than pop_token because pop_token has
	       internal buffering which we don't want to use here.      */
	    fgets(buf, sizeof(buf), fp);
	    sscanf(buf, "%*s %s %*d %*s %*f %*f %*f %*d", token);

	    /* Freed ??? */
	    if (!archive->gamenames[game_c]) {
		if ((archive->gamenames[game_c] = (char *)
		    malloc((strlen(token)+1) * sizeof(char))))
		    strcpy(archive->gamenames[game_c], token);
		else {
		    log_msg(L_ERROR, "Out of memory in archive map!\n");
		    buf_close(fp);
		    map_error(head, archive, (year_c) ? head->ngames : game_c);
		    return (struct archive_map *) 0;
		}
	    }

	    if (skip_lines(fp, (head->ndams + head->nreaches)
                                            * head->nperiods - 1) == EOF)
		break;
	}
	if (feof(fp)) 
	    break;
    }
    if (pop_token(fp, token, "data ->") != EOF) {
	log_msg(L_ERROR,
		"Found more games in archive than header claims!\n");
	map_error(head, archive, (year_c) ? head->ngames : game_c + 1);
	buf_close(fp);
	return (struct archive_map *) 0;
    }
    buf_close(fp);
    if (year_c < head->nyears || game_c < head->ngames) {
	log_msg(L_ERROR, "Flow archive file is shorter than expected\n");
	map_error(head, archive, (year_c) ? head->ngames : game_c + 1);
	return (struct archive_map *) 0;
    }
    archive->ngames = head->ngames;
    archive->nyears = head->nyears;
    free_archive_header(head);
    return archive;
}

/* this routine reads flow and spill information for one game from the
 * current archive file.  flows at the appropriate dams are set, and
 * period spill information is placed in the passed in game_spills
 * variable.  the file pointer will be automatically advanced to the proper
 * location for the requested game/year; it cannot be rewound - a request
 * to do this generates an error condition.
 *	NOTE:  when function parameters game and year are both 0, it is
 * assumed that the file pointer is at the beginning of the data section;
 * this could be considered an intitialization of sorts.
 */

void read_flow_and_spill_from_archive (game, year, archive_data, game_spills,
	fp, tells, archive_elevation_flag)
	int game, year;
	struct archive_header *archive_data;
	struct archive_spill **game_spills;
	FILE *fp;
	long **tells;
        int archive_elevation_flag;	/* use elevations from archive? */
{
    static int cur_game=0, cur_year=0;

    char token[MAX_COMMAND_LEN+1];
    char dam_abbrev[MAX_COMMAND_LEN+1];
    struct period_list_struct *period;
    int reach, dam, ndams, count, per_count;
    float elev_temp;

    if (game == 0 && year == 0)
	cur_game = cur_year = 0;

    /* if the desired year or game doesn't exist... */
    if ( (game >= archive_data->ngames) || /* if asking for a game or year that doesn't */
	 (year >= archive_data->nyears)  ) /* exist in the data file ...                */
    {
	/* wheee! */
	p_error (fp,
		 "%s\n\t%s %d and game %d.\n\t%s %d and max_game %d.\n",
		 "The data file doesn't have data for the game or year you want.",
		 "Looking for year",
		 year, game,
		 "The file has max_year",
		 archive_data->nyears, archive_data->ngames);
	return;
    }

    /* Move to the appropriate spot in the file */
    fseek(fp, tells[year][game], 0);

    log_msg(L_DBGMSG, "Reading flow archive for year %d game %d\n",
	    year, game);

    /* clear dam and reach flow data */
    for (dam=0; dam < archive_data->ndams; dam++)
	if (archive_data->dam_list[dam] != (struct river_segment *) -1) {
	    bzero((char *)archive_data->dam_list[dam]->current->dam.flow,
		  DAYS_IN_SEASON * sizeof(float));

	    /* Make sure that the up segment from each dam is a reach or a
	       headwater */
	    if (archive_data->dam_list[dam]->up->seg_type != REACH_SEG &&
		archive_data->dam_list[dam]->up->seg_type != HEADWATER_SEG)
		log_msg(L_ERROR,
			"%s\n Segment above %s (%s) is not %s\n",
			"River configuration error detected!",
			archive_data->dam_list[dam]->seg_name,
			archive_data->dam_list[dam]->up->seg_name,
			"a reach or headwater"
			);
	}
    for (reach=0; reach < archive_data->nreaches; reach++) 
    if (archive_data->reach_list[reach] != (struct river_segment *)-1) {
        bzero((char *)archive_data->reach_list[reach]->current->reach.flow,
                DAYS_IN_SEASON * sizeof( float ) );
    }

    per_count=0;
    for (period=archive_data->periods; period; period=period->next)
    {
	for (dam=0; dam < archive_data->ndams; dam++)
	{
		/* There are 3 spill values and some may be specified as n/a */
		int whole_day_na, day_na, night_na;
		float whole_day_spill, day_spill, night_spill;

	    if (pop_token(fp, token, "data ->") == EOF ||
		pop_token(fp, token, "data ->") == EOF ||
		pop_token(fp, token, "data ->") == EOF ||
		pop_token(fp, dam_abbrev, "data ->") == EOF)
	    {
		p_error(fp,
			"%s period %d, game %d, year %d.\n",
			"couldn't read in data for", per_count, game, year);
		return;
	    }

	    /* Only read the elevation from the flow archive if the version
	       says it's available */
	    if (archive_data->maj_vers != 0 || archive_data->min_vers > 8) {

		if (!read_float(fp, &elev_temp, "dam elevation ->")) {
		    p_error(fp,
			    "%s period %d, game %d, year %d.\n",
			    "couldn't read in data for", per_count, game,
			    year);
		    return;
		}
	    } else {
		static int flag = 0;

		if (!flag) {
		/* Suggest politely that they use a newer archive file */
		    log_msg(L_WARNING,
			"Reading old version (%d.%d) archive file!\n\t%s",
			archive_data->maj_vers, archive_data->min_vers,
			"Consider creating archive with more recent crisppp.\n"
			);
		    flag = 1;
		}
	    }

	    if (!read_float_or_na(fp, &whole_day_spill,
			    "planned spill ->", &whole_day_na)	||
		!read_float(fp, &game_spills[dam][per_count].over,
			    "over spill ->") 			||
		!read_float(fp, &game_spills[dam][per_count].flow,
			    "total flow ->")
		)
	    {
		p_error(fp,
			"%s period %d, game %d, year %d.\n",
			"Couldn't read in data for", per_count, game, year);
		return;
	    }

	    	/* Read the optional day and night spill values (if they are present on this line) */
	    	if (at_end_of_line(fp) || !read_float_or_na(fp, &day_spill, "day spill ->", &day_na)) day_na = TRUE;
	    	if (at_end_of_line(fp) || !read_float_or_na(fp, &night_spill, "night spill ->", &night_na)) night_na = TRUE;

		/* Put the appropriate spill values into the archive spill structure */
		if (!day_na) game_spills[dam][per_count].day = day_spill;
		else {
			if (!whole_day_na) game_spills[dam][per_count].day = whole_day_spill;
			else p_error(fp, "%s period %d, game %d, year %d.\n",
				"No valid day spill value given for", per_count, game, year);
		}
		if (!night_na) game_spills[dam][per_count].night = night_spill;
		else {
			if (!whole_day_na) game_spills[dam][per_count].night = whole_day_spill;
			else p_error(fp, "%s period %d, game %d, year %d.\n",
				"No valid night spill value given for", per_count, game, year);
		}

	    /* convert flow from cfs to kcfs; put in dam segment */
	    game_spills[dam][per_count].flow /= (float)1000.0;

	    /* does the configured river include this dam? */
	    if (archive_data->dam_list[dam] != (struct river_segment *) -1)
	    {
		/* Was that the right dam?  Down here to advance past data
		 * even if not. */
		if (strcmp(dam_abbrev, archive_data->dam_list[dam]->abbrev))
		{
		    p_error(fp, "dam %s found when %s expected.\n",
			    dam_abbrev, archive_data->dam_list[dam]->abbrev);
		}

		/* check if version new enough to include elevation.
		 * also, only do drawdown if elevation is non-zero
		 * (since the munged files sometimes are missing elev)
		 */
		if (archive_elevation_flag && elev_temp != 0.0 &&
		    (archive_data->maj_vers!=0 || archive_data->min_vers>8))
		{
		/* Change read elevation value to a delta from forebay elev */
		    elev_temp -= archive_data->dam_list[dam]->
			current->dam.fb_elev;

		    /* Invalid elevation delta */
		    if (elev_temp > 0.0)
		    {
			log_msg(L_ERROR, "%s (%.0f) for %s %s%s (%.0f)!\n %s\n",
				"Water elevation read from file",
				elev_temp
				    + archive_data->dam_list[dam]->
				        current->dam.fb_elev,
				archive_data->dam_list[dam]->seg_name,
				" is higher than the max forebay ",
				"elevation of the dam",
				archive_data->dam_list[dam]->
					current->dam.fb_elev,
				"Setting the delta to 0.");
			elev_temp = 0.0;
		    }
		}

		for (count=period->start;
		     ((count <= period->end)
		      && (count - SEASON_START_DAY < DAYS_IN_SEASON));
		     count++)
		{
		    archive_data->dam_list[dam]
			->current->dam.flow[count-SEASON_START_DAY] 
			    = game_spills[dam][per_count].flow;

		    if (archive_elevation_flag &&
			(archive_data->maj_vers != 0
			 || archive_data->min_vers > 8))

			/* enter the elevation in the appropriate segment. */
			switch(archive_data->dam_list[dam]->up->seg_type) {
			  case REACH_SEG:	/* Reach */
			    archive_data->dam_list[dam]->up->current->
				reach.elev_change[count] = elev_temp;
			    break;

			  case HEADWATER_SEG:	/* Headwater */
			    archive_data->dam_list[dam]->up->current->
				headwater.elev_change[count]=elev_temp;
			    break;

			  default:		/* Anything else (error) */
			    break;
			}
		}
	    }
	}

    /* Now do it all over again for any reaches in the flow archive (!).
     */
    ndams = archive_data->ndams;
    for (reach = 0; reach < archive_data->nreaches; reach++) {
		/* There are 3 spill values and some may be specified as n/a */
		int whole_day_na, day_na, night_na;
		float whole_day_spill, day_spill, night_spill;

	    if (pop_token(fp, token, "data ->") == EOF ||
		pop_token(fp, token, "data ->") == EOF ||
		pop_token(fp, token, "data ->") == EOF ||
		pop_token(fp, dam_abbrev, "data ->") == EOF)
	    {
		    p_error(fp,
			    "%s period %d, game %d, year %d.\n",
			    "couldn't read in data for", per_count, game, year);
		    return;
	    }

	    /* Only read the elevation from the flow archive if the version
	       says it's available (and then ignore it) */
	    if (archive_data->maj_vers != 0 || archive_data->min_vers > 8) {

		    if (!read_float(fp, &elev_temp, "reach elevation ->")) {
		        p_error(fp,
			        "%s period %d, game %d, year %d.\n",
			        "couldn't read in data for", per_count, game,
			        year);
		        return;
		    }
	    }

	    if (!read_float_or_na(fp, &whole_day_spill,
			    "planned spill ->", &whole_day_na)	||
		!read_float(fp, &game_spills[reach+ndams][per_count].over,
			    "over spill ->") 			||
		!read_float(fp, &game_spills[reach+ndams][per_count].flow,
			    "total flow ->")
		)
	    {
		p_error(fp,
			"%s period %d, game %d, year %d.\n",
			"Couldn't read in data for", per_count, game, year);
		return;
	    }

	    	/* Read the optional day and night spill values (if they are present on this line) */
	    	if (at_end_of_line(fp) || !read_float_or_na(fp, &day_spill, "day spill ->", &day_na)) day_na = TRUE;
	    	if (at_end_of_line(fp) || !read_float_or_na(fp, &night_spill, "night spill ->", &night_na)) night_na = TRUE;

		/* Put the appropriate spill values into the archive spill structure */
		if (!day_na) game_spills[reach+ndams][per_count].day = day_spill;
		else {
			if (!whole_day_na) game_spills[reach+ndams][per_count].day = whole_day_spill;
			else p_error(fp, "%s period %d, game %d, year %d.\n",
				"No valid day spill value given for", per_count, game, year);
		}
		if (!night_na) game_spills[reach+ndams][per_count].night = night_spill;
		else {
			if (!whole_day_na) game_spills[reach+ndams][per_count].night = whole_day_spill;
			else p_error(fp, "%s period %d, game %d, year %d.\n",
				"No valid night spill value given for", per_count, game, year);
		}

	    /* convert flow from cfs to kcfs; put in dam segment */
	    game_spills[reach+ndams][per_count].flow /= (float)1000.0;

	    /* does the configured river include this dam? */
	    if (archive_data->reach_list[reach] != (struct river_segment *) -1)
	    {
		    /* Was that the right reach?  Down here to advance past data
		     * even if not. */
		    if (strcmp(dam_abbrev, archive_data->reach_list[reach]->abbrev))
		    {
		        p_error(fp, "reach %s found when %s expected.\n",
			        dam_abbrev, archive_data->reach_list[reach]->abbrev);
		    }

		    for (count=period->start;
		         ((count <= period->end)
		          && (count - SEASON_START_DAY < DAYS_IN_SEASON));
		         count++)
		    {
		        archive_data->reach_list[reach]
			    ->current->reach.flow[count-SEASON_START_DAY] 
			        = game_spills[reach+ndams][per_count].flow;
            }
        }
    }
	per_count++;
    }
    cur_game++;
}
