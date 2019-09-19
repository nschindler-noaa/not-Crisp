/* base_gas_mort() computes the base level of mortality integrating
	SS( Dist(z)*a*N(x,y) )dzdx over  x1<=x<=x2, 0<=z<=Dep

	SS( Dist(z)*a*N(x,z) )dzdx
		= S a*N(x)*(S Dist(z)dz) dx  (when N(x,z)<=ncrit)
		= S a*N(x)*1.0*dx
		= S a*N(x) dx	

   Note this code RELIES on the form of:
		 N(x)= Nmix*exp(-l*x) 

   If the formula for N(x) changes then this code must be
   rewritten. 
 
   base_gas_mort() is called by compute_mort()

*/

#include "exp_table.h"

double base_gas_mort(x1,x2,Nmix,len,l,gm)
    double x1,x2,Nmix,len,l;
    GasMortParams *gm;

{
    if(l>0.0)	
	return( gm->a*Nmix*( LOOK_UP_NEXP(l*x1) - LOOK_UP_NEXP(l*x2) )/l );
   
    else{	/* l == 0.0 */
	return( gm->a * Nmix*len );
      
    }
}

