#include "crisp.h"
#include "eqns.h"
#include "qIdcGasDistEquation.h"

/* Global, static object; single instantiation for all dams & species.
 *  Spill efficiency equation.
 */
qIdcGasDistEquation qIdcGasDistEquation_( GDTT_REACH_GasDist );

qIdcGasDistEquation::qIdcGasDistEquation(int iDialogID)
							 : qIdcEquationBase(iDialogID)
{
	/* say which types of equations are allowed for this data.
	 */
	addEqType( EQ_GAS_DISP );
}

void qIdcGasDistEquation::ApplyData(qDatContainer *dat){
	river_segment *riverSeg=NULL;
	riverSeg = getRiverSegmentHandle(dat->getString(0), REACH_SEG);
	equation *eq = NULL;
	if(riverSeg){
		eq = &(riverSeg->current->reach.gas_dist_eqn);
		SetEquationData(eq, dat);
		qIdcEquationBase::ApplyData( dat );
		return;
	}
}


int qIdcGasDistEquation::FillData(qDatContainer *dat){
  	river_segment *riverSeg=NULL;
	riverSeg = getRiverSegmentHandle(dat->getString(0), REACH_SEG);
	equation *eq = NULL;
	if(riverSeg){
		eq = &(riverSeg->current->reach.gas_dist_eqn);
		return GetEquationData(eq, dat);
	}
	return 0;
}



