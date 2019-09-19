#include "crisp.h"			/* includes eqns.h */

/* routine to find an equation from the default table, given the id.
 * returns NULL if not found.
 */

equation *find_default_eq(id)
    int id;
{
    int i;

    i = 0;
    while (i < NUM_EQNS && id != eqn_dflt_tab[i].id)
	i++;
    if (i == NUM_EQNS)
	return(NULL);
    else
	return(&eqn_dflt_tab[i]);
}


