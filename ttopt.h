/** \file
 * \brief options for travel time extraction (not part of COMPASS). */
/* ttopt.h */

#define RELEASE_MULTIPLIER	100000.0
#define	NEWLINE_MULTIPLE	4

#define MAX_DAYS 366

#define VERSION_MAJOR	1
#define VERSION_MINOR	0
#define VERSION_PATCH	0

/* these files must be present for program execution
 * (see header comment in ttopt.c)
 */
#define CRISP_YEARDAT "real96.dat"      /* name of template crisp input file */
#define AWK_TEMPLATE "ttopt.awk_tmpl"   /* template for generated awk script */

/* the ttopt input file may alternatively be specified on the command line */
#define DFLT_INPUT_FILE "./ttopt.input"	/* default ttopt input data file */

#define CRISP_BASENAME "ttime"		/* used to generate name of rls file */ 
#define RLS_EXT "rls"			/* used to generate name of rls file */ 
#define CRISP_DATFILE "ttime.dat"	/* name of generated crisp input file */
#define CRISP_RESULTS "./realtime/realtime.real" /* crisp output data file */

#define AWK_KEYWORD "stockname"		/* key used in awk script */
#define AWK_PROGRAM "ttopt.awk"		/* name of generated awk script */

#define COMMENT_CHAR	'#'
#define SEPARATOR_LINE	\
"============================================================================"

#define WRITE_SEPARATOR_LINE(output_file_pointer) \
    if (putc(COMMENT_CHAR, output_file_pointer) == EOF) \
	return -1; \
    if (!fputs(SEPARATOR_LINE, output_file_pointer)) \
	return -1; \
    if (putc(COMMENT_CHAR, output_file_pointer) == EOF) \
	return -1; \
    if (putc('\n', output_file_pointer) == EOF) \
	return -1

/* traveltime tokens */
#define TT_STOCK	"stock"
#define TT_SPECIES	"species"
#define TT_SITE		"release_site"
#define TT_YEAR		"year"
#define TT_RLS_DATES	"rls_date:length"    
#define TT_PARAMS	"tt_params"
/* #define TT_OBS_SITES	"observations"  (currently unused) */
    
/* crisp tokens */
#define VERSION_TOKEN		"version"
#define VERSION_VALUE  		3    
#define RUN_TOKEN		"run 0\n"
#define REPORT_DATE_TOKEN	"rt_report_date"    
#define RELEASE_TOKEN		"\trelease "
#define RELEASE_SPECIES		"Chinook_1"
#define RELEASE_SITE		"Realtime_Release"
#define STOCK_TOKEN		"stock"
#define RT_STOCK_TOKEN		"rt_stock"
#define DATA_DATE_TOKEN		"rt_data_date"    
#define SMOLT_ONSET_TOKEN	"smolt_onset"
#define SMOLT_ONSET_VALUE	90
#define SMOLT_FINISH_TOKEN	"smolt_finish"
#define SMOLT_FINISH_VALUE	90
#define LENGTH_TOKEN		"length"    
#define VITALITY_TOKEN		"vitality"
#define VITALITY_VALUE		0.0
#define VITALITY_CHANGE_TOKEN	"vitality_change"
#define VITALITY_CHANGE_VALUE	0.0
#define RELEASE_SIZE		4 		/* number of days in release */
#define QUANTITY_TOKEN		"\t\tnumber"
#define END_RELEASE_TOKEN	"\tend release "
#define END_RUN_TOKEN		"end run (0)\n"

typedef struct SplitLine {
    char	*original_line;
    char	**line_tokens;
    unsigned	token_count;
} SplitLine;

/* these params are order-dependent */
enum tt_params { BMIN, BMAX, BFLOW, ALPHA, TSEAS, ALPHA2, BLEN, VVAR,
		 SMOLT_START, SMOLT_FINISH, NUM_TT_PARAMS };

typedef struct CrispRun {
    char *site;
    char *species;
    char *stock;
    int  year;
    double *params;
    int  *rls_dates;		/* date and length are parallel arrays */
    float *rls_lengths;
    char **obs_sites;
} CrispRun;

