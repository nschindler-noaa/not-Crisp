/** \file
 * \brief Data for Monte Carlo GUI. */

#ifndef	_MONTE_FRAME_H
#define	_MONTE_FRAME_H

#include "monte.h"

#ifdef __cplusplus
extern "C" {
#endif

extern struct monte_carlo_data *make_monte_object();

extern int set_altern_defaults(struct altern_data* alt);
int write_altern_data(FILE * outfile, struct altern_data *alternative);
void free_monte_object(struct monte_carlo_data *obj);
void free_alternative_object(struct altern_data *object);

#ifdef __cplusplus
}
#endif

#endif

