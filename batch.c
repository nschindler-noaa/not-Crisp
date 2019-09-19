/** \file
 * \brief Batch mode initialization code.
 */

#include <stdio.h>
#include <stdlib.h>
#ifndef WIN32
#include <sys/param.h>
#endif
#include "crisp.h"
#include "log.h"
#include "monte.h"
#include "run_mode.h"
#include "token.h"

#ifdef WIN32
/* TODO figure out where to get this constant from */
#ifndef MAXPATHLEN
#define MAXPATHLEN (1024)
#endif
#endif


static char *altern_run_filename = ALTERN_LIST_FILE;

/** Load an alternative and parse it */
static int batch_load_alternative(name, data)
	char *name;
	struct altern_data *data;
{
    extern unsigned char open_and_parse_alternative();

    char buf[MAXPATHLEN];

    sprintf(buf, "%s/%s.alt", name, name);
    if (open_and_parse_alternative(buf, data))
	return 1;
    data->changed = 0;
    return 0;
}

/** Initialize batch mode. Monte Carlo mode is assumed. */
void batch_init()
{
    extern FILE *buf_open();
    extern struct monte_carlo_data *make_monte_object();
    extern int set_altern_defaults();

    struct monte_carlo_data *monte_data;
    FILE *run_fp;
    char buf[MAXPATHLEN];
    int data_count = 100;

#ifdef DEBUG
    fprintf(stderr, "Running in batch mode\n");
#endif
    run_mode.running = 1;
    if (run_mode.mode == MONTE) {
	monte_data = make_monte_object();
	if (!(run_fp = buf_open(altern_run_filename, "r"))) {
	    log_msg(L_FATAL, "Can't open alternative run file '%s'\n",
		    altern_run_filename);
	}
	monte_data->alts = (struct altern_data *)
	    malloc(data_count * sizeof(struct altern_data));
	if (!monte_data->alts) {
	    log_msg(L_FATAL, "Out of memory loading alternatives!\n");
	}
	monte_data->nalternatives = 0;
    monte_data->flags = run_mode.summary;
	while (pop_token(run_fp, buf, "Alternative File -> ") != EOF) {
	    bzero(&monte_data->alts[monte_data->nalternatives],
		  sizeof(struct altern_data));
	    set_altern_defaults(&monte_data->alts[monte_data->nalternatives]);
	    if (batch_load_alternative(buf,
				       &monte_data->alts[monte_data->nalternatives]))
		continue;
	    if (++monte_data->nalternatives == data_count) {
		data_count *= 2;
		monte_data->alts = (struct altern_data *)
		    realloc(monte_data->alts, data_count
			    * sizeof(struct altern_data));
		if (!monte_data->alts) {
		    log_msg(L_FATAL, "Out of memory resizing alternative list!\n");
		}
	    }
	}
	monte_data->alts = (struct altern_data *)
	    realloc(monte_data->alts,
		    monte_data->nalternatives * sizeof(struct altern_data));
	if (!monte_data->alts) {
	    log_msg(L_FATAL, "Out of memory downsizing alternative list! (odd)\n");
	}
	if (ferror(run_fp))
	    log_msg(L_ERROR, "Error reading alternative run file\n");
	run_mode.monte_data = monte_data;
#ifdef WIN32
        fclose( run_fp );
#else
	buf_close(run_fp);
#endif
    }
}

