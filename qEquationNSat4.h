#ifndef _qEquationNSat4_
#define _qEquationNSat4_

#include <math.h>

#include "qEquationBase.h"

class qEquationNSat4 : public qEquationBase {

	public:

		qEquationNSat4() : qEquationBase(EQ_NSAT4) {}


	/*
		if (x == 0 && c == 0)
			y = a;
	    else
			y = b*x + a*x/(c + x);
	    break;

	*/
		virtual int process(qDatContainer *dat, qGraphDataToken *token){
			
			double y,a,b,c,x;

			token->dataArray_.clear();
			c = dat->getDouble(6);
			b = dat->getDouble(3);
			a = dat->getDouble(0);

			for(int i =0;i<token->getNumElementsPerLine();i++){
				x = (double)i;
				if (x == 0.0 && c == 0.0)
					y = a;
				else
					y = b*x + a*x/(c + x);
				token->insert(y,0);
			}
			token->setNumLines(1);
			return 1; 
		}


};
	
#endif

