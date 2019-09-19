/** \file
 * \brief Main entry point for batch mode.
 *
 * This file defines the main entry point for batch mode. This is in contrast
 * to extra_batch.c which defines extra support functions.
 */

#include "main.h"
#include "log.h"
#include "file.h"

/** This is the main entry point for batch mode. */
int main (int argc, char *argv[])  {
	int ret;

	if ((ret = process_command_line(argc, argv, 1))) return ret;

	create_log_file();
	locate_image( argv[0] );
	if ((ret = model_startup(argc, argv))) return ret;
	delete_log_file();

	return 0;
}

