/** \file
 * \brief Structure to hold command line argument information.
 *
 * This file defines the structure that holds general information about command line arguments like which files to load, etc.
 */

#ifndef ARGS_H
#define ARGS_H

#ifdef __cplusplus
extern "C" {
#endif

/** Structure to hold command line arguments */
struct compass_args {
    /** The river description file */
    char *river_desc;
    /** The initial data file to load */
    char *initial_data;

	/* These four files allow the user to select water year, operations, calibration, and post-Bonneville data */
	/** River environment file to load */
	char *riv_data;
	/** Dam operations file to load */
	char *ops_data;
	/** Calibration file to load */
	char *clb_data;
	/** Post-Bonneville file to load */
	char *pbn_data;
	/** Misc. override file */
	char *etc_data;

	/** Adult config file */
	char *adult_config_file;
    /** The output control file to write out */
    char *output_control;
#ifdef REALTIME
    /** Realtime input path */
    char *rt_input_path;
#endif
    /** The input file to write out */
    char *output_data;
	/** Whether all GUI options should be exposed */
	int full_gui;
	/** This is a mild hack, it =1 if in adult model  travel time
	                         *calibration mode (use -t command line option).  It causes 
                             *output at 4 timesteps per day (if using 4 timesteps per day).
                             *Otherwise, output is coerced to 2 timesteps per day to 
                             *accomodate the adult-realtime program's expectations.  */
	int tspd4_print4_flag;  

};

/** Parse the command line arguments into the argument structure */
extern int get_compass_args(char *cmdLine, struct compass_args *user_args);

#ifdef __cplusplus
}
#endif
#endif

