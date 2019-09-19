/** \file
 * \brief Default values for equations. */

#include "crisp.h"			/* includes eqns.h and necessary xview .h files */
#include "writeutil.h"

/* default equation table - contains values, ranges, and formulae for each
 * possible equation.  all significant parameters must be named.
 */

equation eqn_dflt_tab[NUM_EQNS] =
{
    /*  val	min	max	name   */
    
    /* class: linear */
    {EQ_LIN0,	2,	2,  2, -1, FLOAT_SCIENTIFIC,	/* id, #active, #total, #saved, divider, float type */
	{	{0,	0,	100.0, 	"  a"},	/* param0 */
		{0,	0,	10.0, 	"  b"},	/* param1 */
		{0,	0,	0, 	""},	/* param2 */
		{0,	0,	0, 	""},	/* param3 */
		{0,	0,	0, 	""},	/* param4 */
		{0,	0,	0, 	""},	/* param5 */
		{0,	0,	0, 	""},	/* param6 */
		{0,	0,	0, 	""},	/* param7 */
		{0,	0,	0, 	""},	/* param8 */
		{0,	0,	0, 	""},	/* param9 */
		{0,	0,	0,	""},	/* param10 */
		{0,	0,	0,	""},	/* param11 */
		{0,	0,	0,	""},	/* param12 */
		{0,	0,	0,	""},	/* param13 */
		{0,	0,	0,	""}		/* param14 */	},
        "Y = a + b*T", "LIN0"},
    {EQ_LIN1,	3,	3,  3, -1, FLOAT_SCIENTIFIC,	/* id, #active, #total, #saved, divider, float type */
    {	{0,	0,	20,	"  a"},	/* param0 */
    	{1.0,	0,	5,	"  b"},	/* param1 */
    	{70,	0,	100,	"var"},	/* param2 */
    	{0,	0,	0, 	""},	/* param3 */
    	{0,	0,	0, 	""},	/* param4 */
    	{0,	0,	0, 	""},	/* param5 */
    	{0,	0,	0, 	""},	/* param6 */
    	{0,	0,	0, 	""},	/* param7 */
    	{0,	0,	0, 	""},	/* param8 */
    	{0,	0,	0, 	""},	/* param9 */
		{0,	0,	0,	""},	/* param10 */
		{0,	0,	0,	""},	/* param11 */
		{0,	0,	0,	""},	/* param12 */
		{0,	0,	0,	""},	/* param13 */
		{0,	0,	0,	""}		/* param14 */	},
	"Y = a + b*X + e", "LIN1"},
    {EQ_LIN2,	3,	3,  3, -1, FLOAT_SCIENTIFIC,	/* id, #active, #total, #saved, divider, float type */
    {	{0,	0,	20,	"  a"},	/* param0 */
    	{1.0,	0,	5,	"  b"},	/* param1 */
    	{0.025,	0,	0.3,	"var"},	/* param2 */
    	{0,	0,	0, 	""},	/* param3 */
    	{0,	0,	0, 	""},	/* param4 */
    	{0,	0,	0, 	""},	/* param5 */
    	{0,	0,	0, 	""},	/* param6 */
    	{0,	0,	0, 	""},	/* param7 */
    	{0,	0,	0, 	""},	/* param8 */
    	{0,	0,	0, 	""},	/* param9 */
		{0,	0,	0,	""},	/* param10 */
		{0,	0,	0,	""},	/* param11 */
		{0,	0,	0,	""},	/* param12 */
		{0,	0,	0,	""},	/* param13 */
		{0,	0,	0,	""}		/* param14 */	},
    	"Y = a + b*X + X*e", "LIN2"},
    {EQ_LIN3,	3,	3,  3, -1, FLOAT_SCIENTIFIC,	/* id, #active, #total, #saved, divider, float type */
    {	{0,	0,	0, 	"  a"},	/* param0 */
    	{0,	0,	0, 	"  b"},	/* param1 */
    	{0,	0,	0, 	"  c"},	/* param2 */
    	{0,	0,	0, 	""},	/* param3 */
    	{0,	0,	0, 	""},	/* param4 */
    	{0,	0,	0, 	""},	/* param5 */
    	{0,	0,	0, 	""},	/* param6 */
    	{0,	0,	0, 	""},	/* param7 */
    	{0,	0,	0, 	""},	/* param8 */
    	{0,	0,	0, 	""},	/* param9 */
		{0,	0,	0,	""},	/* param10 */
		{0,	0,	0,	""},	/* param11 */
		{0,	0,	0,	""},	/* param12 */
		{0,	0,	0,	""},	/* param13 */
		{0,	0,	0,	""}		/* param14 */	},
	"Y = a + b*X + (X**2)*e", "LIN3"},
    
    /* class: exponential */
    {EQ_EXP0,	3,	3,  3, -1, FLOAT_SCIENTIFIC,	/* id, #active, #total, #saved, divider, float type */
    {	{0.05,	0,	0.3,	"  a"},	/* param0 */
    	{1.0,	0,	5.0,	"  b"},	/* param1 */
    	{0.25,	0,	2.0,	"var"},	/* param2 */
    	{0,	0,	0, 	""},	/* param3 */
    	{0,	0,	0, 	""},	/* param4 */
    	{0,	0,	0, 	""},	/* param5 */
    	{0,	0,	0, 	""},	/* param6 */
    	{0,	0,	0, 	""},	/* param7 */
    	{0,	0,	0, 	""},	/* param8 */
    	{0,	0,	0, 	""},	/* param9 */
		{0,	0,	0,	""},	/* param10 */
		{0,	0,	0,	""},	/* param11 */
		{0,	0,	0,	""},	/* param12 */
		{0,	0,	0,	""},	/* param13 */
		{0,	0,	0,	""}		/* param14 */	},
	"Y = b*exp(a*X + e)", "EXP0"},
    {EQ_EXP1,	3,	3,  3, -1, FLOAT_SCIENTIFIC,	/* id, #active, #total, #saved, divider, float type */
    {	{0,	0,	0, 	"  a"},	/* param0 */
    	{0,	0,	0, 	"  b"},	/* param1 */
    	{0,	0,	0, 	"  c"},	/* param2 */
    	{0,	0,	0, 	""},	/* param3 */
    	{0,	0,	0, 	""},	/* param4 */
    	{0,	0,	0, 	""},	/* param5 */
    	{0,	0,	0, 	""},	/* param6 */
    	{0,	0,	0, 	""},	/* param7 */
    	{0,	0,	0, 	""},	/* param8 */
    	{0,	0,	0, 	""},	/* param9 */
		{0,	0,	0,	""},	/* param10 */
		{0,	0,	0,	""},	/* param11 */
		{0,	0,	0,	""},	/* param12 */
		{0,	0,	0,	""},	/* param13 */
		{0,	0,	0,	""}		/* param14 */	},
	"Y = b*exp(a*X) + e", "EXP1"},
    {EQ_EXP2,	3,	3,  3, -1, FLOAT_SCIENTIFIC,	/* id, #active, #total, #saved, divider, float type */
    {	{0.05,	0,	0.3,	"  a"},	/* param0 */
    	{1.0,	0,	5.0,	"  b"},	/* param1 */
    	{0.03,	0,	0.05,	"var"},	/* param2 */
    	{0,	0,	0, 	""},	/* param3 */
    	{0,	0,	0, 	""},	/* param4 */
    	{0,	0,	0, 	""},	/* param5 */
    	{0,	0,	0, 	""},	/* param6 */
    	{0,	0,	0, 	""},	/* param7 */
    	{0,	0,	0, 	""},	/* param8 */
    	{0,	0,	0, 	""},	/* param9 */
		{0,	0,	0,	""},	/* param10 */
		{0,	0,	0,	""},	/* param11 */
		{0,	0,	0,	""},	/* param12 */
		{0,	0,	0,	""},	/* param13 */
		{0,	0,	0,	""}		/* param14 */	},
	"Y = b*exp(a*X) + X*e", "EXP2"},
    {EQ_EXP3,	3,	3,  3, -1, FLOAT_SCIENTIFIC,	/* id, #active, #total, #saved, divider, float type */
    {	{0.05,	0,	0.3,	"  a"},	/* param0 */
    	{1.0,	0,	5.0,	"  b"},	/* param1 */
    	{0.03,	0,	0.05,	"var"},	/* param2 */
    	{0,	0,	0, 	""},	/* param3 */
    	{0,	0,	0, 	""},	/* param4 */
    	{0,	0,	0, 	""},	/* param5 */
    	{0,	0,	0, 	""},	/* param6 */
    	{0,	0,	0, 	""},	/* param7 */
    	{0,	0,	0, 	""},	/* param8 */
    	{0,	0,	0, 	""},	/* param9 */
		{0,	0,	0,	""},	/* param10 */
		{0,	0,	0,	""},	/* param11 */
		{0,	0,	0,	""},	/* param12 */
		{0,	0,	0,	""},	/* param13 */
		{0,	0,	0,	""}		/* param14 */	},
	"Y = b*exp(a*X) + (X**2)*e", "EXP3"},
    
    /* class:  power */
    {EQ_POW0,	3,	3,  3, -1, FLOAT_SCIENTIFIC,	/* id, #active, #total, #saved, divider, float type */
    {	{1.4,	0,	5.0,	"  a"},	/* param0 */
    	{0.17,	0,	5.0,	"  b"},	/* param1 */
    	{0.005,	0,	0.05,	"var"},	/* param2 */
    	{0,	0,	0, 	""},	/* param3 */
    	{0,	0,	0, 	""},	/* param4 */
    	{0,	0,	0, 	""},	/* param5 */
    	{0,	0,	0, 	""},	/* param6 */
    	{0,	0,	0, 	""},	/* param7 */
    	{0,	0,	0, 	""},	/* param8 */
    	{0,	0,	0, 	""},	/* param9 */
		{0,	0,	0,	""},	/* param10 */
		{0,	0,	0,	""},	/* param11 */
		{0,	0,	0,	""},	/* param12 */
		{0,	0,	0,	""},	/* param13 */
		{0,	0,	0,	""}		/* param14 */	},
    	"Y = b*(X**(a + e))", "POW0"},
    {EQ_POW1,	3,	3,  3, -1, FLOAT_SCIENTIFIC,	/* id, #active, #total, #saved, divider, float type */
    {	{0,	0,	0, 	"  a"},	/* param0 */
    	{0,	0,	0, 	"  b"},	/* param1 */
    	{0,	0,	0, 	"  c"},	/* param2 */
    	{0,	0,	0, 	""},	/* param3 */
    	{0,	0,	0, 	""},	/* param4 */
    	{0,	0,	0, 	""},	/* param5 */
    	{0,	0,	0, 	""},	/* param6 */
    	{0,	0,	0, 	""},	/* param7 */
    	{0,	0,	0, 	""},	/* param8 */
    	{0,	0,	0, 	""},	/* param9 */
		{0,	0,	0,	""},	/* param10 */
		{0,	0,	0,	""},	/* param11 */
		{0,	0,	0,	""},	/* param12 */
		{0,	0,	0,	""},	/* param13 */
		{0,	0,	0,	""}		/* param14 */	},
	"Y = b*(X**a) + e", "POW1"},
    {EQ_POW2,	3,	3,  3, -1, FLOAT_SCIENTIFIC,	/* id, #active, #total, #saved, divider, float type */
    {	{1.0,	0,	3.0,	"  a"},	/* param0 */
    	{1.0,	0,	100.0,	"  b"},	/* param1 */
    	{0.025,	0,	0.5,	"var"},	/* param2 */
    	{0,	0,	0, 	""},	/* param3 */
    	{0,	0,	0, 	""},	/* param4 */
    	{0,	0,	0, 	""},	/* param5 */
    	{0,	0,	0, 	""},	/* param6 */
    	{0,	0,	0, 	""},	/* param7 */
    	{0,	0,	0, 	""},	/* param8 */
    	{0,	0,	0, 	""},	/* param9 */
		{0,	0,	0,	""},	/* param10 */
		{0,	0,	0,	""},	/* param11 */
		{0,	0,	0,	""},	/* param12 */
		{0,	0,	0,	""},	/* param13 */
		{0,	0,	0,	""}		/* param14 */	},
		"Y = b*((X/100)**a) + X*e", "POW2"},
    {EQ_POW3,	3,	3,  3, -1, FLOAT_SCIENTIFIC,	/* id, #active, #total, #saved, divider, float type */
    {	{1.4,	0,	5.0,	"  a"},	/* param0 */
    	{0.17,	0,	5.0,	"  b"},	/* param1 */
    	{0.025,	0,	0.5,	"var"},	/* param2 */
    	{0,	0,	0, 	""},	/* param3 */
    	{0,	0,	0, 	""},	/* param4 */
    	{0,	0,	0, 	""},	/* param5 */
    	{0,	0,	0, 	""},	/* param6 */
    	{0,	0,	0, 	""},	/* param7 */
    	{0,	0,	0, 	""},	/* param8 */
    	{0,	0,	0, 	""},	/* param9 */
		{0,	0,	0,	""},	/* param10 */
		{0,	0,	0,	""},	/* param11 */
		{0,	0,	0,	""},	/* param12 */
		{0,	0,	0,	""},	/* param13 */
		{0,	0,	0,	""}		/* param14 */	},
	"Y = b*(X**a) + (X**2)*e", "POW3"},
    /* application specific equations */
    {EQ_NSAT0,	3,	3,  3, -1, FLOAT_SCIENTIFIC,	/* id, #active, #total, #saved, divider, float type */
    {	{30.0,	0,	50.0,	"  a"},	/* param0: gas saturation coeff 1 */
    	{0.025,	0,	0.05,	"  b"},	/* param1: gas saturation coeff 2 */
    	{0.03,	0,	0.1,	"  k"},	/* param2: gas saturation exponent */
    	{0,	0,	0, 	""},	/* param3 */
    	{0,	0,	0, 	""},	/* param4 */
    	{0,	0,	0, 	""},	/* param5 */
    	{0,	0,	0, 	""},	/* param6 */
    	{0,	0,	0, 	""},	/* param7 */
    	{0,	0,	0, 	""},	/* param8 */
    	{0,	0,	0, 	""},	/* param9 */
		{0,	0,	0,	""},	/* param10 */
		{0,	0,	0,	""},	/* param11 */
		{0,	0,	0,	""},	/* param12 */
		{0,	0,	0,	""},	/* param13 */
		{0,	0,	0,	""}		/* param14 */	},
	"Y = b*X + a*(1 - exp(-k*X))", "NSAT0"},
    {EQ_FL_ACT,	2,	2,  2, -1, FLOAT_SCIENTIFIC,	/* id, #active, #total, #saved, divider, float type */
    {	{6.0,	0,	100.0, 	"  a"},	/* param0 */
    	{12.0,	0,	200.0, 	"  b"},	/* param1 */
    	{0,	0,	0, 	""},	/* param2 */
    	{0,	0,	0, 	""},	/* param3 */
    	{0,	0,	0, 	""},	/* param4 */
    	{0,	0,	0, 	""},	/* param5 */
    	{0,	0,	0, 	""},	/* param6 */
    	{0,	0,	0, 	""},	/* param7 */
    	{0,	0,	0, 	""},	/* param8 */
    	{0,	0,	0, 	""},	/* param9 */
		{0,	0,	0,	""},	/* param10 */
		{0,	0,	0,	""},	/* param11 */
		{0,	0,	0,	""},	/* param12 */
		{0,	0,	0,	""},	/* param13 */
		{0,	0,	0,	""}		/* param14 */	},
        "Y = a - b*X/100", "FL_ACT"},
    {EQ_DELAY0,	2,	2,  2, -1, FLOAT_SCIENTIFIC,	/* id, #active, #total, #saved, divider, float type */
    {	{20.0,	0,	100.0,	"  a"},	/* param0 */
        {5.0,	0,	10.0,	"  b"},	/* param1 */
    	{0,	0,	0,	""},	/* param2 */
    	{0,	0,	0, 	""},	/* param3 */
    	{0,	0,	0, 	""},	/* param4 */
    	{0,	0,	0, 	""},	/* param5 */
    	{0,	0,	0, 	""},	/* param6 */
    	{0,	0,	0, 	""},	/* param7 */
    	{0,	0,	0, 	""},	/* param8 */
    	{0,	0,	0, 	""},	/* param9 */
		{0,	0,	0,	""},	/* param10 */
		{0,	0,	0,	""},	/* param11 */
		{0,	0,	0,	""},	/* param12 */
		{0,	0,	0,	""},	/* param13 */
		{0,	0,	0,	""}		/* param14 */	},
	"Y = exp(-a*(X**b))", "DELAY0"},
    {EQ_NSAT3,	2,	2,  2, -1, FLOAT_SCIENTIFIC,	/* id, #active, #total, #saved, divider, float type */
    {	{1.0,	0,	5.0,	"  a"},	/* param0: entrainment 1 */
    	{2.0,	0,	5.0,	"  b"},	/* param1: entrainment 2 */
    	{0,	0,	0,	""},	/* param2 */
    	{0,	0,	0, 	""},	/* param3 */
    	{0,	0,	0, 	""},	/* param4 */
    	{0,	0,	0, 	""},	/* param5 */
    	{0,	0,	0, 	""},	/* param6 */
    	{0,	0,	0, 	""},	/* param7 */
    	{0,	0,	0, 	""},	/* param8 */
    	{0,	0,	0, 	""},	/* param9 */
		{0,	0,	0,	""},	/* param10 */
		{0,	0,	0,	""},	/* param11 */
		{0,	0,	0,	""},	/* param12 */
		{0,	0,	0,	""},	/* param13 */
		{0,	0,	0,	""}		/* param14 */	},
	"gas spill 3", "NSAT3"},
    {EQ_LOG0,	3,	3,  3, -1, FLOAT_SCIENTIFIC,	/* id, #active, #total, #saved, divider, float type */
    {	{0,	0,	20,	"  a"},	/* param0 */
    	{1.0,	0,	20,	"  b"},	/* param1 */
    	{20.0,	0,	50,	"var"},	/* param2 */
    	{0,	0,	0, 	""},	/* param3 */
    	{0,	0,	0, 	""},	/* param4 */
    	{0,	0,	0, 	""},	/* param5 */
    	{0,	0,	0, 	""},	/* param6 */
    	{0,	0,	0, 	""},	/* param7 */
    	{0,	0,	0, 	""},	/* param8 */
    	{0,	0,	0, 	""},	/* param9 */
		{0,	0,	0,	""},	/* param10 */
		{0,	0,	0,	""},	/* param11 */
		{0,	0,	0,	""},	/* param12 */
		{0,	0,	0,	""},	/* param13 */
		{0,	0,	0,	""}		/* param14 */	},
	"Y = a + b*ln(X) + e", "LOG0"},
    {EQ_NSAT1,	3,	3,  3, -1, FLOAT_SCIENTIFIC,	/* id, #active, #total, #saved, divider, float type */
    {	{2.469,	-2.0,	3.0,	"  a"},	/* param0: entrainment 1 */
    	{1.108,	0,	5.0,	"  b"},	/* param1: entrainment 2 */
		{-1.103,	-3.0,	2.0,	"  c"},	/* param2: entrainment 3 */
    	{0,	0,	0, 	""},	/* param3 */
    	{0,	0,	0, 	""},	/* param4 */
    	{0,	0,	0, 	""},	/* param5 */
    	{0,	0,	0, 	""},	/* param6 */
    	{0,	0,	0, 	""},	/* param7 */
    	{0,	0,	0, 	""},	/* param8 */
    	{0,	0,	0, 	""},	/* param9 */
		{0,	0,	0,	""},	/* param10 */
		{0,	0,	0,	""},	/* param11 */
		{0,	0,	0,	""},	/* param12 */
		{0,	0,	0,	""},	/* param13 */
		{0,	0,	0,	""}		/* param14 */	},
	"gas spill 1", "NSAT1"},
    {EQ_NSAT2,	3,	3,  3, -1, FLOAT_SCIENTIFIC,	/* id, #active, #total, #saved, divider, float type */
    {	{3.31,	-10.0,	50.0,	"  a"},	/* param0: entrainment 1 */
    	{0.41,	-2.0,	5.0,	"  b"},	/* param1: entrainment 2 */
		{-0.032,	-2.0,	1.0,	"  c"},	/* param2: entrainment 3 */
    	{0,	0,	0, 	""},	/* param3 */
    	{0,	0,	0, 	""},	/* param4 */
    	{0,	0,	0, 	""},	/* param5 */
    	{0,	0,	0, 	""},	/* param6 */
    	{0,	0,	0, 	""},	/* param7 */
    	{0,	0,	0, 	""},	/* param8 */
    	{0,	0,	0, 	""},	/* param9 */
		{0,	0,	0,	""},	/* param10 */
		{0,	0,	0,	""},	/* param11 */
		{0,	0,	0,	""},	/* param12 */
		{0,	0,	0,	""},	/* param13 */
		{0,	0,	0,	""}		/* param14 */	},
	"gas spill 2", "NSAT2"},
    {EQ_GMORT0,	3,	3,  3, -1, FLOAT_SCIENTIFIC,	/* id, #active, #total, #saved, divider, float type */
    {	{0.000019, 0,	0.001, 	"mlow"},	/* param0 */
    	{0.00558,	0,	0.01, 	" mhi"},	/* param1 */
    	{20,	0,	25, 	"crit"},	/* param2 */
    	{0,	0,	0, 	""},	/* param3 */
    	{0,	0,	0, 	""},	/* param4 */
    	{0,	0,	0, 	""},	/* param5 */
    	{0,	0,	0, 	""},	/* param6 */
    	{0,	0,	0, 	""},	/* param7 */
    	{0,	0,	0, 	""},	/* param8 */
    	{0,	0,	0, 	""},	/* param9 */
		{0,	0,	0,	""},	/* param10 */
		{0,	0,	0,	""},	/* param11 */
		{0,	0,	0,	""},	/* param12 */
		{0,	0,	0,	""},	/* param13 */
		{0,	0,	0,	""}		/* param14 */	},
        "Archaic Gas Mort", "GMORT0"},
    {EQ_DRIFT,	4,	4,  4, -1, FLOAT_SCIENTIFIC,	/* id, #active, #total, #saved, divider, float type */
    {	{300,	0.001,	1000.0, " B0"},	/* param0 */
    	{3.0,	0.001,	100.0, 	" B1"},	/* param1 */
    	{0.3,	0.001,	1.0, 	"  R"},	/* param3 */
    	{0.75,	0.001,	0.999, 	"  h"},	/* param3 */
    	{0,	0,	0, 	""},	/* param4 */
    	{0,	0,	0, 	""},	/* param5 */
    	{0,	0,	0, 	""},	/* param6 */
    	{0,	0,	0, 	""},	/* param7 */
    	{0,	0,	0, 	""},	/* param8 */
    	{0,	0,	0, 	""},	/* param9 */
		{0,	0,	0,	""},	/* param10 */
		{0,	0,	0,	""},	/* param11 */
		{0,	0,	0,	""},	/* param12 */
		{0,	0,	0,	""},	/* param13 */
		{0,	0,	0,	""}		/* param14 */	},
        "Fractional Drift Velocity", "DRIFT"},
    {EQ_DELAY1,	4,	4,  4, -1, FLOAT_SCIENTIFIC,	/* id, #active, #total, #saved, divider, float type */
    {	{75.0,	0,	250.0,	"flow"},	/* param0 */
        {100.0,	0,	250.0,	"depth"},/* param1 */
    	{140.0,	0,	365.0,	"day"},	/* param2 */
    	{1.0,	0.5,	10.0, 	"scale"},/* param3 */
    	{0,	0,	0, 	""},	/* param4 */
    	{0,	0,	0, 	""},	/* param5 */
    	{0,	0,	0, 	""},	/* param6 */
    	{0,	0,	0, 	""},	/* param7 */
    	{0,	0,	0, 	""},	/* param8 */
    	{0,	0,	0, 	""},	/* param9 */
		{0,	0,	0,	""},	/* param10 */
		{0,	0,	0,	""},	/* param11 */
		{0,	0,	0,	""},	/* param12 */
		{0,	0,	0,	""},	/* param13 */
		{0,	0,	0,	""}		/* param14 */	},
	"Dam Delay Probability", "DELAY1"},
    {EQ_FGE,	5,	5,  5, -1, FLOAT_SCIENTIFIC,	/* id, #active, #total, #saved, divider, float type */
	{	{0,	0,	100,	"B0"},   /* initial depth */
    	{0,	0,	100,	"B1"},   /* final depth */
    	{0,	0,	100,	"B2"},	/* param2 */
    	{0,	0,	100, 	"B3"},	/* param3 */
    	{0,	0,	100, 	"B4"},/* smolt date */
    	{0,	0,	0, 	""},	/* param5 */
    	{0,	0,	0, 	""},	/* param6 */
    	{0,	0,	0, 	""},	/* param7 */
    	{0,	0,	0, 	""},	/* param8 */
    	{0,	0,	0, 	""},	/* param9 */
		{0,	0,	0,	""},	/* param10 */
		{0,	0,	0,	""},	/* param11 */
		{0,	0,	0,	""},	/* param12 */
		{0,	0,	0,	""},	/* param13 */
		{0,	0,	0,	""}		/* param14 */	},
	"Night FGE", "FGE"},
    {EQ_NSAT4,	3,	3,  3, -1, FLOAT_SCIENTIFIC,	/* id, #active, #total, #saved, divider, float type */
    {	{30.0,	0,	50.0,	"  a"},	/* param0: gas saturation coeff 1 */
    	{0.025,	0,	0.12,	"  b"},	/* param1: gas saturation coeff 2 */
    	{6.0,	0,	100.0,	"  h"},	/* param2: gas saturation exponent */
    	{0,	0,	0, 	""},	/* param3 */
    	{0,	0,	0, 	""},	/* param4 */
    	{0,	0,	0, 	""},	/* param5 */
    	{0,	0,	0, 	""},	/* param6 */
    	{0,	0,	0, 	""},	/* param7 */
    	{0,	0,	0, 	""},	/* param8 */
    	{0,	0,	0, 	""},	/* param9 */
		{0,	0,	0,	""},	/* param10 */
		{0,	0,	0,	""},	/* param11 */
		{0,	0,	0,	""},	/* param12 */
		{0,	0,	0,	""},	/* param13 */
		{0,	0,	0,	""}		/* param14 */	},
	"Y = b*X + a*X/(h + X)", "NSAT4"},
    {EQ_MIGR,	8,	8,  6, 5, FLOAT_SCIENTIFIC,	/* id, #active, #total, #saved, divider, float type */
    {	{1.2,    -10.0,	20.0,   "Bmin"},	/* param0 */
    	{17,	0,	30, 	"Bmax"},	/* param2 */
    	{0.5,	0,	2.0, 	"Bflow"},/* param1 */
    	{0.2,	0,	2.0, 	"Alpha"},/* param3 */
    	{110,	50.0,	250.0, 	"Tseas"},/* param4 */
    	{0.1,	0,	1.0, 	"Alpha2"},/* param5 */
    	{10.0,	0,	20.0, 	"River vel"},	/* param6 */
    	{100.0,	0,	300.0, 	"Release date"},	/* param7 */
    	{0,	0,	0, 	""},	/* param8 */
    	{0,	0,	0, 	""},	/* param9 */
		{0,	0,	0,	""},	/* param10 */
		{0,	0,	0,	""},	/* param11 */
		{0,	0,	0,	""},	/* param12 */
		{0,	0,	0,	""},	/* param13 */
		{0,	0,	0,	""}		/* param14 */	},
        "Seasonal Flow & Exper Migration", "MIGR"},
    {EQ_GMORT1,	7,	7,  4, 3, FLOAT_SCIENTIFIC,	/* id, #active, #total, #saved, divider, float type */
    {	{0.000019, 0,	0.001, 	"mlow"},	/* param0 */
    	{0.00558,	0,	0.01, 	" mhi"},	/* param1 */
    	{10.9,	5.0,	15.0, 	"crit"},	/* param2 */
    	{2.96,	2.0,	4.0, 	" mcr"},	/* param3 */
    	{40,	20,	80, 	" len"},	/* param4 */
    	{10,	1,	20, 	" vel"},	/* param5 */
    	{100,	20,	100, 	"dpth"},	/* param6 */
    	{0,	0,	0, 	""},	/* param7 */
    	{0,	0,	0, 	""},	/* param8 */
    	{0,	0,	0, 	""},	/* param9 */
		{0,	0,	0,	""},	/* param10 */
		{0,	0,	0,	""},	/* param11 */
		{0,	0,	0,	""},	/* param12 */
		{0,	0,	0,	""},	/* param13 */
		{0,	0,	0,	""}		/* param14 */	},
        "Depth Dependent Gas Mort", "GMORT1"},
    {EQ_GMORT2,	6,	6,  4, 3, FLOAT_SCIENTIFIC,	/* id, #active, #total, #saved, divider, float type */
    {	{0.000019, 0,	0.001, 	"mlow"},	/* param0 */
    	{0.00558,	0,	0.01, 	" mhi"},	/* param1 */
    	{10.9,	5.0,	15.0, 	"crit"},	/* param2 */
    	{2.96,	2.0,	4.0, 	" mcr"},	/* param3 */
    	{40,	5,	80, 	" len"},	/* param4 */
    	{10,	1,	50, 	" vel"},	/* param5 */
    	{0,	0,	0, 	""},	/* param6 */
    	{0,	0,	0, 	""},	/* param7 */
    	{0,	0,	0, 	""},	/* param8 */
    	{0,	0,	0, 	""},	/* param9 */
		{0,	0,	0,	""},	/* param10 */
		{0,	0,	0,	""},	/* param11 */
		{0,	0,	0,	""},	/* param12 */
		{0,	0,	0,	""},	/* param13 */
		{0,	0,	0,	""}		/* param14 */	},
        "Obsolete Density/Depth Dependent Gas Mort", "GMORT2"},
    {EQ_FDENS,	3,	3,  2, 1, FLOAT_SCIENTIFIC,	/* id, #active, #total, #saved, divider, float type */
    {	{39.0,	0.01,	100.0,	"Dmode"},/* param0: max density depth */
    	{100.0,	0.01,	100.0,	"Dbot"}, /* param1: max fish depth */
    	{100.0,	0.01,	100.0, 	"depth"},	/* param2 */
    	{0,	0,	0, 	""},	/* param3 */
    	{0,	0,	0, 	""},	/* param4 */
    	{0,	0,	0, 	""},	/* param5 */
    	{0,	0,	0, 	""},	/* param6 */
    	{0,	0,	0, 	""},	/* param7 */
    	{0,	0,	0, 	""},	/* param8 */
    	{0,	0,	0, 	""},	/* param9 */
		{0,	0,	0,	""},	/* param10 */
		{0,	0,	0,	""},	/* param11 */
		{0,	0,	0,	""},	/* param12 */
		{0,	0,	0,	""},	/* param13 */
		{0,	0,	0,	""}		/* param14 */	},
	"Populaton Density vs. Depth", "FDENS"},
    {EQ_TRANS,	4,	4,  4, -1, FLOAT_SCIENTIFIC,	/* id, #active, #total, #saved, divider, float type */
    {	{15.7,	0,	50.0,	"tt1"},	/* param0: travel time 1 */
    	{0.0,	0,	0.5,	"m1+"},  /* param1: delta mort 1 */
    	{30.0,	0,	150.0,	"tt2"},	/* param2: travel time 2 */
    	{0.0,	0,	0.5, 	"m2+"},	/* param3: delta mort 2 */
    	{0,	0,	0, 	""},	/* param4: variance minus */
    	{0,	0,	0, 	""},	/* param5: variance plus */
    	{0,	0,	0, 	""},	/* param6 */
    	{0,	0,	0, 	""},	/* param7 */
    	{0,	0,	0, 	""},	/* param8 */
    	{0,	0,	0, 	""},	/* param9 */
		{0,	0,	0,	""},	/* param10 */
		{0,	0,	0,	""},	/* param11 */
		{0,	0,	0,	""},	/* param12 */
		{0,	0,	0,	""},	/* param13 */
		{0,	0,	0,	""}		/* param14 */	},
	"Transport Mortality", "TRANS"},
    {EQ_NSAT5,	4,	4,  4, -1, FLOAT_SCIENTIFIC,	/* id, #active, #total, #saved, divider, float type */
    {	{5.0,    0.0,	50.0,	"  b"},	/* param0: y intercept */
    	{0.27,	0.0,	10.0,	"  m"},	/* param1: slope */
    	{0.0,	0.0,	50.0,	" lb"},	/* param2: lower bound */
    	{400.0,	10.0,	500.0, 	" ub"},	/* param3: upper bound */
    	{0,	0,	0, 	""},	/* param4 */
    	{0,	0,	0, 	""},	/* param5 */
    	{0,	0,	0, 	""},	/* param6 */
    	{0,	0,	0, 	""},	/* param7 */
    	{0,	0,	0, 	""},	/* param8 */
    	{0,	0,	0, 	""},	/* param9 */
		{0,	0,	0,	""},	/* param10 */
		{0,	0,	0,	""},	/* param11 */
		{0,	0,	0,	""},	/* param12 */
		{0,	0,	0,	""},	/* param13 */
		{0,	0,	0,	""}		/* param14 */	},
	"Bounded Linear", "NSAT5"},
    {EQ_NSAT6,	5,	5,  5, -1, FLOAT_SCIENTIFIC,	/* id, #active, #total, #saved, divider, float type */
    {	{38.0,   0.0,	100.0,	"  a"},	/* param0: a */
		{-36.0, -100.0,	0.0,	"  b"},	/* param1: b */
		{-0.02,	-10.0,	0.0, 	"  c"},	/* param2: c */
    	{0.0,	0.0,	50.0,	" lb"},	/* param3: lower bound */
    	{400.0,	10.0,	500.0, 	" ub"},	/* param4: upper bound */
    	{0,	0,	0, 	""},	/* param5 */
    	{0,	0,	0, 	""},	/* param6 */
    	{0,	0,	0, 	""},	/* param7 */
    	{0,	0,	0, 	""},	/* param8 */
    	{0,	0,	0, 	""},	/* param9 */
		{0,	0,	0,	""},	/* param10 */
		{0,	0,	0,	""},	/* param11 */
		{0,	0,	0,	""},	/* param12 */
		{0,	0,	0,	""},	/* param13 */
		{0,	0,	0,	""}		/* param14 */	},
	"Bounded Exponential", "NSAT6"},
    {EQ_GAS_DISP,	1,	1,  1, -1, FLOAT_SCIENTIFIC,	/* id, #active, #total, #saved, divider, float type */
    {	{150.0,	0,	365.0,	"day"},	/* param0 */
        {0,	0,	0,	""},	/* param1 */
    	{0,	0,	0,	""},	/* param2 */
    	{0,	0,	0, 	""},	/* param3 */
    	{0,	0,	0, 	""},	/* param4 */
    	{0,	0,	0, 	""},	/* param5 */
    	{0,	0,	0, 	""},	/* param6 */
    	{0,	0,	0, 	""},	/* param7 */
    	{0,	0,	0, 	""},	/* param8 */
    	{0,	0,	0, 	""},	/* param9 */
		{0,	0,	0,	""},	/* param10 */
		{0,	0,	0,	""},	/* param11 */
		{0,	0,	0,	""},	/* param12 */
		{0,	0,	0,	""},	/* param13 */
		{0,	0,	0,	""}		/* param14 */	},
	"TDG Saturation > 100% by Distance", "GAS_DISP"},
    {EQ_GAS_DISP_RIGHT,	1,	1,  1, -1, FLOAT_SCIENTIFIC,	/* id, #active, #total, #saved, divider, float type */
    {	{150.0,	0,	365.0,	"day"},	/* param0 */
        {0,	0,	0,	""},	/* param1 */
    	{0,	0,	0,	""},	/* param2 */
    	{0,	0,	0, 	""},	/* param3 */
    	{0,	0,	0, 	""},	/* param4 */
    	{0,	0,	0, 	""},	/* param5 */
    	{0,	0,	0, 	""},	/* param6 */
    	{0,	0,	0, 	""},	/* param7 */
    	{0,	0,	0, 	""},	/* param8 */
    	{0,	0,	0, 	""},	/* param9 */
		{0,	0,	0,	""},	/* param10 */
		{0,	0,	0,	""},	/* param11 */
		{0,	0,	0,	""},	/* param12 */
		{0,	0,	0,	""},	/* param13 */
		{0,	0,	0,	""}		/* param14 */	},
	"TDG Saturation > 100% by Distance", "GAS_DISP_RIGHT"},
    {EQ_MIGR2,	7,	9,  8, -1, FLOAT_SCIENTIFIC,	/* id, #active, #total, #saved, divider, float type */
    {	{1.2,    -10.0,	20.0,   "Bmin"},	/* param0 */
    	{17,	0,	30, 	"Bmax"},	/* param2 */
    	{0.5,	0,	2.0, 	"Bflow"},/* param1 */
    	{0.2,	0,	2.0, 	"Alpha"},/* param3 */
    	{110,	50.0,	250.0, 	"Tseas"},/* param4 */
    	{10.0,	0,	20.0, 	"River vel"},	/* param5 */
    	{100.0,	0,	300.0, 	"Release date"},	/* param6 */
    	{0.1,	0,	1.0, 	"alpha2"},/* param7, hidden */
 	{0.0,    0,      1.0,    "len"},  /* param8, hidden */
    	{0,	0,	0, 	""},	/* param9 */
		{0,	0,	0,	""},	/* param10 */
		{0,	0,	0,	""},	/* param11 */
		{0,	0,	0,	""},	/* param12 */
		{0,	0,	0,	""},	/* param13 */
		{0,	0,	0,	""}		/* param14 */	},
        "Seasonal Flow & Exper Migration w/ Length", "MIGR2"},
    {EQ_GMORT3,	8,	8,  3, 2, FLOAT_SCIENTIFIC,	/* id, #active, #total, #saved, divider, float type */
    {	{0.000019, 0,	0.001, 	"mlow"},	/* param0 */
    	{0.00558, 0,	0.01, 	" mhi"},	/* param1 */
        {10.9,	5.0,	15.0, 	"crit"},	/* param2 */
    	{0.0 ,	0.0,	50.0, 	"TDG2"},	/* param3 */
    	{0.5,	0.0,    1.0, 	"SplFr"},/* param4 */
    	{0.075,	0,	10.0, 	"theta"},/* param5 */
    	{40,	5,	80, 	"Reach length"},	/* param6 */
    	{10,	1,	50, 	"River vel"},	/* param7 */
    	{0,	0,	0, 	""},	/* param8 */
    	{0,	0,	0, 	""},	/* param9 */
		{0,	0,	0,	""},	/* param10 */
		{0,	0,	0,	""},	/* param11 */
		{0,	0,	0,	""},	/* param12 */
		{0,	0,	0,	""},	/* param13 */
		{0,	0,	0,	""}		/* param14 */	},
        "Density/Depth Dependent Gas Mort", "GMORT3"},
    /* class: cubic */
    {EQ_CUB0,	4,	4,  4, -1, FLOAT_SCIENTIFIC,	/* id, #active, #total, #saved, divider, float type */
    {	{1.0,	0,	50.0, 	"  a"},	/* param0 */
    	{1.0,	0,	50.0, 	"  b"},	/* param1 */
    	{1.0,	0,	50.0, 	"  c"},	/* param2 */
    	{70,	0,	100,	"var"},	/* param3 */
    	{0,	0,	0, 	""},	/* param4 */
    	{0,	0,	0, 	""},	/* param5 */
    	{0,	0,	0, 	""},	/* param6 */
    	{0,	0,	0, 	""},	/* param7 */
    	{0,	0,	0, 	""},	/* param8 */
    	{0,	0,	0, 	""},	/* param9 */
		{0,	0,	0,	""},	/* param10 */
		{0,	0,	0,	""},	/* param11 */
		{0,	0,	0,	""},	/* param12 */
		{0,	0,	0,	""},	/* param13 */
		{0,	0,	0,	""}		/* param14 */	},
        "Y = a*X + b*X^2 + C*X^3 + e", "CUB0"},
    {EQ_CUB1,	4,	4,  4, -1, FLOAT_SCIENTIFIC,	/* id, #active, #total, #saved, divider, float type */
	{    {2.583,	-5.0,	5.0, 	"  a"},	/* param0 */
		{-3.250,	-5.0,	5.0, "  b"},	/* param1 */
    	{1.667,	-5.0,	5.0, 	"  c"},	/* param2 */
    	{70,	0,	100,	"var"},	/* param3 */
    	{0,	0,	0, 	""},	/* param4 */
    	{0,	0,	0, 	""},	/* param5 */
    	{0,	0,	0, 	""},	/* param6 */
    	{0,	0,	0, 	""},	/* param7 */
    	{0,	0,	0, 	""},	/* param8 */
    	{0,	0,	0, 	""},	/* param9 */
		{0,	0,	0,	""},	/* param10 */
		{0,	0,	0,	""},	/* param11 */
		{0,	0,	0,	""},	/* param12 */
		{0,	0,	0,	""},	/* param13 */
		{0,	0,	0,	""}		/* param14 */	},
        "Y = (a*X/100 + b*(X/100)^2 + C*(X/100)^3)*100 + e", "CUB1"},
    {EQ_PRED_TEMP1,	2,	2,  2, -1, FLOAT_SCIENTIFIC,	/* id, #active, #total, #saved, divider, float type */
	{   {1.0,	0.0001,	5.0, 	"  a"},	/* param0 */
    	{0.207,	0.0001,	2.0, 	"  b"},	/* param1 */
    	{0,	0,	0, 	""},	/* param2 */
    	{0,	0,	0, 	""},	/* param3 */
    	{0,	0,	0, 	""},	/* param4 */
    	{0,	0,	0, 	""},	/* param5 */
    	{0,	0,	0, 	""},	/* param6 */
    	{0,	0,	0, 	""},	/* param7 */
    	{0,	0,	0, 	""},	/* param8 */
    	{0,	0,	0, 	""},	/* param9 */
		{0,	0,	0,	""},	/* param10 */
		{0,	0,	0,	""},	/* param11 */
		{0,	0,	0,	""},	/* param12 */
		{0,	0,	0,	""},	/* param13 */
		{0,	0,	0,	""}		/* param14 */	},
        "Y = a * exp(b * T)", "PRED_TEMP1"},
    {EQ_PRED_TEMP2,	3,	3,  3, -1, FLOAT_SCIENTIFIC,	/* id, #active, #total, #saved, divider, float type */
	{   {63.617,	0.0,	25000.0, 	"C_max"},	/* param0 */
    	{0.8425,	0.0,	10.0, 	"alpha"},	/* param1 */
    	{18.5,	0.0,	50.0, 	"T_inf"},	/* param2 */
    	{0,	0,	0, 	""},	/* param3 */
    	{0,	0,	0, 	""},	/* param4 */
    	{0,	0,	0, 	""},	/* param5 */
    	{0,	0,	0, 	""},	/* param6 */
    	{0,	0,	0, 	""},	/* param7 */
    	{0,	0,	0, 	""},	/* param8 */
    	{0,	0,	0, 	""},	/* param9 */
		{0,	0,	0,	""},	/* param10 */
		{0,	0,	0,	""},	/* param11 */
		{0,	0,	0,	""},	/* param12 */
		{0,	0,	0,	""},	/* param13 */
		{0,	0,	0,	""}		/* param14 */	},
        "Y = C_max / (1 + exp( -alpha * (T - T_inf))", "PRED_TEMP2"},
    {EQ_MIGR1,	1,	1,  1, -1, FLOAT_SCIENTIFIC,	/* id, #active, #total, #saved, divider, float type */
    {	{5,    0,	100.0,   "B0"},	/* param0 */
    	{0,	0,	0, 	""},	/* param1 */
    	{0,	0,	0, 	""},	/* param2 */
    	{0,	0,	0, 	""},	/* param3 */
    	{0,	0,	0, 	""},	/* param4 */
    	{0,	0,	0, 	""},	/* param5 */
    	{0,	0,	0, 	""},	/* param6 */
    	{0,	0,	0, 	""},	/* param7 */
    	{0,	0,	0, 	""},	/* param8 */
    	{0,	0,	0, 	""},	/* param9 */
		{0,	0,	0,	""},	/* param10 */
		{0,	0,	0,	""},	/* param11 */
		{0,	0,	0,	""},	/* param12 */
		{0,	0,	0,	""},	/* param13 */
		{0,	0,	0,	""}		/* param14 */	},
        "Constant Migration", "MIGR1"},
    {EQ_MIGR3,	3,	3,  2, 1, FLOAT_SCIENTIFIC,	/* id, #active, #total, #saved, divider, float type */
    {	{5,    -5.0,	50.0,   "B0"},	/* param0 */
    	{0.5,	0,	2.0, 	"Bflow"},/* param1 */
    	{10.0,	0,	100.0, 	"River vel"},	/* param2 */
    	{0,	0,	0, 	""},	/* param3 */
    	{0,	0,	0, 	""},	/* param4 */
    	{0,	0,	0, 	""},	/* param5 */
    	{0,	0,	0, 	""},	/* param6 */
    	{0,	0,	0, 	""},	/* param7 */
    	{0,	0,	0, 	""},	/* param8 */
    	{0,	0,	0, 	""},	/* param9 */
		{0,	0,	0,	""},	/* param10 */
		{0,	0,	0,	""},	/* param11 */
		{0,	0,	0,	""},	/* param12 */
		{0,	0,	0,	""},	/* param13 */
		{0,	0,	0,	""}		/* param14 */	},
        "Linear Flow Migration", "MIGR3"},
    {EQ_MIGR4,	5,	5,  4, 3, FLOAT_SCIENTIFIC,	/* id, #active, #total, #saved, divider, float type */
    {	{1.2,    -5.0,	50.0,   "B0"},	/* param0 */
    	{0.5,	0,	2.0, 	"Bflow"},/* param1 */
    	{0.2,	0,	2.0, 	"Alpha"},/* param2 */
    	{110,	50.0,	250.0, 	"Tseas"},/* param3 */
    	{10.0,	0,	100.0, 	"River vel"},	/* param4 */
    	{0,	0,	0, 	""},	/* param5 */
    	{0,	0,	0, 	""},	/* param6 */
    	{0,	0,	0, 	""},	/* param7 */
    	{0,	0,	0, 	""},	/* param8 */
    	{0,	0,	0, 	""},	/* param9 */
		{0,	0,	0,	""},	/* param10 */
		{0,	0,	0,	""},	/* param11 */
		{0,	0,	0,	""},	/* param12 */
		{0,	0,	0,	""},	/* param13 */
		{0,	0,	0,	""}		/* param14 */	},
        "Seasonal Flow Migration", "MIGR4"},
    {EQ_MIGR5,	3,	3,  2, 1, FLOAT_SCIENTIFIC,	/* id, #active, #total, #saved, divider, float type */
    {	{1.2,    -5.0,	50.0,   "B0"},	/* param0 */
    	{0.1,	  0,	1.0, 	"Blen"},	/* param1 */
     	{0.0,    0,  300.0,    "Fish len"},  /* param2 */
    	{0,	0,	0, 	""},	/* param3 */
    	{0,	0,	0, 	""},	/* param4 */
    	{0,	0,	0, 	""},	/* param5 */
    	{0,	0,	0, 	""},	/* param6 */
    	{0,	0,	0, 	""},	/* param7 */
    	{0,	0,	0, 	""},	/* param8 */
    	{0,	0,	0, 	""},	/* param9 */
		{0,	0,	0,	""},	/* param10 */
		{0,	0,	0,	""},	/* param11 */
		{0,	0,	0,	""},	/* param12 */
		{0,	0,	0,	""},	/* param13 */
		{0,	0,	0,	""}		/* param14 */	},
        "Linear Length Migration", "MIGR5"},
    {EQ_MIGR6,	5,	5,  3, 2, FLOAT_SCIENTIFIC,	/* id, #active, #total, #saved, divider, float type */
    {	{1.2,    -5.0,	50.0,   "B0"},	/* param0 */
    	{0.1,	  0,	1.0, 	"Blen"},	/* param1 */
    	{0.5,	0,	2.0, 	"Bflow"},/* param2 */
     	{0.0,    0,  300.0,    "Fish len"},  /* param3 */
    	{10.0,	0,	100.0, 	"River vel"},	/* param4 */
    	{0,	0,	0, 	""},	/* param5 */
    	{0,	0,	0, 	""},	/* param6 */
    	{0,	0,	0, 	""},	/* param7 */
    	{0,	0,	0, 	""},	/* param8 */
    	{0,	0,	0, 	""},	/* param9 */
		{0,	0,	0,	""},	/* param10 */
		{0,	0,	0,	""},	/* param11 */
		{0,	0,	0,	""},	/* param12 */
		{0,	0,	0,	""},	/* param13 */
		{0,	0,	0,	""}		/* param14 */	},
        "Linear Length + Flow Migration", "MIGR6"},
    {EQ_SPILL_EFFICIENCY,  4,  4,  4, -1, FLOAT_SCIENTIFIC,	/* id, #active, #total, #saved, divider, float type */
	{   {14.07,    10.0, 18.0,   "B0"},    /* param0 */
        {0.182,  0.0,    0.3,   "B1"},     /* param1 */
        {0,      0.0,    1.0,   "B2"},   /* param2 */
    	{0,		 0.0,	 1.0, 	"B3"},	/* param3 */
    	{0,	0,	0, 	""},	/* param4 */
    	{0,	0,	0, 	""},	/* param5 */
    	{0,	0,	0, 	""},	/* param6 */
    	{0,	0,	0, 	""},	/* param7 */
    	{0,	0,	0, 	""},	/* param8 */
    	{0,	0,	0, 	""},	/* param9 */
		{0,	0,	0,	""},	/* param10 */
		{0,	0,	0,	""},	/* param11 */
		{0,	0,	0,	""},	/* param12 */
		{0,	0,	0,	""},	/* param13 */
		{0,	0,	0,	""}		/* param14 */	},
        "exp(B0 + B1*log(1/(1-X))/(1+exp(B0+B1*log(1/(1-X))))", "SPILL_EFFICIENCY"}, //formula innaccurate
    {EQ_FLUSHTRANS,  3,  3,  3, -1, FLOAT_SCIENTIFIC,	/* id, #active, #total, #saved, divider, float type */
	{   {5.8259,    5.0, 6.0,   "a"},     /* param0 */
        {5.3533, 5.0,    6.0,   "b"},     /* param1 */
        {0.98,   0.0,    1.0,   "Vt"},/* param2 */
    	{0,	0,	0, 	""},	/* param3 */
    	{0,	0,	0, 	""},	/* param4 */
    	{0,	0,	0, 	""},	/* param5 */
    	{0,	0,	0, 	""},	/* param6 */
    	{0,	0,	0, 	""},	/* param7 */
    	{0,	0,	0, 	""},	/* param8 */
    	{0,	0,	0, 	""},	/* param9 */
		{0,	0,	0,	""},	/* param10 */
		{0,	0,	0,	""},	/* param11 */
		{0,	0,	0,	""},	/* param12 */
		{0,	0,	0,	""},	/* param13 */
		{0,	0,	0,	""}		/* param14 */	},
        "FLUSH Transport Merit", "FLUSHTRANS"},
    {EQ_DELAY3,	8,	8,  5, 4, FLOAT_SCIENTIFIC,	/* id, #active, #total, #saved, divider, float type */
    {	{0.1, 0.005,	100.0,   "A day"},	/* param0 */
    	{0.2, 0.005,	100.0,   "A night"},	/* param1 */
    	{0.1,	0.0,	1.0,   "B flow"},	/* param2 */
    	{0.3,	0.0,	2.0,   "B Spill"},	/* param3 */
    	{0.0,	0.0,	0.01,   "B date"},	/* param4 */
    	{0,	0,	100.0,     "River vel"},	/* param5 */
    	{0,	0,	1.00,     "Spill fraction"},	/* param6 */
    	{0,	0,	365,     "Julian date"},	/* param7 */
    	{0,	0,	0,   ""},	/* param8 */
    	{0,	0,	0,   ""},	/* param9 */
		{0,	0,	0,	""},	/* param10 */
		{0,	0,	0,	""},	/* param11 */
		{0,	0,	0,	""},	/* param12 */
		{0,	0,	0,	""},	/* param13 */
		{0,	0,	0,	""}		/* param14 */	},
        "Forebay Delay Probability", "DELAY3"},
    {EQ_MIGR7,	3,	3,  2, 1, FLOAT_SCIENTIFIC,	/* id, #active, #total, #saved, divider, float type */
    {	{5,    -5.0,	50.0,   "B0"},	/* param0 */
    	{1.0,  -3.0,	 3.0, 	"Btemp"},/* param1 */
    	{15.0, 10.0,	30.0, 	"River temp"},	/* param2 */
    	{0,	0,	0, 	""},	/* param3 */
    	{0,	0,	0, 	""},	/* param4 */
    	{0,	0,	0, 	""},	/* param5 */
    	{0,	0,	0, 	""},	/* param6 */
    	{0,	0,	0, 	""},	/* param7 */
    	{0,	0,	0, 	""},	/* param8 */
    	{0,	0,	0, 	""},	/* param9 */
		{0,	0,	0,	""},	/* param10 */
		{0,	0,	0,	""},	/* param11 */
		{0,	0,	0,	""},	/* param12 */
		{0,	0,	0,	""},	/* param13 */
		{0,	0,	0,	""}		/* param14 */	},
        "Linear Temperature Migration", "MIGR7"},
    {EQ_MIGR8,	5,	5,  3, 2, FLOAT_SCIENTIFIC,	/* id, #active, #total, #saved, divider, float type */
    {	{5,    -5.0,	50.0,   "B0"},	/* param0 */
    	{0.5,	0,	2.0, 	"Bflow"},/* param1 */
    	{1.0,  -3.0,	 3.0, 	"Btemp"},/* param2 */
    	{10.0,	0,	100.0, 	"River vel"},	/* param3 */
    	{15.0, 10.0,	30.0, 	"River temp"},	/* param4 */
    	{0,	0,	0, 	""},	/* param5 */
    	{0,	0,	0, 	""},	/* param6 */
    	{0,	0,	0, 	""},	/* param7 */
    	{0,	0,	0, 	""},	/* param8 */
    	{0,	0,	0, 	""},	/* param9 */
		{0,	0,	0,	""},	/* param10 */
		{0,	0,	0,	""},	/* param11 */
		{0,	0,	0,	""},	/* param12 */
		{0,	0,	0,	""},	/* param13 */
		{0,	0,	0,	""}		/* param14 */	},
        "Linear Flow & Temp Migration", "MIGR8"},
    {EQ_MIGR9,	6,	6,  5, 3, FLOAT_SCIENTIFIC,	/* id, #active, #total, #saved, divider, float type */
    {	{1.2,    -5.0,	50.0,   "B0"},	/* param0 */
    	{0.5,	0,	2.0, 	"Bflow"},/* param1 */
    	{0.0,	-3.0,	3.0, 	"Btemp"},	/* param2 */
    	{0.2,	0,	2.0, 	"Alpha"},/* param3 */
    	{110,	50.0,	250.0, 	"Tseas"},/* param4 */
    	{10.0,	0,	100.0, 	"River vel"},	/* param5 */
    	{0,	0,	0, 	""},	/* param6 */
    	{0,	0,	0, 	""},	/* param7 */
    	{0,	0,	0, 	""},	/* param8 */
    	{0,	0,	0, 	""},	/* param9 */
		{0,	0,	0,	""},	/* param10 */
		{0,	0,	0,	""},	/* param11 */
		{0,	0,	0,	""},	/* param12 */
		{0,	0,	0,	""},	/* param13 */
		{0,	0,	0,	""}		/* param14 */	},
        "Seasonal Flow & Temp Migration", "MIGR9"},
    {EQ_MIGR10,	9,	9,  7, 5, FLOAT_SCIENTIFIC,	/* id, #active, #total, #saved, divider, float type */
    {	{1.2,    -10.0,	20.0,   "Bmin"},	/* param0 */
    	{17,	0,	30, 	"Bmax"},	/* param1 */
    	{0.5,	0,	2.0, 	"Bflow"},/* param2 */
    	{0.0,	-3.0,	3.0, 	"Btemp"},	/* param3 */
    	{0.2,	0,	2.0, 	"Alpha"},/* param4 */
    	{110,	50.0,	250.0, 	"Tseas"},/* param5 */
    	{0.1,	0,	1.0, 	"Alpha2"},/* param6 */
    	{10.0,	0,	20.0, 	"River vel"},	/* param7 */
    	{100.0,	0,	300.0, 	"Release date"},	/* param8 */
    	{0,	0,	0, 	""},	/* param9 */
		{0,	0,	0,	""},	/* param10 */
		{0,	0,	0,	""},	/* param11 */
		{0,	0,	0,	""},	/* param12 */
		{0,	0,	0,	""},	/* param13 */
		{0,	0,	0,	""}		/* param14 */	},
        "Seasonal Flow & Exper & Temp Migration", "MIGR10"},
    {EQ_RETURN_LOG,  3,  3,  3, -1, FLOAT_SCIENTIFIC,	/* id, #active, #total, #saved, divider, float type */
	{   {0,	-10.0,	10.0,   "B0"},    /* param0 */
        {0,	-10.0,	10.0,   "B1"},     /* param1 */
    	{0,	-10.0,	10.0, 	"B2"},	/* param2 */
    	{0,	0,	0, 	""},	/* param3 */
    	{0,	0,	0, 	""},	/* param4 */
    	{0,	0,	0, 	""},	/* param5 */
    	{0,	0,	0, 	""},	/* param6 */
    	{0,	0,	0, 	""},	/* param7 */
    	{0,	0,	0, 	""},	/* param8 */
    	{0,	0,	0, 	""},	/* param9 */
		{0,	0,	0,	""},	/* param10 */
		{0,	0,	0,	""},	/* param11 */
		{0,	0,	0,	""},	/* param12 */
		{0,	0,	0,	""},	/* param13 */
		{0,	0,	0,	""}		/* param14 */	},
        "Return Rate (Logit: B0 + B1*t +B2*t^2)", "RETURN_LOG"},
    {EQ_MIGR11,  7,  7,  7, -1, FLOAT_SCIENTIFIC,	/* id, #active, #total, #saved, divider, float type */
	{   {0,	-3.0,	3.0,	"B0"},	/* param0 */
		{0,	-3.0,	3.0,	"B1"},	/* param1 */
		{0,	-3.0,	3.0,	"B2"},	/* param2 */
		{0,	-3.0,	3.0,	"B3"},	/* param3 */
		{0,	-3.0,	3.0,	"B4"},	/* param4 */
		{0,	-3.0,	3.0,	"B5"},	/* param5 */
		{0,	-3.0,	3.0,	"B6"},	/* param6 */
    	{0,	0,	0, 	""},	/* param7 */
    	{0,	0,	0, 	""},	/* param8 */
    	{0,	0,	0, 	""},	/* param9 */
		{0,	0,	0,	""},	/* param10 */
		{0,	0,	0,	""},	/* param11 */
		{0,	0,	0,	""},	/* param12 */
		{0,	0,	0,	""},	/* param13 */
		{0,	0,	0,	""}		/* param14 */	},
        "EQ_MIGR11", "MIGR11"},
    {EQ_SIMPLEMORT5,  2,  2,  2, -1, FLOAT_SCIENTIFIC,	/* id, #active, #total, #saved, divider, float type */
	{   {0.00000887,    0, 0.001,   "A"},    /* param0 */
        {3.017,  0.0,    10.0,   "B"},     /* param1 */
    	{0,	0,	0, 	""},	/* param2 */
    	{0,	0,	0, 	""},	/* param3 */
    	{0,	0,	0, 	""},	/* param4 */
    	{0,	0,	0, 	""},	/* param5 */
    	{0,	0,	0, 	""},	/* param6 */
    	{0,	0,	0, 	""},	/* param7 */
    	{0,	0,	0, 	""},	/* param8 */
    	{0,	0,	0, 	""},	/* param9 */
		{0,	0,	0,	""},	/* param10 */
		{0,	0,	0,	""},	/* param11 */
		{0,	0,	0,	""},	/* param12 */
		{0,	0,	0,	""},	/* param13 */
		{0,	0,	0,	""}		/* param14 */	},
        "Simple TURB5 Mortality", "SIMPLEMORT5"},
    {EQ_FLUSHTRANS5,  2,  2,  2, -1, FLOAT_SCIENTIFIC,	/* id, #active, #total, #saved, divider, float type */
	{   {0.3292,    0.0, 1.0,   "a"},     /* param0 */
        {0.1868, 0.0,    1.0,   "b"},     /* param1 */
    	{0,	0,	0,  ""}, /* param2 */
    	{0,	0,	0, 	""},	/* param3 */
    	{0,	0,	0, 	""},	/* param4 */
    	{0,	0,	0, 	""},	/* param5 */
    	{0,	0,	0, 	""},	/* param6 */
    	{0,	0,	0, 	""},	/* param7 */
    	{0,	0,	0, 	""},	/* param8 */
    	{0,	0,	0, 	""},	/* param9 */
		{0,	0,	0,	""},	/* param10 */
		{0,	0,	0,	""},	/* param11 */
		{0,	0,	0,	""},	/* param12 */
		{0,	0,	0,	""},	/* param13 */
		{0,	0,	0,	""}		/* param14 */	},
        "FLUSH TURB5 Transport Merit", "FLUSHTRANS5"},
    {EQ_RSW,  2,  2,  2, -1, FLOAT_SCIENTIFIC,	/* id, #active, #total, #saved, divider, float type */
	{   {0.0,	0.0,	10.0,	"a"},     /* param0 */
        {0.0,	0.0,    5.0,	"b"},     /* param1 */
    	{0,	0,	0, 	""},	/* param2 */
    	{0,	0,	0, 	""},	/* param3 */
    	{0,	0,	0, 	""},	/* param4 */
    	{0,	0,	0, 	""},	/* param5 */
    	{0,	0,	0, 	""},	/* param6 */
    	{0,	0,	0, 	""},	/* param7 */
    	{0,	0,	0, 	""},	/* param8 */
    	{0,	0,	0, 	""},	/* param9 */
		{0,	0,	0,	""},	/* param10 */
		{0,	0,	0,	""},	/* param11 */
		{0,	0,	0,	""},	/* param12 */
		{0,	0,	0,	""},	/* param13 */
		{0,	0,	0,	""}		/* param14 */	},
        "RSW Efficiency", "RSW"},
    {EQ_SURVIVAL_LINEAR,	4,	4,	4, -1, FLOAT_SCIENTIFIC,	/* id, #active, #total, #saved, divider, float type */
	{   {0.0,	-10.0,	10.0,	"a"},	/* param0 */
        {0.0,	-10.0,	10.0,	"b"},	/* param1 */
        {0.0,	-10.0,	10.0,	"c"},	/* param2 */
        {0.0,	-10.0,	10.0,	"d"},	/* param3 */
    	{0,	0,	0, 	""},	/* param4 */
    	{0,	0,	0, 	""},	/* param5 */
    	{0,	0,	0, 	""},	/* param6 */
    	{0,	0,	0, 	""},	/* param7 */
    	{0,	0,	0, 	""},	/* param8 */
    	{0,	0,	0, 	""},	/* param9 */
		{0,	0,	0,	""},	/* param10 */
		{0,	0,	0,	""},	/* param11 */
		{0,	0,	0,	""},	/* param12 */
		{0,	0,	0,	""},	/* param13 */
		{0,	0,	0,	""}		/* param14 */	},
        "Linear Survival (a + b*theta_hat + c*X + d*T)", "SURVIVAL_LINEAR"},
    {EQ_SURVIVAL_Z15,	15,	15,	15,	-1, FLOAT_PRECISE,	/* id, #active, #total, #saved, divider, float type */
	{   {0.0,	-10.0,	10.0,	"a0"},	/* param0 */
        {0.0,	-10.0,	10.0,	"a1"},	/* param1 */
        {0.0,	-10.0,	10.0,	"a2"},	/* param2 */
        {0.0,	-10.0,	10.0,	"a3"},	/* param3 */
    	{0.0,	-10.0,	10.0, 	"a4"},	/* param4 */
    	{0.0,	-10.0,	10.0, 	"a5"},	/* param5 */
    	{0.0,	-10.0,	10.0, 	"a6"},	/* param6 */
    	{0.0,	-10.0,	10.0, 	"B0"},	/* param7 */
    	{0.0,	-10.0,	10.0, 	"B1"},	/* param8 */
    	{0.0,	-10.0,	10.0, 	"B2"},	/* param9 */
		{0.0,	-10.0,	10.0,	"B3"},	/* param10 */
		{0.0,	-10.0,	10.0,	"B4"},	/* param11 */
		{0.0,	-10.0,	10.0,	"B5"},	/* param12 */
		{0.0,	-10.0,	10.0,	"B6"},	/* param13 */
		{0.0,	-10.0,	10.0,	"r"}		/* param14 */	},
        "Z15 Survival: exp( -( (a0 + a1*F + a2*F^2 + a3*T + a4*T^2 + a5*S + a6*S^2) * x + (B0 + B1*F + B2*F^2 + B3*T + B4*T^2 + B5*S + B6*S^2) * t + r * x * t ) )", "SURVIVAL_Z15"},
    {EQ_LATENT_MORT_WTT,	1,	1,	1, -1, FLOAT_SCIENTIFIC,	/* id, #active, #total, #saved, divider, float type */
	{   {0.0,	-100.0,	100.0,	"b"},	/* param0 */
    	{0,	0,	0, 	""},	/* param1 */
    	{0,	0,	0, 	""},	/* param2 */
    	{0,	0,	0, 	""},	/* param3 */
    	{0,	0,	0, 	""},	/* param4 */
    	{0,	0,	0, 	""},	/* param5 */
    	{0,	0,	0, 	""},	/* param6 */
    	{0,	0,	0, 	""},	/* param7 */
    	{0,	0,	0, 	""},	/* param8 */
    	{0,	0,	0, 	""},	/* param9 */
		{0,	0,	0,	""},	/* param10 */
		{0,	0,	0,	""},	/* param11 */
		{0,	0,	0,	""},	/* param12 */
		{0,	0,	0,	""},	/* param13 */
		{0,	0,	0,	""}		/* param14 */	},
        "Latent mort (S3 vs. WTT): 1.0 - exp( -b * (WTT - 2))", "SURVIVAL_LINEAR"},
	{EQ_MIGR12,  5,  5,  5, -1, FLOAT_SCIENTIFIC,	/* id, #active, #total, #saved, divider, float type */
	{   {0,	-3.0,	3.0,	"B0"},	/* param0 */
		{0,	-3.0,	3.0,	"Bfspill"},	/* param1 */
		{0,	-3.0,	3.0,	"Bflow"},	/* param2 */
		{0,	-3.0,	3.0,	"alpha"},	/* param3 */
		{0,	-3.0,	3.0,	"Tinf"},	/* param4 */
		{0,	0,	0,	""},	/* param5 */
		{0,	0,	0,	""},	/* param6 */
    	{0,	0,	0, 	""},	/* param7 */
    	{0,	0,	0, 	""},	/* param8 */
    	{0,	0,	0, 	""},	/* param9 */
		{0,	0,	0,	""},	/* param10 */
		{0,	0,	0,	""},	/* param11 */
		{0,	0,	0,	""},	/* param12 */
		{0,	0,	0,	""},	/* param13 */
		{0,	0,	0,	""}		/* param14 */	},
        "New Migration equation with spill included: B0 + Bfspill*pfishspill + Bflow *velocity*( 1 / (1 + exp(-alpha*(t - Tinf) ) ) )", "MIGR12"},
    {EQ_MIGR13,	9,	9,  7, 5, FLOAT_SCIENTIFIC,	/* id, #active, #total, #saved, divider, float type */
    {	{1.2,    -10.0,	20.0,   "Bmin"},	/* param0 */
    	{17,	0,	30, 	"Bmax"},	/* param2 */
    	{0.5,	0,	2.0, 	"Bflow"},/* param1 */
    	{0.2,	0,	2.0, 	"Alpha"},/* param3 */
    	{110,	50.0,	250.0, 	"Tseas"},/* param4 */
    	{0.1,	0,	1.0, 	"Alpha2"},/* param5 */
		{0,	0,	100.0, 	"Bfspill"},	/* param6 */
    	{10.0,	0,	20.0, 	"River vel"},	/* param7 */
    	{100.0,	0,	300.0, 	"Release date"},	/* param8 */
    	{0,	0,	0, 	""},	/* param9 */
		{0,	0,	0,	""},	/* param10 */
		{0,	0,	0,	""},	/* param11 */
		{0,	0,	0,	""},	/* param12 */
		{0,	0,	0,	""},	/* param13 */
		{0,	0,	0,	""}		/* param14 */	},
        "Seasonal Flow & Exper Migration w/ Spill parameter", "MIGR13"}

};
