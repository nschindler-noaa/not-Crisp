#ifndef _qEquationTrans_
#define _qEquationTrans_

/* Compute the display for the Transport Mortality Equation. 
 */

#include <math.h>

#include "qEquationBase.h"
#include "probability.h"

class qEquationTrans : public qEquationBase {
	protected:

		void process_line( qDatContainer *dat, qGraphDataToken *token,
														double base ) {
			double tt1, m1, tt2, m2, tv;
			int i, nel;

			tt1 = dat->getDouble(0);
			m1 = dat->getDouble(3) + base;
			tt2 = dat->getDouble(6);
			m2 = dat->getDouble(9) + base;
			BOUND_PROB(m1);
			BOUND_PROB(m2);
			nel = token->getNumElementsPerLine();

			for(i =0;i<nel;i++){
				if (tt1 >= tt2)
					tv = 0.0;
				else if (i <= tt1)
					tv = m1;
				else if (i >= tt2)
					tv = m2;
				else
					tv = (m1 + (i - tt1) * ((m2 - m1)/(tt2 - tt1)));
				token->insert( tv * 100.0, 0 );
			}
		}

	public:

		qEquationTrans() : qEquationBase(EQ_TRANS) {}

		virtual int process(qDatContainer *dat, qGraphDataToken *token){
			double base_mort, mort_low, mort_high;

			token->dataArray_.clear();
			base_mort = dat->getDouble(12);
			mort_low = dat->getDouble(13);
			mort_high = dat->getDouble(14);

			process_line( dat, token, base_mort );

			if (mort_high > mort_low) {
				process_line( dat, token, mort_high );
				process_line( dat, token, mort_low );
				token->setNumLines(3);
			}
			else
				token->setNumLines(1);

			return 1; 
		}


};

#endif

