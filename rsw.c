#include <math.h>
#include <stdio.h>

#include "crisp.h"
#include "log.h"
#include "rsw.h"

/** RSW Efficiency
 * that is, the proportion of fish entering the dam that are spilled via
 * a RSW as a function of the proportion of total flow through RSW */

double	rsw_prob(dm, spc, day, time)
    struct	dam	*dm;
    int 	spc, day, time;
{
	const double spill_fraction = dm->spill[day*DAM_TIME_SLC + time];
	const double spill_kcfs = spill_fraction * dm->flow[day];
	/* TODO: RSW should have an array like spill[] and a schedule so that looking at dm->flow[] isn't necessary */

	double x; /* proportion of spill through RSW */
    struct dam_species *dmspc = &(dm->species[spc]);

	if (spill_fraction <= 0 || dm->rsw_spill_cap <= 0) return 0; // Assumed zero if no RSW spill

	if (spill_kcfs < dm->rsw_spill_cap) {
		x = 1.0; /* All spill is allotted to RSW since there's less spill than the RSW cap */
	}
	else {
		x = dm->rsw_spill_cap / spill_kcfs;
	}

    switch (dmspc->rsw_eqn.id) {
	case EQ_RSW:
	{
		const float a = dmspc->rsw_eqn.RSW_A_VAL;
		const float b = dmspc->rsw_eqn.RSW_B_VAL;

		const double exp_term = exp(a + b*log(x/(1 - x)));
		return exp_term / (1 + exp_term);
	}
	case EQ_SPILL_EFFICIENCY:
	{
		const float B0 = dmspc->rsw_eqn.param[0].val;
		const float B1 = dmspc->rsw_eqn.param[1].val;

		const double logit = exp(B0 + B1 * log(x/(1 - x)));
		if (x >= 1.0) return 1.0;
		return logit / (1 + logit);
	}
    default:
	log_msg(L_FATAL, "bad RSW efficiency equation");
    }
    return(0);  /* Should never get here.  Keeps compiler happy.  */
}
