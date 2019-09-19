#ifndef _qEquationDelay3_
#define _qEquationDelay3_

#include <math.h>

#include "crisp.h"
#include "delay.h"
#include "qEquationBase.h"

class qEquationDelay3 : public qEquationBase {

	public:

		qEquationDelay3() : qEquationBase(EQ_DELAY3) {}

		// dam delay equation.
		virtual int process(qDatContainer *dat, qGraphDataToken *token){
            struct river_segment *seg;
			struct species *spec = NULL;
            double alpha_day, alpha_night, beta1, beta2, beta3,
                        velocity, spill, y;
			int i, slice, day;

			token->dataArray_.clear();
			token->setNumLines(1);

			/* Figure out selected species. */
            for(i=0;i<num_specs;i++){
				if(!strcmp(spec_names[i], dat->getString(1)))
					spec = &(specs[i]);
			}
			if (spec == NULL)
				return 0;

            /* Figure out selected river segment. */
        	seg = getRiverSegmentHandle(dat->getString(0), DAM_SEG);
            if (seg == NULL)
                return 0;

			alpha_day = dat->getDouble(0);
			alpha_night = dat->getDouble(3);
			beta1 = dat->getDouble(6);
			beta2 = dat->getDouble(9);
			beta3 = dat->getDouble(12);
			velocity = dat->getDouble(15);  /* miles per day */
			spill = dat->getDouble(18);     /* range 0-1 */
            day = (int)(dat->getDouble(21)+0.001);
            if (day > 365)
                day = 365;

			for(i =0;i<token->getNumElementsPerLine();i++){
				slice = (i / (24 / DAM_TIME_SLC)) + day * DAM_TIME_SLC;
				y = delay_prob(alpha_day, alpha_night, beta1, beta2, beta3,
			                seg->current->dam.daylight_proportion[slice],
			                velocity, spill, day);
				token->insert(y,0);
			}

			return 1; 
		}


};

#endif

