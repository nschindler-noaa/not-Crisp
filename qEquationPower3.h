#ifndef _qEquationPower3_
#define _qEquationPower3_

#include <math.h>

#include "qEquationBase.h"

class qEquationPower3 : public qEquationBase {

	public:

		qEquationPower3() : qEquationBase(EQ_POW3) {}

		//y = b*pow(x,a);    upper = b*(pow(x, a))+err*x*x;

	    
		virtual int process(qDatContainer *dat, qGraphDataToken *token){
			double y,a,b,err;
			int i;
			token->dataArray_.clear();

			a = dat->getDouble(0);
			b = dat->getDouble(3);
			err = sqrt(dat->getDouble(6))*2;

			for(i =0;i<token->getNumElementsPerLine();i++){
				y = b*pow((double)i,a);
				token->insert(y,0);
			}

			if (err > 0.0) {
				for(i =0;i<token->getNumElementsPerLine();i++){
					y = b*pow((double)i,a)+err*(double)i*(double)i;
					token->insert(y,0);
				}
				for(i =0;i<token->getNumElementsPerLine();i++){
					y = b*pow((double)i,a)-err*(double)i*(double)i;
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

