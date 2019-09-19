/* fge.c - routines related to the calculation of fge. */

#include "crisp.h"
#include "fpe.h"

/* compute_fpe - take a rls_seg_data and compute a vector of fpes, placing
 * the result in the supplied vector, which must be of size STEPS_IN_RUN
 * or greater.  returns 1 on success, 0 on error.
 */

int compute_fpe(fpe_vec, data)
    float *fpe_vec;
    struct rls_seg_data *data;
{
#ifdef ROUTE_TRACKING
    int i;

    if (!fpe_vec || !data || !data->project_in || !data->turbine_in)
	return 0;
    
    for (i=0; i < STEPS_IN_RUN; ++i) {
	fpe_vec[i] = 1.0 -
	    ((data->project_in[i])
	     ? data->turbine_in[i] / data->project_in[i] : 0.0);
    }
    return 1;
#else
    return 0;
#endif    
}

