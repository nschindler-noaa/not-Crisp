/* This code computes look up table for exp(-x), where x
   ranges from MIN_EXP_X to MAX_EXP_X by INCR.

   The vector of values created is to be used for all
   reaches, releases, etc and is stored in the 
   global variable: exp_vec.

   Because exp_vec is a global variable, this space can
   be freed by free_exp_vec();
   
*/



#include "exp_table.h"
#include "nrutil_short.h"

double *exp_vec=0;

void make_exp_vec()
/* make_exp_vec creates a vector of length NUM_EXP_INCR+1
containing values for exp(-x) from MIN_EXP_X to MAX_EXP_X
by INCR. 
Pre_Contition: None. 
Post-Condition:exp_vec is a vector of non-negative values.
*/
{

	double x=MIN_EXP_X;
	int i;

	if(!exp_vec){	
	    exp_vec = dvector(1,NUM_EXP_INCR);
	    for(i=0; i<=NUM_EXP_INCR; i++,x+=EXP_INCR)
		exp_vec[i] = exp(-x);
	}

}

void free_exp_vec()
/* Frees the global variable: exp_vec */
{
	free_dvector(exp_vec,1);
	exp_vec=0;
}


