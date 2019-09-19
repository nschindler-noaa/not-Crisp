/** \file
 * \brief Defines the interface for SAR (smolt to adult ratio)
 * or adult return rates (currently based on arrival timing) */

#ifndef	__RETURN_H
#define	__RETURN_H

#include "eqns.h"

#ifdef __cplusplus
extern "C" {
#endif

extern float return_prob(equation *ptr_eqn, int step);

#ifdef __cplusplus
}
#endif

#endif

