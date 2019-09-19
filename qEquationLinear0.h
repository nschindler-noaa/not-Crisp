#ifndef _qEquationLinear0_
#define _qEquationLinear0_

#include "qEquationBase.h"

class qEquationLinear0 : public qEquationBase {

	public:

		qEquationLinear0() : qEquationBase(EQ_LIN0) {}
	
		virtual int process(qDatContainer *dat, qGraphDataToken *token){
			double y,a,b;
			int i;
			token->dataArray_.clear();

			a = dat->getDouble(0);
			b = dat->getDouble(3);
			for(i =0;i<token->getNumElementsPerLine();i++){
				//y = a + b*x;
				y = a+b*(double)i;
				token->insert(y,0);
			}
			token->setNumLines(1);
			return 1; 
		}

};

#endif

