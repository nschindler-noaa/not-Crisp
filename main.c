#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <malloc.h>
#include <string.h>
#include "args.h"
#include "batch.h"
#include "crisp.h"
#include "log.h"
#include "file.h"
#include "sample.h"
#include "run_mode.h"
#include "monte.h"
#include "travel_table.h"
#include "realtime.h"
#include "init_river.h"
#include "main.h"
#include "init.h"
#include "flow.h"
#include "init.h"
#include "parse.h"
#include "var.h"
#include "batch.h"
#include "dataio.h"
#include "spill.h"
#include "gui.h"

#if defined(GUI) && defined(WIN32)
#include <windows.h>
#include <direct.h>
#endif

extern int compute_all_flow();
void 		water_temp_proc();
void		velocity_frame_proc();
void		make_river_param_frame();
void		make_message_proc();
void	    make_monte_carlo_frame();
void		make_analysis_frame();
void		make_compare_frame_proc();
void 		make_state_frame();
void		show_pred_window();
void		make_water_tt_frame();
extern int clear_seg_data(struct release *release, int game);
int compute_seg(struct river_segment *river_seg, int game);
extern int write_output_summary(int game, int year, int alt);
extern void rt_set_datafile(char *name);
extern void reach_model(struct river_segment *river_seg, int game);
extern int dam_model(struct river_segment *river_seg, int game);

#ifdef REALTIME
void		realtime_monte_callback();
void		run_realtime();
#endif

void 		open_filer();
void		report_frame();
void 		date_tool_proc();
extern void	output_report();

/** \brief Run the model for a specific game */
int run_the_model(int game){
	
	struct release *release;
    int i, reach_class;

    /* always propagate downstream before running the model */
    if (compute_all_flow())
    {
	log_msg(L_ERROR, "flow initialization failure; execution aborted.\n");
	return(-1);
    }

    for (release = run->release_list; release; release = release->next)
    {
	release->total_released = 0.0;
    reach_class = release->rls_seg->river_segment->current->reach.rclass;

	for (i = 0; i < STEPS_IN_RUN; ++i)
	    release->total_released += release->number[i];

	if (! clear_seg_data(release, game)) /* also allocates if necessary */
	{
	    log_msg(L_ERROR, "game %d simulation aborted\n", game);
	    return(-1);			/* error */
	}
    }

	/* sample variable parameters before the run */
	if (sample_variable_parameters()) {
	    log_msg(L_ERROR, "sampling stochastic parameters failed\n", game);
		return(-1) /* error */;
	}

    compute_seg(river_mouth, game);

    if (!run_mode.running)		/* user interrupt? */
	return -1;
    else
	return 0;
}

/** \brief general model cleanup for exit */
void model_cleanup()
{
    destroy_travel_table();
}


/** \brief * scenario mode is run from here.
 *
 * We do the standard per-simulation
 * init, then a scenario mode dependent init (computing spill),
 * then call the standard run routine.
 */
    
void run_scenario_mode()
{
    extern void mark_only_dams_active();

    struct release *release;
    unsigned seg_data_count = 0;

	run_mode.running = TRUE;

    if (simulation_init(SCENARIO)) {
		run_mode.running = FALSE;
		return;
	}
    
    mark_only_dams_active();
    set_planned_spill();
    
    /* run game 0 */
    for (release = run->release_list ; release ; release = release->next) {
	struct rls_seg *seg;

	for (seg = release->rls_seg ; seg ; seg = seg->down)
	    ++seg_data_count;
    }

    if (run_the_model(0) == 0) write_output_summary(0, -1, -1);	/* scenario mode */
	run_mode.running = FALSE;
}

/** \brief Compute statistics at a specific segment and then go upstream from there */
int compute_seg(river_seg, game)
    struct river_segment *river_seg;
    int game;
{
    if (river_seg->up)
	compute_seg(river_seg->up, game);
    if (river_seg->fork)
	compute_seg(river_seg->fork, game);

    /* user can interrupt  */
    if (run_mode.running){
		if (!BATCH_MODE_ACTIVE)
			SetStatusLine( river_seg->seg_name );

		if (river_seg->seg_type == REACH_SEG)
		{	
			reach_model(river_seg, game);
		}
		else if (river_seg->seg_type == DAM_SEG)
		{
			dam_model(river_seg, game);
		}
		else
		{
		
			struct release *release;

			for (release = run->release_list; release; release = release->next)
			{
			struct rls_seg *rls_seg = release->rls_seg;

			while (rls_seg && rls_seg->river_segment != river_seg)
				rls_seg = rls_seg->down;
			if ( ! rls_seg)
				continue;		/* doesn't pass this point in river  */
			bcopy((char *)rls_seg->up->data[game]->output,
				  (char *)rls_seg->data[game]->output,
				  STEPS_IN_SEASON * sizeof(float));
			bcopy((char *)rls_seg->up->data[game]->length,
				  (char *)rls_seg->data[game]->length,
				  STEPS_IN_SEASON * sizeof(float));
			}
		}
	}	
	return 0;
}

struct compass_args user_args;

/** \brief Process command line arguments */
int process_command_line(int argc, char** argv, int batch) {
    char *cmdline;
    int i, n;

    user_args.river_desc = NULL;
    user_args.riv_data = NULL;
    user_args.ops_data = NULL;
    user_args.clb_data = NULL;
    user_args.pbn_data = NULL;
    user_args.etc_data = NULL;
    user_args.initial_data = NULL;
    user_args.adult_config_file = NULL;
#ifdef REALTIME
    user_args.rt_input_path = NULL;
#endif
    user_args.output_data = NULL;
    user_args.output_control = NULL;

    /* Create a single command line string by concatenating all of the
     *  arguments passed in.
     */
    for (n = i = 0; i < argc; i++)
        n += strlen( argv[i] ) + 1;
    cmdline = (char *)malloc( n + 5 );
    strcpy( cmdline, argv[0] );

    if (batch) strcat( cmdline, " -b" );

    for (i = 1; i < argc; i++ ) {
        strcat( cmdline, " " );
        strcat( cmdline, argv[i] );
    }

    /* process command line.
     */
    get_compass_args(cmdline, &user_args);

	free(cmdline);

	return 0;
}

/** This function should be called after any log file setup and locate_image are
 * taken care of, but before anything else */
int model_startup() {
    char initial_file[MAXPATHLEN];
    char writename[MAXPATHLEN];

    /* default river desc was set up in user_args
     */
    if (get_river_file(user_args.river_desc)) {
#if	defined(GUI) && defined(WIN32)

	/* Use the Windows registry to locate the install directory and try it */

		int i;
		HKEY hKey;
		unsigned char path[MAXPATHLEN + 1];
		unsigned long type;
		unsigned long pathSize;
		/* Check both the current user and the local machine */
		HKEY hives[] = {HKEY_LOCAL_MACHINE, HKEY_CURRENT_USER};

		for (i=0; i < 2; i++) {
			pathSize = MAXPATHLEN;
			if (RegOpenKeyExA(hives[i], WINDOWS_REGISTRY_SUBKEY, 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS
				&& RegQueryValueExA(hKey, "Path", NULL, &type, path, &pathSize) == ERROR_SUCCESS
				&& type == REG_SZ)
			{
				RegCloseKey(hKey);

				chdir(path);
				break;
			}
		}

	    if (get_river_file(user_args.river_desc)) return 1;
#else
        return 1;
#endif
    }

    current = &run[0];
 
    /* propagate input dam data into the headwaters
     * this flow init must occur before the dam inits
     */
    mark_only_dams_active(); 
    fill_headwaters_from_dams(river_mouth);
    init_compass();
 
    /* If the output control file is specified, read it and then lock the
     *  output settings.
     */
    if (user_args.output_control) {
        if (find_compass_file( user_args.output_control, initial_file ))
            parse_datafile( initial_file, 0, 0 );
        else
            parse_datafile( user_args.output_control, 0, 0 );
        lock_output_settings = 1;
    }

#ifdef REALTIME
    if (RT_MODE_ACTIVE(run_mode.mode)) {
	    /* Realtime mode loads input files differently from other modes */

	    if (user_args.rt_input_path)
	        rt_set_input_pathname( user_args.rt_input_path );
	    if (RT_MODE_ACTIVE(run_mode.mode) && user_args.initial_data) {
	        rt_set_datafile( user_args.initial_data );

		/* Don't load the file twice during parsing (since user_args.initial_data is loaded when the replace flag to parse_datafile() is TRUE */
		free(user_args.initial_data);
		user_args.initial_data = NULL;
	    }
    }
    else
#endif

    {
	    /* Not realtime mode */

	    /* read in initial datafile, if specified
	     */
	    if (user_args.initial_data) {
	        if (find_compass_file(user_args.initial_data, initial_file)) {
	            parse_datafile(initial_file, 0, 0);
	        }
	        else
	            /* call parser anyway, to generate error message
	             */
	            parse_datafile(user_args.initial_data, 0, 0);
	    }
		/* Read in the three component input files, if all are specified */
		else if (user_args.riv_data && user_args.ops_data && user_args.clb_data && user_args.pbn_data) {
	        if (find_compass_file(user_args.pbn_data, initial_file)) parse_datafile(initial_file, 1, 0);
	        else parse_datafile(user_args.pbn_data, 0, 0);
	
	        if (find_compass_file(user_args.clb_data, initial_file)) parse_datafile(initial_file, 1, 0);
	        else parse_datafile(user_args.clb_data, 0, 0);
	
	        if (find_compass_file(user_args.riv_data, initial_file)) parse_datafile(initial_file, 0, 0);
	        else parse_datafile(user_args.riv_data, 0, 0);
	
	        if (find_compass_file(user_args.ops_data, initial_file)) parse_datafile(initial_file, 0, 1);
	        else parse_datafile(user_args.ops_data, 0, 0);
	
			if (user_args.etc_data) {
				if (find_compass_file(user_args.etc_data, initial_file)) parse_datafile(initial_file, 0, 1);
				else parse_datafile(user_args.etc_data, 0, 0);
			}
	
	
		}
    }

    set_planned_spill();
    compute_all_flow();

    if ((run_mode.running = BATCH_MODE_ACTIVE) != FALSE) {
        batch_init();
    }

    if (run_mode.running) {
        if (run_mode.mode == SCENARIO)
            run_scenario_mode();
        else if (run_mode.mode == MONTE)
            run_monte_carlo( run_mode.monte_data );
        else if (RT_MODE_ACTIVE(run_mode.mode))
            run_realtime( run_mode.mode );
        else
            log_msg(L_ERROR, "unknown run mode\n" );

        /* After a batch run, we may output a new data file.
         */
        if (user_args.output_data != NULL) {
            strcpy( writename, user_args.output_data );
            if (write_output_file( writename ))
                log_msg( L_ERROR, "write error for %s\n",
                                        user_args.output_data );
        }
    }

    model_cleanup();

    return 0;
}

