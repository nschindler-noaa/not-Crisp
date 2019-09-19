#include "crisp.h"
#include "eqns.h"
#include "qIdcTransMortEquation.h"

/* Global, static object; single instantiation for all dams & species.
 */
qIdcTransMortEquation qIdcTransMortEquation_( GDTT_DAMEQUATION_Trans );

qIdcTransMortEquation::qIdcTransMortEquation(int iDialogID)
				: qIdcEquationBase(iDialogID) {
	/* say which types of equations are allowed for this data.
	 */
	addEqType( EQ_TRANS );
} 

void qIdcTransMortEquation::ApplyData(qDatContainer *dat){

	river_segment *riverSeg=NULL;
	riverSeg = getRiverSegmentHandle(dat->getString(0), DAM_SEG);
	equation *eq = NULL;
	if(riverSeg){
		for(int i=0;i<num_specs;i++){
			if(!strcmp(spec_names[i], dat->getString(1))){
				eq = &(riverSeg->current->dam.species[i].trans_eqn);
				SetEquationData(eq, dat);
				qIdcEquationBase::ApplyData( dat );
				return;
			}
		}
	}

}

int qIdcTransMortEquation::FillData(qDatContainer *dat){

  	river_segment *riverSeg=NULL;
	riverSeg = getRiverSegmentHandle(dat->getString(0), DAM_SEG);
	equation *eq = NULL;
	if(riverSeg){
		for(int i=0;i<num_specs;i++){
			if(!strcmp(spec_names[i], dat->getString(1))){
				eq = &(riverSeg->current->dam.species[i].trans_eqn);
				int iret = GetEquationData(eq, dat);
				if (iret) {
					/* Get mortality base, min, and max (constants for each
					 *  dam and species combination).
					 */
					FillBrokenStickData(dat, 12, &riverSeg->current->dam.species[i].transport_mort);
					dat->clearDirty();
				}
				return iret;
			}
		}
	}
	return 0;
}



