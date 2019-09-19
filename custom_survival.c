#include "custom_survival.h"
#include "log.h"
#include "crisp.h"
#include <math.h>

#define a (ptr_eqn->param[0].val)
#define b (ptr_eqn->param[1].val)
#define c (ptr_eqn->param[2].val)
#define d (ptr_eqn->param[3].val)

#define a0 (ptr_eqn->param[0].val)
#define a1 (ptr_eqn->param[1].val)
#define a2 (ptr_eqn->param[2].val)
#define a3 (ptr_eqn->param[3].val)
#define a4 (ptr_eqn->param[4].val)
#define a5 (ptr_eqn->param[5].val)
#define a6 (ptr_eqn->param[6].val)
#define B0 (ptr_eqn->param[7].val)
#define B1 (ptr_eqn->param[8].val)
#define B2 (ptr_eqn->param[9].val)
#define B3 (ptr_eqn->param[10].val)
#define B4 (ptr_eqn->param[11].val)
#define B5 (ptr_eqn->param[12].val)
#define B6 (ptr_eqn->param[13].val)
#define r (ptr_eqn->param[14].val)

/* custom X-T-Theta survival function that may be expanded to have
 * more forms in the future. */

float custom_survival(equation *ptr_eqn, float average_temp, float average_flow, float dam_spill /* Proportion of fish spilled at upstream dam */, float distance, float time)
{
    switch (ptr_eqn->id) {
    case EQ_SURVIVAL_LINEAR: {
		double value = a + b * average_temp + c * distance + d * time;

		if (!(current->flags & UNTRUNCATED_RESERVOIR_SURVIVAL_FLAG)) {
			/* Constrained to [0, 1] */
			if (value > 1.0) value = 1.0;
			else if (value < 0.0) value = 0.0;
		}
		return (float) value;
    }

    case EQ_SURVIVAL_Z15: {
		float value = (float) exp(-1.0 * ((a0 + a1 * average_flow + a2 * average_flow * average_flow + a3 * average_temp + a4 * average_temp * average_temp + a5 * dam_spill + a6 * dam_spill * dam_spill) * distance
				+ (B0 + B1 * average_flow + B2 * average_flow * average_flow + B3 * average_temp + B4 * average_temp * average_temp + B5 * dam_spill + B6 * dam_spill * dam_spill) * time + r * distance * time ));

		if (!(current->flags & UNTRUNCATED_RESERVOIR_SURVIVAL_FLAG)) {
			/* Constrained to [0, 1] */
			if (value > 1.0f) value = 1.0f;
			else if (value < 0.0f) value = 0.0f;
		}
		return value;
    }

    default:
	log_msg(L_FATAL, "bad custom_survival_eqn form");
    }
    return(0);  /* Should never get here.  Keeps compiler happy.  */
}
