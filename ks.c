
/* code to do the Kolmogorov-Smirnov Test from "Numerical Recipes".
 * this code has been changed to assume that the data is already sorted,
 * and to accept arrays with bounds 0 : n-1
 */

#include <math.h>
#ifdef WIN32
/* TODO handle this better someday. */
#pragma warning ( disable: 4244 )
#endif

void kstwo(data1,n1,data2,n2,d,prob)
float data1[],data2[],*d,*prob;
int n1,n2;
{
	int j1=0,j2=0;
	float en1,en2,fn1=0.0,fn2=0.0,dt,d1,d2;
	void sort();
	float probks();

	en1=n1;
	en2=n2;
	*d=0.0;
	while (j1 < n1 && j2 < n2) {
		if ((d1=data1[j1]) <= (d2=data2[j2])) {
			fn1=(j1++)/en1;
		}
		if (d2 <= d1) {
			fn2=(j2++)/en2;
		}
		if ((dt=fabs(fn2-fn1)) > *d) *d=dt;
	}
	*prob = (*d) ? probks(sqrt(en1*en2/(en1+en2))*(*d)) : 1.0;
}


#define EPS1 0.001
#define EPS2 1.0e-8

float probks(alam)
float alam;
{
	int j;
	float a2,fac=2.0,sum=0.0,term,termbf=0.0;

	a2 = -2.0*alam*alam;
	for (j=1;j<=100;j++) {
		term=fac*exp(a2*j*j);
		sum += term;
		if (fabs(term) <= EPS1*termbf || fabs(term) < EPS2*sum)
		    return sum;

		fac = -fac;
		termbf=fabs(term);
	}
	return 0.0;
}
