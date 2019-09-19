#include <math.h>
#include <stdio.h>
#include "crisp.h"
#include "log.h"
#include "gas_mort.h"
#include "nsat.h"
#include "fdens.h"
#include "exp_table.h"
#include "compute_gas_mort.h"
#include "nrutil_short.h"

/* the new style(s) of computing gas mortality.  */

double compute_gmort1(sat, m_low, m_hi, crit, crit_slope, k, len, depth, vel)
    double sat, m_low, m_hi, crit, crit_slope, k, len, depth, vel;
{
    double klu = k * len / vel;
    double base_level = m_low * sat * (1 - exp(-klu)) / klu;
    
    /* this uses the supersaturation in the tailrace */

    if (sat <= crit)
	return base_level;
    else
    {
	double nrat = sat / crit;
	double bonus =
	    ((m_hi*crit*crit / (klu*crit_slope*depth)) / 4)
		* (nrat*nrat - 4*nrat + 3 + 2*log(nrat));

	return (base_level + bonus);
    }
}

/* a utility eqn for gas mort 2 */

static double futhork(x, y)
    register double x, y;
{
    x += 1.0;
    y += 1.0;

    return( ((x/3 - 1.5*y)*x + 3*y*y)*x - y*y*y*log(x) );
}


double compute_gmort2(double sat, double m_low, double m_hi, double crit,
		double k, double len, double vel, double u0, double ub, double udepth)
{
    double klu = k * len / vel;
    double base_level = m_low * sat * (1 - exp(-klu)) / klu;

    /* this uses the supersaturation in the tailrace */

    if (sat <= crit)
	return base_level;
    else
    {
	double nrat, ulim, A, m0, mdiff, bonus;

	nrat =  sat / crit;
/*	ulim = MIN(MIN(udepth, ub), nrat-1); */
    ulim = udepth;
    if (ulim > ub)
        ulim = ub;
    if (ulim > nrat-1)
        ulim = nrat-1;
	A =     FD_A(u0, ub, udepth);
	m0 =    FD_M0(A, u0, ub);
	mdiff = FD_Mdiff(A, u0, ub);
	bonus = (futhork(ulim, 0.0) - futhork(0.0,0.0)) * m0 / 6.0;

	if (ulim > u0)
	    bonus += (futhork(ulim, u0) - futhork(u0, u0)) * mdiff / 6.0;

	if (ulim < nrat-1)
	{
	    double f1, f2;

	    f1 = m0/2 * ulim * ulim;
	    f2 = m0/6 * ulim * ulim;

	    if (ulim > u0)
	    {
		double udiff = (ulim - u0);
		f1 += mdiff/2 * udiff * udiff;
		f2 += mdiff/6 * udiff * udiff;
	    }

	    ulim += 1;
	    bonus += (nrat - ulim) * f1 - log(nrat/ulim) * ulim * f1
		+ log(nrat/ulim) * f2;
	}
	
	return (base_level + bonus * m_hi * sat / klu);
    }
}


#ifdef __cplusplus
extern "C" {
#endif

float compute_gmort_eqn(float x /* nsupersat value */, 
						equation *eqn, void *param)
{
    struct species *spc = (struct species *) param;
    static int exp_vec_created = 0;
    double gmort_fac=0;
    
    if (eqn->id == EQ_GMORT0) 
	gmort_fac = COMPUTE_GMORT0(x, eqn->gm_mlow, eqn->gm_mhi, eqn->gm_crit);

    else if (eqn->id == EQ_GMORT1)
	gmort_fac = compute_gmort1(x, eqn->gm_mlow, eqn->gm_mhi, eqn->gm_crit,
			       eqn->gm_mcr,
			       NSAT_DISSP_EXP(current->gas_dissp_exp,
					      eqn->gm_vel, eqn->gm_depth),
			       eqn->gm_len, eqn->gm_depth, eqn->gm_vel);

    else if (eqn->id == EQ_GMORT2)
	/* this one picks up the depth from the fdens eqn */
	gmort_fac = compute_gmort2(x, eqn->gm_mlow, eqn->gm_mhi, eqn->gm_crit,
			       NSAT_DISSP_EXP(current->gas_dissp_exp,
					      eqn->gm_vel,
					      spc->fdens_eqn.fd_depth),
			       eqn->gm_len, eqn->gm_vel,
			       FD_U0(&spc->fdens_eqn, eqn),
			       FD_UB(&spc->fdens_eqn, eqn),
			       FD_Udepth(spc->fdens_eqn.fd_depth, eqn));
    else if (eqn->id == EQ_GMORT3) {
	/* pretend the spill side is "right" */
	double N_L = eqn->gm3_tdg2;
	double N_R = x;
	double fr_R = eqn->gm3_splfr;
	double Nmix = (fr_R * N_R) + (1 - fr_R) * N_L;
	double Ndiff = N_R - N_L;

	FDensParams fd;
	ReachGasSpecs rs;
	GasMortParams gm;

	/* fish density parameters */
	double mA;
	fd.zb = spc->fdens_eqn.fd_dbot;
	fd.zm = spc->fdens_eqn.fd_dmode;
	mA = FD_A(fd.zm, fd.zb, spc->fdens_eqn.fd_depth);
	fd.zdep = fd.zb < spc->fdens_eqn.fd_depth ? fd.zb : spc->fdens_eqn.fd_depth;
	fd.m0 = FD_M0(mA, fd.zm, fd.zb);
	fd.mdiff = FD_Mdiff(mA, fd.zm, fd.zb);

	/* reach params */
	rs.length = eqn->gm3_len;
	rs.depth = spc->fdens_eqn.fd_depth;
	rs.l = NSAT_DISSP_EXP(current->gas_dissp_exp,
			      eqn->gm3_vel,
			      rs.depth) / eqn->gm3_vel;
	rs.lam = rs.l + eqn->gm3_theta / eqn->gm3_vel;

	/* gas params */
	gm.a = eqn->gm3_mlow;
	gm.b = eqn->gm3_mhi;
	gm.ncrit = eqn->gm3_ncrit;

	/* initialize the exp vector */
	if (!exp_vec_created) {
	    make_exp_vec();
	    exp_vec_created = 1;
	}
	
	gmort_fac = (1.0 / rs.length)
	    * compute_gas_mort(0.0 , eqn->gm3_len,
			       fr_R,
			       Nmix,
			       Ndiff,
			       eqn->gm3_len,
			       &rs,&fd,&gm);
    }

    return (100 * (1.0 - exp(-gmort_fac)));
}

#ifdef __cplusplus
}
#endif
