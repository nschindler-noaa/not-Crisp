#ifndef _qEquationFGE_
#define _qEquationFGE_

#include <math.h>

#include "qEquationBase.h"

class qEquationFGE : public qEquationBase {

	public:

		qEquationFGE() : qEquationBase(EQ_FGE) {}

		//Y = a - b*X/100
		virtual int process(qDatContainer *dat, qGraphDataToken *token){
			double y;
			float x;
			int i, dm, spc;
			equation eq;

			token->dataArray_.clear();

			SetEquationData(&eq, dat);

			for(dm=0;dm<num_powerhouses;dm++){
				if(!strcmp(phouses[dm]->seg_name, dat->getString(0)))
					break;
			}
			for (spc = 0; spc < num_specs; spc++) {
				if(!strcmp(specs[spc].name, dat->getString(1)))
					break;
			}
			if (dm >= num_powerhouses || spc >= num_specs)
				return 0;

			for(i =0;i<token->getNumElementsPerLine();i++){
				x = (float)i;
				y = compute_fge_eqn_val( x, &eq, dm, spc, 0 );
				token->insert(y,0);
			}
			for(i =0;i<token->getNumElementsPerLine();i++){
				x = (float)i;
				y = compute_fge_eqn_val( x, &eq, dm, spc, 1 );
				token->insert(y,0);
			}
			for(i =0;i<token->getNumElementsPerLine();i++){
				x = (float)i;
				y = compute_fge_eqn_val( x, &eq, dm, spc, 2 );
				token->insert(y,0);
			}

			token->setNumLines(3);

			return 1; 
		}


};

#endif

