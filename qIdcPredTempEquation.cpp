#include "crisp.h"
#include "eqns.h"
#include "qIdcPredTempEquation.h"

/* Global, static object; single instantiation for all dams & species.
 *  Spill efficiency equation.
 */
qIdcPredTempEquation qIdcPredTempEquation_( GDTT_BEHAVIOREQN_PredTemp );

qIdcPredTempEquation::qIdcPredTempEquation(int iDialogID)
							 : qIdcEquationBase(iDialogID)
{
	/* say which types of equations are allowed for this data.
	 */

	addEqType( EQ_PRED_TEMP1 );
	addEqType( EQ_PRED_TEMP2 );
    addEqType( EQ_LIN0 );
}

void qIdcPredTempEquation::ApplyData(qDatContainer *dat){
	int i, j;

	for(j = 0; j < num_reach_classes; j++)
		if(!strcmp(reach_class_names[j], dat->getString(0)))
			break;
	if (j >= num_reach_classes)
		return;

	equation *eq = NULL;
	for(i=0;i<num_specs;i++){
		if(!strcmp(specs[i].name, dat->getString(1))){
			eq = &(specs[i].stock.pred_temp_response_eqn[j]);
			SetEquationData(eq, dat);
			qIdcEquationBase::ApplyData( dat );
			return;
		}
	}
	for(i=0;i<stocks.num;i++){
		if(!strcmp(stocks.s[i]->name, dat->getString(1))){
			eq = &(stocks.s[i]->pred_temp_response_eqn[j]);
			SetEquationData(eq, dat);
			qIdcEquationBase::ApplyData( dat );
			return;
		}
	}

}


int qIdcPredTempEquation::FillData(qDatContainer *dat){
	int i, j;
	
	for(j = 0; j < num_reach_classes; j++)
		if(!strcmp(reach_class_names[j], dat->getString(0)))
			break;
	if (j >= num_reach_classes)
		return 0;

	equation *eq = NULL;
	for(i=0;i<num_specs;i++){
		if(!strcmp(specs[i].name, dat->getString(1))){
			eq = &(specs[i].stock.pred_temp_response_eqn[j]);
			return GetEquationData(eq, dat);
		}
	}
	for(i=0;i<stocks.num;i++){
		if(!strcmp(stocks.s[i]->name, dat->getString(1))){
			eq = &(stocks.s[i]->pred_temp_response_eqn[j]);
			return GetEquationData(eq, dat);
		}
	}
	return 0;
}



