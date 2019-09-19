/* monte_frame.c
 *
 * GUI functions have been moved to ZasMonteCarloTool.cpp & etc.
 *  Some support funtions remain here.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "crisp.h"	/* includes period.h (required for monte.h) */
#include "log.h"
#include "monte.h"
#include "parseutil.h"
#include "tokens_alt.h"
#include "writeutil.h"
#include "run_mode.h"
#include "gui.h"
#include "unused.h"

static int num_alternatives = 0;
char *setup_file = ALTERN_LIST_FILE;

/*
 * make_monte_object -- allocate and zero a monte_carlo_data object,
 *	then set the default flags,
 *	Returns the object.
 */

struct monte_carlo_data *make_monte_object()
{
    struct monte_carlo_data *object;

    object = (struct monte_carlo_data *)
	malloc(sizeof(struct monte_carlo_data));

    if (!object) {
	log_msg(L_ERROR, "Ran out of free memory in monte carlo\n");
	return 0;
    }

    bzero(object, sizeof(struct monte_carlo_data));

    /* elev and flow flags set by default */
    object->flags = ARCHIVE_ELEV | ARCHIVE_FLOW; 

    return object;
}

/*
 * free_alternative_object(object) -- this routine frees all the associated
 *	variables regarding an altern_data structure, but does *not* free
 *	the structure itself.  This is because this structure is represented
 *	only as an array of pointers.
 */

void free_alternative_object(object)
	struct altern_data *object;
{
    extern void free_multi_data();

    int i;

    if (!object) return;
    if (object->alternative_name) free(object->alternative_name);
	if (object->load_directory) free( object->load_directory );
    if (object->flow_archive) free(object->flow_archive);
    if (object->input_files) {
	for ( i = 0 ; i < object->nyears ; i++)
	{
	    if (object->input_files[i].file_name)
		free(object->input_files[i].file_name);
	    if (object->input_files[i].list)
		free(object->input_files[i].list);
	}
	free(object->input_files);
	object->input_files = 0;
    }
    if (object->multi) free_multi_data(object->multi);
    object->multi = 0;
}

/*
 * free_monte_object -- frees everything associated with a monte_carlo_data
 *	object.  The object is the only parameter.
 *
 *      Unlike free_alternative_object, free_monte_object actually frees the
 *	pointer passed.
 */

void free_monte_object(obj)
	struct monte_carlo_data *obj;
{
    if (!obj) return;
    if (obj->alts) {
	int i;

	for ( i = 0 ; i < obj->nalternatives ; i++ )
	    free_alternative_object(obj->alts + i);
	free(obj->alts);
    }
    free(obj);
}

/*
 * void set_flow_archive(altern, ptr) -- Sets flow_archive in altern to be
 *	equal to the full pathname of the file in pointer.
 */
void set_flow_archive(struct altern_data *altern, char *ptr)
{
    if (!altern->flow_archive || strcmp(altern->flow_archive, ptr)) {
	if (altern->flow_archive)
	    free(altern->flow_archive);
	altern->flow_archive = strdup(ptr);
	altern->changed = 1;
    }
}

/*
 * write_altern_data - Do just that.
 */

int write_altern_data(outfile, alternative)
    FILE * outfile;
    struct altern_data *alternative;
{
#define NUM_PER_LINE 5
    int count;
    extern void perror_msg();

    OUTPUT_ITEM("%s %d\n", TKS_VERSION, CURRENT_OUTFILE_VERSION);
    OUTPUT_STRING("%s \"%s\"\n", TKS_ALTERNATIVE, alternative->alternative_name);
    
    OUTPUT_STRING("\t%s %s\n", TKS_USE_FLOW_FROM,
		  alternative->use_flow_from ? "archive" : "user_data");

    /* write custom ngames only if flow from user data */
    if (alternative->use_flow_from != ARCHIVE_FLOW)
	OUTPUT_ITEM("\t%s %d\n", TKS_NGAMES, alternative->ngames);

    /* write archive file if previously specified, or place holder if
       archive flow requested but filename is missing.
     */
    if (alternative->flow_archive || alternative->use_flow_from == ARCHIVE_FLOW)
	OUTPUT_STRING("\t%s \"%s\"\n",TKS_FLOW_ARCHIVE_NAME,
		      alternative->flow_archive ? alternative->flow_archive
		      : "No_Filename");

    /* write the following if a flow archive file has ever been specified */
    if (alternative->flow_archive)
    {
	OUTPUT_STRING("\t%s %s\n", TKS_USE_SPILL_FROM,
		      (alternative->use_spill_from == ARCHIVE_SPILL) ? 
		      "archive" : "user_data");
	OUTPUT_STRING("\t%s %s\n", TKS_USE_ELEV_FROM,
		      (alternative->use_elev_from == ARCHIVE_ELEV) ? 
		      "archive" : "user_data");
    
	if (alternative->multi) {
	    (void) fprintf(outfile, "\t%s %d %d", TKS_MULTI,
			   alternative->multi->ngames,
			   alternative->multi->nyears);
	    for (count=0 ; count < alternative->multi->ngames ; count++)
		(void) fprintf(outfile, "%s%d%s",
			       (count) % NUM_PER_LINE ? " " : "\t\t",
			       alternative->multi->nruns[count],
			       (count + 1) % NUM_PER_LINE ? "" : "\n");
	    if (count % NUM_PER_LINE) putc('\n', outfile);
	}
    }
    OUTPUT_ITEM("\t%s %d\n", TKS_INPUT_FILES, alternative->nyears);

    for (count=0; count < alternative->nyears; count++)
	(void) fprintf(outfile, "\t\t\"%s\"\n",
		       alternative->input_files[count].file_name ?
		       alternative->input_files[count].file_name :
		       "No_Filename");

    OUTPUT_END("", TKS_ALTERNATIVE, alternative->alternative_name);

    return(0);
}

/* int set_alternative_defaults(struct altern_data* alt) - as the name implies,
 * 	set the default values in a brand new alternative.  non-static.
 */

int set_altern_defaults(alt)
    struct altern_data* alt;
{
    alt->use_spill_from = 0;
    alt->use_elev_from = ARCHIVE_ELEV;
    alt->use_flow_from = ARCHIVE_FLOW;
	return 1;  /* value not significant at this time. */
}

void set_monte_gauges(int game, int year, int alt, struct monte_carlo_data *monte_data)
{
    /* ugh.  the alt parameter has dual meaning.  it is the index of the alt
     * in question.  it is also the display setting for the alt gauge.
     * usually the two are the same thing, but not if the gauge is to
     * be set to 100%, i.e. alt==nalts.  in that case we must use the index
     * of the last alternative.  really gross.
     */
    int display_alt;

	int ng, ny, na;

    display_alt = alt;
    if (alt == monte_data->nalternatives)
	--alt;

	/* This routine is exported by the ZasMonteCarloTool dialog, and will
	 *  check to see if the dialog exists etc.
	 */
	ng = monte_data->alts[alt].ngames;
	ny = monte_data->alts[alt].nyears;
	na = monte_data->nalternatives;

	SetMonteGauges( ng, (int)((game*100) / (ng ? ng : 1)),
					ny, (int) ((year*100)/(ny ? ny : 1)),
					num_alternatives,
					(int) ((display_alt*100)/(na ? na : 1)) );
}

