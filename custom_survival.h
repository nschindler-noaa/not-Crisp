/* \file
 * \brief Support for custom X-T-Theta survival equations such as the currently-used Z15 model. */

#ifndef	__CUSTOM_SURVIVAL_H__
#define __CUSTOM_SURVIVAL_H__

#include "eqns.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Estimate survival based on average water temperature, average flow, spill experienced, reach length, and passage time */
extern float custom_survival(equation *ptr_eqn, float average_temp, float average_flow, float spill, float distance, float time);

#ifdef __cplusplus
}
#endif

#endif
