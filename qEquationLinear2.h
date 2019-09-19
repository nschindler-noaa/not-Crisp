#ifndef _qEquationLinear2_
#define _qEquationLinear2_

#include "qEquationBase.h"

class qEquationLinear2 : public qEquationBase {

	public:

		qEquationLinear2() : qEquationBase(EQ_LIN2) {}
	
		virtual int process(qDatContainer *dat, qGraphDataToken *token){
			double y,a,b,var;
			int i;
			token->dataArray_.clear();

			a = dat->getDouble(0);
			b = dat->getDouble(3);
			var = dat->getDouble(6);

			for(i =0;i<token->getNumElementsPerLine();i++){
				//y = a + b*x;
				y = a+b*(double)i;
				token->insert(y,0);
			}

			if(var > 0.0){
				for(i =0;i<token->getNumElementsPerLine();i++){
					y = a+b*(double)i+2*sqrt(var)*(double)i;
					token->insert(y,0);
				}
				for(i =0;i<token->getNumElementsPerLine();i++){
					y = a+b*(double)i-2*sqrt(var)*(double)i;
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

