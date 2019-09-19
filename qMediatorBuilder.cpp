

#include "qIdcSpillEquation.h"
#include "qIdcNSatEquation.h"
#include "qIdcPredatorProbability.h"
#include "qIdcTransMortEquation.h"
#include "qIdcDamDelayEquation.h"
#include "qIdcGasMortEquation.h"
#include "qIdcGasDistEquation.h"
#include "qIdcFDensEquation.h"
#include "qIdcFGEEquation.h"
#include "qIdcMigrationEquation.h"
#include "qIdcPredTempEquation.h"
#include "qReleaseTool.h"

#include "Qcrisp.h"
#include "qReachClassEquationMed.h"
#include "resource.h"
#include "qGraphMedPosition.h"
#include "qGraphMedDrawData.h"
#include "qEditGraphGDT.h"
#include "qGraphMedDrawAxisYD.h"
#include "qGraphMedDrawAxisXI.h"
#include "qGraphMedDrawLines.h"
#include <qmessagebox.h>
#include "qGraphMedZoom.h"
#include "qGraphDataTokenBuilder.h"
#include "qGraphDataTokenTypes.h"
#include "qDamReachGDT.h"
#include "qGraphMediator.h"
#include "qMediatorBuilder.h"
#include "QmapObject.h"
#include "log.h"
#include <qmessagebox.h>
#include "qUtils.h"
#include "Qmain.h"
#include "qGraphSpeciesMediator.h"
#include "qPassHistGDT.h"
#include "qDialogMedEquationBase.h"
#include "qDialogMedTimeBase.h"


qMediatorBuilder* qMediatorBuilder::_instance = 0;

extern qReleaseTool *theReleaseTool_;  // pathological data connection

qMediatorBuilder::qMediatorBuilder(QWidget* parent) : _w(parent), ref_(0) {
	tokenBuilder_ = qGraphDataTokenBuilder::instance();
}

qMediatorBuilder::~qMediatorBuilder(){

}

qMediatorBuilder* qMediatorBuilder::instance(QWidget *parent){
	if(!_instance){ 
		_instance = new qMediatorBuilder(parent);
	}
	return (qMediatorBuilder*)(_instance);
}


int qMediatorBuilder::makeDialog(const int type, QmapObject *geoObject1, QmapObject *geoObject2){

	return makeDialog(type, (geoObject2->getLabel()));
	
}

int qMediatorBuilder::makeDialog(const int type, const char *source){

	QMessageBox *zm=NULL;
	qGraphDataToken *dt;
	qGraphMediator *gm;
	qGraphPane *gp;
	qGraphDialog *gd;

	switch(type){
		
		case GDTT_DAM_ForeBayDepth:
		case GDTT_DAM_TailRaceDepth:
		case GDTT_DAM_DropRatio:
			dt = tokenBuilder_->getToken(type, source);
			if(dt){
				gd = new qGraphDialog(NULL,0, Qt::WType_TopLevel,dt->getGraphTitle().ascii());
				gm = new qGraphMediator(gd, dt);
				gp = gd->getGraphPane();
				gm->insertSubMediator(new qGraphMedDrawLines(gp));
				gm->insertSubMediator(new qGraphMedDrawAxisYD(gp));
				gm->insertSubMediator(new qGraphMedDrawAxisXI(gp));
				gm->insertSubMediator(new qGraphMedZoom(gp,gd));
				gm->insertSubMediator(new qGraphMedPosition(gp,gd));

				gd->show(); // show the dialog
/* uncomment this to make the dialog editable with left-button drag 
 *				gm->insertSubMediator(new ZasGraphMedDrawData(gp));
 */

			}
			break;

		/* We still need a way to get into this via the mouse tool. Hence
		 *  the big case statement survives. 
		 * TODO finally get rid of big case statement. */
		case GDTT_DAMEQUATION_Trans:
		case GDTT_DAMEQUATION_Spill:
		case GDTT_DAMEQUATION_Delay:
		case GDTT_DAMEQUATION_NSat:
		case GDTT_DAMEQUATION_NightNSat:
		case GDTT_DAMEQUATION_BackupNSat:
		case GDTT_REACH_GasDist:
		case GDTT_DAMEQUATION_FGE:
		case GDTT_BEHAVIOREQN_GasMort:
		case GDTT_BEHAVIOREQN_Migration:
		case GDTT_BEHAVIOREQN_PredTemp:
			makeEquationDialog( type, source );
			break;																								 
		case GDTT_PASSAGE_Dam:
			makeSpeciesDialog( new qDamPassHistGDT( source ) );
			break;
		case GDTT_PASSAGE_Reach:
			makeSpeciesDialog( new qReachPassHistGDT( source ) );
			break;
		case GDTT_PASSAGE_ReachLength:
			makeSpeciesDialog( new qReachLengthHistGDT( source ) );
			break;
		case GDTT_PASSAGE_Bypass:
			makeSpeciesDialog( new qBypassPassHistGDT( source ) );
			break;
		case GDTT_PASSAGE_Spillway:
			makeSpeciesDialog( new qSpillwayPassHistGDT( source ) );
			break;
		case GDTT_PASSAGE_Transport:
			makeSpeciesDialog( new qTransportPassHistGDT( source ) );
			break;
		case GDTT_PASSAGE_Turbine:
			makeSpeciesDialog( new qTurbinePassHistGDT( source ) );
			break;
		case GDTT_REACH_WaterTemp:
			makeGraphDialog( new qReachWaterTempGDT( source ) );
			break;
		case GDTT_REACH_Turbidity:
			makeGraphDialog( new qReachTurbidityGDT( source ) );
			break;
		case GDTT_REACH_NSatOut:
			makeGraphDialog( new qReachNSatGDT( source ) );
			break;
		case GDTT_DAM_WaterTemp:
			makeGraphDialog( new qDamWaterTempGDT( source ) );
			break;
		case GDTT_DAM_Turbidity:
			makeGraphDialog( new qDamTurbidityGDT( source ) );
			break;
		case GDTT_DAM_NSat:
			makeGraphDialog( new qDamNSatGDT( source ) );
			break;
		case GDTT_DAM_Spill:
			makeGraphDialog( new qDamSpillGDT( source ) );
			break;
		case GDTT_DAM_Flow:
			makeGraphDialog( new qDamFlowGDT( source ) );
			break;
		case GDTT_REACH_Flow:
			makeGraphDialog( new qReachFlowGDT( source ) );
			break;
		case GDTT_REACH_Velocity:
			makeGraphDialog( new qReachVelGDT( source ) );
			break;
		case GDTT_HEADWATER_Flow:
			makeEditGraphDialog( new qHeadwaterFlowGDT( source ) );
			break;
		case GDTT_HEADWATER_WaterTemp:
			makeEditGraphDialog( new qHeadwaterTempGDT( source ) );
			break;
		case GDTT_HEADWATER_Turbidity:
			makeEditGraphDialog( new qHeadwaterTurbidityGDT( source ) );
			break;
		case GDTT_DAM_BasinVolume:
			{
			qGraphDialog *d;
			QRect r;
			river_segment *seg = getRiverSegmentHandle( source, DAM_SEG );
			if (seg == NULL)
				break;
			if (seg->current == NULL)
				break;
			if (seg->current->dam.storage_basin == NULL) {
				QMessageBox::warning ( (QWidget*) NULL, "COMPASS", "This dam does not have a storage basin",
					 QMessageBox::Ok, 0, 0 );
						
				break;
			}

			makeEditGraphDialog( new qBasinVolumeGDT( source ), HIDE_ACTIVE );
			d = makeEditGraphDialog( new qEditDamFlowGDT( source ), HIDE_ACTIVE ); 
			r = d->frameGeometry();
			d->move( r.left()+25, r.top()+25 );
			}
			break;

		case GDTT_REACH_Loss:
			makeEditGraphDialog( new qFlowLossGDT( source ) );
			break;
		case GDTT_REACH_ElevationChange:
			makeEditGraphDialog( new qElevationChangeGDT( source ) );
			break;

		case GDTT_RELEASE_Tool:
			if (theReleaseTool_ != NULL) {
				theReleaseTool_->setShown( TRUE);
				theReleaseTool_->selectRelease( source );
			}
			else {
				theReleaseTool_ = new qReleaseTool( _w, source );
			}

		default:
			break;
	}
	
	return 0;

}



/* This is the new way to make mediators - the GraphDataToken is
 *  actually a member of a derived class which knows how to fill
 *  itself with data. For now, these dialogs are not drawable with a
 *  left mouse button down drag. 
 * NOTE: this routine makes a graph dialog with the month/day date
 *  field exposed.*/

int qMediatorBuilder::makeSpeciesDialog( qGraphDataToken *token )
{
	qGraphMediator *gm;
	qGraphPane *gp;
	qGraphDialog *gd;
	if(token){
		gd = new qGraphDialog(NULL, SHOW_COMBO | SHOW_DATE,	
								 Qt::WDestructiveClose | Qt::WType_TopLevel,
			                    token->getGraphTitle().ascii());
		gm = new qGraphSpeciesMediator(gd, token);
		gp = gd->getGraphPane();
		gm->insertSubMediator(new qGraphMedDrawLines(gp));
		gm->insertSubMediator(new qGraphMedDrawAxisYD(gp));
		gm->insertSubMediator(new qGraphMedDrawAxisXI(gp));
		gm->insertSubMediator(new qGraphMedZoom(gp,gd));
		gm->insertSubMediator(new qGraphMedPosition(gp,gd));

		gd->show(); // show the dialog
/* uncomment this to make the dialog editable with left-button drag 
		gm->insertSubMediator(new ZasGraphMedDrawData(gp));
*/
	}
	return 0;
}

/* This is the new way to make mediators - the GraphDataToken is
 *  actually a member of a derived class which knows how to fill
 *  itself with data. these dialogs are not drawable with a
 *  left mouse button down drag.
 * NOTE: this routine makes a graph dialog with the month/day date
 *  field exposed. */
int qMediatorBuilder::makeGraphDialog( qGraphDataToken *token )
{
	qGraphMediator *gm;
	qGraphPane *gp;
	qGraphDialog *gd;
	if(token){
		gd = new qGraphDialog(NULL, SHOW_DATE, Qt::WDestructiveClose | Qt::WType_TopLevel, 
								token->getGraphTitle().ascii());
		gm = new qGraphMediator(gd, token);
		gp = gd->getGraphPane();
		gm->insertSubMediator(new qGraphMedDrawLines(gp));
		gm->insertSubMediator(new qGraphMedDrawAxisYD(gp));
		gm->insertSubMediator(new qGraphMedDrawAxisXI(gp));
		gm->insertSubMediator(new qGraphMedZoom(gp,gd));
		gm->insertSubMediator(new qGraphMedPosition(gp,gd));

		gd->show(); // show the dialog
/* uncomment this to make the dialog editable with left-button drag 
		gm->insertSubMediator(new ZasGraphMedDrawData(gp));
*/
	}
	return 0;
}

/* This is the new way to make mediators - the GraphDataToken is
 *  actually a member of a derived class which knows how to fill
 *  itself with data. these dialogs *are* drawable with a left mouse 
 *  button down drag.
 * NOTE: this routine makes a graph dialog with the month/day date
 *  field exposed and with the bottom line of buttons exposed. */
qGraphDialog *qMediatorBuilder::makeEditGraphDialog( 
									qEditGraphDataToken *token,
									int flags )
{
	qEditGraphMediator *egm;
	qGraphPane *gp;
	qGraphDialog *gd;
	if(token){
		gd = new qGraphDialog(NULL, HIDE_DONE | SHOW_DATE | SHOW_BOTTOM | flags,
					Qt::WDestructiveClose |  Qt::WType_Dialog,token->getGraphTitle().ascii());
		egm = new qEditGraphMediator(gd, token);
		gp = gd->getGraphPane();
		egm->insertSubMediator(new qGraphMedDrawLines(gp));
		egm->insertSubMediator(new qGraphMedDrawAxisYD(gp));
		egm->insertSubMediator(new qGraphMedDrawAxisXI(gp));
		egm->insertSubMediator(new qGraphMedZoom(gp,gd));
		egm->insertSubMediator(new qGraphMedPosition(gp,gd));
		egm->insertSubMediator(new qGraphMedDrawData(gp,token));

		gd->show(); // show the dialog

		return gd;
	}
	return NULL;
}

/* This is the new way to make equation dialogs. We still need a little bit
 *  of a case statement, but it only selects between the different types
 *  of equation dialog which are available.
 */
qEquationDialogBase *qMediatorBuilder::makeEquationDialog( 
							const int type, const char *source ) {

	qEquationDialogBase *zedb =
		new qEquationDialogBase(_w,  Qt::WType_TopLevel | Qt::WDestructiveClose, "Equation Dialog" );

	switch (type) {
		case GDTT_DAMEQUATION_Spill:
			new qDialogMedEquationBase( &qIdcSpillEquation_, zedb, source );
			break;	
		case GDTT_DAMEQUATION_Trans:
			new qDialogMedEquationBase(&qIdcTransMortEquation_, zedb, source );
			break;
		case GDTT_DAMEQUATION_Delay:
			new qDialogMedEquationBase(&qIdcDamDelayEquation_, zedb, source );
	  		zedb->getGraphPane()->drawMethod( CITYSCAPE );
			break;
		case GDTT_DAMEQUATION_NSat:
			new qDialogMedEquationBase(&qIdcNSatEquation_, zedb, source );
			break;
		case GDTT_DAMEQUATION_NightNSat:
			new qDialogMedEquationBase(&qIdcNightNSatEquation_, zedb, source );
			break;
		case GDTT_DAMEQUATION_BackupNSat:
			new qDialogMedEquationBase(&qIdcBackupNSatEquation_, zedb, source );
			break;
		case GDTT_BEHAVIOREQN_GasMort:
			new qDialogMedEquationBase(&qIdcGasMortEquation_, zedb, source );
			break;
		case GDTT_BEHAVIOREQN_FDens:
			new qDialogMedEquationBase(&qIdcFDensEquation_, zedb, source );
			break;
		case GDTT_BEHAVIOREQN_Migration:
			new qReachClassEquationMed(&qIdcMigrationEquation_, zedb, source );
			zedb->showPositionDateLabel( true );
			break;
		case GDTT_BEHAVIOREQN_PredTemp:
			new qReachClassEquationMed(&qIdcPredTempEquation_, zedb, source );
			break;
		case GDTT_DAMEQUATION_FGE:
			new qDialogMedEquationBase(&qIdcFGEEquation_, zedb, source );
			break;
		case GDTT_REACH_GasDist:
			new qDialogMedEquationBase(&qIdcGasDistEquation_, zedb, source );
			break;
		default:
			delete zedb;
			zedb = NULL;
			break;
	}
	return zedb;
}

/* This is the new way to make time-base dialogs. We still need a small
 *  case statement to select which type of time-base dialog is wanted.
 */

int qMediatorBuilder::makeTimeBaseDialog( const int type ) {
	switch (type) {
	case ID_PREDATIONPROB:
		new qDialogMedTimeBase( &qIdcPredatorProbability_,
			new qDialogBase(_w,	Qt::WType_TopLevel | Qt::WDestructiveClose,
				"Dam Predation Probability" ),"Dam Predation Probability" );
		break;
	}
	return 0;
}
