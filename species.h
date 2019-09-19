/** \file
 *
 * \brief external and other definitions for species.c
 */

#include "crisp.h"

#ifndef __SPECIES_H__
#define __SPECIES_H__

#define SPC_VB_STR 	"Vb"
#define SPC_VVAR_STR 	"Vvar"
#define SPC_DRIFT_TIME_STR "drift time"

/* data keys specifying which data to display */
#define SPEC_MENU_KEY	99
#define SPC_VB_DATA	0
#define SPC_VVAR_DATA	1
#define SPC_DRIFT_TIME_DATA	2

/* a few items needed only by the species module */
#define DFLT_NUM_SPECS 5
extern char *dflt_spec_names[];		/* declared in spec_ext.c */

/* externally referenced functions. */
extern int allocate_dam_species(struct river_segment *seg);
extern int allocate_reach_species(struct river_segment *seg);
extern int species_index(struct species *spec);

#endif /* __SPECIES_H__ */

