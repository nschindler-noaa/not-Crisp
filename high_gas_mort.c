/* high_gas_mort.c */

/* high_gas_mort() computes the high level of mortality integrating
        SS( Dist(z)*b*(N(x,y)-NCrit(z))*H(N(x,y)-NCrit(z)) )dzdx 
		over  x1<=x<=x2, 0<=z<=Dep

   This is a VERY simplistic algorithm, essentially a reimann sum
   simply divides up the reach between x1 and x2 by X_DIV, and
   by Z_DIV in the vertical dimension and uses one value of the
   function on each of these blocks. The resulting integral is
   the sum of Dist(zi)*b*(N(xj)-NCrit(zi))*H(N(xj)-NCrit(zi))*area,
   where the area is deltax*deltaz, the area of each of the blocks.

   Note this code RELIES on the form of:
                 N(x)= k1*exp(-l*x) + k2*exp(-lam*x) +NE
   where NE=0
 
   If the formula for N(x) changes then this code must be
   altered.

   This algorithm can be made more accurate by increasing X_DIV and
   Z_DIV. Ideally X_DIV would be about 5 or so and Z_DIV would be
   closer to 20, but this may be too computationally expensive.
   However for most cases the current settings seem to be with in
   .5% of this value. Because the midpoint of the each section is
   used, this tends to underestimate gas mort.

   This routine is called if at least one of the starting TDG values from 
   the left and right flows of the river is greater than the
   critical value nc for TDG.
 
   high_gas_mort() is called by compute_gas_mort()
 
*/

#include "exp_table.h"
#define Z_DIV 10 
#define ZMAX 20 /* Functionally no dissolved gas below 20 feet */
#define X_DIV 1 

double high_gas_mort(x1,x2,c1,c2,lam,l,fd,gm)
double x1,x2,c1,c2,lam,l; 
FDensParams *fd;
GasMortParams *gm;

{   
    register double Ns,zmid,mort,gas_crit;
    double deltaz,zlim,deltax,xmid,area;
    int i,j;

    deltax=(x2-x1)/X_DIV;
    xmid=x1+deltax/2;
    zlim=(fd->zdep>ZMAX?ZMAX:fd->zdep);
    deltaz=zlim/Z_DIV;
    area=deltax*deltaz;

    mort = 0.0;

    for(i=0; i<X_DIV; i++){
   
	Ns=c1*LOOK_UP_NEXP(l*xmid)+c2*LOOK_UP_NEXP(lam*xmid);   
	zmid=deltaz/2.0;
   
	for(j=0; j<Z_DIV; j++){
	  
	    if ((gas_crit = (Ns - GAS_MCRIT*zmid-gm->ncrit)) > 0) {
		if (zmid <= fd->zm) {
		    mort += (fd->m0*zmid) * gm->b
			* gas_crit;
		}
		else {
		    mort += ( fd->m0*zmid + fd->mdiff*(zmid-fd->zm) )
			* gm->b * gas_crit;
		}
	    }

	    zmid+=deltaz;
	}
   
	xmid+=deltax;
 
    }
 
    return(mort*area);

}

