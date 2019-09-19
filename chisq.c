#include <math.h>
#include "log.h"

#ifdef WIN32
/* TODO: handle this issue more elegantly. */
#pragma warning( disable: 4244 )
/* warning C4244: '=' : conversion from 'double ' to 'float ',
	                                           possible loss of data */
#endif

/* utility routines to perform chi-square test, taken from
 * "Numerical Recipes in C", modified only with the substitution of our
 * error reporting function log_msg() for nrerror().
 */

void chstwo(bins1,bins2,nbins,knstrn,df,chsq,prob)
float bins1[],bins2[],*df,*chsq,*prob;
int nbins,knstrn;
{
	int j;
	float temp;
	float gammq();

	*df=nbins-1-knstrn;
	*chsq=0.0;
	for (j=1;j<=nbins;j++)
		if (bins1[j] == 0.0 && bins2[j] == 0.0)
			*df -= 1.0;
		else {
			temp=bins1[j]-bins2[j];
			*chsq += temp*temp/(bins1[j]+bins2[j]);
		}
	*prob=gammq(0.5*(*df),0.5*(*chsq));
}


float gammq(a,x)
float a,x;
{
	float gamser,gammcf,gln;
	void gcf(),gser();

	if (x < 0.0 || a <= 0.0)
	    log_msg(L_ERROR, "Invalid arguments in routine GAMMQ");
	if (x < (a+1.0)) {
		gser(&gamser,a,x,&gln);
		return 1.0-gamser;
	} else {
		gcf(&gammcf,a,x,&gln);
		return gammcf;
	}
}


#define ITMAX 100
#define EPS 3.0e-7

void gcf(gammcf,a,x,gln)
float a,x,*gammcf,*gln;
{
	int n;
	float gold=0.0,g,fac=1.0,b1=1.0;
	float b0=0.0,anf,ana,an,a1,a0=1.0;
	float gammln();

	*gln=gammln(a);
	a1=x;
	for (n=1;n<=ITMAX;n++) {
		an=(float) n;
		ana=an-a;
		a0=(a1+a0*ana)*fac;
		b0=(b1+b0*ana)*fac;
		anf=an*fac;
		a1=x*a0+anf*a1;
		b1=x*b0+anf*b1;
		if (a1) {
			fac=1.0/a1;
			g=b1*fac;
			if (fabs((g-gold)/g) < EPS) {
				*gammcf=exp(-x+a*log(x)-(*gln))*g;
				return;
			}
			gold=g;
		}
	}
	log_msg(L_ERROR,"a too large, ITMAX too small in routine GCF");
}


void gser(gamser,a,x,gln)
float a,x,*gamser,*gln;
{
	int n;
	float sum,del,ap;
	float gammln();

	*gln=gammln(a);
	if (x <= 0.0) {
		if (x < 0.0) log_msg(L_ERROR,"x less than 0 in routine GSER");
		*gamser=0.0;
		return;
	} else {
		ap=a;
		del=sum=1.0/a;
		for (n=1;n<=ITMAX;n++) {
			ap += 1.0;
			del *= x/ap;
			sum += del;
			if (fabs(del) < fabs(sum)*EPS) {
				*gamser=sum*exp(-x+a*log(x)-(*gln));
				return;
			}
		}
		log_msg(L_ERROR,"a too large, ITMAX too small in routine GSER");
		return;
	}
}


float gammln(xx)
float xx;
{
	double x,tmp,ser;
	static double cof[6]={76.18009173,-86.50532033,24.01409822,
		-1.231739516,0.120858003e-2,-0.536382e-5};
	int j;

	x=xx-1.0;
	tmp=x+5.5;
	tmp -= (x+0.5)*log(tmp);
	ser=1.0;
	for (j=0;j<=5;j++) {
		x += 1.0;
		ser += cof[j]/x;
	}
	return -tmp+log(2.50662827465*ser);
}
