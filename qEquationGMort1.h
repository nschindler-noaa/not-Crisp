#ifndef _qEquationGMort1_
#define _qEquationGMort1_

#include <math.h>

#include "qEquationBase.h"

class qEquationGMort1 : public qEquationBase {

	public:

		qEquationGMort1() : qEquationBase(EQ_GMORT1) {}

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
				y = compute_gmort_eqn( x, &eq, &(specs[s]) );
				token->insert(y,0);
			}

			token->setNumLines(1);

			return 1; 
		}


};

#endif

