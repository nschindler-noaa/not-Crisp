#ifndef _qEquationNSat2_
#define _qEquationNSat2_

#include <math.h>

#include "qEquationBase.h"

class qEquationNSat2 : public qEquationBase {

	public:

		qEquationNSat2() : qEquationBase(EQ_NSAT2) {}

		virtual int process(qDatContainer *dat, qGraphDataToken *token){
			
			equation eq;
			double y;
			float x;

			token->dataArray_.clear();

			river_segment *riverSeg=NULL;
			riverSeg = getRiverSegmentHandle(dat->getString(0), DAM_SEG);
			if(riverSeg){
				SetEquationData(&eq, dat);

				for(int i =0;i<token->getNumElementsPerLine();i++){
					x = (float)i;
					y = compute_generic_nsat( x, &eq, riverSeg );
					token->insert(y,0);
				}
				token->setNumLines(1);
				return 1; 
			}
			else
				return 0;
		}

};
	
#endif

