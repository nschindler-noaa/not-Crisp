#include "crisp.h"
#include "eqns.h"
#include "qIdcSpillEquation.h"

/* Global, static object; single instantiation for all dams & species.
 *  Spill efficiency equation.
 */
qIdcSpillEquation qIdcSpillEquation_( GDTT_DAMEQUATION_Spill );

qIdcSpillEquation::qIdcSpillEquation(int iDialogID)
							 : qIdcEquationBase(iDialogID)
{
	/* say which types of equations are allowed for this data.
	 */
	addEqType( EQ_LIN1 );
	addEqType( EQ_LIN2 );
	addEqType( EQ_EXP0 );
	addEqType( EQ_EXP2 );
	addEqType( EQ_POW0 );
	addEqType( EQ_POW2 );
	addEqType( EQ_LOG0 );
	addEqType( EQ_CUB0 );
	addEqType( EQ_CUB1 );
}

void qIdcSpillEquation::ApplyData(qDatContainer *dat){

	river_segment *riverSeg=NULL;
	riverSeg = getRiverSegmentHandle(dat->getString(0), DAM_SEG);
	equation *eq = NULL;
	if(riverSeg){
		for(int i=0;i<num_specs;i++){
			if(!strcmp(spec_names[i], dat->getString(1))){
				eq = &(riverSeg->current->dam.species[i].spill_eqn);
				SetEquationData(eq, dat);
				qIdcEquationBase::ApplyData( dat );
				return;
			}
		}
	}

}


int qIdcSpillEquation::FillData(qDatContainer *dat){

  	river_segment *riverSeg=NULL;
	riverSeg = getRiverSegmentHandle(dat->getString(0), DAM_SEG);
	equation *eq = NULL;
	if(riverSeg){
		for(int i=0;i<num_specs;i++){
			if(!strcmp(spec_names[i], dat->getString(1))){
				eq = &(riverSeg->current->dam.species[i].spill_eqn);
				return GetEquationData(eq, dat);
			}
		}
	}
	return 0;
}



