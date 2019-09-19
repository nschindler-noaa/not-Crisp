/** \file
 * \brief Monte Carlo mode definitions. */

#ifndef MONTE_H
#define MONTE_H

#include "period.h"

#ifdef __cplusplus
extern "C" {
#endif

extern void busy_monte_frame();
extern void unbusy_monte_frame();

/* data type definitions for monte carlo */

/*
 * Note that struct archive_header (and therefore monte.h) requires period.h
 *	and stdio.h to be included or lots of fun errors occur.  
 */

/** Archive map structure for flow archives to use */
struct archive_map {
    int ngames;		/* Number of games in the flow archive */
    int nyears;		/* Number of years in the flow archive */
    char **gamenames;	/* A dynamically allocated array of name strings */
    long **offsets;	/* ftell pointers for games in the archive */
};

/** Archive header structure to store data about a single flow archive headers */
struct archive_header
{
    int ngames;				/* # games present in file */
    int nyears;				/* # years present in file */
    int ndams;
    int nreaches;           /* lengths of lists below */
    struct river_segment **dam_list;	/*dynamically alloced dam pointer list*/
    struct river_segment **reach_list;	/*dynamically alloced reach pointer list*/
    int nperiods;
    PERIOD_LIST *periods;		/* dynamically alloced periods */
    int maj_vers;                       /* Major version number for archive */
    int min_vers;                       /* Minor version number for archive */
};

/** One spill entry in the flow archive row */
struct archive_spill
{
    float day;
    float night;
    float over;
    float flow;
};

/** data required to do a multiple run per game
   Monte Carlo run. */
struct multi_data
{
    int ngames;				/* Number of games (in flow archive) i.e. number of water years */
    int nyears;				/* Number of years (in flow archive) */
    int gamesum;			/* Total number of games run */
    int *nruns;				/* Number of runs per water year (dynamic) */
    long **ftell;			/* Ftell pointers for flow archive */
};

/** Input file */
struct input_file
{
    char *file_name;			/* file name */
    char **list;			/* list of file names if it is an
					   edited control file */
};

/** Monte Carlo alternative data */
struct altern_data
{
    char *load_directory;		/* The directory where the altern
					   data was loaded, reflects the
					   path where the input files are
					   expected to be found.  Not saved. */
    int nyears;				/* number of years to be run for
					   this altern	*/
    int ngames;				/* The number of games to be run
					   for this altern */
    unsigned char changed;		/* boolean: has it been changed since
					   last saved? */
    char *alternative_name;		/* name of this alternative */
    char *flow_archive;			/* name of flow archive file */

    unsigned char use_flow_from;	/* false or ARCHIVE_FLOW */
    unsigned char use_spill_from;	/* false or ARCHIVE_SPILL */
    unsigned char use_elev_from;	/* false or ARCHIVE_ELEV */

    struct input_file *input_files;	 /* dynamically alloced array of
					   compass data file names;  length
					   of array == nyears
					 */
    struct multi_data *multi;		/* Multi data for this game */
};


/* some flag definitions */

enum monte_flags {
    SUMMARY		= 0x01,	/* write summary.alt file */
    ARCHIVE_SPILL	= 0x02,	/* take planned spill from flow archive */
    ARCHIVE_ELEV	= 0x04,	/* take elevation from flow archive */
    ARCHIVE_FLOW	= 0x08,	/* take flow from flow archive */
    MF_REALTIME_MONTE	= 0x10	/* generate realtime output data */
};

#if 0

#define SUMMARY 	0x01		/* write summary.alt file */
#define ARCHIVE_SPILL	0x02		/* take plan spill from flow archive */
#define ARCHIVE_ELEV	0x04		/* take elevation from flow archive */
#define ARCHIVE_FLOW	0x08		/* take flow from flow archive */
#define MF_REALTIME_MONTE	0x10	/* generate realtime output data */

#endif

#define ALTERN_LIST_FILE	".compass-alts"

/** Global Monte Carlo data including alternatives to run. */
struct monte_carlo_data
{
    unsigned int flags;			/* configuration options */
    int nalternatives;			/* #alternatives requested by user */
    struct altern_data *alts;		/* Data associated with the
					   alternatives.  length of list is
					   nalternatives. */
};

extern int run_monte_carlo( struct monte_carlo_data *psmcd );
extern void altern_report_account_for( char *name );
extern void monte_compute_spill_schedules(
    struct archive_header *archive_data, 	/* archive header data */
    struct archive_spill **game_spills,		/* list of archive spills */
    unsigned int arch_spill_flag 	/* use archive planned spill if set */
	);
extern struct archive_spill **alloc_game_spills(int ndams, int nperiods);
extern struct archive_header **get_and_check_archives(struct monte_carlo_data *monte_data, long ****ftells);

extern void set_monte_gauges(int game, int year, int alt, struct monte_carlo_data *monte_data);

#ifdef __cplusplus
}
#endif

#endif /* MONTE_H */

