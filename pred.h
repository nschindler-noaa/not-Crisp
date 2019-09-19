/** \file
 * \brief 	contains a macro for computing predator density, and other predation
 *	equation support.
 */


/** predator density macro uses drop ratio to simulate a volume adjustment.
 *
 *	float *rslt_ptr;	result pointer
 *	float pop;		pred_pop_mean
 *	float drop_ratio;	(-elevation change / depth)
 *				(positive value for less than full pool)
 *
 *	the global runtime flag is also accessed by this macro.
 *
 * note: this macro uses a while structure for proper compilation in any
 * context.
 */

#define SET_PRED_DENS(rslt_ptr, pop, drop_ratio)			\
do {									\
    float drop;								\
									\
    /* note: the drop ratio is adjusted if necessary to avoid      */	\
    /* divide by zero error by setting drop to 0.95 if drop >= 1.0 */	\
    									\
    if (current->flags & PRED_VOL_FLAG) {				\
        drop = ((drop_ratio) >= 1.0) ? 0.95 : (drop_ratio);		\
	*(rslt_ptr) = (pop) / (1.0 - drop);				\
    }									\
    else *(rslt_ptr) = (pop);						\
} while(0)	/* caller appends trailing ; */


/* predation eqn and pred temperature response equation externs */

extern float pred_rate(float activity, float density, float temperature,
		       equation *pred_temp_response_eqn);

extern float pred_temp_response(equation *ptr_eqn, float temperature);
extern float pred_capture_dist(equation *ptr_eqn, float turbidity);

/* pred temp response equation parameter access */
#define TEMP_RESP_A_VAL param[0].val
#define TEMP_RESP_B_VAL param[1].val
#define TEMP_RESP_C_VAL param[2].val

