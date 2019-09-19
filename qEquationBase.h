#ifndef _qEquationBase_
#define _qEquationBase_

#include <math.h>
#include "qGraphDataToken.h"
#include "qDatContainer.h"
#include "eqns.h"
#include "crisp.h"
#include "qUtils.h"

extern "C"
{ 
extern float compute_generic_nsat( float spill, equation *eqn,  void *params );
extern float compute_gmort_eqn( float x, equation *eqn, void *param );
//extern float compute_fdens_eqn( float x, equation *eqn);
extern float compute_migr_eqn_val( float x, equation *eqn, void *param,
                                   int range, int reach_class );
extern float compute_fge_eqn_val(float x, equation *eqn, int dm, int spc, int range);
}

class qEquationBase {

	int iEquationID;

public:

	qEquationBase(int id) : iEquationID(id) {}

	const int getEquationID(){ return iEquationID; }

	virtual int process(qDatContainer *dat, qGraphDataToken *token)=0;
	
	void SetEquationData( equation *eq, qDatContainer *dat ); 
};


#endif

