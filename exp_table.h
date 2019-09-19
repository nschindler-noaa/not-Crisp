/** \file

 * \brief This definition file contains the variable definitions and look
   up macro for the table of exp(-x) exp_vec (a GLOBAL VARIABLE).

   Relevant range for exp(-x) is roughly 0 to 5;
   Beyond that exp(-x) is essentially 1 or 0 respectively.

   An Increment of 0.01 in the x variable is enough to guarantee 
   that the roundoff error for using this table is within 0.01 of the actual 
   value for exp(-x).

   Before the first call of LOOK_UP_NEXP() the function make_exp_vec must
   be called.
   
*/

#define MIN_EXP_X 0.0 
#define MAX_EXP_X 5.0 
#define EXP_INCR 0.0001
#define EXP_INCR_M1 10000.0
#define NUM_EXP_INCR ( (int) ((MAX_EXP_X - MIN_EXP_X)/EXP_INCR) ) 

extern double *exp_vec;
extern void make_exp_vec( void );

/* Before the first call of LOOK_UP_NEXP() the function make_exp_vec must
   be called. NOTE EXP_INCR_M1 is used because we think a multiply is
   faster than a divide, and this code is heavily used. */

#define LOOK_UP_NEXP(val) ( ( (val)>(MAX_EXP_X) ) ? \
  0.0 : (exp_vec[(int)((val)*EXP_INCR_M1)]) )


#include "gas_mort.h"
#include "gas_mort_params.h"
#include "reach_gas_specs.h"
#include "fdens.h"
#include <stdio.h>
#include <math.h>
 

