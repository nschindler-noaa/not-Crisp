#include "crisp.h"
#include "eqns.h"
#include "qIdcFGEEquation.h"
#include "qGlobalEvents.h"

/* Global, static object; single instantiation for all dams & species.
 *  Spill efficiency equation.
 */
qIdcFGEEquation qIdcFGEEquation_( GDTT_DAMEQUATION_FGE );

qIdcFGEEquation::qIdcFGEEquation(int iDialogID)
							 : qIdcEquationBase(iDialogID)
{
	/* say which types of equations are allowed for this data.
	 */
	addEqType( EQ_FGE );
    eventFlag_ = 0;
}

void qIdcFGEEquation::ApplyData(qDatContainer *dat){
	int i,j;
	equation *eq = NULL;
	for(i=0;i<num_powerhouses;i++){
		if(!strcmp(phouses[i]->seg_name, dat->getString(0))){
			for (j = 0; j < num_specs; j++) {
				if(!strcmp(specs[j].name, dat->getString(1))){
					eq = &(phouses[i]->current->dam.species[j].fge_eqn);
					SetEquationData(eq, dat);
					qIdcEquationBase::ApplyData( dat );
					return;
				}
			}
		}
	}
}


int qIdcFGEEquation::FillData(qDatContainer *dat){
	int i,j;
	equation *eq = NULL;

    if (!eventFlag_) {
        // Connect events after all static initialization are complete.
        eventFlag_ = 1;
//	    GEUpdateAllDataTokens_.attach( this, &qIdcFGEEquation::_cbUpdateSelf );
		connect(qEvents::Instance(), SIGNAL(GEUpdateAllDataTokens_()),this,  SLOT(cbUpdateSelf()));
    }

	for(i=0;i<num_powerhouses;i++){
		if(!strcmp(phouses[i]->seg_name, dat->getString(0))){
			for (j = 0; j < num_specs; j++) {
				if(!strcmp(specs[j].name, dat->getString(1))){
					eq = &(phouses[i]->current->dam.species[j].fge_eqn);
					return GetEquationData(eq, dat);
				}
			}
		}
	}
	return 0;
}


/*
void qIdcFGEEquation::_cbUpdateSelf(void *msg){
    qIdcFGEEquation *me = (qIdcFGEEquation *)msg; 
    me->cbUpdateSelf();
}
*/
void qIdcFGEEquation::cbUpdateSelf( ){
    invalidate();
}
