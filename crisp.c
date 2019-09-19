#include <string.h>
#include "crisp.h"
#include "run_mode.h"
#include "version.h"

#ifdef WIN32
static char *version_id = "COMPASS v" COMPASS_VERSION_MAJOR_STRING "." COMPASS_VERSION_MINOR_STRING " (WIN32)";
#endif
#ifdef SOLARIS2
static char *version_id = "COMPASS v" COMPASS_VERSION_MAJOR_STRING "." COMPASS_VERSION_MINOR_STRING " (Solaris 2.x)";
#endif
#if !(defined WIN32 | defined SOLARIS2)
static char *version_id = "COMPASS v" COMPASS_VERSION_MAJOR_STRING "." COMPASS_VERSION_MINOR_STRING;
#endif
    
struct release_site  *release_site;

struct tributary *tributary;

struct river_segment *river_mouth;

float	lat_min, lat_max, lat_range;	/* map limits */
float	lon_min, lon_max, lon_range;

int num_specs = 0;
struct species *specs = NULL;
char **spec_names = NULL;

struct stock_info stocks;

struct run *current, *comparison, run[ NRUNS ];

struct run_mode run_mode =
	{ 0, 0, MODE_NONE, 0, 0, NULL };
	/* not running, no mode, not batch, null monte data, log warnings only. */

/* If set, this flag prevents output settings from being modified by the
 *  input data file parser (only). */
int lock_output_settings = 0;
/* If set, this flag prevents variance suppression from being modified by the
 *  input data file parser (only). */
int disable_variance_suppression = 0;

/* This is a command line flag.  If set, this flag forces the travel vector to
 * be calculated rather than read from a travel table.  This is necessary for
 * calculation of derivitives when using the calibration tool. */
int tt_calib_mode;

/* The pointer **dams is a dynamically allocated array of pointers to all dams;
 * **phouses is the same, but includes the powerhouses
 */

struct river_segment	**dams;
struct river_segment	**phouses;

/* number of dams and powerhouses as calculated at run time*/

int	num_dams,
        num_powerhouses;

/* default list of segments for a variety of purposes */
LIST *default_segment_list = NULL;

/* Array of strings which are used as names for mortality classes in
 *  the data files (version 5 and up).
 */
char *mortality_class_names[N_MORTALITY_CLASS] =
{ "gas_pred", "xt", "custom" };

/* Array of flags which turn individual types of mortality on and off.
 *  These are set from run.mortality_class during simulation_init()
 */
int mortality_type[N_MORTALITY_TYPE];

/* New for v1.7 is the concept of "reach classes". Each reach has a small,
 *  non-negative number defining it's class. Here we give the number
 *  of allowed classes and the name of each. There is an absolute limit
 *  of 10 reach classes, which we expect will never be reached in
 *  practice.
 */
int num_reach_classes = 0;
char *reach_class_names[10] = {NULL, NULL, NULL, NULL, NULL,
                                 NULL, NULL, NULL, NULL, NULL};

/* Notes section */
char *input_notes = NULL;
char *file_creation_date = NULL;
char *file_creator = NULL;
char *file_modification_date = NULL;
char *file_modifier = NULL;

/*
 * char *get_program_version()
 *	returns a pointer to the program version string.  this is a pointer
 *	to static storage, and should not be modified by the caller.
 *	this routine plucks the program version from the general version_id
 *	string.
 */

char *get_program_version()
{
	return version_id;
}

/* Default input values */
struct species species_defaults;
struct headwater headwater_defaults;
struct release release_defaults;
struct reach reach_defaults;
struct reach_species reach_species_defaults;
struct dam_species dam_species_defaults;
struct dam dam_defaults;

/* Default transport target */
struct river_segment *default_transport_target = NULL;

