/** \file
 * \brief Removable spill weir (RSW) support functions */

#ifndef _RSW_H
#define _RSW_H

extern double rsw_prob(struct dam *dm, int spec, int day, int time);

/* pred temp response equation parameter access */
#define RSW_A_VAL param[0].val
#define RSW_B_VAL param[1].val

#endif

