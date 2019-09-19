#include "crisp.h"
#include "eqns.h"
#include "qIdcNSatEquation.h"

/* Global, static object; single instantiation for all dams & species.
 *  Spill efficiency equation.
 */
qIdcNSatEquation qIdcNSatEquation_( GDTT_DAMEQUATION_NSat );

qIdcNSatEquation::qIdcNSatEquation(int iDialogID)
							 : qIdcEquationBase(iDialogID)
{
	/* say which types of equations are allowed for this data.
	 */
	addEqType( EQ_NSAT0 );
	addEqType( EQ_NSAT4 );
	addEqType( EQ_NSAT1 );
	addEqType( EQ_NSAT2 );
	addEqType( EQ_NSAT5 );
	addEqType( EQ_NSAT6 );
}

void qIdcNSatEquation::ApplyData(qDatContainer *dat){

	river_segment *riverSeg=NULL;
	riverSeg = getRiverSegmentHandle(dat->getString(0), DAM_SEG);
	equation *eq = NULL;
	if(riverSeg){
		eq = &(riverSeg->current->dam.nsat_eqn);
		SetEquationData(eq, dat);
		qIdcEquationBase::ApplyData( dat );
		return;
	}
}


int qIdcNSatEquation::FillData(qDatContainer *dat){

  	river_segment *riverSeg=NULL;
	riverSeg = getRiverSegmentHandle(dat->getString(0), DAM_SEG);
	equation *eq = NULL;
	if(riverSeg){
		eq = &(riverSeg->current->dam.nsat_eqn);
		return GetEquationData(eq, dat);
	}
	return 0;
}


/* ******** the same thing again, but at night. ************* */

/* Global, static object; single instantiation for all dams & species.
 *  Spill efficiency equation.
 */
qIdcNightNSatEquation qIdcNightNSatEquation_( GDTT_DAMEQUATION_NightNSat );

qIdcNightNSatEquation::qIdcNightNSatEquation(int iDialogID)
							 : qIdcEquationBase(iDialogID)
{
	/* say which types of equations are allowed for this data.
	 */
	addEqType( EQ_NSAT0 );
	addEqType( EQ_NSAT4 );
	addEqType( EQ_NSAT1 );
	addEqType( EQ_NSAT2 );
	addEqType( EQ_NSAT5 );
	addEqType( EQ_NSAT6 );
}

void qIdcNightNSatEquation::ApplyData(qDatContainer *dat){

	river_segment *riverSeg=NULL;
	riverSeg = getRiverSegmentHandle(dat->getString(0), DAM_SEG);
	equation *eq = NULL;
	if(riverSeg){
		eq = &(riverSeg->current->dam.nsat_night_eqn);
		SetEquationData(eq, dat);
		qIdcEquationBase::ApplyData( dat );
		return;
	}
}


int qIdcNightNSatEquation::FillData(qDatContainer *dat){

  	river_segment *riverSeg=NULL;
	riverSeg = getRiverSegmentHandle(dat->getString(0), DAM_SEG);
	equation *eq = NULL;
	if(riverSeg){
		eq = &(riverSeg->current->dam.nsat_night_eqn);
		return GetEquationData(eq, dat);
	}
	return 0;
}


/* ******** the same thing again, but fallback position if new EQ 5 & 6
 * ********   produce strange results. ************* */

/* Global, static object; single instantiation for all dams & species.
 *  Spill efficiency equation.
 */
qIdcBackupNSatEquation qIdcBackupNSatEquation_( GDTT_DAMEQUATION_BackupNSat );

qIdcBackupNSatEquation::qIdcBackupNSatEquation(int iDialogID)
							 : qIdcEquationBase(iDialogID)
{
	/* say which types of equations are allowed for this data.
	 */
	addEqType( EQ_NSAT0 );
	addEqType( EQ_NSAT4 );
	addEqType( EQ_NSAT1 );
	addEqType( EQ_NSAT2 );
}

void qIdcBackupNSatEquation::ApplyData(qDatContainer *dat){

	river_segment *riverSeg=NULL;
	riverSeg = getRiverSegmentHandle(dat->getString(0), DAM_SEG);
	equation *eq = NULL;
	if(riverSeg){
		eq = &(riverSeg->current->dam.nsat_backup_eqn);
		SetEquationData(eq, dat);
		qIdcEquationBase::ApplyData( dat );
		
        /* The day and night nsat equations may depend on the backup
         *  equation (if they are Bounded Linear or Bounded Exponential
         *  with the bounds set properly), so prod them to recalculate and
         *  redisplay.
         */
        qIdcNSatEquation_.invalidate();
        qIdcNightNSatEquation_.invalidate();
        return;
	}
}


int qIdcBackupNSatEquation::FillData(qDatContainer *dat){

  	river_segment *riverSeg=NULL;
	riverSeg = getRiverSegmentHandle(dat->getString(0), DAM_SEG);
	equation *eq = NULL;
	if(riverSeg){
		eq = &(riverSeg->current->dam.nsat_backup_eqn);
		return GetEquationData(eq, dat);
	}
	return 0;
}





