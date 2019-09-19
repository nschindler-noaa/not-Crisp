/* simpson.c*/

#include "gas_mort_params.h"
#include "fdens.h"

#include "nrutil_short.h"

#include <math.h>
#include <stdio.h>

/* 
   
   simpson_2d() returns the value of the integral:
 
   SS func(x,z) dxdz, where 0=< z <= Dep and
			   x1<= x <= x2

   simpson_2d() uses the 1 dimensional 
   simpson's rule --simpson() to evaluate
   the interior integral.

*/

double simpson(func,n,z1,z2,x,N0,k1,k2,len,lam,l,fd,gm,exp_lx,exp_lamx)
double (*func)(),z1,z2,x,N0,k1,k2,len,lam,l,*exp_lx,*exp_lamx;
FDensParams *fd;
GasMortParams *gm;
int n;
{
	double z,sum,del;
	int j;


	sum=func(z1,x,N0,k1,k2,len,z2,lam,l,fd,gm,exp_lx,exp_lamx)+
		func(z2,x,N0,k1,k2,len,z2,lam,l,fd,gm,exp_lx,exp_lamx);
	del=(z2-z1)/n;

	for (z=z1+del,j=1;j<=n-1;j++,z+=del) {

	    /* if( j % 2 ==0) */
	    if (!(j & 0x01))
		      sum += 2.0*func(z,x,N0,k1,k2,len,z2,lam,l,fd,gm,exp_lx,exp_lamx);
		else
		      sum += 4.0*func(z,x,N0,k1,k2,len,z2,lam,l,fd,gm,exp_lx,exp_lamx);

	}

	return (del*sum/3.0);
}

static double DMAX(double a, double b) {
	return a > b ? a: b;
}

double simpson_2d(func,x1,x2,N0,k1,k2,len,lam,l,dep,fd,gm,exp_lx,exp_lamx)
double (*func)(),dep,x1,x2,N0,k1,k2,len,lam,l,*exp_lx,*exp_lamx;
FDensParams *fd;
GasMortParams *gm;
{
	/* Better programming if num_zdiv and n are passed in.
	   This would make simpson_2d a stand alone procedure.
	*/

	double x,sum,del;
	int n,num_zdiv,j;


	/* For Simpson's integration technique, 
		both n and num_zdiv must be EVEN!!!!)*/

/*	n=DMAX((int) (((x2-x1)/3.0) + .5),2); 
	if((n & 0x01)) n++;
*/
	num_zdiv=((int) DMAX((int) ((N0/4.0) +.5) ,2));
	if ((num_zdiv & 0x01)) num_zdiv++;

	n = 1;
	
	del=(x2-x1)/n;
	sum=0.0;

	for (x=x1+del/2.0,j=0;j<=n-1;j++,x+=del) {

		   sum += simpson(func,num_zdiv,0.0,dep,x,N0,k1,k2,len,lam,l,fd,gm,exp_lx,exp_lamx);	

	}
	
	return (del*sum);
}


