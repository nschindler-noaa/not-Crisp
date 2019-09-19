/** \file
 * \brief Helpers for crisp_calib. This is not part of COMPASS GUI or batch modes! (see \ref crisp_calib)
 */

#include <stdio.h>

/* Crisp Extraction Request block; data supplied by caller */

typedef struct CRRequest {
    char *stock;		/* crisp stock name (e.g. "Imnaha_River_Ch1") */
    int date;			/* julian date of point release */
    char *release_site;
    char *obs_site;		/* observation site (e.g. "McNary_Dam") */
    char *data_token;		/* data to extract (e.g. "Bypass_in") */
    FILE *fp;			/* 0 val from last call */
    char *datfile;		/* name of datafile */

    long last_ftell;		/* fseek optimization.
				   caller should not touch this! */
    
} CRRequest;

/* Crisp Extraction Result Block; filled in by extractor */

typedef struct CRResult {
    double *vec;		/* allocated vector supplied by caller */
    int len;			/* length of vector overwritten with results */
    double rls_size;		/* total size of original point release */
} CRResult;

/* return codes */
enum CRRetCode { CR_OK=0, CR_BADPARMS, CR_BADFILE, CR_NODATA, CR_BADTOKEN,
		 CR_BADDATA };

extern int crisp_extract(CRRequest* req, CRResult* rslt);
extern int crisp_extract_cleanup(CRRequest* req);
