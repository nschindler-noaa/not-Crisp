#ifndef _qEquationPredTemp_
#define _qEquationPredTemp_

#include <math.h>

#include "qEquationBase.h"

extern "C" {
#include "pred.h"
}

class qEquationPredTemp1 : public qEquationBase {

	public:

		qEquationPredTemp1() : qEquationBase(EQ_PRED_TEMP1) {}

		//Y = a * exp(b*X) we think
		virtual int process(qDatContainer *dat, qGraphDataToken *token){
			double y;
			float x;
			int i;
			equation eq;

			token->dataArray_.clear();

			SetEquationData(&eq, dat);

			for(i =0;i<token->getNumElementsPerLine();i++){
				x = (float)i * 0.5;
				y = pred_temp_response( &eq, x );
				token->insert(y,0);
			}

			token->setNumLines(1);

			token->setXMin(0.0);
			token->setXMax(30.0);
			return 1; 
		}


};

class qEquationPredTemp2 : public qEquationBase {

	public:

		qEquationPredTemp2() : qEquationBase(EQ_PRED_TEMP2) {}

		//Y = a * exp(b*X) we think
		virtual int process(qDatContainer *dat, qGraphDataToken *token){
			double y;
			float x;
			int i;
			equation eq;

			token->dataArray_.clear();

			SetEquationData(&eq, dat);

			for(i =0;i<token->getNumElementsPerLine();i++){
				x = (float)i * 0.5;
				y = pred_temp_response( &eq, x );
				token->insert(y,0);
			}

			token->setNumLines(1);

			token->setXMin(0.0);
			token->setXMax(30.0);
			return 1; 
		}


};

#endif

