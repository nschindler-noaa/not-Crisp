/** \file
 * \brief Header for crisp_calib program. This is not part of the normal COMPASS batch or GUI version (see \ref crisp_calib)!
 */

/* crisp_calib.h */

/* pick up the crisp datafile version number in this header */
#include "writeutil.h"

#define RELEASE_MULTIPLIER	100000.0
#define	NEWLINE_MULTIPLE	4

#define MAX_DAYS 366

#define VERSION_MAJOR	1
#define VERSION_MINOR	0
#define VERSION_PATCH	0

/* the crisp_calib input file may alternatively be specified on the command line */
#define DFLT_INPUT_FILE "./calib.input"	/* default crisp_calib input data file */

#define CRISP_DATFILE "calib_tmp.dat"	/* name of generated crisp input file */
#define CRISP_RLSFILE "calib_tmp.rls"	/* name of generated rls file */ 
#define CRISP_RESULTS "./realtime/realtime.real" /* crisp output data file */

#define AWK_PROGRAM "calib_tmp.awk"	/* name of generated awk script */

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

/* general tokens */
#define CC_CRISP_DATA   "crisp_datafile"
#define CC_RIVER_DESC   "river_description_file" 
#define CC_AWK_SCRIPT   "awk_script"
#define CC_AWK_PARAMETERS "awk_parameters"   
#define CC_AWK_SED_CMD  "awk_sed_cmd"
    
/* release tokens */
#define CC_SITE		"release_site"
#define CC_STOCK	"stock"
#define CC_SPECIES	"species"
#define CC_SMOLT_ONSET  "smolt_onset"    
#define CC_SMOLT_FINISH "smolt_finish"    
#define CC_RLS_DATES	"rls_date:length:number"    

/* #define TT_OBS_SITES	"observations"  (currently unused) */
    
/* crisp tokens */
#define VERSION_TOKEN		"version"
/*#define VERSION_VALUE  		6    */
#define VERSION_VALUE  		CURRENT_OUTFILE_VERSION
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


typedef struct ReleaseInfo {
    struct ReleaseInfo *next;
    
    char *site;
    char *species;
    char *stock;
    int smolt_onset;
    int smolt_finish;
    int  *rls_dates;		/* date, length, and size are parallel arrays */
    float *rls_lengths;
    float *rls_numbers;
} ReleaseInfo;

typedef struct CrispCalib {
    char *datafile;
    char *river_description;
    char *awk_script;
    char *awk_parameters;
    char *awk_sed_cmd;

    ReleaseInfo *releases;
} CrispCalib;


