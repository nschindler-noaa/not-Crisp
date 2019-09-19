#ifndef _qEquationExp0_
#define _qEquationExp0_

#include <math.h>

#include "qEquationBase.h"

class qEquationExp0 : public qEquationBase {

	public:

		qEquationExp0() : qEquationBase(EQ_EXP0) {}

		//y = b*exp(a*x);
		virtual int process(qDatContainer *dat, qGraphDataToken *token){
			double td,a,b,var;
			int i;
			token->dataArray_.clear();

			a = dat->getDouble(0);
			b = dat->getDouble(3);
			var = dat->getDouble(6);

			for(i =0;i<token->getNumElementsPerLine();i++){
				td = b*exp(a*(double)i);
				token->insert(td,0);
			}

			if (var > 0.0) {
				for(i =0;i<token->getNumElementsPerLine();i++){
					td = b*exp(a*(double)i+2*sqrt(var));
					token->insert(td,0);
				}
				for(i =0;i<token->getNumElementsPerLine();i++){
					td = b*exp(a*(double)i-2*sqrt(var));
					token->insert(td,0);
				}
				token->setNumLines(3);
			}
			else
				token->setNumLines(1);

			return 1; 
		}


};

#endif

