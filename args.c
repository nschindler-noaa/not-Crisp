#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "crisp.h"
#include "args.h"
#include "batch.h"
#include "defaults.h"
#include "log.h"
#include "monte.h"
#include "vectable.h"
#include "version.h"
#include "writeutil.h"

/* handle command-line argument processing */

/* this is always called before other argument checking, so it may
 * safely set the default run mode, possibly to be overridden by later
 * arguments.
 */
void check_args_for_batch_flag(argc, argv)
	int argc;
	char **argv;
{
    for (--argc ; argc ; --argc) {
	if (!strncmp(argv[argc], "-b", 2)) {
	    run_mode.batch = 1;/*SET_BATCH_MODE();*/
	    run_mode.mode = MONTE; /* default mode for batch runs */
	    return;
	}
    }
}

/* (NOTE: This comment is old. Use comapssb --help to get the command line format!)
 * int get_compass_args( char *cmdLine, struct compass_args *user_args)
 *	parse the command line to locate possible arguments.  return them
 *	in the compass_args struct.  malloc string space as necessary.
 *      also loads default values if not specified on the command line.
 *
 *	command line format:
 *		compass [-bsmi] [-r river_desc] [-f data_file] [-p realtime_path] 
 *
 *	return:		0 success
 *		       -1 failure
 */

char *get_opt(char *cline, char **result){

	char *output=NULL;	
	char temp[80];

 	int i=1;
	int j=0;
 	while(cline[i] == ' ')
		i++;
	while(cline[i] != ' ' && cline[i] != '\0')
		temp[j++] = cline[i++];

	temp[j] = '\0';
	output = malloc(sizeof(char)*(strlen(temp)+1));
	*result = cline + i;
	if (output)
		strcpy(output, temp);
	return output;
}

int get_compass_args(char *cmdLine, struct compass_args *user_args)
{
	static char *version = 
	 "COMPASS Passage Model v" COMPASS_VERSION_MAJOR_STRING "." COMPASS_VERSION_MINOR_STRING "\n\n";

    static char *usage =
     "Usage: compassb [-l{wmrd}] [-bsmiugt] [-r river_desc] [-k calib_file]\n"
	 "       [-e river_env_file] [-d dam_ops_file] [-p post_bonneville_file]\n"
     "       [-x extra_config_file] [-c output_control_file]\n"
	 "       [-f scenario_file] [-o output_file] [-v]\n";
	char *p;
	
    int mode_flag = run_mode.mode = MODE_NONE;
	int realtime_flag = 0;
    int summary_flag = 0;
	log_warning = 1;
	log_message = log_raw = log_debug = 0;
    user_args->river_desc = NULL;
	user_args->riv_data = NULL;
    user_args->ops_data = NULL;
    user_args->clb_data = NULL;
    user_args->pbn_data = NULL;
    user_args->etc_data = NULL;
    user_args->initial_data = NULL;
    user_args->output_data = user_args->output_control = NULL;
	user_args->adult_config_file = NULL;
	user_args->tspd4_print4_flag = 0;
#ifdef REALTIME
    user_args->rt_input_path = NULL;
#endif
    user_args->full_gui = 0;
    tt_calib_mode = 0;

	if (strstr(cmdLine,"-h") || strstr(cmdLine,"-?") || strstr(cmdLine, "--help")){
		printf(version);
		printf(usage);
		exit(0);
	}
	else if (strstr(cmdLine,"--version")) {
		printf(version);
		exit(0);
	}

	/* Outer loop searches for option specifications (starting with "-" or
	 *  "/". Inner loops run over possible multiple options within each
	 *  specification.
	 */

	p = cmdLine;
	while (*p) {
		/* Look for next option specification.
		 */
		while (*p) {
			if (*p == '-')
				break;
			p++;
		}
		if (!*p)
			break;
		p++;

		/* Decode an option string. "p" points past the '-' designator.
		 */
		while (*p) {
			if (*p == 'l') {
				/* Loop over logging suboptions.
				 */
				p++;
				log_warning = log_message = log_raw = log_debug = 0;
				while (*p) {
					if (*p == 'w')
						log_warning = 1;
					else if (*p == 'm')
						log_message = 1;
					else if (*p == 'r')
						log_raw = 1;
					else if (*p == 'd')
						log_debug = 1;
					else
						break;
					p++;
				}
				continue;
			}
			else if (*p == 'b') {
				SET_BATCH_MODE;
			}
			else if (*p == 's') {
				mode_flag = SCENARIO;
			}
			else if (*p == 'm') {
				mode_flag = MONTE;
			}
			else if (*p == 'u') {
				summary_flag = SUMMARY;
			}
			else if (*p == 'g') {
				user_args->full_gui = 1;
			}
            else if (*p == 't') {
				tt_calib_mode = 1;
				user_args->tspd4_print4_flag = 1;
			}
#ifdef REALTIME
			else if (*p == 'i') {
				realtime_flag = 1;
			}
#endif
			else if (*p == 'r') {
				user_args->river_desc = get_opt( p, &p );
				break;
			}
			else if (*p == 'e') {
				user_args->riv_data = get_opt( p, &p );
				break;
			}
			else if (*p == 'd') {
				user_args->ops_data = get_opt( p, &p );
				break;
			}
			else if (*p == 'k') {
				user_args->clb_data = get_opt( p, &p );
				break;
			}
			else if (*p == 'p') {
				user_args->pbn_data = get_opt( p, &p );
				break;
			}
			else if (*p == 'x') {
				user_args->etc_data = get_opt( p, &p );
				break;
			}
			else if (*p == 'f') {
				user_args->initial_data = get_opt( p, &p );
				break;
			}
			else if (*p == 'a' ) {
				user_args->adult_config_file = get_opt( p, &p );
				break;
			}
			else if (*p == 'o') {
				user_args->output_data = get_opt( p, &p );
				break;
			}
			else if (*p == 'c') {
				user_args->output_control = get_opt( p, &p );
				break;
			}
			else if (*p == 'v') {
				output_all = TRUE;
				break;
			}
			else if (*p && *p != ' ') {
				printf( "Unknown option %c\n", *p );
				exit(0);
			}
			else
				break;
			p++;
		}

	}

	/* Fixup options after scanning command line.
	 */
	if (user_args->river_desc == NULL) {
		user_args->river_desc = malloc(sizeof(char)*(strlen(DFLT_RIVER_DESC)+1));
		strcpy(user_args->river_desc,DFLT_RIVER_DESC);
	}

	if (mode_flag == MODE_NONE) {
		if (realtime_flag)
			mode_flag = MONTE;
		else if (BATCH_MODE_ACTIVE)
			mode_flag = MONTE;
		else
			mode_flag = SCENARIO;
	}

	if (realtime_flag) {
		if (mode_flag == SCENARIO)
			mode_flag = RT_SCENARIO;
		else
			mode_flag = RT_MONTE;
	}

	run_mode.mode = mode_flag;
    run_mode.summary = summary_flag;

	if (user_args->initial_data == NULL && (user_args->riv_data == NULL && user_args->ops_data == NULL && user_args->clb_data == NULL && user_args->pbn_data == NULL)) {
		if (!RT_MODE_ACTIVE(run_mode.mode) &&
			!(BATCH_MODE_ACTIVE && run_mode.mode == MONTE)) {
			//printf("BASE.DAT CODE\n");
			//user_args->initial_data = malloc(sizeof(char)*(strlen(DFLT_DATAFILE)+1));
			//strcpy(user_args->initial_data,DFLT_DATAFILE);
		}
	}

    return 0;
}
