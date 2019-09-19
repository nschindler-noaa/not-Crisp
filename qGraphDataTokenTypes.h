#ifndef _qGraphDataTokenTypes_h_
#define _qGraphDataTokenTypes_h_

const int GDTT_DAM_Spill				= 4000;
const int GDTT_DAM_Flow					= 4001;
const int GDTT_DAM_EditFlow             = 4002;
const int GDTT_DAM_NSat					= 4003;
const int GDTT_DAM_ForeBayDepth			= 4004;
const int GDTT_DAM_TailRaceDepth		= 4005;
const int GDTT_DAM_DropRatio			= 4006;
const int GDTT_DAM_BasinVolume          = 4007;
const int GDTT_DAM_WaterTemp			= 4008;
const int GDTT_DAM_Turbidity			= 4009;

const int GDTT_REACH_ElevationChange	= 4100;
const int GDTT_REACH_Velocity			= 4102;
const int GDTT_REACH_Loss				= 4103;
const int GDTT_REACH_WaterTemp			= 4104;
const int GDTT_REACH_NSatOut			= 4106;
const int GDTT_REACH_Flow				= 4107;
const int GDTT_REACH_GasDist            = 4108;
const int GDTT_REACH_Turbidity			= 4109;


const int GDTT_HEADWATER_Flow			= 4200;
const int GDTT_HEADWATER_WaterTemp		= 4202;
const int GDTT_HEADWATER_Turbidity		= 4203;

const int GDTT_DAMEQUATION_Spill			= 4300;
const int GDTT_DAMEQUATION_NSat				= 4301;
const int GDTT_DAMEQUATION_FGE				= 4302;
const int GDTT_DAMEQUATION_Trans			= 4303;
const int GDTT_DAMEQUATION_Delay			= 4304;
const int GDTT_DAMEQUATION_NightNSat		= 4305;
const int GDTT_DAMEQUATION_BackupNSat		= 4306;

/* Behavior menu equations.
 */
const int GDTT_BEHAVIOREQN_GasMort			= 4350;
const int GDTT_BEHAVIOREQN_FDens			= 4351;
const int GDTT_BEHAVIOREQN_Migration		= 4352;
const int GDTT_BEHAVIOREQN_PredTemp			= 4353;
const int GDTT_BEHAVIOREQN_SimpleMort	    = 4354;
const int GDTT_BEHAVIOREQN_FLUSHTrans   	= 4355;

/* Passage Histogram types. */
const int GDTT_PASSAGE_Dam       = 4400;
const int GDTT_PASSAGE_Reach     = 4401;
const int GDTT_PASSAGE_Bypass    = 4402;
const int GDTT_PASSAGE_Spillway  = 4403;
const int GDTT_PASSAGE_Transport = 4404;
const int GDTT_PASSAGE_Turbine   = 4405;
const int GDTT_PASSAGE_ReachLength = 4406;

/* Release tool for given release */
const int GDTT_RELEASE_Tool		 = 4500;
#endif


