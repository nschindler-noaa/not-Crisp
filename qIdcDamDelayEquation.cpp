#include "crisp.h"
#include "eqns.h"
#include "qIdcDamDelayEquation.h"

/* Global, static object; single instantiation for all dams & species.
 */
qIdcDamDelayEquation qIdcDamDelayEquation_( GDTT_DAMEQUATION_Delay );

qIdcDamDelayEquation::qIdcDamDelayEquation(int iDialogID)
				: qIdcEquationBase(iDialogID) {
	/* say which types of equations are allowed for this data.
	 */
	addEqType( EQ_DELAY3 );
} 

void qIdcDamDelayEquation::ApplyData(qDatContainer *dat){

    river_segment *riverSeg=NULL;
	riverSeg = getRiverSegmentHandle(dat->getString(0), DAM_SEG);
	equation *eq = NULL;
	if(riverSeg){
		for(int i=0;i<num_specs;i++){
			if(!strcmp(spec_names[i], dat->getString(1))){
				eq = &(riverSeg->current->dam.species[i].delay_eqn);
				SetEquationData(eq, dat);
				qIdcEquationBase::ApplyData( dat );
				return;
			}
		}
	}
}

int qIdcDamDelayEquation::FillData(qDatContainer *dat){

  	river_segment *riverSeg=NULL;
	riverSeg = getRiverSegmentHandle(dat->getString(0), DAM_SEG);
	equation *eq = NULL;
	if(riverSeg){
		for(int i=0;i<num_specs;i++){
			if(!strcmp(spec_names[i], dat->getString(1))){
				eq = &(riverSeg->current->dam.species[i].delay_eqn);
				return GetEquationData(eq, dat);
			}
		}
	}
	return 0;
}
