#ifndef _qEquationDelay1_
#define _qEquationDelay1_

#include <math.h>

#include "crisp.h"
#include "delay.h"
#include "qEquationBase.h"

class qEquationDelay1 : public qEquationBase {

	public:

		qEquationDelay1() : qEquationBase(EQ_DELAY1) {}

		// dam delay equation.
		virtual int process(qDatContainer *dat, qGraphDataToken *token){
			double flow, depth, scale, y;
			int i, slice, day;
			struct species *spec = NULL;

			token->dataArray_.clear();

			flow = dat->getDouble(0);
			depth = dat->getDouble(3);
			day = (int)(dat->getDouble(6) + 0.001);
			scale = dat->getDouble(9);
			token->setNumLines(1);

			for(i=0;i<num_specs;i++){
				if(!strcmp(spec_names[i], dat->getString(1)))
					spec = &(specs[i]);
			}
			if (spec == NULL)
				return 0;

			for(i =0;i<token->getNumElementsPerLine();i++){
				slice = (i / (24 / DAM_TIME_SLC)) + day * DAM_TIME_SLC;
				y = (0.0);  /* This equation not used at present. */

				token->insert(y,0);
			}

			return 1; 
		}


};

#endif

