#include "qEquationFactory.h"
#include "qEquationSpill.h"
#include "qEquationExp0.h"
#include "qEquationExp1.h"
#include "qEquationExp2.h"
#include "qEquationExp3.h"
#include "qEquationGasDist.h"
#include "qEquationGMort0.h"
#include "qEquationGMort1.h"
#include "qEquationGMort2.h"
#include "qEquationGMort3.h"
#include "qEquationLinear0.h"
#include "qEquationLinear1.h"
#include "qEquationLinear2.h"
#include "qEquationLinear3.h"
#include "qEquationMigration.h"
#include "qEquationNSat0.h"
#include "qEquationNSat1.h"
#include "qEquationNSat2.h"
#include "qEquationNSat4.h"
#include "qEquationNSat5.h"
#include "qEquationNSat6.h"
#include "qEquationPower0.h"
#include "qEquationPower1.h"
#include "qEquationPower2.h"
#include "qEquationPower3.h"
#include "qEquationLog0.h"
#include "qEquationDelay0.h"
#include "qEquationDelay1.h"
#include "qEquationDelay3.h"
#include "qEquationFlAct.h"
#include "qEquationFDens.h"
#include "qEquationFGE.h"
#include "qEquationTrans.h"
#include "qEquationPredTemp.h"
#include "qEquationCube.h"

qEquationFactory *qEquationFactory::instance_ = 0;


qEquationFactory::qEquationFactory(){



}

qEquationFactory::~qEquationFactory(){

}

qEquationFactory *qEquationFactory::instance(){

	if(!instance_)
		instance_ = new qEquationFactory;
	return instance_;

}

qEquationBase *qEquationFactory::BuildEquation(int id){


	switch(id){

		case EQ_LIN0:
			return new qEquationLinear0();
		case EQ_LIN1:
			return new qEquationLinear1();
		case EQ_LIN2:
			return new qEquationLinear2();
		case EQ_LIN3:
			return new qEquationLinear3();
		case EQ_EXP0:
			return new qEquationExp0();
		case EQ_EXP1:
			return new qEquationExp1();
		case EQ_EXP2:
			return new qEquationExp2();
		case EQ_EXP3:
			return new qEquationExp3();
		case EQ_POW0:
			return new qEquationPower0();
		case EQ_POW1:		
			return new qEquationPower1();
		case EQ_POW2:		
			return new qEquationPower2();
		case EQ_POW3:		
			return new qEquationPower3();
		case EQ_NSAT0:	
			return new qEquationNSat0();
		case EQ_NSAT1:	
			return new qEquationNSat1();
		case EQ_NSAT2:	
			return new qEquationNSat2();
		case EQ_NSAT4:	
			return new qEquationNSat4();
		case EQ_NSAT5:	
			return new qEquationNSat5();
		case EQ_NSAT6:	
			return new qEquationNSat6();
		case EQ_FL_ACT:	
			return new qEquationFlAct();
		case EQ_DELAY0:	
			return new qEquationDelay0();
		case EQ_DELAY1:	
			return new qEquationDelay1();
		case EQ_DELAY3:	
			return new qEquationDelay3();
		case EQ_LOG0:		
			return new qEquationLog0();
		case EQ_TRANS:	
			return new qEquationTrans();
		case EQ_GMORT0:	
			return new qEquationGMort0();
		case EQ_GMORT1:
			return new qEquationGMort1();
		case EQ_GMORT2:	
			return new qEquationGMort2();
		case EQ_GMORT3:	
			return new qEquationGMort3();
		case EQ_FDENS:
			return new qEquationFDens();
		case EQ_MIGR:
        case EQ_MIGR1:
        case EQ_MIGR2:
        case EQ_MIGR3:
        case EQ_MIGR4:
        case EQ_MIGR5:
        case EQ_MIGR6:
        case EQ_MIGR7:
        case EQ_MIGR8:
        case EQ_MIGR9:
        case EQ_MIGR10:
			return new qEquationMigration();
		case EQ_PRED_TEMP1:
			return new qEquationPredTemp1();
		case EQ_PRED_TEMP2:
			return new qEquationPredTemp2();
        case EQ_CUB0:
            return new qEquationCube0();
        case EQ_CUB1:
            return new qEquationCube1();
		case EQ_FGE:
			return new qEquationFGE();
        case EQ_GAS_DISP:
            return new qEquationGasDist();

		/* Types not yet implemented. */
		case EQ_NSAT3:	
		case EQ_DRIFT:	
		default:
			/* Should never get here. */
			return NULL;
	}

	return NULL; /* Keeps compiler happy. */

}

