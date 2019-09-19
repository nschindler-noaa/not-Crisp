#ifndef _qEquationMigration_
#define _qEquationMigration_

#include <math.h>

#include "qEquationBase.h"

class qEquationMigration : public qEquationBase {

	public:

		qEquationMigration() : qEquationBase(EQ_MIGR) {}

		//Y = a - b*X/100
		virtual int process(qDatContainer *dat, qGraphDataToken *token){
			double y;
			float x;
			int i, sp = -1, st = -1;
			equation eq;

			token->dataArray_.clear();

			for(sp=0;sp<num_specs;sp++)
				if(!strcmp(specs[sp].name, dat->getString(1)))
					break;
			if (sp >= num_specs) {
				sp = -1;
				for(st=0;st<stocks.num;st++)
					if(!strcmp(stocks.s[st]->name, dat->getString(1)))
						break;
				if (st >= stocks.num)
					return 0; /* should never happen. */
			}
	
			SetEquationData(&eq, dat);


			for(i =0;i<token->getNumElementsPerLine();i++){
				x = (float)i + 80;
                // TODO: last argument is reach_class - get a valid value for it.
				y = compute_migr_eqn_val( x, &eq,
					sp >= 0 ? &(specs[sp].stock) : stocks.s[st], 0, 0 );
				token->insert(y,0);
			}
			for(i =0;i<token->getNumElementsPerLine();i++){
				x = (float)i + 80;
                // TODO: last argument is reach_class - get a valid value for it.
				y = compute_migr_eqn_val( x, &eq,
					sp >= 0 ? &(specs[sp].stock) : stocks.s[st], 1, 0 );
				token->insert(y,0);
			}
			for(i =0;i<token->getNumElementsPerLine();i++){
				x = (float)i + 80;
                // TODO: last argument is reach_class - get a valid value for it.
				y = compute_migr_eqn_val( x, &eq,
					sp >= 0 ? &(specs[sp].stock) : stocks.s[st], 2, 0 );
				token->insert(y,0);
			}

			token->setNumLines(3);

			token->setXMin(80.0);
			token->setXMax(250.0);
			return 1; 
		}


};

#endif

