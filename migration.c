#include <math.h>
#include <stdio.h>
#include "crisp.h"
#include "log.h"
#include "math.h"
#include "migration.h"

/* This computes a generally useful quantity.
 */
/* t is the julian date */
#define MB1FAC_INTERNAL(B1, ALFA, Tinf, t)				\
((B1) /(1.0 + exp(-(ALFA) * ((t) - (Tinf)))))


float mb1factor( equation *eqn, double t ) {
    /* t is the julian date */
    switch (eqn->id) {

    case EQ_MIGR:
	case EQ_MIGR13:
    case EQ_MIGR2:
        return MB1FAC_INTERNAL(eqn->migr_bflow, 
                        eqn->migr_alpha, eqn->migr_tinf, t);

    case EQ_MIGR4:
        return MB1FAC_INTERNAL( eqn->param[1].val, eqn->param[2].val,
                                eqn->param[3].val, t );

    case EQ_MIGR9:
        return MB1FAC_INTERNAL( eqn->param[1].val, eqn->param[3].val,
                                eqn->param[4].val, t );
    case EQ_MIGR10:
        return MB1FAC_INTERNAL(eqn->param[2].val, 
                        eqn->param[4].val, eqn->param[5].val, t);
	case EQ_MIGR12:
		return MB1FAC_INTERNAL( eqn->param[2].val, eqn->param[3].val,
                                eqn->param[4].val, t );
    }

    return 0.0;  /* default case for equations which do not use MB1FAC. */
}


/* This routine computes values for the actual simulation (rz_model.c).
 *  Note that not all arguments are needed by all equation forms.
 */
double compute_migr_eqn( equation *eqn, double mb1facr, double velocity,
          double temperature, double tmig, double fish_length, double dam_spill,
			double julian_day, double scale ) {
	double returnValue; //Value computed to be returned.

    switch (eqn->id) {
    case EQ_MIGR:
        returnValue = MIGR_RT(eqn->migr_bmin,
				    eqn->migr_bmax,
				    eqn->param[5].val,
				    mb1facr,
				    velocity, 
				    tmig,
				    0,
				    scale);
		break;

	case EQ_MIGR13:
        returnValue = MIGR_RT(eqn->migr_bmin,
				    eqn->migr_bmax,
				    eqn->param[5].val,
				    mb1facr,
				    velocity, 
				    tmig,
				    0,
				    scale);
		returnValue += (eqn->param[6].val * dam_spill / scale);
		break;
        
    case EQ_MIGR2:
        returnValue = MIGR_RT(eqn->migr_bmin,
				    eqn->migr_bmax,
				    eqn->param[7].val,
				    mb1facr,
				    velocity, 
				    tmig,
				    eqn->migr_blen * fish_length,
				    scale);
		break;
        
    case EQ_MIGR1:
        /* Bmin */
		returnValue = eqn->param[0].val / scale;
		break;

    case EQ_MIGR3:
        /* Bmin + Bflow * velocity */
		returnValue = (eqn->param[0].val + eqn->param[1].val * velocity) / scale;
		break;

    case EQ_MIGR4:
        /* Bmin + vel * mb1fac */
        returnValue = (eqn->param[0].val + mb1facr * velocity) / scale;
		break;

    case EQ_MIGR5:
        /* Bmin + Blen * length */
        returnValue = (eqn->param[0].val + eqn->param[1].val * fish_length) / scale;
		break;

    case EQ_MIGR6:
        /* Bmin + Blen * length + Bflow * velocity */
        returnValue = (eqn->param[0].val + eqn->param[1].val * fish_length +
                 eqn->param[2].val * velocity) / scale;
		break;

    case EQ_MIGR7:
        /* Bmin + Btemp * average temperature */
        returnValue = (eqn->param[0].val + eqn->param[1].val * temperature) / scale;
		break;

    case EQ_MIGR8:
        /* Bmin + Bflow * velocity + Btemp * average temperature */
        returnValue = (eqn->param[0].val + eqn->param[1].val * velocity +
                    eqn->param[2].val * temperature) / scale;
		break;
    case EQ_MIGR9:
        /* Bmin + vel * mb1fac + Btemp * average temperature */
        returnValue = (eqn->param[0].val + mb1facr * velocity + eqn->param[2].val * temperature) / scale;
		break;
    case EQ_MIGR10:
        returnValue = MIGR_RT2(eqn->param[0].val,
				    eqn->param[1].val,
				    eqn->param[6].val,
				    mb1facr,
					eqn->param[4].val,
				    velocity, 
					temperature,
				    tmig,
				    0,
				    scale);
		break;

	case EQ_MIGR11:
		returnValue = eqn->param[0].val + eqn->param[1].val * temperature + eqn->param[2].val * temperature * temperature + eqn->param[3].val * dam_spill + eqn->param[4].val * julian_day + eqn->param[5].val * velocity + eqn->param[6].val * julian_day * velocity;
		break;

	case EQ_MIGR12:
		returnValue = (eqn->param[0].val + eqn->param[1].val * dam_spill + velocity * mb1facr) / scale;
		break;
    }
	if (returnValue > current->min_migr_rate)
		return returnValue;
	return current->min_migr_rate;
}

/*
 * float compute_migr_eqn_val(float x, equation *eqn, void *param,int range,
      int reach_class)
 *	computes the value for the migration equation display.  the param field
 *	is the species.  the range field specifies whether
 *	to return a value representing the mean, low, or high.  values for
 *	these are 0, 1, and 2, respectively. The reach_class field is which
 *  reach class we are computing this for.
 */
/* This routine computes values for the equation editing windows, only.
 */
#ifdef __cplusplus
extern "C" {
#endif

float compute_migr_eqn_val(float x, equation *eqn, void *param, int range,
                           int reach_class)
{
    StockT *stock = (StockT *) param;
    float val, tmig;
	variable_parameter *mvcoef_dist;


    switch (eqn->id) {
    case EQ_MIGR:
        tmig = x - eqn->param[7].val;
        if (tmig < 0)
	        return -1;
        else if (eqn->migr_bmin > eqn->migr_bmax)
        {
	        log_msg(L_ERROR, "bmin > bmax in migration equation\n");
	        return -1;
        }
        val = compute_migr_eqn( eqn, mb1factor( eqn, x ), eqn->param[6].val, 0,
                                tmig, 0, 0, 0, 1 );
        break;

    case EQ_MIGR2:
        tmig = x - eqn->migr_rls;
        if (tmig < 0)
	        return -1;
        else if (eqn->migr_bmin > eqn->migr_bmax)
        {
	        log_msg(L_ERROR, "bmin > bmax in migration equation\n");
	        return -1;
        }
        val = compute_migr_eqn( eqn, mb1factor( eqn, x ), eqn->migr_vel, 0,
                                tmig, 0, 0, 0, 1 );
        break;

    case EQ_MIGR4:
        val = compute_migr_eqn( eqn, mb1factor( eqn, x ), eqn->param[4].val,
                                0, 0, 0, 0, 0, 1 );
        break;

    case EQ_MIGR1:
        val = compute_migr_eqn( eqn, 0, 0, 0, 0, 0, 0, 0, 1 );
        break;

    case EQ_MIGR3:
        val = compute_migr_eqn( eqn, 0, eqn->param[2].val,
                                0, 0, 0, 0, 0, 1 );
        break;

    case EQ_MIGR5:
        val = compute_migr_eqn( eqn, 0, 0, 0,
                                0, eqn->param[2].val, 0, 0, 1 );
        break;

    case EQ_MIGR6:
        val = compute_migr_eqn( eqn, 0, eqn->param[4].val,
                                0, 0, eqn->param[3].val, 0, 0, 1 );
        break;

    case EQ_MIGR7:
        val = compute_migr_eqn( eqn, 0, 0, eqn->param[2].val,
                                0, 0, 0, 0, 1 );
        break;

    case EQ_MIGR8:
        val = compute_migr_eqn( eqn, 0, eqn->param[3].val, 
                                eqn->param[4].val, 0, 0, 0, 0, 1 );
        break;

    case EQ_MIGR9:
        val = compute_migr_eqn( eqn, mb1factor( eqn, x ), eqn->param[5].val,
                                0, 0, 0, 0, 0, 1 );
        break;

    case EQ_MIGR10:
        tmig = x - eqn->param[8].val;
        if (tmig < 0)
	        return -1;
        else if (eqn->migr_bmin > eqn->migr_bmax)
        {
	        log_msg(L_ERROR, "bmin > bmax in migration equation\n");
	        return -1;
        }
        val = compute_migr_eqn( eqn, mb1factor( eqn, x ), eqn->param[7].val, 0,
                                tmig, 0, 0, 0, 1 );
        break;

    default:
        return 0.0; /* should never get here. */
    }

    if (!(mvcoef_dist = get_variable_parameter(&stock->mvcoef[reach_class])) || mvcoef_dist->parameters.broken_stick.low == mvcoef_dist->parameters.broken_stick.high)
		range = 0;

	if (!mvcoef_dist) return (val * stock->mvcoef[reach_class]);
    
    switch (range)
    {
        case 0:				/* return mean value */
	    return (val * mvcoef_dist->parameters.broken_stick.mean);
        case 1:				/* return low value */
	    return (val * mvcoef_dist->parameters.broken_stick.low);
        case 2:				/* return high value */
	    return (val * mvcoef_dist->parameters.broken_stick.high);
    }
    return 0;  /* should never get here.  Keeps compiler happy.  */
}

#ifdef __cplusplus
}
#endif

