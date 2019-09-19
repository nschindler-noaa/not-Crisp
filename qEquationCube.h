#ifndef _qEquationCube_
#define _qEquationCube_

#include "qEquationBase.h"

class qEquationCube0 : public qEquationBase {

	public:

		qEquationCube0() : qEquationBase(EQ_CUB0) {}
	
		virtual int process(qDatContainer *dat, qGraphDataToken *token){
			double x,y,a,b,c,var;
			int i;
			token->dataArray_.clear();

			a = dat->getDouble(0);
			b = dat->getDouble(3);
            c = dat->getDouble(6);
			var = dat->getDouble(9);

			for(i =0;i<token->getNumElementsPerLine();i++){
				//y = a*x + b*x^2 + c*x^3 + e;
                x = (double)i;
				y = a*x + b*x*x + c*x*x*x;
				token->insert(y,0);
			}
			if(var > 0.0){
				for(i =0;i<token->getNumElementsPerLine();i++){
                    x = (double)i;
				    y = a*x + b*x*x + c*x*x*x + 2*sqrt(var);
					token->insert(y,0);
				}
				for(i =0;i<token->getNumElementsPerLine();i++){
                    x = (double)i;
				    y = a*x + b*x*x + c*x*x*x - 2*sqrt(var);
					token->insert(y,0);
				}
				token->setNumLines(3);
			}
			else
				token->setNumLines(1);

			return 1; 
		}

};

class qEquationCube1 : public qEquationBase {

	public:

		qEquationCube1() : qEquationBase(EQ_CUB1) {}
	
		virtual int process(qDatContainer *dat, qGraphDataToken *token){
			double x,y,a,b,c,var;
			int i;
			token->dataArray_.clear();

			a = dat->getDouble(0);
			b = dat->getDouble(3);
            c = dat->getDouble(6);
			var = dat->getDouble(9);

			for(i =0;i<token->getNumElementsPerLine();i++){
				//y = (a*x/100 + b*(x/100)^2 + c*(x/100)^3)*100 + e;
                x = (double)i / 100.0;
				y = (a*x + b*x*x + c*x*x*x) * 100.0;
				token->insert(y,0);
			}
			if(var > 0.0){
				for(i =0;i<token->getNumElementsPerLine();i++){
                    x = (double)i / 100.0;
				    y = (a*x + b*x*x + c*x*x*x) * 100.0 + 2*sqrt(var);
					token->insert(y,0);
				}
				for(i =0;i<token->getNumElementsPerLine();i++){
                    x = (double)i / 100.0;
				    y = (a*x + b*x*x + c*x*x*x) * 100.0 - 2*sqrt(var);
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

