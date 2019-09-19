/* moment.c
 *
 * Snagged from Numerical Recipes.  Pruned out average deviation, and
 *	made sure it always sets all parameters.
 */

#include <math.h>

#ifdef WIN32
/* TODO handle this better someday. */
#pragma warning (disable: 4244 )
#endif

#define CLOSE_TO_ZERO 0.000001

void moment(data,n,ave,sdev,svar,skew,curt)
int n;
float data[],*ave,*sdev,*svar,*skew,*curt;
{
	int j;
	float s,p;
	double t_svar, t_skew, t_curt;

	if (n <= 1) return;	/* error */
	s=0.0;
	for (j=0;j<n;j++) s += data[j];
	*ave=s/n;
	t_svar = t_skew = t_curt = 0.0;
	for (j=0;j<n;j++) {
	    s = fabs(data[j] - *ave);
	    t_svar += (p = s*s);
	    t_skew += (p *= s);
	    t_curt += (p *= s);
	}
	*svar = t_svar/(n-1.0);
	*sdev = sqrt(*svar);
	if (*svar > CLOSE_TO_ZERO) {
	    *skew = t_skew / ((float) n*(*svar)*(*sdev));
	    *curt = (t_curt / ((float) n*(*svar)*(*svar))) - 3.0;
	} else {
	    *skew = 0.0;
	    *curt = 0.0;
	}
}
