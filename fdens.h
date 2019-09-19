/** \File
 * Defs for fish density equation */
#ifndef FDENS_H
#define FDENS_H

#include "gas_mort.h"

#define fd_dmode param[0].val
#define fd_dbot  param[1].val
#define fd_depth param[2].val

extern double compute_fdens();

/* these are used to get proper U values from the dens and gmort eqns */

#define FD_UVAL(depth, crit, crit_slope) ((depth) * (crit_slope) / (crit))

#define FD_U0(fd_eqn, gm_eqn)						\
(FD_UVAL((fd_eqn)->fd_dmode, (gm_eqn)->gm_crit, (gm_eqn)->gm_mcr))

#define FD_UB(fd_eqn, gm_eqn)						\
(FD_UVAL((fd_eqn)->fd_dbot, (gm_eqn)->gm_crit, (gm_eqn)->gm_mcr))

#define FD_Udepth(depth, gm_eqn)					\
(FD_UVAL((depth), (gm_eqn)->gm_crit, (gm_eqn)->gm_mcr))

/* some more macros for density/depth calcs */

#define FD_M0(A, dmode, dbot) (2*(A) / ((dbot) * (dmode)))
#define FD_Mdiff(A, dmode, dbot) (-2*(A) / ((dmode) * ((dbot) - (dmode))))

#define FD_A(dmode, dbot, depth)					\
(((depth) > (dbot)) ? 1.0						\
 : (((depth) > (dmode)) ? ((dbot) * ((dbot) - (dmode)) /                \
    ((dbot) * ((dbot) - (dmode)) - ((dbot) - (depth)) * ((dbot) - (depth)))) \
    : ((dbot)*(dmode) / ((depth)*(depth)))))


/** The Fish density parameters in FDensParams are species and reach specific */
typedef struct {
        double zb;
        double zm;
        double zdep;
        double m0;
        double mdiff;
}FDensParams;
 

#endif    
