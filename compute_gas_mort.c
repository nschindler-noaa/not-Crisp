/** \file
 * \brief Algorithm for computing gas bubble disease mortality
   */

#include "exp_table.h"
#include "nrutil_short.h"
#include <stdio.h>
#include <math.h>

/** \brief Percent mortality per day computed during the reservoir timestep i.
 *
   Done by splitting the reservoir into two parts: spill side and power side based
   on %-spill.  Then the integration routine simpson_2d() is called. If the TDG
   saturation is less thn the critical value on both sides of the river, than a much
   simpler routine to compute mortality base_gas_mort() is called.
 *
 * \return Percent mortality per day during reservoir timestep i
 *
 */
double compute_gas_mort(x1,x2,fr_R,Nmix,Ndif,len,rs,fd,gm)
    double x1,x2,fr_R,Nmix,Ndif,len;
    ReachGasSpecs *rs;
    FDensParams *fd;
    GasMortParams *gm;
{
    double c1,c2,c3,NX1_L,NX1_R;
    double high_gas_mort(),base_gas_mort();
    double nexp_l;
    double nexp_lam;

    /* Parameters for the Nitrogen functions */
    /* On spill side: Ns = c1*exp(-l*x) + c2*exp(-lam*x) +NE and
       on power side: Ns = c1*exp(-l*x) + c3*exp(-lam*x) +NE.
       */
    
    c1= Nmix-NE;
    c2= Ndif*(1-fr_R);
    c3= -1*Ndif*fr_R;

    /* Nsat values at the beginning of the slug */
    nexp_l =   LOOK_UP_NEXP(rs->l*x1);
    nexp_lam = LOOK_UP_NEXP(rs->lam*x1);
    NX1_R = c1*nexp_l + c2*nexp_lam + NE;
    NX1_L = c1*nexp_l + c3*nexp_lam + NE;


    /* NEED Some energy savors here. */
    /* If N0==0, then no mortality so skip integrals*/
    /* If N0<=ncrit at surface, then no region of high mortality, so a simpler routine
       is called to compute mortality of the region.
       */
   
    /*  If N_L=N_R (or close ie |N_L-N_R|<1), then there is no point in 
        splitting reservoir in two. In this case, the value for N_R is 
        used for both sides of the river, as evidenced by passing only 
        c1,c2 to the high_gas_mort() routine. (For the base_gas_mort() 
        routine only the mixed value=c1 matters anyway.) This was a time 
        saving device, however for extreme values of gas mortality
        parameters, stored in gm, this causes a "kink" in the resulting
        % mortality/day curve displayed in COMPASS.*/ 
    if (fabs(NX1_R-NX1_L) <=1.0){ 

	if ((NX1_R > NX1_L ? NX1_R : NX1_L) <= gm->ncrit)
	    /* No High mort, use simpler routine to calculate mortality */
	    return( base_gas_mort(x1,x2,c1,len,rs->l,gm));
			
	else 		
	    return( base_gas_mort(x1,x2,c1,len,rs->l,gm)
		    + high_gas_mort(x1,x2,c1,c2,rs->lam,rs->l,fd,gm) );
			
    }

    else {		/* Left and right flow mortalities computed separately */

	if((NX1_R > NX1_L ? NX1_R : NX1_L)<= gm->ncrit)
	    /* No High mort, use simpler routine to calculate mortality */
	    return( base_gas_mort(x1,x2,c1,len,rs->l,gm));

	else
	    return(  base_gas_mort(x1,x2,c1,len,rs->l,gm)+ 						fr_R*( high_gas_mort(x1,x2,c1,c2,rs->lam,rs->l,fd,gm)) +
		    (1-fr_R)*( high_gas_mort(x1,x2,c1,c3,rs->lam,rs->l,fd,gm))  );
				
    }


}
