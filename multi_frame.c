/* multi_frame.c
 *
 * GUI functions have been superceded - some utility functions remain.
 */

#ifdef WIN32
#include <stdlib.h>
#endif
#include <string.h>
#include "crisp.h"
#include "crisp_defs.h"
#include "monte.h"
#include "log.h"

#define MAX_RUNS	9999

#define FILE_CONTINUATION_STRING	"[...]"
#define FILE_CONTINUATION_SIZE		\
	((int)(sizeof(FILE_CONTINUATION_STRING) - 1))

/*
 * void free_multi_data(struct multi_data *data) - Frees the pointer passed
 *	in, and all pointers associated with it.
 */
void free_multi_data(data)
	struct multi_data *data;
{
    int i;

    if (!data) return;
    if (data->nruns) free(data->nruns);
    if (data->ftell) {
	for (i = 0 ; i < data->nyears ; i++)
	    if (data->ftell[i]) free(data->ftell[i]);
	free(data->ftell);
    }
    free(data);
}

