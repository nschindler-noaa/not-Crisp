#ifndef _qEquationFDens_
#define _qEquationFDens_

#include <math.h>

#include "qEquationBase.h"

class qEquationFDens : public qEquationBase {

	public:

		qEquationFDens() : qEquationBase(EQ_FDENS) {}

		//Y = a - b*X/100
		virtual int process(qDatContainer *dat, qGraphDataToken *token){
			double y;
			float x;
			int i, s;
			equation eq;

			token->dataArray_.clear();

			for(s=0;s<num_specs;s++)
				if(!strcmp(specs[s].name, dat->getString(1)))
					break;
			if (s >= num_specs)
				return 0;   /* Should never happen. */
	
			SetEquationData(&eq, dat);


			for(i =0;i<token->getNumElementsPerLine();i++){
				x = (float)i;
//				y = compute_fdens_eqn( x, &eq);
				y = 0.0;
				token->insert(y,0);
			}

			token->setNumLines(1);

			return 1; 
		}


};

#endif

