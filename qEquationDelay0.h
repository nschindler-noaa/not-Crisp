#ifndef _qEquationDelay0_
#define _qEquationDelay0_

#include <math.h>

#include "qEquationBase.h"

class  qEquationDelay0 : public qEquationBase {

	public:

		qEquationDelay0() : qEquationBase(EQ_DELAY0) {}

		//y = b*Delay0(a*x);
		virtual int process(qDatContainer *dat, qGraphDataToken *token){
			double y,a,b,var;
			int i;
			token->dataArray_.clear();

			a = dat->getDouble(0);
			b = dat->getDouble(3);
			if(dat->getDouble(6) > 0.0 && dat->getDouble(8) > 0.0)
				var = dat->getDouble(6)/dat->getDouble(8);
			else 
				var = 0.0;

			for(i =0;i<token->getNumElementsPerLine();i++){
				y = exp(-a*pow((double)i, b));

				token->insert(y,0);
			}

			if(var > 0.0){
				for(i =0;i<token->getNumElementsPerLine();i++){
					y = exp(-(a+a*var)*pow((double)i, (b+b*var)));
					token->insert(y,0);
				}
				for(i =0;i<token->getNumElementsPerLine();i++){
					y = exp(-(a-a*var)*pow((double)i, (b-b*var)));
					token->insert(y,0);
				}
				token->setNumLines(3);
			}
			else
				token->setNumLines(1);

			return 1; 
		}


};

#endif

