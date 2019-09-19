/** \file
 * \brief   Contains the definition parameters for the 
	mortality parameters used in the computation functions: 
	base_gas_mort.c, simpson.c, compute_gas_mort.c
*/


#ifndef GAS_MORT_PARAMS_H
#define GAS_MORT_PARAMS_H


typedef struct{
	double a;		/* Low mortality slope */
	double b;		/* High mortality slope */
	double ncrit;		/* Critical nitrogen level */
} GasMortParams;

/* Slope of decrease for critical nitrogen level */
/* Reference: Water Quality Guidelines for Dissolved Gas Supersaturation
   Aspen Applied Sciences Ltd.
   February 1994, p46:  73.89mmHg/meter depth compensation==2.96 (100%/ft)
*/
#define GAS_MCRIT (2.96) 	


#endif
