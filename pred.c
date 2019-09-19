#include <math.h>
#include <stdio.h>

#include "crisp.h"
#include "log.h"
#include "pred.h"
#include "sample.h"

extern char *hours();


/* predation equation section */

/* pred_rate() - basic predation equation */

float pred_rate(float activity, float density, float temperature,
		equation *pred_temp_response_eqn)
{
    return (activity * density * pred_temp_response(pred_temp_response_eqn, temperature));
}


/* predator temperature response equation.  several forms. */

float pred_temp_response(equation *ptr_eqn, float temperature)
{
    switch (ptr_eqn->id) {
    case EQ_PRED_TEMP1: {
	float a = ptr_eqn->TEMP_RESP_A_VAL;
	float b = ptr_eqn->TEMP_RESP_B_VAL;

	return (a * exp(b * temperature));
    }
    case EQ_PRED_TEMP2: {
	float a = ptr_eqn->TEMP_RESP_A_VAL;
	float b = ptr_eqn->TEMP_RESP_B_VAL;
	float c = ptr_eqn->TEMP_RESP_C_VAL;

	return (a / (1 + exp(-b * (temperature - c))));
    }
    case EQ_LIN0: {
	float a = ptr_eqn->TEMP_RESP_A_VAL;
	float b = ptr_eqn->TEMP_RESP_B_VAL;
    return a + b * temperature;
    }
    case EQ_EXP1:
	return(ptr_eqn->param[1].val
		    * exp(ptr_eqn->param[0].val * temperature));
    default:
	log_msg(L_FATAL, "bad pred temp response eqn");
    }
    return(0);  /* Should never get here.  Keeps compiler happy.  */
}

float pred_capture_dist(equation *ptr_eqn, float turbidity) {
    switch (ptr_eqn->id) {
    case EQ_EXP1:
	return(ptr_eqn->param[1].val
		    * exp(ptr_eqn->param[0].val * turbidity));
    default:
	log_msg(L_FATAL, "bad pred capture distance eqn");
    }
    return(0);  /* Should never get here.  Keeps compiler happy.  */
}
