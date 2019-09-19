/** \file
 * \brief Definitions for spill-related functions at dams. */

#ifndef	_SPILL_H_
#define	_SPILL_H_

#include "crisp.h"

#ifdef __cplusplus
extern "C" {
#endif

extern double	spill_prob(struct dam *dm, int spc, int day, int time);
extern void set_dam_planned_spill(struct river_segment *dam);
extern void set_planned_spill();
extern void set_legacy_spill(struct river_segment *dam);
extern void set_dam_flow_routing(struct river_segment *dam, int day);
extern void remove_all_spill_scheds();

#ifdef __cplusplus
}
#endif

#endif
