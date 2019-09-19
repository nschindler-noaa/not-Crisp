/* \file
 * \brief Equation representation in the model.
 *
 * \section equations Equations in COMPASS
 * Equations are represented in COMPASS in a generic way so that the parsing
 * routines only need be written once.
 *
 * Equations are identified by number (which is how they are selected in input
 * files) and they may have several parameters that can be defined. These
 * parameters have minimum and maximum values, but these are for use in the
 * graphical equation editors. See the equation numbers in eqns.h.
 *
 * One important note to make about equations is that the equations themselves
 * are not implemented in any way inside of eqns.h or eqns_dflt.c. The
 * equations are instead implemented in side switch control structures in the
 * files where they are intended to be used. This means that random equations
 * cannot be used wherever the user wants to use them, but only in places where
 * the actual equations are implemented. This may keep the user from making
 * misleading changes (since the arguments to the equations may be in different
 * orders depending on the context), but this also means that some equations
 * (such as EQ_LIN0) are implemented in several different places.
 */

#ifndef EQNS_H
#define EQNS_H

#include "writeutil.h"

/* exported routines listed at end of file */

/* various definitions */
/** Total number of possible equations */
#define NUM_EQNS	61		
/** Max num of params in any equation */
#define EQN_NPARAMS	15		

/* equation ids.  used to reference an eq in the default table. */

#define EQ_LIN0		0
#define EQ_LIN1		1
#define EQ_LIN2		2
#define EQ_LIN3		3
#define EQ_EXP0		4
#define EQ_EXP1		5
#define EQ_EXP2		6
#define EQ_EXP3		7
#define EQ_POW0		8
#define EQ_POW1		9
#define EQ_POW2		10
#define EQ_POW3		11
#define EQ_NSAT0	12
#define EQ_FL_ACT	13
#define EQ_DELAY0	14
#define EQ_NSAT3	15
#define EQ_LOG0		16
#define EQ_NSAT1	17
#define EQ_NSAT2	18
#define EQ_GMORT0	19
#define EQ_DRIFT	20
#define EQ_DELAY1	21
#define EQ_FGE		22
#define EQ_NSAT4	23
#define EQ_MIGR		24
#define EQ_GMORT1	25
#define EQ_GMORT2	26
#define EQ_FDENS	27
#define EQ_TRANS	28
#define EQ_NSAT5	29
#define EQ_NSAT6	30
#define EQ_GAS_DISP 31
#define EQ_GAS_DISP_RIGHT 32  /* Unused */
#define EQ_MIGR2	33
#define EQ_GMORT3	34
#define EQ_CUB0		35
#define EQ_CUB1		36
#define EQ_PRED_TEMP1   37
#define EQ_PRED_TEMP2   38
#define EQ_MIGR1    39
#define EQ_MIGR3    40
#define EQ_MIGR4    41
#define EQ_MIGR5    42
#define EQ_MIGR6    43
#define EQ_SPILL_EFFICIENCY  44
#define EQ_FLUSHTRANS  45	/* Unused */
#define EQ_DELAY3   46
#define EQ_MIGR7    47
#define EQ_MIGR8    48
#define EQ_MIGR9	49
#define EQ_MIGR10	50
#define EQ_RETURN_LOG	51
#define EQ_MIGR11	52
#define EQ_SIMPLEMORT5  53	/* Unused */
#define EQ_FLUSHTRANS5  54	/* Unused */
#define	EQ_RSW	55
#define EQ_SURVIVAL_LINEAR	56
#define EQ_SURVIVAL_Z15	57
#define EQ_LATENT_MORT_WTT	58
#define EQ_MIGR12	59
#define EQ_MIGR13	60
 
/* specific equation parameter references */
/* EQ_NSAT0, 4 */
#define gsat_a_val	param[0].val
#define gsat_b_val	param[1].val
#define gsat_k_val	param[2].val
#define gsat_h_val	param[2].val
/* EQ_FL_ACT */
#define k0_val		param[0].val
#define k1_val		param[1].val
/* EQ_DELAY */
#define flow_delay_coef param[0].val
#define flow_delay_exp  param[1].val
    
/** Parameter struct used in displaying equations */
struct eq_param
{
    double	val;			/** Value */
    double	min;			/** Minimum value (for the GUI) */
    double	max;			/** Maximum value (for the GUI) */
    char	*name;			/** Parameter name */
};

/** An equation record, usable for many different types of equations */
typedef struct
{
    int			id;			/** Type of equation */
    int			num_params;		/** Num params displayed */
    int 		total_params;		/** Total num params used */
    int         saved_params;   /** Number of params written to file. */
    int         divider_param;  /** Divide sliders after param "n" - -1 = don't */
	enum FloatType precision; /** Precision of output; should probably be of:
							   * - FLOAT_SCIENTIFIC (6 decimal digits)
							   * - FLOAT_PRECISE (12 decimal digits). */
    struct eq_param	param[EQN_NPARAMS];	/** Parameter values */
    char		*formula;		/** String representation */
	char		*name;			/** Name of the equation so that data files don't have to mess around with numbers */
} equation;

/** Equation display table type */
typedef struct
{
    int	active;				/** Index of active eqn in table */
    int current;			/** Index of currently viewed eqn */
    int	modified;			/** Bit flag, eqn is/isn't modified */
    int	tab_size;			/** Size of table */
    int num_eqns;			/** Number entries currently used */
    equation *eq_tab;			/** Eq table, decl'd or alloced */
    float xmin;				/** X minimum */
    float xmax;				/** X maximum */
    char *xlabel;			/** Label string for x axis */
    float ymin;				/** Y minimum */
    float ymax;				/** Y maximum */
    char **ylabels;			/** Null-terminated list of y labels */
    equation *target_eq;		/** Actual equation target data */
    void *external_params;		/** Equation parameters not displayed */
    void (*update_fn)();		/** Secondary update function */
} eq_display;

/* window size definitions */

#define L_MARGIN        50
#define	R_MARGIN	20
#define	B_MARGIN	40
#define T_MARGIN        20
#define PARAM_HEIGHT	23

/* exported routines */
#ifdef __cplusplus
extern "C" {
#endif
extern equation eqn_dflt_tab[];		/** Table of all possible equations */

int init_eqns();			/** Init the module */
void eqns_done_proc();			/** Hide the frame */
void set_active_eqn();			/** Select new active, e.g. when target data changes */
void make_active_eqn();			/** Activate button proc */
void reset_equation();			/** Reset button proc */
void resize_eqn_canvas();		/** Canvas resize proc */
void handle_eq_event();			/** Canvas event proc */
void repaint_eqn_canvas();		/** Canvas repaint proc */
void show_eqn_window();			/** Frame show proc */
void eqn_cycle();			/** Eqn choice item notify proc */
equation *find_default_eq(int);		/** Direct access to default list by id*/
int max_params_in_table();		/** Maximum # params in any eq in table*/
#ifdef __cplusplus
}
#endif

/* macro to copy equations; stmt */

#define copy_equation(ptr_from, ptr_to)    *(ptr_to) = *(ptr_from)					

#endif
