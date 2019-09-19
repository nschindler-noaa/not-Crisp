/** \file
 */
#ifndef C2_MONTE_INDEX_H
#define C2_MONTE_INDEX_H

#include "struct_at.h"

struct monte_index {
    int years;
    int release_count;
    int segment_count;
    char *name;
    char **releases;
    char **segments;
};

extern int make_monte_index_from_output(char *altern);
extern int write_monte_index(char *filename, struct at_struct *monte_data);


#endif
