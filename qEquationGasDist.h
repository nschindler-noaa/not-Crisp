#ifndef _qEquationGasDist_
#define _qEquationGasDist_

#include <math.h>

#include "qEquationBase.h"
#include "nsat.h"

class qEquationGasDist : public qEquationBase {

	public:

		qEquationGasDist() : qEquationBase(EQ_GAS_DISP) {}

		virtual int process(qDatContainer *dat, qGraphDataToken *token){
			
			equation eq;
			float gas_out_left, gas_out_right;
			float x;
            int i, step;
			struct river_segment *riverSeg=NULL;
            int xlen = token->getNumElementsPerLine();
            float rlen;

			token->dataArray_.clear();

			riverSeg = getRiverSegmentHandle(dat->getString(0), REACH_SEG);
            if(riverSeg){
			    rlen = riverSeg->current->reach.length;
                token->setXMax( rlen );
				SetEquationData(&eq, dat);
				//We are  making a round off error cast here
                step = (int)(eq.param[0].val * DAM_TIME_SLC);

				for(i=0;i<xlen;i++){
					x = (float)(i * rlen / (xlen-1));
					compute_gas_eqn_val( riverSeg, step, x, 
                                      &gas_out_left, &gas_out_right );
					token->insert(gas_out_left,0);
				}
				for(i=0;i<xlen;i++){
					x = (float)(i * rlen / (xlen-1));
					compute_gas_eqn_val( riverSeg, step, x, 
                                      &gas_out_left, &gas_out_right );
					token->insert(gas_out_right,0);
				}
				token->setNumLines(2);
				return 1; 
			}
			else
				return 0;
		}

};
	
#endif

