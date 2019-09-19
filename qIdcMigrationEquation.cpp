#include "crisp.h"
#include "eqns.h"
#include "qIdcGasMortEquation.h"
#include "qIdcMigrationEquation.h"

/* Global, static object; single instantiation for all dams & species.
 *  Spill efficiency equation.
 */
qIdcMigrationEquation qIdcMigrationEquation_( GDTT_BEHAVIOREQN_Migration );

qIdcMigrationEquation::qIdcMigrationEquation(int iDialogID)
							 : qIdcEquationBase(iDialogID)
{
	/* say which types of equations are allowed for this data.
	 */
    addEqType( EQ_MIGR );
    addEqType( EQ_MIGR1 );
    addEqType( EQ_MIGR3 );
    addEqType( EQ_MIGR4 );
    addEqType( EQ_MIGR5 );
    addEqType( EQ_MIGR6 );
    addEqType( EQ_MIGR7 );
    addEqType( EQ_MIGR8 );
    addEqType( EQ_MIGR9 );
    addEqType( EQ_MIGR10 );
}

void qIdcMigrationEquation::ApplyData(qDatContainer *dat){
	int i, j;

	for(j = 0; j < num_reach_classes; j++)
		if(!strcmp(reach_class_names[j], dat->getString(0)))
			break;
	if (j >= num_reach_classes)
		return;

	equation *eq = NULL;
	for(i=0;i<num_specs;i++){
		if(!strcmp(specs[i].name, dat->getString(1))){
			eq = &(specs[i].stock.migration_eqn[j]);
			SetEquationData(eq, dat);
			qIdcEquationBase::ApplyData( dat );
			return;
		}
	}
	for(i=0;i<stocks.num;i++){
		if(!strcmp(stocks.s[i]->name, dat->getString(1))){
			eq = &(stocks.s[i]->migration_eqn[j]);
			SetEquationData(eq, dat);
			qIdcEquationBase::ApplyData( dat );
			return;
		}
	}

}


int qIdcMigrationEquation::FillData(qDatContainer *dat){
	int i, j;
	
	for(j = 0; j < num_reach_classes; j++)
		if(!strcmp(reach_class_names[j], dat->getString(0)))
			break;
	if (j >= num_reach_classes)
		return 0;

	equation *eq = NULL;
	for(i=0;i<num_specs;i++){
		if(!strcmp(specs[i].name, dat->getString(1))){
			eq = &(specs[i].stock.migration_eqn[j]);
			return GetEquationData(eq, dat);
		}
	}
	for(i=0;i<stocks.num;i++){
		if(!strcmp(stocks.s[i]->name, dat->getString(1))){
			eq = &(stocks.s[i]->migration_eqn[j]);
			return GetEquationData(eq, dat);
		}
	}
	return 0;
}



