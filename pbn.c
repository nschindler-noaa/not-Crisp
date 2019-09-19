/* This file contains the code related to post-Bonneville calculations */

#include "crisp.h"
#include "eqns.h"

const char *post_bonneville_hypothesis_names[NUM_POST_BONNEVILLE] = {
	"sar_vs_date",
	"latent_mortality",
	"constant_d",
	"s3_vs_wtt"
};

const char *post_bonneville_hypothesis_names_formatted[NUM_POST_BONNEVILLE] = {
	"SAR vs. Date",
	"Latent Mortality",
	"Constant D",
	"S3 vs Water Travel Time"
};

/** Compute inriver latent mortality using the S3 vs. WTT algorithm */
float s3_vs_wtt_inriver_latent_mort(equation *ptr_eqn, float wtt)
{
    switch (ptr_eqn->id) {
    case EQ_LATENT_MORT_WTT: {
		return 1.0 - exp( -ptr_eqn->param[0].val * (wtt - 2.0));
    }

    default:
	log_msg(L_FATAL, "bad inriver_latent_mort_equation form");
    }
    return(0);  /* Should never get here.  Keeps compiler happy.  */
}

