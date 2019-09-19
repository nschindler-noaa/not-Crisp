#ifndef _qEquationFlAct_
#define _qEquationFlAct_

#include <math.h>

#include "qEquationBase.h"

class qEquationFlAct : public qEquationBase {

	public:

		qEquationFlAct() : qEquationBase(EQ_FL_ACT) {}

		//Y = a - b*X/100
		virtual int process(qDatContainer *dat, qGraphDataToken *token){
			double td,a,b,var;
			int i;
			token->dataArray_.clear();

			a = dat->getDouble(0);
			b = dat->getDouble(3);
			if(dat->getDouble(6) > 0.0 && dat->getDouble(8) > 0.0)
				var = dat->getDouble(6)/dat->getDouble(8);
			else 
				var = 0.0;

			for(i =0;i<token->getNumElementsPerLine();i++){
				td = a-b*(double)i/100.0;
				token->insert(td,0);
			}

			if(var > 0.0){
				for(i =0;i<token->getNumElementsPerLine();i++){
					td = (a+a*var)-(b+b*var)*(double)i/100.0;
					token->insert(td,0);
				}
				for(i =0;i<token->getNumElementsPerLine();i++){
					td = (a-a*var)-(b-b*var)*(double)i/100.0;
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

