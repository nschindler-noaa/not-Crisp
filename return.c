#include "crisp.h"
#include "return.h"
#include "log.h"
#include <math.h>

#define APRIL_1	(91*STEPS_PER_DAY)

#define B0 (ptr_eqn->param[0].val)
#define B1 (ptr_eqn->param[1].val)
#define B2 (ptr_eqn->param[2].val)

float return_prob(equation *ptr_eqn, int step)
{
    switch (ptr_eqn->id) {
    case EQ_RETURN_LOG: {
		double day = ((double)(step - APRIL_1)) / STEPS_PER_DAY;
		double logit = B0 + B1 * day + B2 * day * day;

		return (float) (exp(logit)/(1 + exp(logit)));
    }

    default:
	log_msg(L_FATAL, "bad return rate equation form");
    }

    return 0.0f;
}

