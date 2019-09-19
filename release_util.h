/* \file
 * \brief Functions for maniuplating releases of fish. */
#ifndef RELEASE_UTIL_H
#define RELEASE_UTIL_H

#include "crisp.h"

extern LIST *rt_associate_releases( /* struct release *releases */ );
extern void free_assoc_list( /* LIST *assoc_list */ );
extern struct rls_seg_data *collapse_release_data(
    /* LIST *rlsp, struct river_segment *segment, int *first_day,
       float *rls_total */ );

#endif
