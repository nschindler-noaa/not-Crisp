/** \file
 * \brief Header for batch mode functions
 */

#ifndef C1_BATCH_H
#define C1_BATCH_H

#include "run_mode.h"

/** Enable batch mode */
#define SET_BATCH_MODE	(run_mode.batch = 1)
/** Check to see if batch mode is active */
#define BATCH_MODE_ACTIVE	(run_mode.batch)

extern void batch_init();

#endif
