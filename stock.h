/** \file
 * \brief Defines the data structure for stocks (groups of fish from a common release point with similar characteristics). */

#ifndef STOCK_H
#define STOCK_H

#include "crisp_defs.h"
#include "eqns.h"
#include "var.h"

/**
 *
 * The general design is that there are a number of custom stocks,
 * designated int the river description file, the data for which are
 * gathered in the global stock_info struct below.  in addition, each
 * species contains one Stock object, which is "generic" data for that
 * species.

 * The key usage is that each release keeps a handle to one Stock
 * object, either one of the custom globals or the generic from the
 * appropriate species for that release.
 */

typedef struct stock {
    char *name;
    /* equation []'s and float []'s are dimensioned by number of reach classes. */
    equation	migration_eqn[10];
    float	mvcoef[10];		/** migration variance */
    float   distance_coeff[10];  /** "a" in "sqrt( a * x^2 + b * t^2 )" */
	float	time_coeff[10];      /** "b" in "sqrt( a * x^2 + b * t^2 )" */

	float	sigma_d[10];	/** reach survival error parameter */

    float	*migr_b1fac[10]; /** [num_reach_classes][STEPS_IN_SEASON] */
    float	vvar[10];

	/** This equation is used with the CUSOM_CLASS mortality class so that
	 * additional X-T-Theta models may be implemented and used easily. */
    equation    custom_survival_eqn[10];

    equation    pred_temp_response_eqn[10];

	/* These equations are used to estimate a return rate for adults based on arrival timing at the transport destination (i.e. below Bonneville) */
	equation	inriver_return_eqn;
	equation	transport_return_eqn;

	/* COMPASS reservoir survival model stuff */
	float	reach_survival_coef[10]; /** Reach survival coefficient (alpha) */
	/* float	time_coef[10]; */ /* Already in species for XT model */ /* Reach survival time coefficient (bee) */
	equation	pred_capture_dist_eqn[10]; /** Predator capture distance equation (gee of turbidity) */
    /* equation    pred_temp_response_eqn[10]; */ /* Already in the model */

} StockT;

/* declarations for Stock methods and related fns. */

extern StockT *construct_stock( /* char *name */ );
extern void destroy_stock( /* Stock **stockpp */ );
extern int new_global_stock( /* char *name */ );

/** struct to contain all global stock related data, including array of
 * Stocks.  a single global instance of this exists in the system.
 */

struct stock_info {
    StockT **s;			/* array of pointers to stocks */
    int num;			/* number of pointers */
};

/** Default values for StockT. These disable as many features as possible
 * and when an input file is written out, only values that differ from
 * these defaults are written out. Thus, old parameters that are
 * generally disabled are hidden. */

extern StockT stock_defaults;

#endif

