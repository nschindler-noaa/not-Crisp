/** \file
 * \brief Extract travel time (not part of COMPASS). */
#include <stdio.h>

/* Travel Time Request block; data supplied by caller */

typedef struct TTRequest {
    char *stock;		/* crisp stock name (e.g. "Imnaha_River_Ch1") */
    int date;			/* julian date of point release */
    char *obs_site;		/* observation site (e.g. "McNary_Dam") */
    char *data_token;		/* data to extract (e.g. "Bypass_in") */
    FILE *fp;			/* 0 val from last call */
    char *datfile;		/* name of datafile */

    long last_ftell;		/* fseek optimization.
				   caller should not touch this! */
    
} TTRequest;

/* Travel Time Result Block; filled in by extractor */

typedef struct TTResult {
    double *vec;		/* allocated vector supplied by caller */
    int len;			/* length of vector overwritten with results */
    double rls_size;		/* total size of original point release */
} TTResult;

/* return codes */
enum TTRetCode { TT_OK=0, TT_BADPARMS, TT_BADFILE, TT_NODATA, TT_BADTOKEN,
		 TT_BADDATA };

extern int ttextract();
extern int ttextract_cleanup();

