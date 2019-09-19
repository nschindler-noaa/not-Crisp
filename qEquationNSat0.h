#ifndef _qEquationNSat0_
#define _qEquationNSat0_

#include <math.h>

#include "qEquationBase.h"

class qEquationNSat0 : public qEquationBase {

	public:

		qEquationNSat0() : qEquationBase(EQ_NSAT0) {}



		//y = b*x + a*(1.0-exp(-k*x));

		virtual int process(qDatContainer *dat, qGraphDataToken *token){
			
			double y,a,b,k,x;

			token->dataArray_.clear();
			k = dat->getDouble(6);
			b = dat->getDouble(3);
			a = dat->getDouble(0);

			for(int i =0;i<token->getNumElementsPerLine();i++){
				x = (double)i;
				y = b*x + a*(1.0-exp(-k*x));
				token->insert(y,0);
			}
			token->setNumLines(1);
			return 1; 
		}


};
	
#endif

