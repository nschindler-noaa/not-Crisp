/** \file
 * \brief externs from travel_table.c */

#ifndef TRAVEL_TABLE_H
#define TRAVEL_TABLE_H

#ifdef __cplusplus
extern "C"
{
#endif
    
#include "vectable.h"

#ifndef M_SQRT2
#define M_SQRT2  1.414213562373
#endif
/*  Cumulative of the standard normal distribution; macro for speed  */
#define   phi(x) (0.5 + erf((x)/M_SQRT2)/2)
extern double erf( double x );

extern struct vec_table *init_travel_table();
extern void destroy_travel_table();

#ifdef __cplusplus
}
#endif

#endif // TRAVEL_TABLE_H
