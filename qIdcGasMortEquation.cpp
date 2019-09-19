#include "crisp.h"
#include "eqns.h"
#include "qIdcGasMortEquation.h"

/* Global, static object; single instantiation for all dams & species.
 *  Spill efficiency equation.
 */
qIdcGasMortEquation qIdcGasMortEquation_( GDTT_BEHAVIOREQN_GasMort );

qIdcGasMortEquation::qIdcGasMortEquation(int iDialogID)
							 : qIdcEquationBase(iDialogID)
{
	/* say which types of equations are allowed for this data.
	 */
	addEqType( EQ_GMORT3 );
}

void qIdcGasMortEquation::ApplyData(qDatContainer *dat){

	equation *eq = NULL;
	for(int i=0;i<num_specs;i++){
		if(!strcmp(specs[i].name, dat->getString(1))){
			eq = &(specs[i].gmort_eqn);
			SetEquationData(eq, dat);
			qIdcEquationBase::ApplyData( dat );
			return;
		}
	}
}


int qIdcGasMortEquation::FillData(qDatContainer *dat){

	equation *eq = NULL;
	for(int i=0;i<num_specs;i++){
		if(!strcmp(specs[i].name, dat->getString(1))){
			eq = &(specs[i].gmort_eqn);
			return GetEquationData(eq, dat);
		}
	}
	return 0;
}



