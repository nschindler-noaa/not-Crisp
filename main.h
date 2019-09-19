/** \file
 * \brief High level startup and model running definitions. */

#ifndef C1_MAIN_H
#define C1_MAIN_H

#include "args.h"

#ifdef __cplusplus
extern "C" {
#endif

extern int run_the_model(int game);
extern void run_scenario_mode();
extern void model_cleanup();

/* Process the command line and other initialization.
 * This is shared code between the command line and GUI
 *
 * Returns 0 on success, non-zero on error */
extern struct compass_args user_args;
extern int process_command_line(int argc, char **argv, int default_to_batch_mode);
extern int model_startup();
extern int run_the_model(int game);

#ifdef __cplusplus
}
#endif
#endif
