#include "crisp.h"
#include "eqns.h"
#include "qIdcGasMortEquation.h"
#include "qIdcFDensEquation.h"

/* Global, static object; single instantiation for all dams & species.
 *  Spill efficiency equation.
 */
qIdcFDensEquation qIdcFDensEquation_( GDTT_BEHAVIOREQN_FDens );

qIdcFDensEquation::qIdcFDensEquation(int iDialogID)
							 : qIdcEquationBase(iDialogID)
{
	/* say which types of equations are allowed for this data.
	 */
	addEqType( EQ_FDENS );
}

void qIdcFDensEquation::ApplyData(qDatContainer *dat){

	equation *eq = NULL;
	for(int i=0;i<num_specs;i++){
		if(!strcmp(specs[i].name, dat->getString(1))){
			eq = &(specs[i].fdens_eqn);
			SetEquationData(eq, dat);
			qIdcEquationBase::ApplyData( dat );
			break;
		}
	}

	/* The population density vs. depth relationship is used to calculate
	 *  gas mortality. So notify the gas mort IDC that something has changed:
	 *  any dialog which depends on it will therefore recalculate whatever
	 *  it displays.
	 */
	qIdcGasMortEquation_.invalidate();
}


int qIdcFDensEquation::FillData(qDatContainer *dat){

	equation *eq = NULL;
	for(int i=0;i<num_specs;i++){
		if(!strcmp(specs[i].name, dat->getString(1))){
			eq = &(specs[i].fdens_eqn);
			return GetEquationData(eq, dat);
		}
	}
	return 0;
}



