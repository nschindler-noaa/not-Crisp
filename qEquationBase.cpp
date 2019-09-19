/* qEquationBase.cpp
 *  Base class for equation displayers.
 */

#include "qEquationBase.h"

/* This puts equation data from the cache back into the underlying data
 *  structures. We assume that we just "got" this data, so we don't
 *  anticipate mismatches.
 * NOTE this code is duplicated in idcEquationBase.
 * TODO remove the duplication.
 */
void qEquationBase::SetEquationData(equation *eq, qDatContainer *dat){

	if(!eq || !dat)
		return;
	eq->id = dat->getInt(0);
	eq->total_params = dat->getInt(1);
	eq->num_params = dat->getInt(2);
	eq->formula = find_default_eq(eq->id)->formula;
	for(int j=0;j<eq->total_params;j++){
		eq->param[j].val = dat->getDouble(j*3);
		eq->param[j].min = dat->getDouble(j*3+1);
		eq->param[j].max = dat->getDouble(j*3+2);
	}
	return;
}
