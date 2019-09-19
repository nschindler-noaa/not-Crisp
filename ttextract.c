/** \file
 * \brief Contains code to do data extraction after a recent
 *   	travel time crisp run (Not part of COMPASS).
 */

#include <stdlib.h>
#include <string.h>
#ifndef WIN32
#include <unistd.h>
#endif

#include "ttextract.h"

/** \brief perform an extraction of a data vector
 * from a recent crisp travel-time run.
 *
 *  The format of the datafile
 * extracted from must be that of a "realtime.real" file.  This
 * routine takes pointers to TTRequest and TTResult structs.  The
 * request struct contains a file pointer ("fp"), which must be 0 on
 * the first call to this routine for a given datafile, in which case
 * the filename specified by "datfile" is opened and the file pointer
 * placed in "fp" for use in the next call.  For proper operation of
 * the fseek optimizations, "fp" must not be modified by the user in
 * subsequent calls on the same datafile.  When finished with a
 * datafile, function "ttextract_cleanup(req)" must be called, which
 * will close the file.
 *
 * The extraction is placed in the TTResult struct.  The "vec" field
 * must already contain an allocated vector of doubles large enough in
 * size to contain the result, and should be initialized to 0.  The
 * "len" field will be set to the length of the vector overwritten by
 * new data.  The "rls_size" will be set to the total size of the
 * original release corresponding to the requested data.
 *
 * This routine returns 0 on success and non-zero on error.
 */

int ttextract(req, rslt)
    TTRequest *req;
    TTResult *rslt;
{
    static char input_buffer[BUFSIZ];
    static char str1[BUFSIZ];
    static char stock[256];
    static char last_obs_site[256];

    int i;
    int found;
    int start;
    int len;

    /* sanity checks */
    if (!req->stock || !req->obs_site || !req->data_token
	|| !rslt->vec)
	return TT_BADPARMS;

    /* set the requested stock name */
    sprintf(stock,"%s_%d", req->stock, req->date);

    if (!req->fp) {
	req->last_ftell = -1;
	last_obs_site[0] = '\0';
	if (!(req->fp=fopen(req->datfile, "r"))) 
	    return TT_BADFILE;
    }
	
    /* search for the segment */
    /* check cached search segment, and jump there if applicable */
    if (req->last_ftell >= 0 && !strcmp(req->obs_site, last_obs_site)) {
	fseek(req->fp, req->last_ftell, SEEK_SET);
    }
    else {
	rewind(req->fp);
	found = 0;
	while (!found) {
	    if (!(fgets(input_buffer, BUFSIZ, req->fp)))
		return TT_NODATA;
	    sscanf(input_buffer, "%s", str1);
	    if (!strcmp(str1, "Segment")) {
		if (1 != sscanf(input_buffer, "%*s%s", str1))
		    return TT_BADTOKEN;
		if (!strcmp(str1, req->obs_site)) {
		    found = 1;
		    req->last_ftell = ftell(req->fp);
		    strcpy(last_obs_site, req->obs_site);
		}
	    }
	}
    }
    /* search for the release */
    found = 0;
    while (!found) {
	if (!(fgets(input_buffer, BUFSIZ, req->fp)))
	    return TT_NODATA;
	sscanf(input_buffer, "%s", str1);
	if (!strcmp(str1, "Release")) {
	    if (1 != sscanf(input_buffer, "%*s%*s%*s%*s%*s%s", str1))
		return TT_BADTOKEN;
	    if (!strcmp(str1, stock))
		found = 1;
	}
    }
    if (2 != fscanf(req->fp, "%*s %d %*s %*d %*s %lf", &start, &rslt->rls_size))
	return TT_BADTOKEN;

    /* find the data */
    found = 0;
    while (!found) {
	if (!(fgets(input_buffer, BUFSIZ, req->fp)))
	    return TT_NODATA;
	sscanf(input_buffer, "%s", str1);
	if (!strcmp(str1, req->data_token)) {
	    if (1 != sscanf(input_buffer, "%*s%d", &len))
		return TT_BADTOKEN;
	    found = 1;
	}
	else if (!strcmp(str1, "Release") || !strcmp(str1, "Segment")) {
	    /* data token keyword is missing */
	    return TT_NODATA;
	}
    }
    /* extract data */
    for (i=0; i < start; ++i)
	rslt->vec[i] = 0;
    for (i=0; i < len; ++i)
	if (1 != fscanf(req->fp, "%lf", &rslt->vec[start+i]))
	    return TT_BADDATA;
    rslt->len = start+len;

    return TT_OK;
}

/** cleanup extraction by closing data file and resetting stashed ftell */
int ttextract_cleanup(req)
    TTRequest *req;
{
    fclose(req->fp);
    req->fp = 0;
    req->last_ftell = -1;
    return 0;
}

