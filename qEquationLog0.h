#ifndef _qEquationLog0_
#define _qEquationLog0_

#include <math.h>

#include "qEquationBase.h"

class qEquationLog0 : public qEquationBase {

	public:

		qEquationLog0() : qEquationBase(EQ_LOG0) {}

		//y = a + b*log(x);
		virtual int process(qDatContainer *dat, qGraphDataToken *token){
			double td,a,b,var;
			int i;
			token->dataArray_.clear();

			a = dat->getDouble(0);
			b = dat->getDouble(3);
			var = dat->getDouble(6);

			token->insert( 0.0, 0 );  /* log(0) is undefined */
			for(i =1;i<token->getNumElementsPerLine();i++){
				td = a+b*log((double)i);
				token->insert(td,0);
			}

			if (var > 0.0) {
				token->insert( 0.0, 0 );  /* log(0) is undefined */
				for(i =1;i<token->getNumElementsPerLine();i++){
					td = a+b*log((double)i)+sqrt(var)*2.0;
					token->insert(td,0);
				}
				token->insert( 0.0, 0 );  /* log(0) is undefined */
				for(i =1;i<token->getNumElementsPerLine();i++){
					td = a+b*log((double)i)-sqrt(var)*2.0;
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

