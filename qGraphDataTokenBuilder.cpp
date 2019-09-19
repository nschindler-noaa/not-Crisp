#include "qGraphDataTokenBuilder.h"
#include "qGraphDataTokenTypes.h"
#include "mapConstants.h"
#include "qDatContainer.h"
#include "crisp.h"
#include "qUtils.h"

qGraphDataTokenBuilder *qGraphDataTokenBuilder::instance_ = 0;


qGraphDataTokenBuilder::qGraphDataTokenBuilder(){



}

qGraphDataTokenBuilder::~qGraphDataTokenBuilder(){

}

qGraphDataTokenBuilder *qGraphDataTokenBuilder::instance(){

	if(!instance_)
		instance_ = new qGraphDataTokenBuilder;
	return instance_;

}

qGraphDataToken *qGraphDataTokenBuilder::getToken(int dataType, qDatContainer *dat){

	qGraphDataToken *token=NULL;
	switch(dataType){
		case GDTT_DAMEQUATION_Spill:
			token = new qGraphDataToken("Dam Spill Equation", dat->getString(2), dat->getInt(0), DAM);
			token->setNumElementsPerLine(100);
			token->setGraphXAxisLabel("% river spilled");
			token->setGraphYAxisLabel("% passage in spill");
			token->setYMax(100.0);
			token->setHelpPage( 21 );
			break;
		case GDTT_DAMEQUATION_Trans:
			token = new qGraphDataToken("Transport Mortality Equation", dat->getString(2), dat->getInt(0), DAM);
			token->setNumElementsPerLine(100);
			token->setGraphXAxisLabel("water particle travel time");
			token->setGraphYAxisLabel("% transport mortality");
			token->setYMax(100.0);
			token->setHelpPage( 22 );
			break;
		case GDTT_DAMEQUATION_Delay:
			token = new qGraphDataToken("Dam Delay Equation", dat->getString(2), dat->getInt(0), DAM);
			token->setNumElementsPerLine(24);
			token->setGraphXAxisLabel("time of day");
			token->setGraphYAxisLabel("delay prob");
			token->setYMax(1.0);
			token->setHelpPage( 23 );
			break;
		case GDTT_DAMEQUATION_NSat:
			token = new qGraphDataToken("TDG Saturation > 100% Equation (Day)",
				                     dat->getString(2), dat->getInt(0), DAM);
			token->setNumElementsPerLine(250);
			token->setGraphXAxisLabel("kcfs spilled");
			token->setGraphYAxisLabel("% TDG saturation above 100");
			token->setYMax(40.0);
			token->setHelpPage( 51 );
			break;
		case GDTT_DAMEQUATION_NightNSat:
			token = new qGraphDataToken("TDG Saturation > 100% Equation (Night)",
				                     dat->getString(2), dat->getInt(0), DAM);
			token->setNumElementsPerLine(250);
			token->setGraphXAxisLabel("kcfs spilled");
			token->setGraphYAxisLabel("% TDG saturation above 100");
			token->setYMax(40.0);
			token->setHelpPage( 51 );
			break;
		case GDTT_DAMEQUATION_BackupNSat:
			token = new qGraphDataToken("TDG Saturation > 100% Equation (Backup)",
				                     dat->getString(2), dat->getInt(0), DAM);
			token->setNumElementsPerLine(250);
			token->setGraphXAxisLabel("kcfs spilled");
			token->setGraphYAxisLabel("% TDG saturation above 100");
			token->setYMax(40.0);
			token->setHelpPage( 51 );
			break;
		case GDTT_DAMEQUATION_FGE:
			token = new qGraphDataToken("Night FGE Equation",
				                     dat->getString(2), dat->getInt(0), DAM);
			token->setNumElementsPerLine(100);
			token->setGraphXAxisLabel("fish age since smoltification (days)");
			token->setGraphYAxisLabel("% passage in bypass");
			token->setYMax(100.0);
			token->setHelpPage( 25 );
			break;
		case GDTT_BEHAVIOREQN_GasMort:
			token = new qGraphDataToken("Gas Mortality",
				                     dat->getString(2), dat->getInt(0), DAM);
			token->setNumElementsPerLine(50);
			token->setGraphXAxisLabel("% TDG Sat > 100");
			token->setGraphYAxisLabel("% mort / day");
			token->setYMax(20.0);
			token->setHelpPage( 26 );
			break;
		case GDTT_REACH_GasDist:
			token = new qGraphDataToken("Gas Distribution",
				                     dat->getString(2), dat->getInt(0), REACH);
			token->setNumElementsPerLine(100);
			token->setGraphYAxisLabel("% TDG Sat > 100");
			token->setGraphXAxisLabel("distance along reach (mi)  -  red=left, blue=right");
			token->setYMax(40.0);
			token->setHelpPage( 58 );
			break;
		case GDTT_BEHAVIOREQN_FDens:
			token = new qGraphDataToken("Population Density vs. Depth",
				                     dat->getString(2), dat->getInt(0), DAM);
			token->setNumElementsPerLine(100);
			token->setGraphXAxisLabel("depth (ft)");
			token->setGraphYAxisLabel("pop density (%/v.ft)");
			token->setYMax(15.0);
			token->setHelpPage( 27 );
			break;
		case GDTT_BEHAVIOREQN_SimpleMort:
			token = new qGraphDataToken("Simple Mortality",
				                     dat->getString(2), dat->getInt(0), DAM);
			token->setNumElementsPerLine(100);
			token->setGraphXAxisLabel("days since onset of migration");
			token->setGraphYAxisLabel("mortality fraction per day");
			token->setHelpPage( 87 );
			token->setYMax(1.0);
			break;
		case GDTT_BEHAVIOREQN_FLUSHTrans:
			token = new qGraphDataToken("FLUSH Transport Merit (D)",
				                     dat->getString(2), dat->getInt(0), DAM);
			token->setNumElementsPerLine(100);
			token->setGraphXAxisLabel("survival fraction in-river");
			token->setGraphYAxisLabel("merit of transport (D)");
			token->setHelpPage( 88 );
			token->setYMax(2.0);
			break;
		case GDTT_BEHAVIOREQN_Migration:
			token = new qGraphDataToken("Migration",
				                     dat->getString(2), dat->getInt(0), DAM);
			token->setNumElementsPerLine(170);
			token->setGraphXAxisLabel("Julian Day");
			token->setGraphYAxisLabel("mi / day");
			token->setHelpPage( 28 );
			token->setYMax(30.0);
			break;
		case GDTT_BEHAVIOREQN_PredTemp:
			token = new qGraphDataToken("Predator Temperature Response",
				                     dat->getString(2), dat->getInt(0), DAM);
			token->setNumElementsPerLine(61);
			token->setGraphXAxisLabel("Temperature (deg C)");
			token->setGraphYAxisLabel("Pred Tmp Rsp");
			token->setHelpPage( 89 );
			token->setYMax(100.0);
			break;
	}

	token->setYMin(0.0);
	return token;

}

qGraphDataToken *qGraphDataTokenBuilder::getToken(
			int dataType, const char *sourceName,
			char *speciesName){
	

	qGraphDataToken *token=NULL;

	river_segment *riverSeg=NULL;
	float tf=0.0;
	switch(dataType){
/////////////////////////////////////
//DAM Graphs
		case GDTT_DAM_ForeBayDepth:
			riverSeg = getRiverSegmentHandle(sourceName, DAM_SEG);	
			if(riverSeg && riverSeg->current->dam.forebay_depth){
				token = new qGraphDataToken("Dam Forebay Depth", sourceName, dataType, DAM);
				
				token->setNumLines(1);
				token->setNumElementsPerLine(DAYS_IN_SEASON);
				for(int i=0;i<token->getNumElementsPerLine();i++)
					token->insert(riverSeg->current->dam.forebay_depth[i]);
				token->insertLabel("Depth");
				token->setHelpPage( 29 );
			}
			break;
		case GDTT_DAM_WaterTemp:
			riverSeg = getRiverSegmentHandle(sourceName, DAM_SEG);	
			if(riverSeg && riverSeg->current->dam.water_temp){
				token = new qGraphDataToken("Dam Water Temperature", sourceName, dataType, DAM);
				
				token->setNumLines(1);
				token->setNumElementsPerLine(DAYS_IN_SEASON);
				for(int i=0;i<token->getNumElementsPerLine();i++){
					tf = 0.0;
					for(int j=0;j<STEPS_PER_DAY;j++)
						tf += riverSeg->current->dam.water_temp[i*STEPS_PER_DAY+j];
					tf = tf/(float)STEPS_PER_DAY;
					token->insert(tf);
				}
				token->insertLabel("Temperature (deg C)");
				token->setHelpPage( 18 );
			}
			break;
		case GDTT_DAM_Turbidity:
			riverSeg = getRiverSegmentHandle(sourceName, DAM_SEG);	
			if(riverSeg && riverSeg->current->dam.turbidity){
				token = new qGraphDataToken("Dam Turbidity", sourceName, dataType, DAM);
				
				token->setNumLines(1);
				token->setNumElementsPerLine(DAYS_IN_SEASON);
				for(int i=0;i<token->getNumElementsPerLine();i++){
					tf = 0.0;
					for(int j=0;j<STEPS_PER_DAY;j++)
						tf += riverSeg->current->dam.turbidity[i*STEPS_PER_DAY+j];
					tf = tf/(float)STEPS_PER_DAY;
					token->insert(tf);
				}
				token->insertLabel("Turbidity");
				token->setHelpPage( 418 );
			}
			break;
		case GDTT_DAM_TailRaceDepth:
			riverSeg = getRiverSegmentHandle(sourceName, DAM_SEG);	
			if(riverSeg && riverSeg->current->dam.tailrace_depth){
				token = new qGraphDataToken("Dam Tailrace Depth", sourceName, dataType, DAM);
				
				token->setNumLines(1);
				token->setNumElementsPerLine(DAYS_IN_SEASON);
				for(int i=0;i<token->getNumElementsPerLine();i++)
					token->insert(riverSeg->current->dam.tailrace_depth[i]);
				token->insertLabel("Depth");
				token->setHelpPage( 30 );
			}
			break;
		case GDTT_DAM_DropRatio:
			riverSeg = getRiverSegmentHandle(sourceName, DAM_SEG);	
			if(riverSeg && riverSeg->current->dam.drop_ratio){
				token = new qGraphDataToken("Dam Drop Ratio (calc during simulation)", sourceName, dataType, DAM);
				
				token->setNumLines(1);
				token->setNumElementsPerLine(DAYS_IN_SEASON);
				for(int i=0;i<token->getNumElementsPerLine();i++)
					token->insert(riverSeg->current->dam.drop_ratio[i]);
				token->insertLabel("Drop Ratio");
				token->setHelpPage( 31 );
			}
			break;
		case GDTT_DAM_Spill:
			riverSeg = getRiverSegmentHandle(sourceName, DAM_SEG);	
			if(riverSeg && riverSeg->current->dam.spill){
				token = new qGraphDataToken("Spill Rate", sourceName, dataType, DAM);
				
				token->setNumLines(1);
				token->setNumElementsPerLine(DAYS_IN_SEASON);
				for(int i=0;i<token->getNumElementsPerLine();i++){
					tf = 0.0;
					for(int j=0;j<DAM_TIME_SLC;j++)
						tf  += riverSeg->current->dam.spill[i*DAM_TIME_SLC+j];
					tf = tf/(float)DAM_TIME_SLC;
					token->insert(tf);
				}
				token->insertLabel("Spill (kcfs)");
				token->setHelpPage( 16 );
			}
			break;
/////////////////////////////////////
//Reach Graphs
		case GDTT_REACH_ElevationChange:
			riverSeg = getRiverSegmentHandle(sourceName, REACH_SEG);	
			if(riverSeg && riverSeg->current->reach.elev_change){
				token = new qGraphDataToken("Reach Elevation Change", sourceName, dataType, REACH,GR_NEGATIVE);
				token->setNumLines(1);
				token->setNumElementsPerLine(DAYS_IN_SEASON);
				for(int i=0;i<token->getNumElementsPerLine();i++){
					tf = riverSeg->current->reach.elev_change[i];
					token->insert(tf);
				}
				token->insertLabel("Delta from max (ft)");
				token->setHelpPage( 32 );
			}
			break;
		case GDTT_REACH_WaterTemp:
			riverSeg = getRiverSegmentHandle(sourceName, REACH_SEG);	
			if(riverSeg && riverSeg->current->reach.water_temp){
				token = new qGraphDataToken("Reach Water Temperature", sourceName, dataType, REACH);
				token->setNumLines(1);
				token->setNumElementsPerLine(DAYS_IN_SEASON);
				for(int i=0;i<token->getNumElementsPerLine();i++){
					tf = 0.0;
					for(int j=0;j<STEPS_PER_DAY;j++)
						tf += riverSeg->current->reach.water_temp[i*STEPS_PER_DAY+j];
					tf = tf/(float)STEPS_PER_DAY;
					token->insert(tf);
				}
				token->insertLabel("Temperature (deg C)");
				token->setHelpPage( 19 );
			}
			break;
		case GDTT_REACH_Turbidity:
			riverSeg = getRiverSegmentHandle(sourceName, REACH_SEG);	
			if(riverSeg && riverSeg->current->reach.turbidity){
				token = new qGraphDataToken("Reach Turbidity", sourceName, dataType, REACH);
				token->setNumLines(1);
				token->setNumElementsPerLine(DAYS_IN_SEASON);
				for(int i=0;i<token->getNumElementsPerLine();i++){
					tf = 0.0;
					for(int j=0;j<STEPS_PER_DAY;j++)
						tf += riverSeg->current->reach.turbidity[i*STEPS_PER_DAY+j];
					tf = tf/(float)STEPS_PER_DAY;
					token->insert(tf);
				}
				token->insertLabel("Turbidity");
				token->setHelpPage( 419 );
			}
			break;
		case GDTT_REACH_Loss:
			riverSeg = getRiverSegmentHandle(sourceName, REACH_SEG);	
			if(riverSeg && riverSeg->current->reach.loss){
				token = new qGraphDataToken("Loss", sourceName, dataType, REACH, GR_BOTH);
				token->setNumLines(1);
				token->setNumElementsPerLine(DAYS_IN_SEASON);
				for(int i=0;i<token->getNumElementsPerLine();i++){
					tf = riverSeg->current->reach.loss[i];
					token->insert(tf);
				}
				token->insertLabel("Loss (kcfs)");
				token->setHelpPage( 33 );
			}
			break;

		default:
			break;

	}

	token->setYBounds( 1 );

	return token;
}


void qGraphDataTokenBuilder::applyToken(qGraphDataToken *token){




}
