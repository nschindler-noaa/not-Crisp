/** \file
 * \brief CRiSP Preprocessor header file. Not part of main COMPASS model.
 *
 * This has something to do with flow archives, but it's not a main part of the model so I'm not sure. */

#define VERSION	"V0.9"
#define PROGRAM "CRiSP Flow Archive"

#define MAXDAMS	       40

#define SAMFILE        1
#define HYDROSIM       2
#define HYSSRFILE      3
#define HYSSRFILE2     4 /* BPA Strike back! */

/* HYSSR bit mask field values */
#define QOUT		1
#define	SPILLFLO	2
#define ENERGY		4
#define	ELEV		8 
#define LOAD		16
#define SYSGEN		32
#define SURPLUS		64
#define	GROK	        128	

/* insert mask values */
#define TOTAL		1
#define PLAN_SPILL	2
#define ENERGY 		4 /* unused */
#define	ELEV		8 
#define OVER_SPILL	16 /* Barely used */

#define ALL		(PLAN_SPILL | OVER_SPILL | TOTAL | ELEV)

/* What sort of line to expect next in a hydrosim */
#define DAM		1
#define PERIODS		2
#define DATA		3
#define SOR		4

/* Exit error codes */
#define BADPARAMS      10	/* Parameters don't make sense */
#define BOGUSDATA      11	/* Data doesn't conform */
#define BADHEADER      12	/* Unrecognized header format */

/* Other useful defines */
#define BADDAM	       -99	/* Bogus Dam return code */

#define ABS(x) ((x) < 0) ? ((x) * -1) : (x)
#define SKIPWHITE(x) while(isspace(*x)) ((x)++)
#define SKIPBLACK(x) while((*x) && !isspace(*x)) ((x)++)
#define LEFTSHIFT(x) (x) = (((x) & (unsigned char) 128) >> 7) | ((x) << 1)

struct _dam_period {
    int 		period;  /* 1 through 15 */
    int  		year;
    char 		dam[4];
    float 		elev, plan_spill, over_spill, total;
};

struct _water_year {
    int  		year;
    struct _dam_period  *periods[MAXDAMS][14];
    struct _water_year 	*next;
};

struct _power_year {
    unsigned int 	year;
    struct _water_year  *water_year;
    struct _power_year 	*next;
};

struct _flow_data {
    int	 		games, years; /* Number of each        */
    unsigned short 	dams[MAXDAMS];		/* Index mask into dams  */
    unsigned short int  input_type;		/* the input file format */
    struct _power_year 	*annum;			/* power year list       */
    char		*date;			/* Time stamp on input fl*/
};

struct _dam_strings {
    char code[4], samname[7], sorname[7], hysname[7], hysname2[5];
};

struct _period_strings {
    char code[4], hyssr[5], julian[8];
};

struct _int_list {
    int year;
    struct _int_list *next;
};

typedef struct _dam_period dam_period_t;
typedef struct _water_year water_year_t;
typedef struct _power_year power_year_t;
typedef struct _flow_data  flow_data_t;
typedef struct _dam_strings dam_strings;
typedef struct _int_list int_list;

static dam_strings dam_match_array[] = {
    /* three letter codes and names of dams, COE style, SOR style, hyssr2 */
    {"",  ""      , "MICA  ", "MICA"  , "MICA"}, /* ?? */
    {"",  ""      , "ARROW ", "ARROW" , "ARRO"}, /* ?? */
    {"LIB",  ""   , "LIBBY ", "LIBBY" , "LIBB"}, /* Libby dam */
    {"",  ""      , "BONNER", ""      , ""}, /* Flows at Bonners Ferry are based on
					    Libby outflows and the following side
					    flows below LIbby. */
    {"",  ""      , "DUNCAN", "DUNCAN", "DUNC"}, /* ?? */
    {"",  ""      , "CORA L", "CORRA" , "CORA"}, /* ?? */
    {"",  ""      , "BRILL ", "BRILLI", "BRIL"}, /* ?? */
    {"HGH",  ""   , "H HORS", "HUNGRY", "HGH"}, /* Hungry Horse dam */
    {"",  ""      , "COLFLS", ""      , ""}, /* ?? outflow only -- we tossed this from SAMFILES */
    {"",  ""      , "KERR  ", "KERR"  , "KERR"}, /* ?? */
    {"ALF",  ""   , "ALBENI", "ALBENI", "ALBF"}, /* Albeni Falls dam */
    
    {"GCL",  ""      , "COULEE", "GRAND" , "GRCL"}, /* Grand Coulee dam */ 
    {"CHJ",  ""      , "CH JOE", "CHIEF" , "CHJO"}, /* Chief Joseph dam */	
    {"WEL",  "WELLS ", "WELLS ", "WELLS" , "WELS"}, /* Wells dam */
    {"CHL",  ""      , ""      , ""      , ""}, /* Lake Chelan */
    {"RRH",  "R RECH", "R RECH", "ROCKY" , "ROCR"}, /* Rocky Reach dam */
    {"RIS",  "ROCK I", "ROCK I", "ROCK"  , "ROCI"}, /* Rock Island dam */
    {"WAN",  "WANAP ", "WANAP ", "WANAPU", "WANA"}, /* Wanapum dam */
    {"PRD",  "PRIEST", "PRIEST", "PRIEST", "PRIR"}, /* Priest Radids dam */
    {""   ,  ""      , "BRNLEE", "BROWNL", "BRNL"}, /* Brownlee */
    
    {"DWR",  ""      , "DWRSHK", "DWORSH", "DWOR"}, /* Dworshak dam */
    {""   ,  ""      , "SPALDG", ""      , ""}, /* Spalding */ 
    /* Flows at Spalding are based on Dworshak
       outflows and the following side flows
       below Dworshak. Outflow and Side flow. */
    {"LWG",  "LR GRN", "LR.GRN", "GRANIT", "LWRG"}, /* Lower Granite dam */
    {"LGS",  "L GOOS", "L GOOS", "LITTLE", "LITG"}, /* Little Goose dam */
    {"LMN",  "LR MON", "LR MON", "MONUME", "LOMO"}, /* Lower Monumental dam */
    {"IHR",  "ICE H ", "ICE H ", "ICE"   , "ICEH"}, /* Ice Harbor dam */
    
    {"MCN",  "MCNARY", "MCNARY", "MCNARY", "MCNA"}, /* McNary dam */
    {"JDA",  "J DAY ", "J DAY ", "JOHN"  , "JOND"}, /* John Day dam */
    {"TDA",  "DALLES", "DALLES", "THE"   , "TDA" }, /* The Dalles dam */
    {"BON",  "BONN  ", "BONN  ", "BONNEV", "BONN"}, /* Bonneville dam */
    /* ------  Willamette Basin Projects -------------------------------- */
    {"HCR",  ""      , ""      , "", ""}, /* Hills Creek dam */
    {"LOP",  ""      , ""      , "", ""}, /* Lookout Point dam */
    {"DEX",  ""      , ""      , "", ""}, /* Dexter dam */
    {"CGR",  ""      , ""      , "", ""}, /* Cougar dam */
    {"GPR",  ""      , ""      , "", ""}, /* Green Peter dam */
    {"FOS",  ""      , ""      , "", ""}, /* Foster dam */
    {"DET",  ""      , ""      , "", ""}, /* Detroit dam */
    {"BCL",  ""      , ""      , "", ""}, /* Big Cliff dam */
    {"LOS",  ""      , ""      , "", ""}, /* Lost Creek dam */
    {"",     ""      , ""      , "", ""}  /* This is the end */  
};

static struct _period_strings period_names[] = {
    {"JAN", "JAN", "0:31"},     {"FEB", "FEB",  "32:59"},
    {"MAR", "MAR", "60:90"},    {"AP1", "_105", "91:105"}, 
    {"AP2", "APR", "106:120"},  {"MAY", "MAY",  "121:151"},
    {"JUN", "JUN", "152:181"},  {"JUL", "JUL",  "182:212"},
    {"AG1", "_227","213:228"},  {"AG2", "AUG",  "229:243"},
    {"SEP", "SEP", "244:273"},  {"OCT", "OCT" , "274:304"},
    {"NOV", "NOV", "305:334"},  {"DEC", "DEC" , "335:365"}
};

static char space[] = " \t\n";

void compose_flow_archive();
void make_header();
char *dam_match();
char *dam_list();
char *period_string(), *period_list();
void flush_sor_data();
void get_header_info();
int parse_sam_data(), parse_sor_data();
int parse_hyssr_data(), parse_hyssr_data2();


