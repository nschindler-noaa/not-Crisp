#ifndef _qEquationSpill_
#define _qEquationSpill_

#include "qEquationBase.h"

class qEquationSpill : public qEquationBase {

	public:

		qEquationSpill(int id) : qEquationBase(id) {}
	
		virtual int process(qDatContainer *dat, qGraphDataToken *token){

			return 0;
		}
	
};

#endif

