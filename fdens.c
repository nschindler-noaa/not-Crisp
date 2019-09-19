#include <math.h>
#include <stdio.h>
#include "crisp.h"
#include "log.h"
#include "math.h"
#include "gas_mort.h"
#include "fdens.h"

/*
 * float compute_fdens(float x, float dmode, float dbot, depth)
 */

double compute_fdens(x, dmode, dbot, depth)
    double x;
    double dmode;
    double dbot;
    double depth;
{
    double A, m0;

    if (x < 0 || x > dbot || x > depth)
	return 0;

    A = FD_A(dmode, dbot, depth);
    m0 = FD_M0(A, dmode, dbot);

    if (x - dmode <= 0)
	return x*m0;
    else
	return (x*m0 + FD_Mdiff(A, dmode, dbot) * (x - dmode));
}

/*
 * float compute_fdens_eqn(float age, equation *eqn,int range)
 *	computes the value for the fdens equation display.
 */

#ifdef __cplusplus
extern "C" {
#endif

float compute_fdens_eqn(float x, equation *eqn)
{
    if (eqn->fd_dmode > eqn->fd_dbot)
    {
	log_msg(L_ERROR, "bad population density eqn paramater values\n");
	return -1;
    }
    else
	return(compute_fdens(x, eqn->fd_dmode, eqn->fd_dbot, eqn->fd_depth)
	       *100.0);
}

#ifdef __cplusplus
}
#endif

