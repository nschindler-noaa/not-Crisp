/** \file */

#define RELEASE_MULTIPLIER	100000.0
#define	NEWLINE_MULTIPLE	4

#define MAX_DAYS 366

#define VERSION_MAJOR	1
#define VERSION_MINOR	0
#define VERSION_PATCH	0

/* INPUT_PATH_YR_FIELD value MUST MATCH year portion of DFLT_INPUT_PATH!! */
#define INPUT_PATH_YR_FIELD "YY"
#define DFLT_INPUT_PATH "/misc/realtime/chin1/pit/out/YY/bestpred"

#define OUTPUT_FILE "realtime.rls"
#define DEFAULT_RUN_FILE "rtrr.default"

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

/* realtime tokens */
#define RT_RIVER		":RIVER_NAME:"
#define RT_STOCK		":STOCK_NAME:"
#define RT_REPDATE		":TIME_STAMP:"
#define RT_SELECTIONS		":SELECTIONS:"
#define RT_METHOD		":METHOD_NAME:"
#define RT_LDD			":LDD_STAMP:"
#define RT_BESTPRED		"Best_pred:"
    
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
#define AGE_TOKEN		"smolt_onset"
#define AGE_VALUE		80
#define LAST_TOKEN		"smolt_finish"
#define LAST_VALUE		80
#define VITALITY_TOKEN		"vitality"
#define VITALITY_VALUE		0.0
#define VITALITY_CHANGE_TOKEN	"vitality_change"
#define VITALITY_CHANGE_VALUE	0.0
#define RELEASE_SIZE		1 		/* number of days in release */
#define QUANTITY_TOKEN		"\t\tnumber"
#define END_RELEASE_TOKEN	"\tend release "
#define END_RUN_TOKEN		"end run (0)\n"

typedef struct RealTimeDate {
    unsigned    day;
    unsigned    month;
    unsigned    year;
    unsigned    julian;
} RealTimeDate;

typedef struct SplitLine {
    char	*original_line;
    char	**line_tokens;
    unsigned	token_count;
} SplitLine;

typedef struct RealTimeHeader {
    char		*name;		/* river name */
    RealTimeDate	timestamp;	/* time stamp of rt file */
    char 		*selections;	/* jacknife selections */
    char		*method;	/* rt method */
    RealTimeDate	ldd_stamp;	/* last day of data in this file */
} RealTimeHeader;

typedef struct RealTimePrediction {
    RealTimeDate	date;
    double		prediction;
} RealTimePrediction;

/* vector of best predictions for a given ldd */
typedef struct BestPrediction {
    RealTimeDate	first_day;	/* first day of data */
    RealTimeDate	ldd;		/* last day of data (ldd) */
    RealTimePrediction	*preds;		/* vector of predictions */
    unsigned		pred_count;	/* length of preds vector */
    unsigned 		reference_count; /* whether preds vector contains
					  * allocated or referenced info
					  */
} BestPrediction;

typedef struct RealTimeData {
    RealTimeHeader	*header;
    BestPrediction	**bests;	/* array of bestpred vector ptrs
					   for various ldds */
} RealTimeData;

typedef struct CrispRun {
    char		*realtime_name;
    char		*rt_selection_code;
    char		*crisp_site;
    char		*crisp_species;
    char 		*crisp_stock;
    RealTimeDate	ldd;
} CrispRun;

