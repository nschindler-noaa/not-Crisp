/** \file
 * \brief Input file parser.
 *
 * \section parser COMPASS Input File Parser
 * The COMPASS input file parser is written in a straightforward manner but
 * this has led to a rather gargantuan parse.c file. Rather than defining the
 * form of different types of input tokens (e.g. equations, floating point
 * numbers, integers, etc.) this parser relies on writing the code for
 * pretty much every token by hand. Of course, functions are used to do the
 * grunt work, but there still has to be huge switch statements with sections
 * for each possible token in each possible place and this whole mess is on the
 * verge of becoming unmaintainable.
 *
 * Of course, focusing on what's here rather than what should be here, the
 * parser makes heavy use of macros (especially for reach classes) to make the
 * code smaller and hopefully easier to read. When adding tokens, my
 * suggestion is to find a token that's already the same type as what you need,
 * copy it and modify it until it does what you want.
 */

#include <stdlib.h>
#ifndef WIN32
#include <unistd.h>
#endif
#include <math.h>
#include <stdio.h>
#include <string.h>

#include <errno.h>

#include "parseutil.h"
#include "c1tokens.h"
#include "token.h"
#include "crisp.h"
#include "log.h"
#include "river.h"
#include "batch.h"
#include "gas_mort.h"
#include "release.h"
#include "table.h"
#include "transport.h"
#include "flow.h"
#include "init.h"
#include "pred.h"
#include "output_flags.h"
#include "writeutil.h"
#include "spill.h"

#ifdef REALTIME
#include "realtime.h"
#endif

/* Forward definitions of functions in this file. */
static int parse_transport (PARSER_FILE *buffer, struct river_segment *seg,
							int run_index, const char *end_token);

/* this little hack is here to supply scratch
   variables to recursive fcns w/o lots of overhead */
static char scratch1[MAX_COMMAND_LEN + 1], scratch2[MAX_COMMAND_LEN + 1];

/*------------------------------------------------------------------------*/
/*   set up all the list tokens */
/*------------------------------------------------------------------------*/

static command_struct level0_tokens []  =
{ { TK_RUN, TKS_RUN },
  { TK_SPEC, TKS_SPEC },
  { TK_STOCK, TKS_STOCK },
  { TK_PRED_ACT, TKS_PRED_ACT },
  { TK_NOTES, TKS_NOTES },
  { TK_FILE_CREATION_DATE, TKS_FILE_CREATION_DATE },
  { TK_FILE_CREATOR, TKS_FILE_CREATOR },
  { TK_FILE_MODIFICATION_DATE, TKS_FILE_MODIFICATION_DATE },
  { TK_FILE_MODIFIER, TKS_FILE_MODIFIER },
  { TK_ADD_RLS, TKS_RLS },
  { TK_ADD_RLS, TKS_ADD_RLS },
  { TK_MOD_RLS, TKS_MOD_RLS },
  { TK_REACH, TKS_REACH },
  { TK_HDWTR, TKS_HDWTR },
  { TK_DAM, TKS_DAM },
  { TK_PRED_ACT_EXP, TKS_PRED_ACT_EXP },
  { TK_GAS_DIS_EXP, TKS_GAS_DIS_EXP },
  { TK_WTEMP, TKS_WTEMP },		
  { TK_FLOW_PROP, TKS_FLOW_PROP },		
  { TK_UFREE, TKS_UFREE },
  { TK_LENGTH_WEIGHT_B0, TKS_LENGTH_WEIGHT_B0 },
  { TK_LENGTH_WEIGHT_B1, TKS_LENGTH_WEIGHT_B1 },
  { TK_PREY_ENERGY_DENSITY, TKS_PREY_ENERGY_DENSITY },
  { TK_FORK_THRESHOLD, TKS_FORK_THRESHOLD },	
  { TK_RUN_FLAGS, TKS_RUN_FLAGS },
  { TK_REACH_CLASS_NAME, TKS_REACH_CLASS_NAME },
  { TK_NUM_REACH_CLASSES, TKS_NUM_REACH_CLASSES },	
  { TK_MORTALITY_CLASS, TKS_MORTALITY_CLASS },		
  { TK_POST_BONNEVILLE_HYPOTHESIS, TKS_POST_BONNEVILLE_HYPOTHESIS },		
  { TK_FLOW_SEG, TKS_FLOW_SEG },
  { TK_WATER_TT_UPPER, TKS_WATER_TT_UPPER }, 
  { TK_WATER_TT_LOWER, TKS_WATER_TT_LOWER }, 
  { TK_MIN_MIGR_RATE, TKS_MIN_MIGR_RATE },
  { TK_WATER_TT_FIRST_DAY, TKS_WATER_TT_FIRST_DAY },
  { TK_WATER_TT_LAST_DAY, TKS_WATER_TT_LAST_DAY },
#ifdef REALTIME  
  { TK_RT_REPORT_DATE, TKS_RT_REPORT_DATE },
#endif  
  { TK_INCLUDE, TKS_INCLUDE},		
  { TK_VERSION, TKS_VERSION }, 
  { TK_END, TKS_END },
  { 0, "\0" }
};

static command_struct species_tokens []  =
{ { TK_VB, TKS_VB },
  { TK_VVAR, TKS_VVAR},
  { TK_VVAR1, TKS_VVAR1},
  { TK_DRIFT, TKS_DRIFT},
  { TK_DRIFT_EQUATION, TKS_DRIFT_EQUATION},
  { TK_GMORT_EQUATION, TKS_GMORT_EQUATION},
  { TK_FDENS_EQUATION, TKS_FDENS_EQUATION},
  { TK_MIGR_EQUATION, TKS_MIGR_EQUATION},
  { TK_PRED_TR_EQUATION, TKS_PRED_TR_EQUATION},
  { TK_INRIVER_RETURN_EQUATION, TKS_INRIVER_RETURN_EQUATION},
  { TK_TRANSPORT_RETURN_EQUATION, TKS_TRANSPORT_RETURN_EQUATION},
  { TK_INRIVER_LATENT_MORT_EQUATION, TKS_INRIVER_LATENT_MORT_EQUATION},
  { TK_INRIVER_LATENT_MORT, TKS_INRIVER_LATENT_MORT},
  { TK_TRANSPORT_LATENT_MORT, TKS_TRANSPORT_LATENT_MORT},
  { TK_DIFFERENTIAL_RETURN, TKS_DIFFERENTIAL_RETURN},
  { TK_PRED_CAPTURE_DIST_EQN, TKS_PRED_CAPTURE_DIST_EQN},
  { TK_CUSTOM_SURVIVAL_EQN, TKS_CUSTOM_SURVIVAL_EQN},
  { TK_PCOEF_MN, TKS_PCOEF_MN},
  { TK_PCOEF_VAR, TKS_PCOEF_VAR},
  { TK_PCOEF_LOW, TKS_PCOEF_LOW},
  { TK_PCOEF_HIGH, TKS_PCOEF_HIGH},
  { TK_GMCOEF_MN, TKS_GMCOEF_MN},
  { TK_GMCOEF_VAR, TKS_GMCOEF_VAR},
  { TK_GMCOEF_LOW, TKS_GMCOEF_LOW},
  { TK_GMCOEF_HIGH, TKS_GMCOEF_HIGH},
  { TK_RPCOEF, TKS_RPCOEF},
  { TK_RPCOEF_MEAN, TKS_RPCOEF_MEAN},
  { TK_RPCOEF_LOW, TKS_RPCOEF_LOW},
  { TK_RPCOEF_HIGH, TKS_RPCOEF_HIGH},
  { TK_RPCOEF_V15_MEAN, TKS_RPCOEF_V15_MEAN},
  { TK_RPCOEF_V15_LOW, TKS_RPCOEF_V15_LOW},
  { TK_RPCOEF_V15_HIGH, TKS_RPCOEF_V15_HIGH},
  { TK_RPCOEF_V16_MEAN, TKS_RPCOEF_V16_MEAN},
  { TK_RPCOEF_V16_LOW, TKS_RPCOEF_V16_LOW},
  { TK_RPCOEF_V16_HIGH, TKS_RPCOEF_V16_HIGH},
  { TK_PRED_MORT_A, TKS_PRED_MORT_A},
  { TK_PRED_MORT_V16_B, TKS_PRED_MORT_V16_B},
  { TK_TIME_COEFF, TKS_TIME_COEFF},
  { TK_DISTANCE_COEFF, TKS_DISTANCE_COEFF},
  { TK_SIGMA_D, TKS_SIGMA_D},
  { TK_PPRIME_A, TKS_PPRIME_A},
  { TK_PPRIME_B, TKS_PPRIME_B},
  { TK_FPCOEF, TKS_FPCOEF},
  { TK_FPCOEF_MEAN, TKS_FPCOEF_MEAN},
  { TK_FPCOEF_LOW, TKS_FPCOEF_LOW},
  { TK_FPCOEF_HIGH, TKS_FPCOEF_HIGH},
  { TK_TPCOEF, TKS_TPCOEF},
  { TK_TPCOEF_MEAN, TKS_TPCOEF_MEAN},
  { TK_TPCOEF_LOW, TKS_TPCOEF_LOW},
  { TK_TPCOEF_HIGH, TKS_TPCOEF_HIGH},
  { TK_GMEXP, TKS_GMEXP},
  { TK_GMTHRESH, TKS_GMTHRESH},
  { TK_NOON_DIST, TKS_NOON_DIST},
  { TK_NIGHT_DIST, TKS_NIGHT_DIST},
  { TK_DELAY_CONST, TKS_DELAY_CONST},
  { TK_DELAY_THRESH, TKS_DELAY_THRESH},
  { TK_DVCOEF_MEAN, TKS_DVCOEF_MEAN},
  { TK_DVCOEF_LOW, TKS_DVCOEF_LOW},
  { TK_DVCOEF_HIGH, TKS_DVCOEF_HIGH},
  { TK_MRCOEF, TKS_MRCOEF},
  { TK_MRCOEF_MEAN, TKS_MRCOEF_MEAN},
  { TK_MRCOEF_LOW, TKS_MRCOEF_LOW},
  { TK_MRCOEF_HIGH, TKS_MRCOEF_HIGH},
  { TK_REACH_SURVIVAL_COEF, TKS_REACH_SURVIVAL_COEF},
  { TK_Y_PRED_CAL, TKS_Y_PRED_CAL},
  { TK_GM_LO_SLOPE, TKS_GM_LO_SLOPE},
  { TK_GM_HI_SLOPE, TKS_GM_HI_SLOPE},
  { TK_GM_CRITICAL, TKS_GM_CRITICAL},
  { TK_INCLUDE, TKS_INCLUDE},
  { TK_END, TKS_END },
  { 0, "\0" }
};

static command_struct stock_tokens []  =
{ { TK_VVAR1, TKS_VVAR1 },
  { TK_MRCOEF, TKS_MRCOEF},
  { TK_MRCOEF_MEAN, TKS_MRCOEF_MEAN},
  { TK_MRCOEF_LOW, TKS_MRCOEF_LOW},
  { TK_MRCOEF_HIGH, TKS_MRCOEF_HIGH},
  { TK_MIGR_EQUATION, TKS_MIGR_EQUATION},
  { TK_PRED_TR_EQUATION, TKS_PRED_TR_EQUATION},
  { TK_INRIVER_RETURN_EQUATION, TKS_INRIVER_RETURN_EQUATION},
  { TK_TRANSPORT_RETURN_EQUATION, TKS_TRANSPORT_RETURN_EQUATION},
  { TK_PRED_CAPTURE_DIST_EQN, TKS_PRED_CAPTURE_DIST_EQN},
  { TK_CUSTOM_SURVIVAL_EQN, TKS_CUSTOM_SURVIVAL_EQN},
  { TK_TIME_COEFF, TKS_TIME_COEFF},
  { TK_DISTANCE_COEFF, TKS_DISTANCE_COEFF},
  { TK_SIGMA_D, TKS_SIGMA_D},
  { TK_REACH_SURVIVAL_COEF, TKS_REACH_SURVIVAL_COEF},
  { TK_INCLUDE, TKS_INCLUDE},
  { TK_END, TKS_END },
  { 0, "\0" }
};

#if 0
static command_struct run_tokens []  =
{ { TK_PRED_ACT, TKS_PRED_ACT },
  { TK_ADD_RLS, TKS_RLS },
  { TK_ADD_RLS, TKS_ADD_RLS },
  { TK_MOD_RLS, TKS_MOD_RLS },
  { TK_REACH, TKS_REACH },
  { TK_HDWTR, TKS_HDWTR },
  { TK_DAM, TKS_DAM },
  { TK_PRED_ACT_EXP, TKS_PRED_ACT_EXP },
  { TK_GAS_DIS_EXP, TKS_GAS_DIS_EXP },
  { TK_WTEMP, TKS_WTEMP },		
  { TK_FLOW_PROP, TKS_FLOW_PROP },		
  { TK_UFREE, TKS_UFREE },		
  { TK_LENGTH_WEIGHT_B0, TKS_LENGTH_WEIGHT_B0 },
  { TK_LENGTH_WEIGHT_B1, TKS_LENGTH_WEIGHT_B1 },
  { TK_PREY_ENERGY_DENSITY, TKS_PREY_ENERGY_DENSITY },
  { TK_FORK_THRESHOLD, TKS_FORK_THRESHOLD },	
  { TK_RUN_FLAGS, TKS_RUN_FLAGS },		
  { TK_REACH_CLASS_NAME, TKS_REACH_CLASS_NAME },
  { TK_NUM_REACH_CLASSES, TKS_NUM_REACH_CLASSES },	
  { TK_MORTALITY_CLASS, TKS_MORTALITY_CLASS },		
  { TK_POST_BONNEVILLE_HYPOTHESIS, TKS_POST_BONNEVILLE_HYPOTHESIS },		
  { TK_FLOW_SEG, TKS_FLOW_SEG },
  { TK_WATER_TT_UPPER, TKS_WATER_TT_UPPER }, 
  { TK_WATER_TT_LOWER, TKS_WATER_TT_LOWER }, 
  { TK_WATER_TT_FIRST_DAY, TKS_WATER_TT_FIRST_DAY }, 
  { TK_WATER_TT_LAST_DAY, TKS_WATER_TT_LAST_DAY }, 
  { TK_INCLUDE, TKS_INCLUDE},		
  { TK_END, TKS_END },			
  { 0, "\0" }
};
#endif

static command_struct release_tokens []  =
{
    { TK_RLS_VIT, TKS_RLS_VIT },
	{ TK_RLS_ADD, TKS_RLS_ADD },
    { TK_RLS_VIT_CHANGE, TKS_RLS_VIT_CHANGE },
    { TK_INITIAL_SPILL_EXPERIENCE, TKS_INITIAL_SPILL_EXPERIENCE },
    { TK_RLS_AGE, TKS_RLS_AGE },
    { TK_RLS_SMOLT_ON, TKS_RLS_SMOLT_ON },
    { TK_RLS_SMOLT_OFF, TKS_RLS_SMOLT_OFF },
    { TK_RLS_NUM, TKS_RLS_NUM },
    { TK_RLS_LENGTH, TKS_RLS_LENGTH },
    { TK_STOCK, TKS_STOCK },
    { TK_RT_STOCK, TKS_RT_STOCK },
    { TK_RT_DATA_DATE, TKS_RT_DATA_DATE },
    { TK_INCLUDE, TKS_INCLUDE},
    { TK_END, TKS_END},
    { 0, "\0" }
};

static command_struct reach_tokens []  =
{
    { TK_RCH_PRED_MEAN, TKS_RCH_PRED_MEAN },
    { TK_RCH_PRED_MEAN_V15, TKS_RCH_PRED_MEAN_V15 },
    { TK_RCH_PRED_MEAN_V16, TKS_RCH_PRED_MEAN_V16 },
    { TK_RCH_PRED_DIST, TKS_RCH_PRED_DIST },
    { TK_RCH_PRED_VAR, TKS_RCH_PRED_VAR },
    { TK_RCH_LOSS, TKS_RCH_LOSS },
    { TK_RCH_LOSS_MAX, TKS_RCH_LOSS_MAX },
    { TK_RCH_LOSS_MIN, TKS_RCH_LOSS_MIN },
    { TK_RCH_VELOCITY, TKS_RCH_VELOCITY },
    { TK_RCH_FLOW_MAX, TKS_RCH_FLOW_MAX },
    { TK_RCH_FLOW, TKS_RCH_FLOW },
    { TK_RCH_ELEVATION, TKS_RCH_ELEVATION },
    { TK_DELTA_TEMP, TKS_DELTA_TEMP },
    { TK_RCH_MONTE_OUT, TKS_RCH_MONTE_OUT },
    { TK_OUTPUT_SETTINGS, TKS_OUTPUT_SETTINGS },
    { TK_REACH_CLASS, TKS_REACH_CLASS },
    { TK_RCH_GAS_THETA, TKS_RCH_GAS_THETA },
    { TK_OUTPUT_GAS, TKS_OUTPUT_GAS },
	{ TK_INPUT_TEMP, TKS_INPUT_TEMP },
	{ TK_INPUT_TURB, TKS_INPUT_TURB },
    { TK_INCLUDE, TKS_INCLUDE},
    { TK_END, TKS_END },
    { 0, "\0" }
};

static command_struct hdwtr_tokens []  =
{
    { TK_HW_FLOW_COEF, TKS_HW_FLOW_COEF },
    { TK_HW_FLOW_MAX, TKS_HW_FLOW_MAX },
    { TK_HW_FLOW, TKS_HW_FLOW },
    { TK_INCLUDE, TKS_INCLUDE},
    { TK_HW_WTEMP, TKS_HW_WTEMP },
    { TK_HW_FLOW_MEAN, TKS_HW_FLOW_MEAN },
    { TK_HW_MONTE_OUT, TKS_HW_MONTE_OUT },
    { TK_OUTPUT_SETTINGS, TKS_OUTPUT_SETTINGS },
    { TK_OUTPUT_GAS, TKS_OUTPUT_GAS },
	{ TK_INPUT_TURB, TKS_INPUT_TURB },
    { TK_END, TKS_END },
    { 0, "\0" }
};

static command_struct dam_tokens []  =
{
    { TK_SPL_MRT, TKS_SPL_MRT },
    { TK_SPL_MRT_MN, TKS_SPL_MRT_MN },
    { TK_SPL_MRT_VAR, TKS_SPL_MRT_VAR },
    { TK_SPL_MRT_LOW, TKS_SPL_MRT_LOW },
    { TK_SPL_MRT_HIGH, TKS_SPL_MRT_HIGH },
    { TK_BY_MRT, TKS_BY_MRT },
    { TK_BY_MRT_MN, TKS_BY_MRT_MN },
    { TK_BY_MRT_VAR, TKS_BY_MRT_VAR },
    { TK_BY_MRT_LOW, TKS_BY_MRT_LOW },
    { TK_BY_MRT_HIGH, TKS_BY_MRT_HIGH },
    { TK_TURB_MRT, TKS_TURB_MRT },
    { TK_TURB_MRT_MN, TKS_TURB_MRT_MN },
    { TK_TURB_MRT_VAR, TKS_TURB_MRT_VAR },
    { TK_TURB_MRT_LOW, TKS_TURB_MRT_LOW },
    { TK_TURB_MRT_HIGH, TKS_TURB_MRT_HIGH },
    { TK_TRNS_MRT, TKS_TRNS_MRT },
    { TK_TRNS_MRT_MN, TKS_TRNS_MRT_MN },
    { TK_TRNS_MRT_VAR, TKS_TRNS_MRT_VAR },
    { TK_TRNS_MRT_LOW, TKS_TRNS_MRT_LOW },
    { TK_TRNS_MRT_HIGH, TKS_TRNS_MRT_HIGH },
    { TK_MEAN_FOREBAY_TRANSIT_TIME, TKS_MEAN_FOREBAY_TRANSIT_TIME },
    { TK_SEP_PROB, TKS_SEP_PROB },
    { TK_DAM_PRED, TKS_DAM_PRED },
    { TK_DAM_PRED_PROB, TKS_DAM_PRED_PROB },
    { TK_DAM_PRED_DENS, TKS_DAM_PRED_DENS },
    { TK_SPL_EQN_CLASS, TKS_SPL_EQN_CLASS },
    { TK_SPL_EQN_ERFN, TKS_SPL_EQN_ERFN },
    { TK_SPL_EQN_VAL, TKS_SPL_EQN_VAL },
    { TK_SPL_EQN_RANGE, TKS_SPL_EQN_RANGE },
    { TK_DAY_NPSS_MN, TKS_DAY_NPSS_MN },
    { TK_DAY_NPSS_VAR, TKS_DAY_NPSS_VAR },
    { TK_DAY_FGE, TKS_DAY_FGE },
    { TK_DAY_FGE_MN, TKS_DAY_FGE_MN },
    { TK_DAY_FGE_VAR, TKS_DAY_FGE_VAR },
    { TK_DAY_FGE_LOW, TKS_DAY_FGE_LOW },
    { TK_DAY_FGE_HIGH, TKS_DAY_FGE_HIGH },
    { TK_NT_NPSS_MN, TKS_NT_NPSS_MN },
    { TK_NT_NPSS_VAR, TKS_NT_NPSS_VAR },
    { TK_PASSAGE, TKS_PASSAGE },
    { TK_NT_FGE, TKS_NT_FGE },
    { TK_NT_FGE_MN, TKS_NT_FGE_MN },
    { TK_NT_FGE_VAR, TKS_NT_FGE_VAR },
    { TK_NT_FGE_LOW, TKS_NT_FGE_LOW },
    { TK_NT_FGE_HIGH, TKS_NT_FGE_HIGH },
    { TK_DAM_SPEC, TKS_DAM_SPEC },
    { TK_DAM_SPILL, TKS_DAM_SPILL },
    { TK_DAY_SPILL, TKS_DAY_SPILL },
    { TK_NT_SPILL, TKS_NT_SPILL },
    { TK_TRANSPORT, TKS_TRANSPORT },
    { TK_DAM_FLOW, TKS_DAM_FLOW },
    { TK_DAM_FLOW_MAX, TKS_DAM_FLOW_MAX },
    { TK_DAM_LOSS, TKS_DAM_LOSS },
    { TK_DAM_LOSS_MAX, TKS_DAM_LOSS_MAX },
    { TK_DAM_LOSS_MIN, TKS_DAM_LOSS_MIN },
    { TK_SLC_EQN_CLASS, TKS_SLC_EQN_CLASS },
    { TK_SLC_EQN_ERFN, TKS_SLC_EQN_ERFN },
    { TK_SLC_EQN_VAL, TKS_SLC_EQN_VAL },
    { TK_SLC_EQN_RANGE, TKS_SLC_EQN_RANGE },
    { TK_SPILL_EQUATION, TKS_SPILL_EQUATION},
    { TK_NSAT_EQUATION, TKS_NSAT_EQUATION},
    { TK_NSAT_DAY_EQUATION, TKS_NSAT_DAY_EQUATION},
    { TK_NSAT_NIGHT_EQUATION, TKS_NSAT_NIGHT_EQUATION},
    { TK_NSAT_BACKUP_EQUATION, TKS_NSAT_BACKUP_EQUATION},
    { TK_DELAY_EQUATION, TKS_DELAY_EQUATION},
    { TK_SLUICE_EQUATION, TKS_SLUICE_EQUATION},
    { TK_PLAN_SPILL, TKS_PLAN_SPILL},
    { TK_FISH_SPILL, TKS_FISH_SPILL},
    { TK_PLAN_SPILL_DAY, TKS_PLAN_SPILL_DAY},
    { TK_PLAN_SPILL_NIGHT, TKS_PLAN_SPILL_NIGHT},
    { TK_PWRHS_CAP, TKS_PWRHS_CAP},
    { TK_ADDL_PHOUSE, TKS_ADDL_PHOUSE},
    { TK_PWRHS_WEIGHT, TKS_PWRHS_WEIGHT},
    { TK_PWRHS_SCHED, TKS_PWRHS_SCHED},
    { TK_PWRHS_PRIORITY, TKS_PWRHS_PRIORITY},
    { TK_STORAGE_VOLUME, TKS_STORAGE_VOLUME },
    { TK_DAM_FLOW_MIN, TKS_DAM_FLOW_MIN },
    { TK_PRED_POP_TR, TKS_PRED_POP_TR },
    { TK_TR_TIME, TKS_TR_TIME },
    { TK_TR_LENGTH, TKS_TR_LENGTH },
    { TK_FGE_EQN, TKS_FGE_EQN },
    { TK_DAM_PRED_DENS_FB, TKS_DAM_PRED_DENS_FB },
    { TK_DAM_PRED_DENS_TR, TKS_DAM_PRED_DENS_TR },
    { TK_SPILL_CAP, TKS_SPILL_CAP },
    { TK_MONTE_OUT, TKS_MONTE_OUT },
    { TK_OUTPUT_SETTINGS, TKS_OUTPUT_SETTINGS },
    { TK_DELAY_SCALAR, TKS_DELAY_SCALAR },
    { TK_TRANS_EQN, TKS_TRANS_EQN },
    { TK_DAM_GAS_THETA, TKS_DAM_GAS_THETA },
    { TK_K_ENTRAIN, TKS_K_ENTRAIN },
    { TK_RSW_EQUATION, TKS_RSW_EQUATION },
    { TK_SPILL_RSW_PROP, TKS_SPILL_RSW_PROP },
    { TK_RSW_SPILL_CAP, TKS_RSW_SPILL_CAP },
    { TK_SPILL_DELAY, TKS_SPILL_DELAY },
    { TK_BYPASS_DELAY, TKS_BYPASS_DELAY },
    { TK_SLUICEWAY_DELAY, TKS_SLUICEWAY_DELAY },
    { TK_TURBINE_DELAY, TKS_TURBINE_DELAY },
    { TK_RSW_DELAY, TKS_RSW_DELAY },
    { TK_SPILL_DAY, TKS_SPILL_DAY },
    { TK_BYPASS_DAY, TKS_BYPASS_DAY },
    { TK_SLUICEWAY_DAY, TKS_SLUICEWAY_DAY },
    { TK_TURBINE_DAY, TKS_TURBINE_DAY },
    { TK_RSW_DAY, TKS_RSW_DAY },
    { TK_RSW_MRT, TKS_RSW_MRT },
    { TK_RSW_MRT_MN, TKS_RSW_MRT_MN },
    { TK_RSW_MRT_LOW, TKS_RSW_MRT_LOW },
    { TK_RSW_MRT_HIGH, TKS_RSW_MRT_HIGH },
    { TK_SLUICEWAY_PROP, TKS_SLUICEWAY_PROP },
    { TK_SLUICEWAY_PROP_MN, TKS_SLUICEWAY_PROP_MN },
    { TK_SLUICEWAY_PROP_LOW, TKS_SLUICEWAY_PROP_LOW },
    { TK_SLUICEWAY_PROP_HIGH, TKS_SLUICEWAY_PROP_HIGH },
    { TK_SLUICEWAY_MRT, TKS_SLUICEWAY_MRT },
    { TK_SLUICEWAY_MRT_MN, TKS_SLUICEWAY_MRT_MN },
    { TK_SLUICEWAY_MRT_LOW, TKS_SLUICEWAY_MRT_LOW },
    { TK_SLUICEWAY_MRT_HIGH, TKS_SLUICEWAY_MRT_HIGH },
    { TK_OUTPUT_GAS, TKS_OUTPUT_GAS },
	{ TK_INPUT_TEMP, TKS_INPUT_TEMP },
	{ TK_INPUT_TURB, TKS_INPUT_TURB },
    { TK_INCLUDE, TKS_INCLUDE},		
    { TK_END, TKS_END},			
    { 0, "\0" }
};

static command_struct trns_tokens []  =
{
    { TK_START_BY_DATE, TKS_START_BY_DATE },
    { TK_START_DATE,  TKS_START_DATE },
    { TK_START_COUNT, TKS_START_COUNT },
    { TK_MAX_RESTARTS, TKS_MAX_RESTARTS },
    { TK_RESTART_BY_DATE, TKS_RESTART_BY_DATE },
    { TK_RESTART_DATE, TKS_RESTART_DATE },
    { TK_RESTART_COUNT, TKS_RESTART_COUNT },
    { TK_END_DATE, TKS_END_DATE },
    { TK_END_COUNT, TKS_END_COUNT },
    { TK_NUM_LOW_DAYS, TKS_NUM_LOW_DAYS },
    { TK_HFL_PASSED, TKS_HFL_PASSED },
    { TK_HI_FLOW, TKS_HI_FLOW },
    { TK_HFL_SPEC, TKS_HFL_SPEC },
    { TK_RATE, TKS_RATE },
    { TK_RLS_PT, TKS_RLS_PT },
    { TK_INCLUDE, TKS_INCLUDE},
    { TK_END, TKS_END },
    { 0, "\0" }
};

static command_struct eq_tokens [] =
{
    { TK_PARAMETER, TKS_PARAMETER},
    { TK_INCLUDE, TKS_INCLUDE},
    { TK_END, TKS_END},
    { 0, "\0" }
};
static command_struct dist_tokens [] =
{
    { TK_DIST_BROKEN_STICK, TKS_DIST_BROKEN_STICK},
    { TK_INCLUDE, TKS_INCLUDE},
    { 0, "\0" }
};
static command_struct broken_stick_tokens [] =
{
    { TK_DIST_BS_MEAN, TKS_DIST_BS_MEAN},
    { TK_DIST_BS_LOW, TKS_DIST_BS_LOW},
    { TK_DIST_BS_HIGH, TKS_DIST_BS_HIGH},
    { TK_INCLUDE, TKS_INCLUDE},
    { TK_END, TKS_END},
    { 0, "\0" }
};

/* Default values for reach classes SHOULD NOT overwrite existing values
 * for parameters. Rather than require default values to be placed first
 * in the input file, we instead use a simple bit array to determine if
 * the parameter for a specific reach class has already been defined.
 * This ensures that default values will be filled in but will never
 * overwrite manually-defined values.
 * A bit array mask should exist for each reach class-dependent parameter
 * in a struct species or a StockT */
/* These get/set a value as overridden in a bit array mask */
/* DEFAULT_OVERRIDE should be called whenever a reach class-dependent
 * parameter is set for a reach class while parsing! */
#define DEFAULT_OVERRIDE(mask, rc) mask |= 1<<rc
#define DEFAULT_OVERRIDDEN(mask, rc) (mask & 1<<rc)
/* The actual masks are defined here */
/* Note that the masks are longs, since arrays of booleans would be
 * something of a waste */
typedef struct {
    long	reach_pcoef;  /* reach predation coefficients */
    long	pprime_a;     /* used to calculate p' for growth alg. */
    long	pprime_b;
} species_mask;
typedef struct {
    long	migration_eqn;
    long	mvcoef;		/* migration variance */
    /*long	migr_b1fac;*/ /* These aren't set in parse.c... TODO: find where they are */
    long	vvar;
    long    pred_temp_response_eqn;
	long	pred_capture_dist_eqn;
	long	custom_survival_eqn;
    long	distance_coeff;  /* "a" in "sqrt( a * x^2 + b * t^2 )" */
	long	time_coeff;      /* "b" in "sqrt( a * x^2 + b * t^2 )" */
	long	sigma_d;      /* "b" in "sqrt( a * x^2 + b * t^2 )" */
	long	reach_survival_coef;
} stock_mask;

species_mask* species_masks = NULL;
stock_mask* species_stock_masks = NULL; /* Default stock values per species */
stock_mask* stock_masks = NULL;

/* fill_default_reach_class_values() fills default reach class values into
 * other reach classes (it uses two utility functions to accomplist this)
 * and should be called once parsing is completed. Since parse_datafile is
 * only called once per model run, this function is called from there.
 *
 * Note that if parse_datafile is NOT called before a run, or is called
 * multiple times for a single run then both this function and reset_masks()
 * will not behave correctly! */
/* Just to make the code cleaner, use this macro */
#define RC_COPY_DEFAULT(param, type, ptr, mask) \
	if (!DEFAULT_OVERRIDDEN(mask->param, i)) { \
		bcopy(&ptr->param[0], &ptr->param[i], sizeof(type)); \
	} 
/* Notice that variable_parameter structures must be copied as well as values! */
#define RC_COPY_DEFAULT_AND_DIST(_param, _type, _ptr, _mask) \
	if (!DEFAULT_OVERRIDDEN(_mask->_param, i)) { \
		variable_parameter *vp; \
		bcopy(&_ptr->_param[0], &_ptr->_param[i], sizeof(_type)); \
		if (vp = get_variable_parameter(&_ptr->_param[0])) { \
			variable_parameter *vp2 = (variable_parameter*) malloc(sizeof(variable_parameter)); \
			bcopy(vp, vp2, sizeof(variable_parameter)); \
			vp2->variable = &_ptr->_param[i]; \
			add_variable_parameter(vp2); \
		} \
	}
void fill_default_stock_reach_class_values(StockT *stock, stock_mask *mask) {
	int i;
	for (i = 1; i < num_reach_classes; i++) {
		RC_COPY_DEFAULT(migration_eqn,	equation,	stock,	mask);
		RC_COPY_DEFAULT(mvcoef,	float,	stock,	mask);
		/* TODO: migr_b1fac? */
		RC_COPY_DEFAULT(vvar,	float,	stock,	mask);
		RC_COPY_DEFAULT(pred_temp_response_eqn,	equation,	stock,	mask);
		RC_COPY_DEFAULT(pred_capture_dist_eqn,	equation,	stock,	mask);
		RC_COPY_DEFAULT(custom_survival_eqn,	equation,	stock,	mask);
		RC_COPY_DEFAULT(time_coeff,	float,	stock,	mask);
		RC_COPY_DEFAULT(distance_coeff,	float,	stock,	mask);
		RC_COPY_DEFAULT(sigma_d,	float,	stock,	mask);
		RC_COPY_DEFAULT(reach_survival_coef,	float,	stock,	mask);
	}
}
void fill_default_species_reach_class_values(struct species *spec, species_mask *mask, stock_mask *smask) {
	int i;
	for (i = 1; i < num_reach_classes; i++) {
		RC_COPY_DEFAULT(reach_pcoef,	float,	spec,	mask);
		RC_COPY_DEFAULT(pprime_a,	float,	spec,	mask);
		RC_COPY_DEFAULT(pprime_b,	float,	spec,	mask);
	}

	/* Generic stock */
	fill_default_stock_reach_class_values(&spec->stock, smask);
}
void fill_default_reach_class_values() {
	int i;

	/* Copy species parameters */
	for (i=0; i < num_specs; i++) fill_default_species_reach_class_values(&specs[i], &species_masks[i], &species_stock_masks[i]);
	/* Copy stock parameters */
	for (i=0; i < stocks.num; i++) fill_default_stock_reach_class_values(stocks.s[i], &stock_masks[i]);
}

/* Initialize default value masks if they aren't already present
 * See comment about overriding default values near top */
void init_masks() {
	if (species_masks == NULL) {
		species_masks = (species_mask*) calloc(num_specs, sizeof(species_mask));
	}
	if (species_stock_masks == NULL) {
		species_stock_masks = (stock_mask*) calloc(num_specs, sizeof(stock_mask));
	}
	if (stock_masks == NULL) {
		stock_masks = (stock_mask*) calloc(stocks.num, sizeof(stock_mask));
	}
}
void reset_masks() {
	/* Reset masks if they exist */
	if (species_masks) bzero(species_masks, num_specs*sizeof(species_mask));
	if (species_stock_masks) bzero(species_stock_masks, num_specs*sizeof(stock_mask));
	if (stock_masks) bzero(stock_masks, stocks.num*sizeof(stock_mask));

	/* Create them if they don't (they are zeroed on creation */
	init_masks();
}

static Table	*comment_hash = 0;

struct banner {
    char	      **lines;
    unsigned		linecount;
    unsigned		linesize;
};

struct banner_node {
    struct banner      *comments;
    char	       *filename_key;
};

/*------------------------------------------------------------------------*/
/* ref_spec (spec_name) returns the index of the species spec_name or
   -1 if it is not a species name */
/*------------------------------------------------------------------------*/
static int ref_spec(spec_name)
    char *spec_name;
{
    static COMMAND_LIST name_list=0;
    int count=0;
   
    if (!name_list)
    {
	/* set up a list of all the fish names */
	name_list = com_create_list(name_list, 0); 
	for (count=0; count < num_specs; count++)
	    name_list = com_add_to_list(name_list,
					count+1,
					lowercase(specs[count].name,scratch1));
    }
    
    return (com_reference_full(name_list, lowercase(spec_name,scratch1)) - 1);
}

/*------------------------------------------------------------------------*/
/* ref_stock (stock_name) returns the index of the stock stock_name or
   -1 if it is not a stock name */
/*------------------------------------------------------------------------*/
static int ref_stock(stock_name)
    char *stock_name;
{
    static COMMAND_LIST name_list=0;
    int count=0;
   
    if (!name_list)
    {
	/* set up a list of all the fish names */
	name_list = com_create_list(name_list, 0); 
	for (count=0; count < stocks.num; count++)
	    name_list = com_add_to_list(name_list,
					count+1,
					lowercase(stocks.s[count]->name,
						  scratch1));
    }
    
    return (com_reference_full(name_list, lowercase(stock_name,scratch1)) - 1);
}

/*------------------------------------------------------------------------*/
/* find_site looks for the release site matching the name passed to it.
   It returns a pointer to that site or 0 if it is not found */
/*------------------------------------------------------------------------*/
static struct release_site *find_site(site_list, name)
    struct release_site *site_list;
    char *name;
{
    if (!site_list)
	return (0);
    else if ( strcmp( lowercase(site_list->name,scratch1), lowercase(name,scratch2)) == 0 )
    {
	return (site_list);
    }
    else
	return (find_site (site_list->next, name));
}

/*------------------------------------------------------------------------*/
/* find_exact_release looks for the release matching the species, site, and
   start_date passed to it.
   It returns a pointer to that site or 0 if it is not found */
/*------------------------------------------------------------------------*/
static struct release
*find_exact_release(release_list, spec_rec, site, start_date)

    struct release *release_list;
    struct species *spec_rec;
    struct release_site *site;
    int start_date;
{
    if (!release_list)
	return (0);
    else if (release_list->species == spec_rec &&
	     release_list->site == site &&
	     release_list->start_date == start_date)
	return (release_list);
    else
	return (find_exact_release (release_list->next, spec_rec, site, start_date));
}


static HashKey banner_key(banner_datum)
	struct banner_node	*banner_datum;
{
    return (HashKey) banner_datum->filename_key;
}

static void free_banner(banner)
	struct banner	*banner;
{
    if (!banner) return;

    if (banner->lines) {
	while (banner->linecount--)
	    free(banner->lines[banner->linecount]);
	free(banner->lines);
    }
    free(banner);
}

static struct banner_node *comment_lookup(filename)
	char	*filename;
{
    char	*filepart;

    if (!comment_hash) {
	comment_hash = init_table(0, 0, (HashKey (*)(TableDatum)) banner_key,
				  0);
	if (!comment_hash) return 0;
    }
    filepart = (char *)strrchr(filename, PATHSEPERATOR);
    if (!filepart)
	filepart = filename;
    else
	++filepart;
    return table_lookup(comment_hash, filepart);
}

static struct banner *open_commentary(filename)
	char	*filename;
{
    struct banner_node	       *table_entry;
    struct banner	       *file_banner;
	char *filepart;

    if ((table_entry = comment_lookup(filename)) != NULL) {
		if (!table_delete(comment_hash, table_entry))
			return 0;
		free_banner(table_entry->comments);
		free(table_entry->filename_key);
		free(table_entry);
    }
    if (!(table_entry = (struct banner_node *)
		malloc(sizeof(struct banner_node))))
	return 0;
    if (!(file_banner = (struct banner *) malloc(sizeof(struct banner)))) {
	free(table_entry);
	return 0;
    }

	/* Use filename part only as key (trim off path prefix).
	 */
	filepart = (char *)strrchr( filename, PATHSEPERATOR );
	if (filepart)
		filepart++;
	else
		filepart = filename;
    if (!(table_entry->filename_key = strdup(filepart))) {
	free(file_banner);
	free(table_entry);
	return 0;
    }
    bzero(file_banner, sizeof(struct banner));
    table_entry->comments = file_banner;
    if (!table_insert(comment_hash, table_entry)) {
	free(file_banner);
	free(table_entry->filename_key);
	free(table_entry);
	return 0;
    }
    return file_banner;
}

static void close_commentary(filename)
	char *filename;
{
    struct banner_node		*table_entry;

    if ((table_entry = comment_lookup(filename)) != NULL) {
	if (!table_entry->comments->linesize) {
	    if (!table_delete(comment_hash, table_entry))
		return;
	    if (table_entry->comments->lines)
		free(table_entry->comments->lines);
	    if (table_entry->comments)
		free(table_entry->comments);
	    if (table_entry->filename_key)
		free(table_entry->filename_key);
	    free(table_entry);
	}
    }
}

static void add_line_to_banner(banner, line)
	struct banner	       *banner;
	char		       *line;
{
    if (banner->linecount >= banner->linesize) {
	banner->linesize = banner->linesize ? banner->linesize*2 : 10;
	banner->lines = (banner->lines) ? (char **) realloc(banner->lines,
		banner->linesize * sizeof(char *))
	    : (char **) malloc(banner->linesize * sizeof(char *));
	if (!banner->lines) {
	    banner->linesize = banner->linecount = 0;
	    return;
	}
    }
    banner->lines[banner->linecount++] = strdup(line);
}

static int is_banner_comment(line)
	char		*line;
{
    if (!line || !COMMENT_TOKEN_TEST(*line)) return FALSE;
    ++line;
    if (!*line || *line == '\n')
	return FALSE;
    while (*line && *line != '\n') {
	if (*line != '=' && !COMMENT_TOKEN_TEST(*line))
	    return FALSE;
	++line;
    }
    return TRUE;
}

/* This routine checks to see that a valid version token has been seen for
 *  this buffer. This routine should be called before parsing any token
 *  which has different meaning in different versions of the data file format.
 */
static void version_check( PARSER_FILE *buffer, char *token ) {
    if (buffer->version < 0)
        p_error( buffer->fp, 
                 "No version known for this file, parsing token %s.\n",
                 token );
}


static char *get_line(buffer)
	PARSER_FILE	*buffer;
{
    char file_buffer[1024];
    unsigned int i;

    /* Pete's pop_line function corrupts the stack. */
    if (fgets(file_buffer, sizeof(file_buffer)-1, buffer->fp) == NULL)
		return 0;
	for (i = 0; i < sizeof(file_buffer)-1 && file_buffer[i] != '\0'
				&& file_buffer[i] != '\r'
				&& file_buffer[i] != '\n'; i++)
		continue;
	file_buffer[i] = '\0';
    incr_buf_line(buffer->fp);
    return strdup(file_buffer);
}

static char peek(buffer)
	PARSER_FILE	*buffer;
{
    char		peek_character;

    peek_character = getc(buffer->fp);
    ungetc(peek_character, buffer->fp);
    return peek_character;
}

void parse_comments(filename, buffer)
	char		*filename;
	PARSER_FILE	*buffer;
{
    char	       *comment_line=0;
    int			banner_count = 0;
    struct banner      *commentary;

    if ((commentary = open_commentary(filename)) == 0)
	return;
    while (COMMENT_TOKEN_TEST(peek(buffer))) {
	if (comment_line)
	    free(comment_line);
	comment_line = get_line(buffer);
	if (is_banner_comment(comment_line)) {
	    if (++banner_count > 2) {
		break;
	    }
	    else continue;
	}
	if (banner_count < 2)
	    continue;
	add_line_to_banner(commentary, &comment_line[1]);
    }
    if (comment_line)
	free(comment_line);
    close_commentary(filename);
}

void write_saved_comments(fp, filename)
	FILE	*fp;
	char	*filename;
{
    struct banner_node	       *saved_comments;
    struct banner	       *comment_lines;
    unsigned			linecount;

    if (!(saved_comments = comment_lookup(filename)))
	return;
    comment_lines = saved_comments->comments;
    for (linecount = 0 ; linecount < comment_lines->linecount ; ++linecount) {
	putc('#', fp);
	fputs(comment_lines->lines[linecount], fp);
	putc('\n', fp);
    }
    write_separator(fp);
}

/*------------------------------------------------------------------------*/
/* parse equation puts the appropriate numbers in the appropriate places in
   the equation record.  Warning messages are printed for
   syntax errors. */
static void parse_equation(buffer, eq, name, end_token)
    PARSER_FILE *buffer;
    equation *eq;
    char *name;
	const char* end_token;
{
    char token[MAX_COMMAND_LEN+1];
    static COMMAND_LIST eq_list=0;
    equation dummy_eq; /* used to recover from errors. */

    int i, id = -1, param=0;

    /* figure out which equation */
    if (!is_int(name))
    {
		int j;

		for (j=0; j < NUM_EQNS; j++) {
			if (!strcmp(name, eqn_dflt_tab[j].name)) {
				id = j;
				break;
			}
		}

		if (id < 0) {
			p_error(buffer->fp,
				 "Unknown equation name: {%s}\n",
				 name);
			reset_input(buffer->fp);
		}
    }
    else
        id = atoi(name);

    /* fill in default vals for this eqn */
    if (id < 0 || id >= NUM_EQNS) {
        p_error( buffer->fp, "Equation number must be between 0 and %d\n", NUM_EQNS-1 );
        reset_input( buffer->fp );
        id = -1;
    }
    if (id == -1) {
        p_error( buffer->fp, "Will attempt to skip rest of invalid equation.\n" );
        eq = &dummy_eq;  /* Don't disturb incoming equation contents. */
        eq->id = 0;
        eq->num_params = EQN_NPARAMS;
        eq->total_params = EQN_NPARAMS;
        for (i = 0; i < EQN_NPARAMS; i++)
            eq->param[i].min = eq->param[i].val = eq->param[i].max = 0.0;
    }
    else {
        eq->id = id;
        copy_equation(find_default_eq(eq->id), eq);    
    }
    
    /* set up the equation list */
    if (!eq_list)
    {
	eq_list = com_create_list(eq_list, eq_tokens);
    }

    while (1) {
	if (pop_token(buffer->fp, token, "equation command -> ") == EOF)
	    {
		if (can_return_from(buffer))
		{
		    buffer = return_after_parse(buffer);
		    continue;
		}
		else
		    {
			p_warning(buffer->fp, 
				  "EOF detected while parsing equation.\n");
			return;
		    }
	    }
	switch (com_reference(eq_list, lowercase(token, scratch1)))
	{
	case TK_PARAMETER:
	    if (!read_int(buffer->fp,
			      &param,
			      "parameter number ->"))
	    {
		/* if there is an error, read int will already have printed a short error mess. */
	        p_error(buffer->fp,
			"Parsing parameter # field for equation %s.  Line discarded.\n",
			name);
		reset_input(buffer->fp);
		break;
	    }

        if (param >= eq->total_params) {
            p_error( buffer->fp, 
            "equation %s only has %d parameters. Param %d discarded.\n",
                        name, eq->total_params, param );
    		reset_input(buffer->fp);
            break;
        }

	    if (!read_double(buffer->fp,
			      &eq->param[param].val,
			      "equation value ->"))
	    {
		/* if there is an error, read int will already have printed a short error mess. */
	        p_error(buffer->fp,
			"Parsing value field for equation %s. Line discarded.\n",
			name);
		reset_input(buffer->fp);
		break;
	    }		

	    if (pop_token(buffer->fp, token, "equation minimum ->")==EOF)
		return;

	    if (!is_float(token))
	    {
		push_last_token(buffer->fp);
		break;
	    }
	    eq->param[param].min = strtod(token, NULL);

	    if (!read_double(buffer->fp,
			      &eq->param[param].max,
                  "maximum value ->")) {
    		/* if there is an error, read int will already have printed a short error mess. */
	        p_error(buffer->fp,
			"Parsing float for equation %s for value field.\n",
			name);
            break;
        }

        if (eq->param[param].min > eq->param[param].val ||
                    eq->param[param].val > eq->param[param].max) {

            /* Here min <= val <= max has been violated. Get default
             *  min and max and expand them, if necessary, to cover actual
             *  value.
             */
            equation *defeq;
            defeq = find_default_eq(eq->id);
            eq->param[param].min = defeq->param[param].min;
            eq->param[param].max = defeq->param[param].max;
            if (eq->param[param].min > eq->param[param].val)
                eq->param[param].min = eq->param[param].val;
            if (eq->param[param].max < eq->param[param].val)
                eq->param[param].max = eq->param[param].val;
            p_error( buffer->fp,
            "equation %s, param %d, min<=val<=max was violated.\n",
                    name, param );
        }

	    break;
	case TK_INCLUDE:
	    buffer = include_to_parse(buffer);
	    break;
	case TK_END:
		{
			char t[MAX_COMMAND_LEN+1];
			if (pop_token(buffer->fp, t, "ending command ->") == EOF) {
				p_error(buffer->fp, "Unexpected EOF parsing end token\n");
				return;
			}
			if (!strcmp(t, end_token)) {
				reset_input(buffer->fp);
				return;
			}
			else {
				reset_input(buffer->fp);
				p_error(buffer->fp, "End token found in %s for %s, ignoring line\n", end_token, t);
			}
		}
		break;
	default:
	    UNKNOWN_COMMAND_ERROR (buffer->fp, token);
	}
	    
    }
}

/*------------------------------------------------------------------------*/
/* parse_broken_stick parses a broken stick distribution for a variable
 * parameter as defined in var.h. */
static void parse_broken_stick(buffer, variable)
    PARSER_FILE *buffer;
    float *variable;
{
	static const char *end_token = "broken_stick";
    char token[MAX_COMMAND_LEN+1];
    static COMMAND_LIST broken_stick_list=0;
	float mean = 0.5;
	float low = 0.0;
	float high = 1.0;
    
    /* set up the equation list */
    if (!broken_stick_list)
    {
	broken_stick_list = com_create_list(broken_stick_list, broken_stick_tokens);
    }

    while (1) {
	if (pop_token(buffer->fp, token, "broken stick command -> ") == EOF)
	    {
		if (can_return_from(buffer))
		{
		    buffer = return_after_parse(buffer);
		    continue;
		}
		else
		    {
			p_warning(buffer->fp, 
				  "EOF detected while parsing broken stick distribution.\n");
			return;
		    }
	    }
	switch (com_reference(broken_stick_list, lowercase(token, scratch1)))
	{
	case TK_DIST_BS_MEAN:
	    if (!read_float(buffer->fp,
			      &mean,
			      "mean value ->"))
	    {
		/* if there is an error, read int will already have printed a short error mess. */
	        p_error(buffer->fp,
			"Parsing mean field for distribution. Line discarded.\n");
		reset_input(buffer->fp);
	    }	
	    break;
	case TK_DIST_BS_LOW:
	    if (!read_float(buffer->fp,
			      &low,
			      "low value ->"))
	    {
		/* if there is an error, read int will already have printed a short error mess. */
	        p_error(buffer->fp,
			"Parsing mean field for distribution. Line discarded.\n");
		reset_input(buffer->fp);
	    }	
	    break;
	case TK_DIST_BS_HIGH:
	    if (!read_float(buffer->fp,
			      &high,
			      "high value ->"))
	    {
		/* if there is an error, read int will already have printed a short error mess. */
	        p_error(buffer->fp,
			"Parsing mean field for distribution. Line discarded.\n");
		reset_input(buffer->fp);
	    }	
	    break;
	case TK_INCLUDE:
	    buffer = include_to_parse(buffer);
	    break;
	case TK_END:
		{
			char t[MAX_COMMAND_LEN+1];
			if (pop_token(buffer->fp, t, "ending command ->") == EOF) {
				p_error(buffer->fp, "Unexpected EOF parsing end token\n");
				return;
			}
			if (!strcmp(t, end_token)) {
				reset_input(buffer->fp);

				/* Add/replace the broken stick distribution */
				add_broken_stick_parameter(variable, mean, low, high);

				return;
			}
			else {
				reset_input(buffer->fp);
				p_error(buffer->fp, "End token found in %s for %s, ignoring line\n", end_token, t);
			}
		}
		break;
	default:
	    UNKNOWN_COMMAND_ERROR (buffer->fp, token);
	}
	    
    }
}
/*------------------------------------------------------------------------*/
/* parse_float_or_dist is for handling generic floating point parameters
 * that may be either static values or distributions, according to the
 * input file. */
static int parse_float_or_dist(buffer, variable, prompt)
    PARSER_FILE *buffer;
    float *variable;
	char *prompt;
{
    char token[MAX_COMMAND_LEN+1];
    static COMMAND_LIST dist_list=0;
    
    /* set up the equation list */
    if (!dist_list)
    {
	dist_list = com_create_list(dist_list, dist_tokens);
    }

	if (pop_token(buffer->fp, token, "float or distribution command -> ") == EOF)
	    {
		if (can_return_from(buffer))
		{
		    buffer = return_after_parse(buffer);
			return 0;
		}
		else
		    {
			p_warning(buffer->fp, 
				  "EOF detected while parsing float or distribution.\n");
			return 0;
		    }
	    }

	switch (com_reference(dist_list, lowercase(token, scratch1)))
	{
	case TK_DIST_BROKEN_STICK:
		parse_broken_stick(buffer, variable);
		break;
	case TK_INCLUDE:
	    buffer = include_to_parse(buffer);
	    break;
	default:
		if (is_float(token)) {
			variable_parameter *vp;

			push_last_token(buffer->fp);
			read_float(buffer->fp, variable, prompt);

			/* remove variable parameter since this is static */
			if ((vp = get_variable_parameter(variable)) != NULL) {
				remove_variable_parameter(vp);
			}
			break;
		}
		else UNKNOWN_COMMAND_ERROR (buffer->fp, token);
	}

	return 1;
}

/* Macros for making parsing a bit cleaner */

/* Macro for parsing a float */
#define PARSE_FLOAT(var, prompt, error_message, error_message_parameter) \
{\
	    if (!read_float(buffer->fp, &var, prompt)) \
		P_ERROR(error_message, error_message_parameter);\
}
/* Macro for parsing a float or distribution */
#define PARSE_FLOAT_OR_DIST(var, prompt, error_message, error_message_parameter) \
{\
	    if (!parse_float_or_dist(buffer, &var, prompt)) \
		P_ERROR(error_message, error_message_parameter);\
}
/* Reach class-related macros */
#define PARSE_REACH_CLASS_FLOAT(var, ptr, mask, name) \
{ \
	int j = read_reach_class(buffer, name); \
	PARSE_FLOAT((ptr)->var[j], name " ->", "Parsing float or dist. for %s.\n", name); \
	DEFAULT_OVERRIDE((mask).var, j); \
}
#define PARSE_REACH_CLASS_FLOAT_OR_DIST(var, ptr, mask, name) \
{ \
	int j = read_reach_class(buffer, name); \
	PARSE_FLOAT_OR_DIST((ptr)->var[j], name, "Parsing float or dist. for %s.\n", name); \
	DEFAULT_OVERRIDE((mask).var, j); \
}


#define PARSE_BROKEN_STICK_DIST_PARAM(var, param, prompt, error_message, error_message_param)  \
{\
	float tempf; \
	variable_parameter *dist = get_variable_parameter(&(var)); \
	if (!(dist)) (dist) = add_broken_stick_parameter(&(var), 0.5, 0.0, 1.0); \
	if (!(dist) || \
		!read_float(buffer->fp,&tempf,prompt) || (dist)->type != DIST_BROKEN_STICK) \
			P_ERROR(error_message, error_message_param);\
	else (dist)->parameters.broken_stick.param = tempf; \
}


/*------------------------------------------------------------------------*/
/* parse dam puts the appropriate numbers in the appropriate places in
   the record corresponding to dam_name.  If dam_name does not exist,
   it flushes the buffer and returns.  Warning messages are printed for
   syntax errors.

   There is no return value */
/*------------------------------------------------------------------------*/
/* this little bit of ugliness makes the rest of the program much neater */
#define PARSE_DAM_SPECIES_FLOAT(var, prompt, error_message)  \
{\
    if (pop_token(buffer->fp, token, "species -> ") != EOF) \
    {\
	if ((spec_index = ref_spec ( unders_to_spaces ( token ) )) >= 0) \
	{\
	    if (!read_float(buffer->fp,&dam->run[run_index].dam.species[spec_index].var,prompt)) \
		P_ERROR(error_message, dam_name);\
	}\
	else 	\
	{\
	    P_ERROR ("unknown species {%s}: line discarded\n", token); \
	    reset_input(buffer->fp); \
	}\
    } \
    else \
	P_WARNING ("unexpected EOF while looking for species name. dam = %s\n", dam_name);\
}
/* A macro for parsing a dam, species-specific float that may have a distribution instead */
#define PARSE_DAM_SPECIES_FLOAT_OR_DIST(var, prompt, error_message)  \
{\
    if (pop_token(buffer->fp, token, "species -> ") != EOF) \
    {\
	if ((spec_index = ref_spec ( unders_to_spaces ( token ) )) >= 0) \
	{\
		PARSE_FLOAT_OR_DIST(dam->run[run_index].dam.species[spec_index].var, prompt, error_message, dam_name); \
	}\
	else 	\
	{\
	    P_ERROR ("unknown species {%s}: line discarded\n", token); \
	    reset_input(buffer->fp); \
	}\
    } \
    else \
	P_WARNING ("unexpected EOF while looking for species name. dam = %s\n", dam_name);\
}

/* Legacy macro so that old paramters such as turbine_mort_mean or turbine_mort_low
 * may be supported in addition to the newer interface that uses a single value
 * that may optionally have a distribution (that is essentially transparent to the
 * rest of the code) */
#define PARSE_DAM_SPECIES_BROKEN_STICK_DIST(var, param, prompt, error_message)  \
{\
    if (pop_token(buffer->fp, token, "species -> ") != EOF) \
    {\
	if ((spec_index = ref_spec ( unders_to_spaces ( token ) )) >= 0) \
	{\
		PARSE_BROKEN_STICK_DIST_PARAM(dam->run[run_index].dam.species[spec_index].var, param, prompt, error_message, dam_name); \
	}\
	else 	\
	{\
	    P_ERROR ("unknown species {%s}: line discarded\n", token); \
	    reset_input(buffer->fp); \
	}\
    } \
    else \
	P_WARNING ("unexpected EOF while looking for species name. dam = %s\n", dam_name);\
}

#define PARSE_DAM_SPECIES_FLOAT_ARRAY(var, maximum, prompt, error_message) \
{\
    if (pop_token(buffer->fp, token, "species -> ") != EOF) \
	if ((spec_index = ref_spec ( unders_to_spaces ( token ) )) >= 0) \
	{\
	    if (!read_float_array(buffer->fp,\
			dam->run[run_index].dam.species[spec_index].var, \
			maximum, \
			ACT_NO_CONV, \
			1, \
			prompt))\
		P_ERROR(error_message, dam_name);\
	}\
	else \
	{\
	    P_ERROR ("unknown species {%s}: line discarded\n", token); \
	    reset_input(buffer->fp);\
	} \
    else \
	P_WARNING ("unexpected EOF while looking for species name. dam = %s\n", dam_name);\
}

#define PARSE_DAM_SPECIES_INT(var, prompt, error_message) \
{\
    if (pop_token(buffer->fp, token, "species -> ") != EOF) \
	if ((spec_index = ref_spec ( unders_to_spaces ( token ) )) >= 0)\
	{\
	    if (!read_int(buffer->fp,&dam->run[run_index].dam.species[spec_index].var,prompt)) \
		P_ERROR(error_message, dam_name);\
	}\
	else \
	{\
	    P_ERROR ("unknown species {%s}: line discarded\n", token); \
	    reset_input(buffer->fp);\
	} \
    else \
	P_WARNING ("unexpected EOF while looking for species name. dam = %s\n", dam_name);\
}

/* Same thing but for reaches.
 */
#define PARSE_REACH_SPECIES_FLOAT(var, prompt, error_message)  \
{\
    if (pop_token(buffer->fp, token, "species -> ") != EOF) \
    {\
	if ((spec_index = ref_spec ( unders_to_spaces ( token ) )) >= 0) \
	{\
	    if (!read_float(buffer->fp,&reach->run[run_index].reach.species[spec_index].var,prompt)) \
		P_ERROR(error_message, reach_name);\
	}\
	else 	\
	{\
	    P_ERROR ("unknown species {%s}: line discarded\n", token); \
	    reset_input(buffer->fp); \
	}\
    } \
    else \
	P_WARNING ("unexpected EOF while looking for species name. reach = %s\n", reach_name);\
}

#define PARSE_REACH_CLASS_BROKEN_STICK_DIST_PARAM(var, ptr, mask, param, name) \
{ \
	int j = read_reach_class(buffer, name); \
	PARSE_BROKEN_STICK_DIST_PARAM((ptr)->var[j], param, name, "Parsing float or dist. for %s.\n", name); \
	DEFAULT_OVERRIDE((mask).var, j); \
}

/* parse_dam() also handles powerhouses */

static void parse_dam (buffer, dam_name, run_index, end_token)
    PARSER_FILE *buffer;
    char *dam_name;
    int run_index;
	const char* end_token;
{
    unsigned char fake_dam = 0;
    /* this hack is so that obsolete dams can be discarded */
    /* ^..^ it can be gotten rid of by putting the find_seg
       call outside the parse_dam() routine */
    char token[MAX_COMMAND_LEN+1];
    static COMMAND_LIST dam_list=0;

    struct river_segment *dam = 0;
    int spec_index=0;
    unsigned int utemp;
    
    log_msg(L_DBGMSG, "parsing %s\n", dam_name);

    /* find the dam in question */
    if ( !(dam = find_seg_by_name(river_mouth, DAM_SEG,
				  unders_to_spaces(dam_name) ))
	 && !(dam = find_seg_by_name(river_mouth, POWERHOUSE_SEG,
				     unders_to_spaces(dam_name) )) )
    {
	P_WARNING ("%s does not exist: I will try and discard the dam data\n", dam_name);
	dam = new_river_segment (DAM_SEG);
	strcpy(dam->seg_name = (char *)malloc(strlen(dam_name)+1), dam_name);
	fake_dam = 1;
    }

    /* set up the dam list */
    if (!dam_list)
    {
	dam_list = com_create_list(dam_list, dam_tokens);
    }

    while (1) {
        if (pop_token(buffer->fp, token, "dam command -> ") == EOF)
	{
            if (can_return_from(buffer))
	    {
                buffer = return_after_parse(buffer);
		continue;
	    }
            else
            {
		p_warning(buffer->fp,
			  "encountered EOF before end statement while parsing dam %s\n",
			  dam_name);
		reset_input(buffer->fp);
                return;
            }
        }

	switch (com_reference(dam_list, lowercase(token, scratch1)))
	{
	    /* dam species tokens */
	case TK_DAM_PRED_DENS:
	    p_warning(buffer->fp,
		      "obsolete token {%s}.  Parsing as {%s}.\n",
		      TKS_DAM_PRED_DENS, TKS_DAM_PRED_DENS_FB);
        /* Fall through intentionally */

    case TK_DAM_PRED_DENS_FB:
        if (buffer->version <= 6) {
            float tempf;
	        if (!read_float(buffer->fp, &tempf, "dam pred dens fb ->"))
            {
	            p_error(buffer->fp,
                    "Parsing float for dam predator forebay density for dam %s.\n",
			        dam_name);
            }
            else {
                int k;
                p_warning( buffer->fp, "token {%s} being applied for all species at %s\n",
                    TKS_DAM_PRED_DENS_FB, dam_name );
                for (k = 0; k < num_specs; k++)
                    dam->run[run_index].dam.species[k].pred_pop_mean = tempf;
            }
        }
        else {
    	    PARSE_DAM_SPECIES_FLOAT (pred_pop_mean,
				     "dam pred dens fb ->",
				     "Parsing float for dam predator forebay density for dam %s.\n");
        }
	    break;
	case TK_PRED_POP_TR:
	    p_warning(buffer->fp,
		      "obsolete token {%s}.  Parsing as {%s}.\n",
		      TKS_PRED_POP_TR, TKS_DAM_PRED_DENS_TR);
        /* Fall through intentionally. */

	case TK_DAM_PRED_DENS_TR:
        if (buffer->version <= 6) {
            float tempf;
	        if (!read_float(buffer->fp, &tempf, "dam pred dens tr ->"))
            {
	            p_error(buffer->fp,
                    "Parsing float for dam predator tailrace density for dam %s.\n",
			        dam_name);
            }
            else {
                int k;
                p_warning( buffer->fp, "token {%s} being applied for all species at %s\n",
                    TKS_DAM_PRED_DENS_TR, dam_name );
                for (k = 0; k < num_specs; k++)
                    dam->run[run_index].dam.species[k].pred_pop_tailrace = tempf;
            }
        }
        else {
    	    PARSE_DAM_SPECIES_FLOAT (pred_pop_tailrace,
				     "dam pred dens tr ->",
				     "Parsing float for dam predator tailrace density for dam %s.\n");
        }
	    break;
    case TK_MEAN_FOREBAY_TRANSIT_TIME:
	    PARSE_DAM_SPECIES_FLOAT (mean_forebay_transit_time,
				     "mean forebay transit time ->",
				     "Parsing float for mean forebay transit time for dam %s.\n");
	    break;
	case TK_SPL_MRT:
	    PARSE_DAM_SPECIES_FLOAT_OR_DIST (spill_mort,
				     "spill mort. ->",
				     "Parsing float for spill mort. for dam %s.\n");
	    break;
	case TK_SPL_MRT_MN:
	    PARSE_DAM_SPECIES_BROKEN_STICK_DIST (spill_mort, mean,
				     "spill mort. mean ->",
				     "Parsing float for spill mort. mean for dam %s.\n");
	    break;
	case TK_SPL_MRT_LOW:
	    PARSE_DAM_SPECIES_BROKEN_STICK_DIST (spill_mort, low,
				     "spill mort. low ->",
				     "Parsing float for spill mort. low for dam %s.\n");
	    break;
	case TK_SPL_MRT_HIGH:
	    PARSE_DAM_SPECIES_BROKEN_STICK_DIST (spill_mort, high,
				     "spill mort. high ->",
				     "Parsing float for spill mort. high for dam %s.\n");
	    break;

	case TK_SLUICEWAY_PROP:
	    PARSE_DAM_SPECIES_FLOAT_OR_DIST (sluiceway_prop,
				     "sluiceway prop ->",
				     "Parsing float for sluiceway prop for dam %s.\n");
	    break;
	case TK_SLUICEWAY_PROP_MN:
	    PARSE_DAM_SPECIES_BROKEN_STICK_DIST (sluiceway_prop, mean,
				     "sluiceway prop mean ->",
				     "Parsing float for sluiceway prop for dam %s.\n");
	    break;
	case TK_SLUICEWAY_PROP_LOW:
	    PARSE_DAM_SPECIES_BROKEN_STICK_DIST (sluiceway_prop, low,
				     "sluiceway prop low ->",
				     "Parsing float for sluiceway prop for dam %s.\n");
	    break;
	case TK_SLUICEWAY_PROP_HIGH:
	    PARSE_DAM_SPECIES_BROKEN_STICK_DIST (sluiceway_prop, high,
				     "sluiceway prop high ->",
				     "Parsing float for sluiceway prop for dam %s.\n");
	    break;


	case TK_SLUICEWAY_MRT:
	    PARSE_DAM_SPECIES_FLOAT_OR_DIST (sluiceway_mort,
				     "sluiceway mort. ->",
				     "Parsing float for sluiceway mort. for dam %s.\n");
	    break;
	case TK_SLUICEWAY_MRT_MN:
	    PARSE_DAM_SPECIES_BROKEN_STICK_DIST (sluiceway_mort, mean,
				     "sluiceway mort. mean ->",
				     "Parsing float for sluiceway mort. mean for dam %s.\n");
	    break;
	case TK_SLUICEWAY_MRT_LOW:
	    PARSE_DAM_SPECIES_BROKEN_STICK_DIST (sluiceway_mort, low,
				     "sluiceway mort. low ->",
				     "Parsing float for sluiceway mort. low for dam %s.\n");
	    break;
	case TK_SLUICEWAY_MRT_HIGH:
	    PARSE_DAM_SPECIES_BROKEN_STICK_DIST (sluiceway_mort, high,
				     "sluiceway mort. high ->",
				     "Parsing float for sluiceway mort. high for dam %s.\n");
	    break;
	case TK_BY_MRT:
	    PARSE_DAM_SPECIES_FLOAT_OR_DIST (bypass_mort,
				     "bypass mort. ->",
				     "Parsing float for bypass mort. for dam %s.\n");
	    break;
	case TK_BY_MRT_MN:
	    PARSE_DAM_SPECIES_BROKEN_STICK_DIST (bypass_mort, mean,
				     "bypass mort. mean ->",
				     "Parsing float for bypass mort. mean for dam %s.\n");
	    break;
	case TK_BY_MRT_LOW:
	    PARSE_DAM_SPECIES_BROKEN_STICK_DIST (bypass_mort, low,
				     "bypass mort. low. ->",
				     "Parsing float for bypass mort. low for dam %s.\n");
	    break;
	case TK_BY_MRT_HIGH:
	    PARSE_DAM_SPECIES_BROKEN_STICK_DIST (bypass_mort, high,
				     "bypass mort. high. ->",
				     "Parsing float for bypass mort. high for dam %s.\n");
	    break;
	case TK_TURB_MRT:
	    PARSE_DAM_SPECIES_FLOAT_OR_DIST (turbine_mort,
				     "turbine mort. ->",
				     "Parsing float for turbine mort. for dam %s.\n");
	    break;
	case TK_TURB_MRT_MN:
	    PARSE_DAM_SPECIES_BROKEN_STICK_DIST (turbine_mort, mean,
				     "turbine mort. mean ->",
				     "Parsing float for turbine mort. mean for dam %s.\n");
	    break;
	case TK_TURB_MRT_LOW:
	    PARSE_DAM_SPECIES_BROKEN_STICK_DIST (turbine_mort, low,
				     "turbine mort. low ->",
				     "Parsing float for turbine mort. low for dam %s.\n");
	    break;
	case TK_TURB_MRT_HIGH:
	    PARSE_DAM_SPECIES_BROKEN_STICK_DIST (turbine_mort, high,
				     "turbine mort. high ->",
				     "Parsing float for turbine mort. high for dam %s.\n");
	    break;
	case TK_TRNS_MRT:
	    PARSE_DAM_SPECIES_FLOAT_OR_DIST (transport_mort,
				     "transport mort. ->",
				     "Parsing float for transport mort. for dam %s.\n");
	    break;
	case TK_TRNS_MRT_MN:
	    PARSE_DAM_SPECIES_BROKEN_STICK_DIST (transport_mort, mean,
				     "transport mort. mean ->",
				     "Parsing float for transportation mort. mean for dam %s.\n");
	    break;
	case TK_TRNS_MRT_LOW:
	    PARSE_DAM_SPECIES_BROKEN_STICK_DIST (transport_mort, low,
				     "transport mort. low ->",
				     "Parsing float for transportation mort. low for dam %s.\n");
	    break;
	case TK_TRNS_MRT_HIGH:
	    PARSE_DAM_SPECIES_BROKEN_STICK_DIST (transport_mort, high,
				     "transport mort. high ->",
				     "Parsing float for transportation mort. high for dam %s.\n");
	    break;
	case TK_SEP_PROB:
	    PARSE_DAM_SPECIES_FLOAT (separation_prob,
				     "separation prob. ->",
				     "Parsing float for separation probability for dam %s.\n");
	    break;
	case TK_DAM_PRED:
	    p_warning(buffer->fp,
		      "token {%s} has been renamed to {%s}.  Parsing.\n",
		      TKS_DAM_PRED, TKS_DAM_PRED_PROB);   
	    /* no break; fall through to parse as renamed token */
	case TK_DAM_PRED_PROB:
	    PARSE_DAM_SPECIES_FLOAT_ARRAY (pred,
					   DAM_TIME_SLC,
					   "predation probability->",
					   "Parsing float array for predation probability for dam %s\n");
	    break;
	case TK_DAY_FGE:
	    PARSE_DAM_SPECIES_FLOAT_OR_DIST (day_fge,
				     "day fge ->",
				     "Parsing float for day fge for dam %s.\n");
	    break;
	case TK_DAY_FGE_MN:
	    PARSE_DAM_SPECIES_BROKEN_STICK_DIST (day_fge, mean,
				     "daytime fge mean ->",
				     "Parsing float for daytime fge mean for dam %s.\n");
	    break;
	case TK_DAY_FGE_LOW:
	    PARSE_DAM_SPECIES_BROKEN_STICK_DIST (day_fge, low,
				     "daytime fge low ->",
				     "Parsing float for daytime fge low for dam %s.\n");
	    break;
	case TK_DAY_FGE_HIGH:
	    PARSE_DAM_SPECIES_BROKEN_STICK_DIST (day_fge, high,
				     "daytime fge high ->",
				     "Parsing float for daytime fge high for dam %s.\n");
	    break;
	case TK_NT_FGE:
	    PARSE_DAM_SPECIES_FLOAT_OR_DIST (night_fge,
				     "night fge ->",
				     "Parsing float for night fge for dam %s.\n");
	    break;
	case TK_NT_FGE_MN:
	    PARSE_DAM_SPECIES_BROKEN_STICK_DIST (night_fge, mean,
				     "night fge mean ->",
				     "Parsing float for night fge mean for dam %s.\n");
	    break;
	case TK_NT_FGE_LOW:
	    PARSE_DAM_SPECIES_BROKEN_STICK_DIST (night_fge, low,
				     "night fge low ->",
				     "Parsing float for night fge low for dam %s.\n");
	    break;
	case TK_NT_FGE_HIGH:
	    PARSE_DAM_SPECIES_BROKEN_STICK_DIST (night_fge, high,
				     "night fge high ->",
				     "Parsing float for night fge high for dam %s.\n");
	    break;
	case TK_DAM_SPEC:
	    break;
	case TK_SPILL_EQUATION:
        version_check( buffer, TKS_SPILL_EQUATION );
	    if (pop_token(buffer->fp, token, "species -> ") != EOF) 
	    {
		if ((spec_index = ref_spec ( unders_to_spaces ( token ) )) >= 0) 
		{
		    if (pop_token(buffer->fp, token, "equation id ->") == EOF)
		    {
		        p_warning(buffer->fp,
				  "unexpected EOF while looking for equation id. dam = %s\n",
				  dam_name);
			break;
		    }
		    /* EQ__CUBX equation ids have changed */
		    if (buffer->version < 5) {
			if (is_int(token)) {
			    int id = atoi(token);
			    if (id == 30) {
				sprintf(token, "%d", EQ_CUB0);
				p_warning(buffer->fp, "spill cubic eqn number has changed.  Updating from %d to %d.\n", id, EQ_CUB0);
			    }
			    if (id == 31) {
				sprintf(token, "%d", EQ_CUB1);
				p_warning(buffer->fp, "spill cubic eqn number has changed.  Updating from %d to %d.\n", id, EQ_CUB1);
			    }
			}
		    }
		    parse_equation(buffer,
				   &dam->run[run_index].dam.species[spec_index].spill_eqn,
				   token, TKS_SPILL_EQUATION);
		}
		else
		{
		    equation tmp_eqn;

		    p_warning(buffer->fp,
			      "unknown species {%s} in eqn: skipping\n",
			      token); 

		    if (pop_token(buffer->fp, token, "equation id ->") == EOF)
		    {
		        p_warning(buffer->fp,
				  "unexpected EOF while looking for equation id. dam = %s\n",
				  dam_name);
			break;
		    }
		    parse_equation(buffer, &tmp_eqn, token, TKS_SPILL_EQUATION);
		}
	    } 
	    else 
	        p_warning(buffer->fp,
			  "unexpected EOF while looking for species name. dam = %s\n",
			  dam_name);
	    break;
	case TK_TRANS_EQN:
	    if (pop_token(buffer->fp, token, "species -> ") != EOF) 
	    {
		if ((spec_index = ref_spec ( unders_to_spaces ( token ) )) >= 0) 
		{
		    if (pop_token(buffer->fp, token, "equation id ->") == EOF)
		    {
		        p_warning(buffer->fp,
				  "unexpected EOF while looking for equation id. dam = %s\n",
				  dam_name);
			break;
		    }
		    parse_equation(buffer,
				   &dam->run[run_index].dam.species[spec_index].trans_eqn,
				   token, TKS_TRANS_EQN);
		}
		else  
		{
		    equation tmp_eqn;
		    
		    p_warning(buffer->fp,
			      "unknown species {%s} in eqn: skipping\n",
			      token); 
		    
		    if (pop_token(buffer->fp, token, "equation id ->") == EOF)
		    {
		        p_warning(buffer->fp,
				  "unexpected EOF while looking for equation id. dam = %s\n",
				  dam_name);
			break;
		    }
		    parse_equation(buffer, &tmp_eqn, token, TKS_TRANS_EQN);
		}
	    } 
	    else 
	        p_warning(buffer->fp,
			  "unexpected EOF while looking for species name. dam = %s\n",
			  dam_name);
	    break;
	case TK_FGE_EQN:
	    if (pop_token(buffer->fp, token, "species -> ") != EOF) 
	    {
		if ((spec_index = ref_spec ( unders_to_spaces ( token ) )) >= 0) 
		{
		    if (pop_token(buffer->fp, token, "equation id ->") == EOF)
		    {
		        p_warning(buffer->fp,
				  "unexpected EOF while looking for equation id. dam = %s\n",
				  dam_name);
			break;
		    }
		    parse_equation(buffer,
				   &dam->run[run_index].dam.species[spec_index].fge_eqn,
				   token, TKS_FGE_EQN);
		}
		else  
		{

		    equation tmp_eqn;
		    p_warning(buffer->fp,
			      "unknown species {%s} in eqn: skipping\n",
			      token); 
		    
		    if (pop_token(buffer->fp, token, "equation id ->") == EOF)
		    {
		        p_warning(buffer->fp,
				  "unexpected EOF while looking for equation id. dam = %s\n",
				  dam_name);
			break;
		    }
		    parse_equation(buffer, &tmp_eqn, token, TKS_FGE_EQN);
		}
	    } 
	    else 
	        p_warning(buffer->fp,
			  "unexpected EOF while looking for species name. dam = %s\n",
			  dam_name);
	    break;

	case TK_DELAY_EQUATION:
	{
	    equation tmp_eqn;

        version_check( buffer, TKS_DELAY_EQUATION );

	    /* delay equation resurrected in version 5 */
	    if (buffer->version < 5) {
		p_warning(buffer->fp,
			  "obsolete token {%s}.  Data discarded.\n",
			  TKS_DELAY_EQUATION);
	    }
	    
	    if (pop_token(buffer->fp, token, "species -> ") != EOF) 
	    {
		if ((spec_index = ref_spec ( unders_to_spaces ( token ) )) >= 0) 
		{
		    equation* eqn = (buffer->version < 5) ? &tmp_eqn
			: &dam->run[run_index].dam.species[spec_index].delay_eqn;
		    if (pop_token(buffer->fp, token, "equation id ->") == EOF)
		    {
		        p_warning(buffer->fp,
				  "unexpected EOF while looking for equation id.\n", 0);
			break;
		    }
		    parse_equation(buffer, eqn, token, TKS_DELAY_EQUATION);

            /* Special hack: version 5 data files had the correct dam
             *  delay equation, except that we want to update the Aday and
             *  Anight parameter upper limits.
             */
            if (buffer->version == 5) {
                equation *tmp_eqn = find_default_eq(eqn->id);
                eqn->param[0].max = tmp_eqn->param[0].max;
                eqn->param[1].max = tmp_eqn->param[1].max;
            }
		}
		else  
		{
		    /* older version delay eqn had no species */
		    if (buffer->version >= 5) {
			p_warning(buffer->fp,
				  "unknown species {%s} in eqn: skipping\n",
				  token); 

			if (pop_token(buffer->fp, token, "equation id ->") == EOF)
			{
			    p_warning(buffer->fp,
				      "unexpected EOF while looking for equation id. dam = %s\n",
				      dam_name);
			    break;
			}
		    }
		    parse_equation(buffer, &tmp_eqn, token, TKS_DELAY_EQUATION);
		    break;
		}
	    } 
	    else 
	        p_warning(buffer->fp,
			  "unexpected EOF while looking for species name. dam = %s\n",
			  dam_name);
	    break;
	}
	/* non species dependent tokens */
	case TK_TR_LENGTH:
	    if (!read_float(buffer->fp,
			    &dam->run[run_index].dam.tailrace_length,
			    "dam tr length ->"))
	        p_error(buffer->fp,
			"Parsing float for tailrace length for dam %s.\n",
			dam_name);
	    break;
	case TK_SPILL_CAP:
	    if (!read_float(buffer->fp,
			    &dam->run[run_index].dam.spill_cap,
			    "dam spill cap ->"))
	        p_error(buffer->fp,
			"Parsing float for spill cap for dam %s.\n",
			dam_name);
	    break;
	case TK_DELAY_SCALAR:
	    p_warning(buffer->fp,
		      "obsolete token {%s}.  Data discarded.\n",
		      TKS_DELAY_SCALAR);
		(void) skip_numbers(buffer->fp, 1);
	    break;
	case TK_PWRHS_CAP:
	    if (!read_float(buffer->fp,
			    &dam->run[run_index].dam.powerhouse_capacity,
			    "dam pwrhs capacity ->"))
	        p_error(buffer->fp,
			"Parsing float for powerhouse capacity for %s.\n",
			dam_name);
	    break;
	case TK_PWRHS_PRIORITY:
	    if (!read_int(buffer->fp,
			  &dam->run[run_index].dam.phouse_priority,
			  "dam pwrhs priority ->"))
	        p_error(buffer->fp,
			"Parsing float for powerhouse priority for %s.\n",
			dam_name);
	    break;
	case TK_PWRHS_SCHED:
	    if (dam->run[run_index].dam.phouse_schedule)
		destroy_period_struct(dam->run[run_index].dam.phouse_schedule);
	    dam->run[run_index].dam.phouse_schedule=parse_periods(buffer->fp);
	    break;
	case TK_ADDL_PHOUSE:
	    if (pop_token(buffer->fp, token, "powerhouse name ->") != EOF)
		parse_dam (buffer, token, run_index, TKS_ADDL_PHOUSE);
	    else
		P_ERROR ("unexpected EOF while looking for powerhouse name in run %d\n",
			 run_index);
	    break;
	case TK_NSAT_EQUATION:
	    /* old style nsat eqn.  read into all three nsat eqns. */
	    if (pop_token(buffer->fp, token, "equation id ->") == EOF)
	    {
	        p_warning(buffer->fp,
			  "unexpected EOF while looking for equation id. dam = %s\n",
			  dam_name);
		break;
	    }
	    parse_equation(buffer, &dam->run[run_index].dam.nsat_eqn, token, TKS_NSAT_EQUATION);
	    copy_equation(&dam->run[run_index].dam.nsat_eqn,
			  &dam->run[run_index].dam.nsat_night_eqn);
	    copy_equation(&dam->run[run_index].dam.nsat_eqn,
			  &dam->run[run_index].dam.nsat_backup_eqn);
	    break;
	case TK_NSAT_DAY_EQUATION:
	    if (pop_token(buffer->fp, token, "equation id ->") == EOF)
	    {
	        p_warning(buffer->fp,
			  "unexpected EOF while looking for equation id. dam = %s\n",
			  dam_name);
		break;
	    }
	    parse_equation(buffer, &dam->run[run_index].dam.nsat_eqn, token, TKS_NSAT_DAY_EQUATION);
	    break;
	case TK_NSAT_NIGHT_EQUATION:
	    if (pop_token(buffer->fp, token, "equation id ->") == EOF)
	    {
	        p_warning(buffer->fp,
			  "unexpected EOF while looking for equation id. dam = %s\n",
			  dam_name);
		break;
	    }
	    parse_equation(buffer, &dam->run[run_index].dam.nsat_night_eqn,
			   token, TKS_NSAT_NIGHT_EQUATION);
	    break;
	case TK_NSAT_BACKUP_EQUATION:
	    if (pop_token(buffer->fp, token, "equation id ->") == EOF)
	    {
	        p_warning(buffer->fp,
			  "unexpected EOF while looking for equation id. dam = %s\n",
			  dam_name);
		break;
	    }
	    parse_equation(buffer, &dam->run[run_index].dam.nsat_backup_eqn,
			   token, TKS_NSAT_BACKUP_EQUATION);
	    break;
	case TK_TRANSPORT:
	    if (!parse_transport(buffer, dam, run_index, TKS_TRANSPORT))
	        p_error(buffer->fp,
			"Parsing transport for dam %s\n",
			dam_name);
	    break;
	case TK_DAM_FLOW:
	    if (!read_float_array(buffer->fp,
				  dam->run[run_index].dam.flow,
				  DAYS_IN_SEASON,
				  ACT_NO_CONV,
				  1,
				  "dam flow ->"))
	        p_error(buffer->fp,
			"Parsing float array for flow for dam %s\n",
			dam_name);
	    break;
	case TK_DAM_FLOW_MAX:
	    if (!read_float(buffer->fp,
			    &dam->run[run_index].dam.flow_max,
			    "dam flow max ->"))
	        p_error(buffer->fp,
			"Parsing float for dam flow maximum for dam %s.\n",
			dam_name);
	    break;
	case TK_DAM_FLOW_MIN:
	    if (!read_float(buffer->fp,
			    &dam->run[run_index].dam.flow_min_project,
			    "dam flow min ->"))
	        p_error(buffer->fp,
			"Parsing float for dam flow minimum for dam %s.\n",
			dam_name);
	    break;
	case TK_STORAGE_VOLUME:
	    if (!dam->run[run_index].dam.storage_basin) {
		p_warning(buffer->fp, "storage volume specified for dam %s%s\n",
			  dam_name, ", with no reservoir in description file");
		(void) skip_numbers(buffer->fp, DAYS_IN_SEASON);
	    } else {
		if (!read_float_array(buffer->fp,
				      dam->run[run_index].dam.storage_basin->volume,
				      DAYS_IN_SEASON,
				      ACT_NO_CONV,
				      1,
				      "storage volume ->"))
		    p_error(buffer->fp,
			    "Parsing float array for storage volume for dam %s\n",
			    dam_name);
	    }
	    break;
	case TK_PLAN_SPILL:
	    if (dam->run[run_index].dam.legacy_planned_spill)
		destroy_period_struct(dam->run[run_index].dam.legacy_planned_spill);
	    dam->run[run_index].dam.legacy_planned_spill =
		parse_periods(buffer->fp);
	    break;
	case TK_FISH_SPILL:
	    if (dam->run[run_index].dam.legacy_fish_spill)
		destroy_period_struct(dam->run[run_index].dam.legacy_fish_spill);
	    dam->run[run_index].dam.legacy_fish_spill =
		parse_periods(buffer->fp);
	    break;
	case TK_PLAN_SPILL_DAY:
	    if (dam->run[run_index].dam.planned_spill_day)
		destroy_period_struct(dam->run[run_index].dam.planned_spill_day);
	    dam->run[run_index].dam.planned_spill_day =
		parse_periods(buffer->fp);
	    break;
	case TK_PLAN_SPILL_NIGHT:
	    if (dam->run[run_index].dam.planned_spill_night)
		destroy_period_struct(dam->run[run_index].dam.planned_spill_night);
	    dam->run[run_index].dam.planned_spill_night =
		parse_periods(buffer->fp);
	    break;
	case TK_DAM_GAS_THETA:
	    if (!read_float(buffer->fp,
			    &dam->run[run_index].dam.gas_theta,
			    "dam gas theta ->"))
	        p_error(buffer->fp,
			"Parsing float for dam gas theta for dam %s.\n",
			dam_name);
	    break;

	case TK_K_ENTRAIN:
	    if (!read_float(buffer->fp,
			    &dam->run[run_index].dam.k_entrain,
			    "dam k entrain ->"))
	        p_error(buffer->fp,
			"Parsing float for k entrainment for dam %s.\n",
			dam_name);
	    break;

	case TK_OUTPUT_GAS:
	    if (pop_token(buffer->fp, token, "gas flag -> ") != EOF) {
		if (strcmp(lowercase(token, scratch1), "off") == 0) {
		    if (dam->run[run_index].dam.initial_gas)
			free(dam->run[run_index].dam.initial_gas);
		    dam->run[run_index].dam.initial_gas = 0;
		}
		else if (strcmp(lowercase(token, scratch1), "on") == 0) {
		    if (dam->run[run_index].dam.initial_gas)
			free(dam->run[run_index].dam.initial_gas);
		    dam->run[run_index].dam.initial_gas 
			= (float *) malloc(sizeof(float)*DAM_SLICES_IN_SEASON);
		    if (!read_float_array(buffer->fp,
					  dam->run[run_index].dam.initial_gas,
					  DAM_SLICES_IN_SEASON,
					  ACT_NO_CONV,
					  1,
					  "output gas ->"))
			P_ERROR("Parsing float array for initial_gas for %s\n",
				dam->seg_name);
		}
		else {
		    P_ERROR ("input gas: bad reset flag {%s}: line discarded\n",
			     token);
		    reset_input(buffer->fp);
		}
	    }
	    else
		P_WARNING ("unexpected EOF while %s\n",
			   "looking for input gas flag.");

	    break;
/* ================dam "input_temp" input parser.=================*/
	case TK_INPUT_TEMP:
	    if (pop_token(buffer->fp, token, "temp flag -> ") != EOF) {
		if (strcmp(lowercase(token, scratch1), "off") == 0) {
		    dam->run[run_index].dam.init_temp_flag = 0;
		}
		else if (strcmp(lowercase(token, scratch1), "on") == 0) {
			dam->run[run_index].dam.init_temp_flag = 1;
		    if (!read_float_array(buffer->fp,
					  dam->run[run_index].dam.water_temp,
					  DAYS_IN_SEASON,
					  ACT_DUP,
					  STEPS_PER_DAY,
					  "input temp ->"))
			P_ERROR("Parsing float array for initial water_temp for %s\n",
				dam->seg_name);
		}
		else {
		    P_ERROR ("input water temps: bad reset flag {%s}: line discarded\n",
			     token);
		    reset_input(buffer->fp);
		}
	    }
	    else
		P_WARNING ("unexpected EOF while %s\n",
			   "looking for input water temp flag.");

	    break;

/* ================dam "input_turbidity" input parser.=================*/
	case TK_INPUT_TURB:
	    if (pop_token(buffer->fp, token, "turbidity flag -> ") != EOF) {
		if (strcmp(lowercase(token, scratch1), "off") == 0) {
		    dam->run[run_index].dam.turbidity_flag = 0;
		}
		else if (strcmp(lowercase(token, scratch1), "on") == 0) {
			dam->run[run_index].dam.turbidity_flag = 1;
		    if (!read_float_array(buffer->fp,
					  dam->run[run_index].dam.turbidity,
					  DAYS_IN_SEASON,
					  ACT_DUP,
					  STEPS_PER_DAY,
					  "input turbidity ->"))
			P_ERROR("Parsing float array for initial turbidity for %s\n",
				dam->seg_name);
		}
		else {
		    P_ERROR ("input turbidity: bad reset flag {%s}: line discarded\n",
			     token);
		    reset_input(buffer->fp);
		}
	    }
	    else
		P_WARNING ("unexpected EOF while %s\n",
			   "looking for input turbidity flag.");

	    break;
/*	============================================  */
/* ================dam RSW input parser.=================*/
	case TK_RSW_EQUATION:
	    if (pop_token(buffer->fp, token, "species -> ") != EOF) 
	    {
		if ((spec_index = ref_spec ( unders_to_spaces ( token ) )) >= 0) 
		{
		    if (pop_token(buffer->fp, token, "equation id ->") == EOF)
		    {
		        p_warning(buffer->fp,
				  "unexpected EOF while looking for equation id. dam = %s\n",
				  dam_name);
			break;
		    }
		    parse_equation(buffer,
				&dam->run[run_index].dam.species[spec_index].rsw_eqn, token, TKS_RSW_EQUATION);
		}
	    } 
	    else 
	        p_warning(buffer->fp,
			  "unexpected EOF while looking for species name. dam = %s\n",
			  dam_name);
	    break;
	case TK_SPILL_RSW_PROP:
		{
			float f;
			read_float(buffer->fp, &f, "spill_rsw_prop ->");
			log_msg(L_WARNING, "Obsolete token spill_rsw_prop ignored\n");
			break;
		}
	case TK_RSW_SPILL_CAP:
	    if (!read_float(buffer->fp,
			      &dam->run[run_index].dam.rsw_spill_cap,
			      "rsw_spill_cap ->"))
		P_ERROR("Parsing float for RSW spill cap for run %d.\n",
			run_index);
	    break;
	case TK_SPILL_DELAY:
	    PARSE_DAM_SPECIES_FLOAT (spill_delay,
				     "spill delay ->",
				     "Parsing float for spill delay for dam %s.\n");
	    break;
	case TK_BYPASS_DELAY:
	    PARSE_DAM_SPECIES_FLOAT (bypass_delay,
				     "bypass delay ->",
				     "Parsing float for bypass delay for dam %s.\n");
	    break;
	case TK_SLUICEWAY_DELAY:
	    PARSE_DAM_SPECIES_FLOAT (sluiceway_delay,
				     "sluiceway delay ->",
				     "Parsing float for sluiceway delay for dam %s.\n");
	    break;
	case TK_TURBINE_DELAY:
	    PARSE_DAM_SPECIES_FLOAT (turbine_delay,
				     "turbine delay ->",
				     "Parsing float for turbine delay for dam %s.\n");
	    break;
	case TK_RSW_DELAY:
	    PARSE_DAM_SPECIES_FLOAT (rsw_delay,
				     "rsw delay ->",
				     "Parsing float for rsw delay for dam %s.\n");
	    break;
	case TK_SPILL_DAY:
	    PARSE_DAM_SPECIES_FLOAT (spill_day,
				     "spill day ->",
				     "Parsing float for spill during day for dam %s.\n");
	    break;
	case TK_BYPASS_DAY:
	    PARSE_DAM_SPECIES_FLOAT (bypass_day,
				     "bypass day ->",
				     "Parsing float for bypass during day for dam %s.\n");
	    break;
	case TK_SLUICEWAY_DAY:
	    PARSE_DAM_SPECIES_FLOAT (sluiceway_day,
				     "sluiceway day ->",
				     "Parsing float for sluiceway during day for dam %s.\n");
	    break;
	case TK_TURBINE_DAY:
	    PARSE_DAM_SPECIES_FLOAT (turbine_day,
				     "turbine day ->",
				     "Parsing float for turbine during day for dam %s.\n");
	    break;
	case TK_RSW_DAY:
	    PARSE_DAM_SPECIES_FLOAT (rsw_day,
				     "rsw day ->",
				     "Parsing float for rsw during day for dam %s.\n");
	    break;
	case TK_RSW_MRT:
	    PARSE_DAM_SPECIES_FLOAT_OR_DIST (rsw_mort,
				     "RSW mort. ->",
				     "Parsing float for RSW mort. for dam %s.\n");
	    break;
	case TK_RSW_MRT_MN:
	    PARSE_DAM_SPECIES_BROKEN_STICK_DIST (rsw_mort, mean,
				     "RSW mortality mean ->",
				     "Parsing float for RSW mortality mean for dam %s.\n");
	    break;
	case TK_RSW_MRT_LOW:
	    PARSE_DAM_SPECIES_BROKEN_STICK_DIST (rsw_mort, low,
				     "RSW mortality low ->",
				     "Parsing float for RSW mortality low for dam %s.\n");
	    break;
	case TK_RSW_MRT_HIGH:
	    PARSE_DAM_SPECIES_BROKEN_STICK_DIST (rsw_mort, high,
				     "RSW mortality high ->",
				     "Parsing float for RSW mortality high for dam %s.\n");
	    break;
/*	============================================  */

	    /* obsolete dam tokens */
	case TK_SPL_MRT_VAR:
	    p_warning(buffer->fp,
		      "obsolete token {%s}.  Data discarded.\n",
		      TKS_SPL_MRT_VAR);
	    if (pop_token(buffer->fp, token, "species -> ") != EOF)
		(void) skip_numbers(buffer->fp, 1);
	    break;
	case TK_BY_MRT_VAR:
	    p_warning(buffer->fp,
		      "obsolete token {%s}.  Data discarded.\n",
		      TKS_BY_MRT_VAR);
	    if (pop_token(buffer->fp, token, "species -> ") != EOF)
		(void) skip_numbers(buffer->fp, 1);
	    break;
	case TK_TURB_MRT_VAR:
	    p_warning(buffer->fp,
		      "obsolete token {%s}.  Data discarded.\n",
		      TKS_TURB_MRT_VAR);
	    if (pop_token(buffer->fp, token, "species -> ") != EOF)
		(void) skip_numbers(buffer->fp, 1);
	    break;
	case TK_TRNS_MRT_VAR:
	    p_warning(buffer->fp,
		      "obsolete token {%s}.  Data discarded.\n",
		      TKS_TRNS_MRT_VAR);
	    if (pop_token(buffer->fp, token, "species -> ") != EOF)
		(void) skip_numbers(buffer->fp, 1);
	    break;
	case TK_PASSAGE:
	    p_warning(buffer->fp,
		      "obsolete token {%s}.  Data discarded.\n",
		      TKS_PASSAGE);
	    if (pop_token(buffer->fp, token, "species -> ") != EOF)
		(void) skip_numbers(buffer->fp, DAM_TIME_SLC);
	    break;
	case TK_SPL_EQN_CLASS:
	    p_warning(buffer->fp,
		      "obsolete token {%s}.  Data discarded.\n",
		      TKS_SPL_EQN_CLASS);
	    if (pop_token(buffer->fp, token, "species -> ") != EOF)
		(void) skip_numbers(buffer->fp, 1);
	    break;
	case TK_SPL_EQN_ERFN:
	    p_warning(buffer->fp,
		      "obsolete token {%s}.  Data discarded.\n",
		      TKS_SPL_EQN_ERFN);
	    if (pop_token(buffer->fp, token, "species -> ") != EOF)
		(void) skip_numbers(buffer->fp, 1);
	    break;
	case TK_SPL_EQN_VAL:
	    p_warning(buffer->fp,
		      "obsolete token {%s}.  Data discarded.\n",
		      TKS_SPL_EQN_VAL);
	    if (pop_token(buffer->fp, token, "species -> ") != EOF)
		(void) skip_numbers(buffer->fp, 3);
	    break;
	case TK_SPL_EQN_RANGE:
	    p_warning(buffer->fp,
		      "obsolete token {%s}.  Data discarded.\n",
		      TKS_SPL_EQN_RANGE);
	    if (pop_token(buffer->fp, token, "species -> ") != EOF)
		(void) skip_numbers(buffer->fp, 3);
	    break;
	case TK_DAY_NPSS_MN:
	    p_warning(buffer->fp,
		      "obsolete token {%s}.  Data discarded.\n",
		      TKS_DAY_NPSS_MN);
	    if (pop_token(buffer->fp, token, "species -> ") != EOF)
		(void) skip_numbers(buffer->fp, 1);
	    break;
	case TK_DAY_NPSS_VAR:
	    p_warning(buffer->fp,
		      "obsolete token {%s}.  Data discarded.\n",
		      TKS_DAY_NPSS_VAR);
	    if (pop_token(buffer->fp, token, "species -> ") != EOF)
		(void) skip_numbers(buffer->fp, 1);
	    break;
	case TK_DAY_FGE_VAR:
	    p_warning(buffer->fp,
		      "obsolete token {%s}.  Data discarded.\n",
		      TKS_DAY_FGE_VAR);
	    if (pop_token(buffer->fp, token, "species -> ") != EOF)
		(void) skip_numbers(buffer->fp, 1);
	    break;
	case TK_NT_NPSS_MN:
	    p_warning(buffer->fp,
		      "obsolete token {%s}.  Data discarded.\n",
		      TKS_NT_NPSS_MN);
	    if (pop_token(buffer->fp, token, "species -> ") != EOF)
		(void) skip_numbers(buffer->fp, 1);
	    break;
	case TK_NT_NPSS_VAR:
	    p_warning(buffer->fp,
		      "obsolete token {%s}.  Data discarded.\n",
		      TKS_NT_NPSS_VAR);
	    if (pop_token(buffer->fp, token, "species -> ") != EOF)
		(void) skip_numbers(buffer->fp, 1);
	    break;
	case TK_NT_FGE_VAR:
	    p_warning(buffer->fp,
		      "obsolete token {%s}.  Data discarded.\n",
		      TKS_NT_FGE_VAR);
	    if (pop_token(buffer->fp, token, "species -> ") != EOF)
		(void) skip_numbers(buffer->fp, 1);
	    break;
	case TK_DAY_SPILL:
	    P_WARNING("obsolete token {%s}.  Data discarded.\n",
		      TKS_DAY_SPILL);
	    (void) skip_numbers(buffer->fp, DAYS_IN_SEASON);
	    break;
	case TK_NT_SPILL:
	    P_WARNING("obsolete token {%s}.  Data discarded.\n",
		      TKS_NT_SPILL);
	    (void) skip_numbers(buffer->fp, DAYS_IN_SEASON);
	    break;
	case TK_DAM_SPILL:
	    P_WARNING("obsolete token {%s}.  Data discarded.\n",
		      TKS_DAY_SPILL);
	    (void) skip_numbers(buffer->fp, DAYS_IN_SEASON*DAM_TIME_SLC);
	    break;
	case TK_TR_TIME:
	    p_warning(buffer->fp,
		      "obsolete token {%s}.  Data discarded.\n",
		      TKS_TR_TIME);
	    (void) skip_numbers(buffer->fp, 1);
	    break;
	case TK_PWRHS_WEIGHT:
	    P_WARNING("obsolete token {%s}.  Data discarded.\n",
		      TKS_PWRHS_WEIGHT);
	    (void) skip_numbers(buffer->fp, 1);
	    break;
	case TK_DAM_LOSS:
	    P_WARNING("obsolete token {%s}.  Data discarded.\n", TKS_DAM_LOSS);
	    (void) skip_numbers(buffer->fp, DAYS_IN_SEASON);
	    break;
	case TK_DAM_LOSS_MAX:
	    p_warning(buffer->fp,
		      "obsolete token {%s}.  Data discarded.\n",
		      TKS_DAM_LOSS_MAX);
	    (void) skip_numbers(buffer->fp, 1);
	    break;
	case TK_DAM_LOSS_MIN:
	    p_warning(buffer->fp,
		      "obsolete token {%s}.  Data discarded.\n",
		      TKS_DAM_LOSS_MIN);
	    (void) skip_numbers(buffer->fp, 1);
	    break;
	case TK_SLC_EQN_CLASS:
	    p_warning(buffer->fp,
		      "obsolete token {%s}.  Data discarded.\n",
		      TKS_SLC_EQN_CLASS);
	    if (pop_token(buffer->fp, token, "species -> ") != EOF)
		(void) skip_numbers(buffer->fp, 1);
	    break;
	case TK_SLC_EQN_ERFN:
	    p_warning(buffer->fp,
		      "obsolete token {%s}.  Data discarded.\n",
		      TKS_SLC_EQN_ERFN);
	    if (pop_token(buffer->fp, token, "species -> ") != EOF)
		(void) skip_numbers(buffer->fp, 1);
	    break;
	case TK_SLC_EQN_VAL:
	    p_warning(buffer->fp,
		      "obsolete token {%s}.  Data discarded.\n",
		      TKS_SLC_EQN_VAL);
	    if (pop_token(buffer->fp, token, "species -> ") != EOF)
		(void) skip_numbers(buffer->fp, 3);
	    break;
	case TK_SLC_EQN_RANGE:
	    p_warning(buffer->fp,
		      "obsolete token {%s}.  Data discarded.\n",
		      TKS_SLC_EQN_RANGE);
	    if (pop_token(buffer->fp, token, "species -> ") != EOF)
		(void) skip_numbers(buffer->fp, 3);
	    break;
	    
	case TK_SLUICE_EQUATION:
	{
	    equation tmp_eqn;
	    
	    p_warning(buffer->fp,
		      "obsolete token {%s}.  Data discarded.\n",
		      TKS_SLUICE_EQUATION);
	    if (pop_token(buffer->fp, token, "species -> ") == EOF
		|| pop_token(buffer->fp, token, "equation id ->") == EOF)
	    {
		p_warning(buffer->fp, "unexpected EOF while discarding.\n", 0);
		break;
	    }
	    else
		parse_equation(buffer, &tmp_eqn, token, TKS_SLUICE_EQUATION);
	    
	    break;
	}    
	case TK_MONTE_OUT:
	    p_warning(buffer->fp,
		      "obsolete token {%s}.  Parsing as %s.\n",
		      TKS_MONTE_OUT, TKS_OUTPUT_SETTINGS);
    case TK_OUTPUT_SETTINGS:
        if (!read_uns(buffer->fp, &utemp,
			  "output settings ->"))
		P_ERROR("Parsing integer for output settings for dam %s\n",
			dam_name);
        /* Apply new output settings; guard against illegal values. */
        if (!lock_output_settings)
            dam->output_settings = utemp &
                    (PASSAGE_FLAG | TRANSPORT_FLAG | ROUTING_FLAG |
                     FLOW_FLAG | WATERTEMP_FLAG | NSAT_FLAG | SPILL_FLAG | TURBIDITY_FLAG | DAY_SPILL_FLAG | NIGHT_SPILL_FLAG);
	    break;
	case TK_INCLUDE:
	    buffer = include_to_parse(buffer);
	    break;
	case TK_END:
		{
			char t[MAX_COMMAND_LEN+1];
			if (pop_token(buffer->fp, t, "ending command ->") == EOF) {
				p_error(buffer->fp, "Unexpected EOF parsing end token\n");
				return;
			}
			if (!strcmp(t, end_token)) {
				reset_input(buffer->fp);
				if (fake_dam)
				{
				if (destroy_river_segment(dam))
					p_error(buffer->fp, "Could not destroy the dam segment\n", 0);
				else
					p_error(buffer->fp, "\tDiscarded the data for dam %s\n", dam_name);
				}
				return;
			}
			else {
				reset_input(buffer->fp);
				p_error(buffer->fp, "End token found in %s for %s, ignoring line\n", end_token, t);
			}
		}
		break;
	default:
	    UNKNOWN_COMMAND_ERROR (buffer->fp, token);
	}
    }
}


/*------------------------------------------------------------------------*/
/* parse transport creates a transport record (allocates memory, fills with
   default values) if none exists, then attaches it to the designated dam.
   If the record cannot be created it flushes the buffer and returns.
   It then parses the following tokens and updates the appropriate values in
   the transport record, until an END token is reached.  Warning messages
   are printed for syntax errors.

   The return value is FALSE if there was an error creating the transport
   record, otherwise TRUE is returned */
/*------------------------------------------------------------------------*/
static int parse_transport (buffer, seg, run_index, end_token)
    PARSER_FILE *buffer;
    struct river_segment *seg;
    int run_index;
	const char *end_token;
{
    char token[MAX_COMMAND_LEN+1];
    int number;
    static COMMAND_LIST trns_list=0;
    struct transport transport;
    struct transport *trns;
    struct river_segment *tmp_seg;
    struct dam *dam = &seg->run[run_index].dam;
    char *dam_name = seg->seg_name;

    trns = &transport;			/* use pointer for consistency */
    copy_trns((struct transport *)NULL, trns);	/* init to defaults */
    trns->source_seg = seg;

    /* set up the trns list */
    if (!trns_list)
    {
	trns_list = com_create_list(trns_list, trns_tokens);
    }

    log_msg(L_DBGMSG, "parsing transport for dam %s\n", dam_name);

    while (1)
    {
        if (pop_token(buffer->fp, token, "transport command -> ") == EOF)
	    {
            if (can_return_from(buffer))
	    {
                buffer = return_after_parse(buffer);
		continue;
	    }
            else
		{
		P_WARNING(
		   "encountered EOF before end statement while parsing transport at dam %s\n",
			dam_name);
		reset_input(buffer->fp);
		dam->transport = NULL;
                return(FALSE);
            }
        }

	switch (com_reference(trns_list, lowercase(token, scratch1)))
	{
	case TK_START_BY_DATE:
	    if (!read_int(buffer->fp,
			      &trns->start_by_date,
			      "start transport by date (1=yes; 0=no)? ->"))
		P_ERROR("Parsing int for transport start by date/count for dam %s.\n",
			dam_name);
	    break;
	case TK_START_DATE:
	    if (!read_int(buffer->fp,
			      &trns->start_date,
			      "transport start date ->"))
		P_ERROR("Parsing int for transport start date for dam %s.\n",
			dam_name);
	    break;
	case TK_START_COUNT:
	    if (!read_int(buffer->fp,
			      &trns->start_count,
			      "transport start count ->"))
		P_ERROR("Parsing int for transport start count for dam %s.\n",
			dam_name);
	    break;
	case TK_MAX_RESTARTS:
	    if (!read_int(buffer->fp,
			      &trns->max_restarts,
			      "transport max # restarts ->"))
		P_ERROR("Parsing int for transport max # restarts for dam %s.\n",
			dam_name);
	    break;
	case TK_RESTART_BY_DATE:
	    if (!read_int(buffer->fp,
			      &trns->restart_by_date,
			      "transport restart by date ->"))
		P_ERROR("Parsing int for transport restart by date for dam %s.\n",
			dam_name);
	    break;
	case TK_RESTART_DATE:
	    if (!read_int(buffer->fp,
			      &trns->restart_date,
			      "transport restart date ->"))
		P_ERROR("Parsing int for transport restart date for dam %s.\n",
			dam_name);
	    break;
	case TK_RESTART_COUNT:
	    if (!read_int(buffer->fp,
			      &trns->restart_count,
			      "transport restart count ->"))
		P_ERROR("Parsing int for transport restart count for dam %s.\n",
			dam_name);
	    break;
    case TK_END_DATE:
	    if (!read_int(buffer->fp,
			      &trns->end_date,
			      "transport end date ->"))
		P_ERROR("Parsing int for transport end date for dam %s.\n",
			dam_name);
	    break;
	case TK_END_COUNT:
	    if (!read_int(buffer->fp,
			      &trns->end_count,
			      "transport end count ->"))
		P_ERROR("Parsing int for transport end count for dam %s.\n",
			dam_name);
	    break;
	case TK_NUM_LOW_DAYS:
	    if (!read_int(buffer->fp,
			      &trns->num_low_days,
			      "transport number low days ->"))
		P_ERROR("Parsing int for transport number low days for dam %s.\n",
			dam_name);
	    break;
	case TK_HFL_PASSED:
	    if (!read_float(buffer->fp,
			      &trns->hfl_passed_perc,
			      "percent passed for full transport ->"))
		P_ERROR("Parsing float for transport passed perc for dam %s.\n",
			dam_name);
	    break;
	case TK_HI_FLOW:
	    if (!read_float(buffer->fp,
			      &trns->hi_flow,
			      "high flow for partial transport ->"))
		P_ERROR("Parsing float for transport high flow for dam %s.\n",
			dam_name);
	    break;
	case TK_RATE:
	    if (!read_float(buffer->fp,
			      &trns->rate,
			      "transport rate ->"))
		P_ERROR("Parsing float for transport rate for dam %s.\n",
			dam_name);
	    break;
	case TK_HFL_SPEC:
	    /* set the species ptr */
	    if (pop_token(buffer->fp, token, "species -> ") != EOF)
		if ((number = ref_spec ( unders_to_spaces ( token ) )) >= 0)
		    trns->hfl_spec = &specs[number];
		else
		{
		    P_ERROR ("transport: unknown species {%s}: line discarded\n",
			     token);
		    reset_input(buffer->fp);
		}
	    else
		P_WARNING ("unexpected EOF while %s\n","looking for transport species name.");
	    break;
	case TK_RLS_PT:
	    /* set the target seg ptr */
	    if (pop_token(buffer->fp, token, "release point (reach) -> ") != EOF)
	    {
		if ((tmp_seg = find_seg_by_name(river_mouth, REACH_SEG,
					  unders_to_spaces ( token ) )) == 0)
		{
		    P_ERROR ("unknown transport target reach {%s}: line discarded\n",
			     token);
		    reset_input(buffer->fp);
		}
		else
		    trns->target_seg = tmp_seg;		/* found it */
	    }
	    else	
		P_WARNING ("unexpected EOF while %s\n",
			 "looking for transport target reach name.");
	    break;
	case TK_INCLUDE:
	    buffer = include_to_parse(buffer);
	    break;
	case TK_END:
		{
			char t[MAX_COMMAND_LEN+1];
			if (pop_token(buffer->fp, t, "ending command ->") == EOF) {
				p_error(buffer->fp, "Unexpected EOF parsing end token\n");
				return(FALSE);
			}
			if (!strcmp(t, end_token)) {
				reset_input(buffer->fp);
				if (add_transport(trns))
				return(TRUE);
				else
				{
				dam->transport = NULL;
				P_ERROR ("transport record data invalid for dam %s, discarded\n", dam_name);
				return(FALSE);
				}
			}
			else {
				reset_input(buffer->fp);
				p_error(buffer->fp, "End token found in %s for %s, ignoring line\n", end_token, t);
			}
		}
		break;
	default:
	    UNKNOWN_COMMAND_ERROR (buffer->fp, token);
	}
    }
}


/*------------------------------------------------------------------------*/
/* parse hdwtr puts the appropriate numbers in the appropriate places in
   the record corresponding to hdwtr_name.  If hdwtr_name does not exist,
   it flushes the buffer and returns.  Warning messages are printed for
   syntax errors.

   There is no return value */
/*------------------------------------------------------------------------*/
static void parse_hdwtr (buffer, hdwtr_name, run_index, end_token)
    PARSER_FILE *buffer;
    char *hdwtr_name;
    int run_index;
	const char* end_token;
{
    unsigned char fake_hdwtr = 0;
    /* this hack is so that obsolete hdwtrs can be discarded */
    /* ^..^ it can be gotten rid of by putting the find_seg
       call outside the parse_hdwtr() routine */
    char token[MAX_COMMAND_LEN+1];
    static COMMAND_LIST hdwtr_list=0;

    struct river_segment *hdwtr = 0;
    unsigned int utemp;

    /* find the headwater in question */
    if ( (hdwtr = find_seg_by_name(river_mouth, HEADWATER_SEG,
			   unders_to_spaces(hdwtr_name) )) == 0)
    {
	P_ERROR ("headwater %s does not exist: I will try and discard data\n", hdwtr_name);
	hdwtr = new_river_segment (HEADWATER_SEG);
	strcpy(hdwtr->seg_name = (char *)malloc(strlen(hdwtr_name)+1),
	       hdwtr_name);
	fake_hdwtr = 1;
    }

    /* set up the hdwtr list */
    if (!hdwtr_list)
    {
	hdwtr_list = com_create_list(hdwtr_list, hdwtr_tokens);
    }

    log_msg(L_DBGMSG, "parsing headwater %s\n", hdwtr_name);

    while (1)
    {
        if (pop_token(buffer->fp, token, "headwater command -> ") == EOF)
        {
            if (can_return_from(buffer))
	    {
                buffer = return_after_parse(buffer);
		continue;
	    }
            else
            {
		P_WARNING("encountered EOF before end statement while parsing headwater %s\n",
			hdwtr_name);
		reset_input(buffer->fp);
                return;
            }
        }
	switch (com_reference(hdwtr_list, lowercase(token, scratch1)))
	{
	case TK_HW_FLOW_COEF:
	    P_WARNING("obsolete token {%s}.  Data discarded.\n",
		      TKS_HW_FLOW_COEF);
	    (void) skip_numbers(buffer->fp, 1);
	    break;
	case TK_HW_FLOW_MAX:
	    if (!read_float(buffer->fp,
			      &hdwtr->run[run_index].headwater.flow_max,
			      "headwater flow max ->"))
		P_ERROR("Parsing float for flow max for %s.\n",
			hdwtr->seg_name);
	    break;
	case TK_HW_FLOW_MEAN:
	    if (!read_float(buffer->fp,
			      &hdwtr->run[run_index].headwater.flow_mean,
			      "headwater flow mean ->"))
		P_ERROR("Parsing float for flow mean for %s.\n",
			hdwtr->seg_name);
	    break;
	case TK_HW_FLOW:
	    if (!read_float_array(buffer->fp,
				    hdwtr->run[run_index].headwater.flow,
				    DAYS_IN_SEASON,
				    ACT_NO_CONV,
				    1,
				    "flow ->"))
		P_ERROR("Parsing float array for flow for %s\n",
			hdwtr->seg_name);
	    break;
	case TK_HW_WTEMP:
        version_check( buffer, TKS_HW_WTEMP );
	    if (buffer->version < 1)
	    {
		if (!read_float_array(buffer->fp,
				    hdwtr->run[run_index].headwater.water_temp,
				    STEPS_IN_SEASON,
				    ACT_NO_CONV,
				    1,
				    "water temperature  ->"))
		    P_ERROR("Parsing float array for water temp for %s\n",
			    hdwtr->seg_name);
	    }
	    else
	    {
		if (!read_float_array(buffer->fp,
				hdwtr->run[run_index].headwater.water_temp,
				DAYS_IN_SEASON,
				ACT_DUP,
				STEPS_PER_DAY,
				"water temperature ->"))
		    P_ERROR("Parsing float array for water temp for %s\n",
			    hdwtr->seg_name);
	    }
	    break;
	case TK_HW_MONTE_OUT:
	    p_warning(buffer->fp,
		      "obsolete token {%s}.  Parsing as %s.\n",
		      TKS_HW_MONTE_OUT, TKS_OUTPUT_SETTINGS);
    case TK_OUTPUT_SETTINGS:
	    if (!read_uns(buffer->fp, &utemp,
			  "output settings ->"))
		P_ERROR("Parsing int for output settings for %s\n",
			hdwtr->seg_name);
        /* Apply new output settings, guard against illegal values. */
        if (!lock_output_settings)
            hdwtr->output_settings = utemp & (FLOW_FLAG | WATERTEMP_FLAG | TURBIDITY_FLAG);
	    break;
	case TK_OUTPUT_GAS:
	    if (pop_token(buffer->fp, token, "gas flag -> ") != EOF) {
		if (strcmp(lowercase(token, scratch1), "off") == 0) {
		    if (hdwtr->run[run_index].headwater.initial_gas)
			free(hdwtr->run[run_index].headwater.initial_gas);
		    hdwtr->run[run_index].headwater.initial_gas = 0;
		}
		else if (strcmp(lowercase(token, scratch1), "on") == 0) {
		    if (hdwtr->run[run_index].headwater.initial_gas)
			free(hdwtr->run[run_index].headwater.initial_gas);
		    hdwtr->run[run_index].headwater.initial_gas 
			= (float *) malloc(sizeof(float)*DAM_SLICES_IN_SEASON);
		    if (!read_float_array(buffer->fp,
				hdwtr->run[run_index].headwater.initial_gas,
				DAM_SLICES_IN_SEASON,
				ACT_NO_CONV,
				1,
				"output gas ->"))
		    P_ERROR("Parsing float array for initial_gas for %s\n",
			    hdwtr->seg_name);
		}
		else {
		    P_ERROR ("input gas: bad reset flag {%s}: line discarded\n",
			     token);
		    reset_input(buffer->fp);
		}
	    }
	    else
		P_WARNING ("unexpected EOF while %s\n",
			   "looking for input gas flag.");

	    break;

		/* ================hdwtr "input_turbidity" input parser.=================*/
	case TK_INPUT_TURB:
	    if (pop_token(buffer->fp, token, "turbidity flag -> ") != EOF) {
		if (strcmp(lowercase(token, scratch1), "off") == 0) {
		    hdwtr->run[run_index].headwater.turbidity_flag = 0;
		}
		else if (strcmp(lowercase(token, scratch1), "on") == 0) {
			hdwtr->run[run_index].headwater.turbidity_flag = 1;
		    if (!read_float_array(buffer->fp,
					  hdwtr->run[run_index].headwater.turbidity,
					  DAYS_IN_SEASON,
					  ACT_DUP,
					  STEPS_PER_DAY,
					  "input turbidity ->"))
			P_ERROR("Parsing float array for initial turbidity for %s\n",
				hdwtr->seg_name);
		}
		else {
		    P_ERROR ("input turbidity: bad reset flag {%s}: line discarded\n",
			     token);
		    reset_input(buffer->fp);
		}
	    }
	    else
		P_WARNING ("unexpected EOF while %s\n",
			   "looking for input turbidity flag.");

	    break;
/*	============================================  */
	case TK_INCLUDE:
	    buffer = include_to_parse(buffer);
	    break;
	case TK_END:
		{
			char t[MAX_COMMAND_LEN+1];
			if (pop_token(buffer->fp, t, "ending command ->") == EOF) {
				p_error(buffer->fp, "Unexpected EOF parsing end token\n");
				return;
			}
			if (!strcmp(t, end_token)) {
				if (fake_hdwtr)
				{
				if (destroy_river_segment(hdwtr))
					p_error(buffer->fp, "Could not destroy the hdwtr segment\n", 0);
				else
					p_error(buffer->fp, "\tDiscarded the data for hdwtr %s\n", hdwtr_name);
				}
				reset_input(buffer->fp);
				return;
			}
			else {
				reset_input(buffer->fp);
				p_error(buffer->fp, "End token found in %s for %s, ignoring line\n", end_token, t);
			}
		}
		break;
	default:
	    UNKNOWN_COMMAND_ERROR (buffer->fp, token);
	}
    }
}

/*------------------------------------------------------------------------*/
/* parse reach puts the appropriate numbers in the appropriate places in
   the record corresponding to reach_name.  If reach_name does not exist,
   it flushes the buffer and returns.  Warning messages are printed for
   syntax errors.

   There is no return value */
/*------------------------------------------------------------------------*/
static void parse_reach (buffer, reach_name, run_index, end_token)
    PARSER_FILE *buffer;
    char *reach_name;
    int run_index;
	const char *end_token;
{
    unsigned char fake_reach = 0;
    int i, spec_index;

    /* this hack is so that obsolete dams can be discarded */
    /* ^..^ it can be gotten rid of by putting the find_seg
       call outside the parse_reach() routine */
    char token[MAX_COMMAND_LEN+1];
    static COMMAND_LIST reach_list=0;
    float dummy_array[STEPS_IN_SEASON];
    unsigned int utemp;

    struct river_segment *reach = 0;

    /* find the reach in question */
    if ( (reach = find_seg_by_name(river_mouth, REACH_SEG,
			   unders_to_spaces(reach_name) )) == 0)
    {
	/* ^..^ discard the entire reach instead of just this line */
	P_ERROR ("reach %s does not exist: I will try and discard data.\n", reach_name);
	reach = new_river_segment (REACH_SEG);
	strcpy(reach->seg_name = (char *)malloc(strlen(reach_name)+1),
	       reach_name);
	fake_reach = 1;
    }
    
    /* set up the reach list */
    if (!reach_list)
    {
	reach_list = com_create_list(reach_list, reach_tokens);
    }

    log_msg(L_DBGMSG, "parsing reach %s\n", reach_name);

	/* Set initial temperature and turbidity input flags  (necessary
	 * when re-setting temperatures from GUI using old dat file w/o these tokens */
	reach->run[run_index].reach.init_temp_flag = 0;
	reach->run[run_index].reach.turbidity_flag = 0;

    while (1)
    {
        if (pop_token(buffer->fp, token, "reach command -> ") == EOF)
        {
            if (can_return_from(buffer))
	    {
                buffer = return_after_parse(buffer);
		continue;
	    }
            else
            {
		P_WARNING("encountered EOF before end statement while parsing reach %s\n",
			reach_name);
		reset_input(buffer->fp);
		return;
            }
        }

	switch (com_reference(reach_list, lowercase(token, scratch1)))
	{

    
    case TK_RCH_PRED_MEAN_V15:
	    p_warning(buffer->fp,
	    "obsolete token {%s}, parsing as {%s}\n",
	        TKS_RCH_PRED_MEAN_V15, TKS_RCH_PRED_MEAN );
        /* fall through is intentional */
    case TK_RCH_PRED_MEAN:
        if (buffer->version <= 6) {
            float tempf;
	        if (!read_float(buffer->fp, &tempf,
			          "predator population mean->"))
		        P_ERROR("Parsing float for reach predator mean for run %d.\n",
			        run_index);
            else {
                int k;
                p_warning( buffer->fp, "token {%s} being applied for all species at %s\n",
                    TKS_RCH_PRED_MEAN, reach_name );
                for (k = 0; k < num_specs; k++)
                    reach->run[run_index].reach.species[k].pred_pop_mean = tempf;
            }
        }
        else {
    	    PARSE_REACH_SPECIES_FLOAT (pred_pop_mean,
				     "reach pred dens ->",
				     "Parsing float for reach predator density for reach %s.\n");
        }
	    break;

    case TK_RCH_PRED_MEAN_V16:
	    p_warning(buffer->fp,
	          "obsolete token {%s}, ignoring\n", TKS_RCH_PRED_MEAN_V16 );
	    (void) skip_numbers(buffer->fp, 1);
	    break;

    case TK_RCH_PRED_DIST:
	    if (!read_float(buffer->fp,
			      &reach->run[run_index].reach.pred_dist_coef,
			      "predator distribution coefficient ->"))
		P_ERROR("Parsing float for reach predator distribution coef. for run %d.\n",
			run_index);
	    break;
	case TK_RCH_PRED_VAR:
	    P_WARNING("obsolete token {%s}.  Data discarded.\n", TKS_RCH_PRED_VAR);
	    (void) skip_numbers(buffer->fp, 3);
	    break;
	case TK_RCH_LOSS:
	    if (!read_float_array(buffer->fp,
				   reach->run[run_index].reach.loss,
				   DAYS_IN_SEASON,
				   ACT_NO_CONV,
				   1,
				   "loss ->"))
		P_ERROR("Parsing float array for predator activity for run %d\n",
			run_index);
	    break;
	case TK_RCH_LOSS_MAX:
	    if (!read_float(buffer->fp,
			      &reach->run[run_index].reach.loss_max,
			      "loss max->"))
		P_ERROR("Parsing float for reach loss max for run %d.\n",
			run_index);
	    break;
	case TK_RCH_LOSS_MIN:
	    if (!read_float(buffer->fp,
			      &reach->run[run_index].reach.loss_min,
			      "loss min->"))
		P_ERROR("Parsing float for reach loss min for run %d.\n",
			run_index);
	    break;
	case TK_RCH_VELOCITY:
	    P_WARNING("reach velocity no longer supported, discarding\n",
		    run_index);
	    if (!read_float_array(buffer->fp,
				   dummy_array,
				   STEPS_IN_SEASON,
				   ACT_NO_CONV,
				   1,
				   "velocity ->"))
		P_ERROR("Parsing float array for reach_velocity for run %d\n",
			run_index);
	    break;
	case TK_RCH_FLOW_MAX:
	    if (!read_float(buffer->fp,
			      &reach->run[run_index].reach.flow_max,
			      "flow max->"))
		P_ERROR("Parsing float for reach flow max for run %d.\n",
			run_index);
	    break;
	case TK_RCH_FLOW:
	    if (!read_float_array(buffer->fp,
				   reach->run[run_index].reach.flow,
				   DAYS_IN_SEASON,
				   ACT_NO_CONV,
				   1,
				   "flow ->"))
		P_ERROR("Parsing float array for reach flow for run %d\n",
			run_index);
	    break;
	case TK_DELTA_TEMP:
	    if (!read_float_array(buffer->fp,
				  reach->run[run_index].reach.delta_temp,
				  DAYS_IN_SEASON,
				  ACT_DUP,
				  STEPS_PER_DAY,
				  "reach delta temperature ->"))
	        p_error(buffer->fp,
			"Parsing float array for delta temperature for reach %s\n",
			reach->seg_name);
	    break;
	case TK_RCH_ELEVATION:
	    if (!read_float_array(buffer->fp,
				   reach->run[run_index].reach.elev_change,
				   DAYS_IN_SEASON,
				   ACT_NO_CONV,
				   1,
				   "elevation change ->"))
		P_ERROR("Parsing float array for reach elevation for run %d\n",
			run_index);
	    break;
	case TK_RCH_MONTE_OUT:
	    p_warning(buffer->fp,
		      "obsolete token {%s}.  Parsing as %s.\n",
		      TKS_RCH_MONTE_OUT, TKS_OUTPUT_SETTINGS);
    case TK_REACH_CLASS:
        if (pop_token(buffer->fp, token, "reach class ->") != EOF) {
            for (i = 0; i < num_reach_classes; i++)
                if (strcmp( token, reach_class_names[i] ) == 0)
                    break;
            if (i == num_reach_classes) {
                p_error( buffer->fp, "Unknown reach class name %s seen.\n",
                          token );
                break;
            }
            reach->run[run_index].reach.rclass = i;
        }
        break;
    case TK_OUTPUT_SETTINGS:
	    if (!read_uns(buffer->fp, &utemp,
			  "output settngs ->"))
		P_ERROR("Parsing integer for reach output settings for run %d\n",
			run_index);
        /* Apply new output settings, guard against invalid values. */
        if (!lock_output_settings)
            reach->output_settings = utemp &
                (PASSAGE_FLAG | TRANSPORT_FLAG | LENGTH_FLAG | FLOW_FLAG | WATERTEMP_FLAG |
                 NSAT_FLAG | VELOCITY_FLAG | LOSS_FLAG | ELEVATION_FLAG | TURBIDITY_FLAG |
				 SPILL_EXPERIENCED_FLAG);
	    break;
	case TK_RCH_GAS_THETA:
	    if (!read_float(buffer->fp,
			      &reach->run[run_index].reach.gas_theta,
			      "gas theta ->"))
		P_ERROR("Parsing float for reach gas theta for run %d.\n",
			run_index);
	    break;
	case TK_OUTPUT_GAS:
	    if (pop_token(buffer->fp, token, "gas flag -> ") != EOF) {
		if (strcmp(lowercase(token, scratch1), "off") == 0) {
		    if (reach->run[run_index].reach.initial_gas)
			free(reach->run[run_index].reach.initial_gas);
		    reach->run[run_index].reach.initial_gas = 0;
		}
		else if (strcmp(lowercase(token, scratch1), "on") == 0) {
		    if (reach->run[run_index].reach.initial_gas)
			free(reach->run[run_index].reach.initial_gas);
		    reach->run[run_index].reach.initial_gas 
			= (float *) malloc(sizeof(float)*DAM_SLICES_IN_SEASON);
		    if (!read_float_array(buffer->fp,
				reach->run[run_index].reach.initial_gas,
				DAM_SLICES_IN_SEASON,
				ACT_NO_CONV,
				1,
				"output gas ->"))
		    P_ERROR("Parsing float array for initial_gas for %s\n",
			    reach->seg_name);
		}
		else {
		    P_ERROR ("input gas: bad reset flag {%s}: line discarded\n",
			     token);
		    reset_input(buffer->fp);
		}
	    }
	    else
		P_WARNING ("unexpected EOF while %s\n",
			   "looking for input gas flag.");

	    break;

/* ==================reach "input_temp" input parser============ */
	case TK_INPUT_TEMP:
	    if (pop_token(buffer->fp, token, "temp flag -> ") != EOF) {
		if (strcmp(lowercase(token, scratch1), "off") == 0) {
		    reach->run[run_index].reach.init_temp_flag = 0;
		}
		else if (strcmp(lowercase(token, scratch1), "on") == 0) {
			reach->run[run_index].reach.init_temp_flag = 1;
		    if (!read_float_array(buffer->fp,
				reach->run[run_index].reach.water_temp,
				DAYS_IN_SEASON,
				ACT_DUP,
				STEPS_PER_DAY,
				"output temp ->"))
		    P_ERROR("Parsing float array for initial_water_temp for %s\n",
			    reach->seg_name);
		}
		else {
		    P_ERROR ("input water temps: bad reset flag {%s}: line discarded\n",
			     token);
		    reset_input(buffer->fp);
		}
	    }
	    else
		P_WARNING ("unexpected EOF while %s\n",
			   "looking for input water temp flag.");

	    break;

		/* ================reach "input_turbidity" input parser.=================*/
	case TK_INPUT_TURB:
	    if (pop_token(buffer->fp, token, "turbidity flag -> ") != EOF) {
		if (strcmp(lowercase(token, scratch1), "off") == 0) {
		    reach->run[run_index].reach.turbidity_flag = 0;
		}
		else if (strcmp(lowercase(token, scratch1), "on") == 0) {
			reach->run[run_index].reach.turbidity_flag = 1;
		    if (!read_float_array(buffer->fp,
					  reach->run[run_index].reach.turbidity,
					  DAYS_IN_SEASON,
					  ACT_DUP,
					  STEPS_PER_DAY,
					  "input turbidity ->"))
			P_ERROR("Parsing float array for initial turbidity for %s\n",
				reach->seg_name);
		}
		else {
		    P_ERROR ("input turbidity: bad reset flag {%s}: line discarded\n",
			     token);
		    reset_input(buffer->fp);
		}
	    }
	    else
		P_WARNING ("unexpected EOF while %s\n",
			   "looking for input turbidity flag.");

	    break;
/*	============================================  */

	case TK_INCLUDE:
	    buffer = include_to_parse(buffer);
	    break;
	case TK_END:
		{
			char t[MAX_COMMAND_LEN+1];
			if (pop_token(buffer->fp, t, "ending command ->") == EOF) {
				p_error(buffer->fp, "Unexpected EOF parsing end token\n");
				return;
			}
			if (!strcmp(t, end_token)) {
				if (fake_reach)
				{
				if (destroy_river_segment(reach))
					p_error(buffer->fp, "Could not destroy the reach segment\n", 0);
				else
					p_error(buffer->fp, "\tDiscarded the data for reach %s\n", reach_name);
				}
				reset_input(buffer->fp);
				return;
			}
			else {
				reset_input(buffer->fp);
				p_error(buffer->fp, "End token found in %s for %s, ignoring line\n", end_token, t);
			}
		}
		break;
	default:
	    UNKNOWN_COMMAND_ERROR (buffer->fp, token);
	}
    }  /* end while */
}


/*------------------------------------------------------------------------*/
/* parse  release puts the appropriate numbers in the apprpriate places
   in a record that is passed to it.

   There is no return value */
/*------------------------------------------------------------------------*/
static void parse_release (buffer, rls, run_index, end_token)
    PARSER_FILE *buffer;
    struct release *rls;
    int run_index;
	const char *end_token;
{
    char token[MAX_COMMAND_LEN+1];
    static COMMAND_LIST release_list=0;

    /* set up the release list menu */
    if (!release_list)
    {
        release_list = com_create_list(release_list, release_tokens);
    }

    /* parse all the rest of the stuff */
    while (1)
    {
        if (pop_token(buffer->fp, token, "release command -> ") == EOF)
        {
            if (can_return_from(buffer))
	    {
                buffer = return_after_parse(buffer);
		continue;
	    }
            else
            {
		P_WARNING("encountered EOF before end statement while parsing run %d\n",
			run_index);
		reset_input(buffer->fp);
		return;
            }
        }
	switch (com_reference(release_list, lowercase(token, scratch1)))
	{
	case TK_RLS_VIT:
	    if (!read_float(buffer->fp,
			      &rls->vitality_at_release,
			      "vitality at release ->"))
		P_ERROR("Parsing float for vitality at release for release at %s.\n",
			rls->site->name);
	    break;
	case TK_RLS_VIT_CHANGE:
	    if (!read_float(buffer->fp,
			      &rls->vitality_change_rate,
			      "vitality change rate ->"))
		P_ERROR("Parsing float for vitality change rate for release at %s.\n",
			rls->site->name);
	    break;
	case TK_INITIAL_SPILL_EXPERIENCE:
	    if (!read_float(buffer->fp,
			      &rls->initial_spill_experience,
			      "initial spill experience ->"))
		P_ERROR("Parsing float for initial spill experience for release at %s.\n",
			rls->site->name);
	    break;
	case TK_RLS_LENGTH:
	    if (!read_float(buffer->fp,
			      &rls->length,
			      "length at release ->"))
		P_ERROR("Parsing float for length at release for release at %s.\n",
			rls->site->name);
	    break;
	case TK_RLS_AGE:
	    if (!read_int(buffer->fp,
			    &rls->smolt_onset,
			    "age at release ->"))
		P_ERROR("Parsing float for age for release at %s.\n",
			rls->site->name);
	    log_msg(L_WARNING, "outdated token {%s}, using as %s\n",
		    TKS_RLS_AGE, TKS_RLS_SMOLT_ON);
	    break;
	case TK_RLS_SMOLT_ON:
	    if (!read_int(buffer->fp,
			    &rls->smolt_onset,
			    "smoltification onset ->"))
		P_ERROR("Parsing float for smoltification onset for release at %s.\n",
			rls->site->name);
	    break;
	case TK_RLS_SMOLT_OFF:
	    if (!read_int(buffer->fp,
			    &rls->smolt_finish,
			    "smoltification onset ->"))
		P_ERROR("Parsing float for smoltification finish for release at %s.\n",
			rls->site->name);
	    break;
	  case TK_STOCK:
	    /* set the stock ptr */
	    if (pop_token(buffer->fp, token, "stock -> ") != EOF) {
		int stock_index;
		if ((stock_index = ref_stock(unders_to_spaces (token))) < 0) {
		    /* if an unidentified stock is Generic, take no action.
		     * otherwise, output error message.
		     */
		    if (strcmp(token, "Generic")) {
			char tmp[256];
			sprintf(tmp, "unknown stock {%s} for release at %s: line discarded\n",
				token, rls->site->name);
			P_ERROR ("%s", tmp);
		    }
		    break;
		}
		else 
		    rls->stock = stocks.s[stock_index];
	    }
	    else {
		P_WARNING ("unexpected EOF while %s\n",
			   "looking for stock name.");
	    }
	    break;    
#ifdef REALTIME	    
	  case TK_RT_STOCK:
	    /* this token is used to pick up the realtime data. */
	    if (pop_token(buffer->fp, token, "rt stock name -> ") != EOF) {
		if (!rls->rt)
		    rls->rt = construct_rtinfo(token);
		else
		    rt_reset_name(rls->rt, token);
	    }
	    else {	
		P_WARNING ("unexpected EOF while %s\n",
			 "looking for release realtime stock name.");
		
	    }
	    break;
	  case TK_RT_DATA_DATE:
	    if (!rls->rt)
		rls->rt = construct_rtinfo("");
	    if (!read_int(buffer->fp,
			  &rls->rt->data_date,
			  "rt data date ->"))
		P_ERROR("Parsing float for rt data date for release at %s.\n",
			rls->site->name);
	    break;
#endif	  
	   case TK_RLS_ADD:
	    if (!read_int(buffer->fp,
			    &rls->add_setting,
			    "add setting ->"))
		P_ERROR("Parsing int for add setting at %s.\n",
			rls->site->name);
	    break;
	case TK_RLS_NUM:
        version_check( buffer, TKS_RLS_NUM );
	    if (buffer->version < 1)
	    {
		if (!read_float_array(buffer->fp,
				      rls->number,
				      STEPS_IN_RUN,
				      ACT_NO_CONV,
				      1,
				      "number released ->"))
		    P_ERROR("Parsing float array for number released at %s\n",
			    rls->site->name);
	    }
	    else
	    {
			if(rls->add_setting==1){
				if (!read_float_array(buffer->fp,
					      rls->number,
					      DAYS_IN_RUN,
					      ACT_HALF,
					      STEPS_PER_DAY,
					      "number released ->"))
			    P_ERROR("Parsing float array for number released at %s\n",
				    rls->site->name);
			}else{
				if (!read_float_array(buffer->fp,
					      rls->number,
					      DAYS_IN_RUN,
					      ACT_SPLIT,
					      STEPS_PER_DAY,
					      "number released ->"))
			    P_ERROR("Parsing float array for number released at %s\n",
				    rls->site->name);
			}
	    }
	    break;
	case TK_INCLUDE:
	    buffer = include_to_parse(buffer);
	    break;
	case TK_END:
		{
			char t[MAX_COMMAND_LEN+1];
			if (pop_token(buffer->fp, t, "ending command ->") == EOF) {
				p_error(buffer->fp, "Unexpected EOF parsing end token\n");
				return;
			}
			if (!strcmp(t, end_token)) {
				reset_input(buffer->fp);
				return;
			}
			else {
				reset_input(buffer->fp);
				p_error(buffer->fp, "End token found in %s for %s, ignoring line\n", end_token, t);
			}
		}
		break;
	default:
	    UNKNOWN_COMMAND_ERROR (buffer->fp, token);
	} /* switch */
    } /* while */
}

static void parse_mod_release (buffer, run_index, end_token)
    PARSER_FILE *buffer;
    int run_index;
	const char *end_token;
{
    char token[MAX_COMMAND_LEN+1];

    int spec_index, start_date;
    
    struct release *rls;
    struct release_site *site;

    /* set the species ptr */
    if (pop_token(buffer->fp, token, "species -> ") != EOF)
    {
	if ( (spec_index = ref_spec ( unders_to_spaces ( token ) )) < 0 )
	{
	    P_ERROR ("unknown species {%s}: line discarded\n",
		     token);
	    reset_input(buffer->fp);
	    return;
	}
    }
    else
    {
	P_WARNING ("unexpected EOF while %s\n", "looking for species name.");
	return;
    }
    
    /* set the site ptr */
    if (pop_token(buffer->fp, token, "site -> ") != EOF)
    {
	if ((site = find_site(release_site, unders_to_spaces ( token ) )) == 0)
	{
	    P_ERROR ("unknown site {%s}: line discarded\n",
		     token);
	    reset_input(buffer->fp);
	    return;
	}	
    }
    else
    {
	P_ERROR ("unexpected EOF while %s\n", "looking for site name.");
	return;
    }

    /* parse the starting date */
    if ( !(read_int(buffer->fp, &start_date, "starting date ->")) )
    {
	P_ERROR ("Parsing starting date for release at %s: line discarded.\n",
		 site->name);
	return;
    }

    if ((rls = find_exact_release(run[run_index].release_list, &specs[spec_index], site, start_date)) == 0)
    {
	P_ERROR ("unknown release at {%s}: discarding line.\n You should get a large number of error messeges.\n",
		 site->name);
	reset_input(buffer->fp);
	return;
    }

    log_msg(L_DBGMSG, "Modifying release for %s at %s on %d\n",
	    specs[spec_index].name,
	    site->name,
	    start_date);

    parse_release (buffer, rls, run_index, end_token);
}

/*------------------------------------------------------------------------*/
/* parse add release creates a release record and initializes the record to
   some default values, and allocates space for the rel_seg-> list. It
   then calls parse_release.

   There is no return value */
/*------------------------------------------------------------------------*/
static void parse_add_release (buffer, run_index, end_token)
    PARSER_FILE *buffer;
    int run_index;
	const char *end_token;
{
    char token[MAX_COMMAND_LEN+1];

    int fake_release = 0;
    int number=0;
    struct release *rls;

    /* allocate space for the release */
    rls = construct_release();
    /* set to default values */
    copy_release(NULL, rls);
 
    /* set the species ptr */
    if (pop_token(buffer->fp, token, "species -> ") != EOF)
	if ((number = ref_spec ( unders_to_spaces ( token ) )) >= 0)
	{
	    rls->species = &specs[number];
	    rls->stock = &rls->species->stock;
	}
	else
	{
	    P_ERROR ("unknown species {%s}: data discarded\n",
		     token);
	    fake_release=1;
	    rls->species = &specs[0];
	    rls->stock = &rls->species->stock;
	}
    else
    {
	P_WARNING ("unexpected EOF while %s\n", "looking for species name.");
	delete_release (rls);
	return;
    }
    
    /* set the site ptr */
    if (pop_token(buffer->fp, token, "site -> ") != EOF)
    {
	if ((rls->site = find_site(release_site, unders_to_spaces ( token ) )) == 0)
	{
	    P_ERROR ("unknown site {%s}: data discarded\n",
		     token);
	    fake_release = 1;
	    rls->site = release_site;
	}
    }
    else
    {
	P_ERROR ("unexpected EOF while %s\n", "looking for site name.");
	delete_release(rls);
	return;
    }

    /* parse the starting date */
    if ( !(read_int(buffer->fp, &rls->start_date, "starting date ->")) )
    {
	P_ERROR ("Parsing starting date for release at %s: release not created.\n",
		 rls->site->name);
	delete_release(rls);
	return;
    }

    log_msg(L_DBGMSG, "Parsing release for %s at %s on %d\n",
	    rls->species->name,
	    rls->site->name,
	    rls->start_date);

    /* set up the rls_seg stuff and add release to list */
    current = &run[run_index];
    parse_release (buffer, rls, run_index, end_token);
    activate_release(rls);
    if (fake_release) delete_release(rls);
}

/* Returns reach class number */
int read_reach_class( buffer, name )
    PARSER_FILE *buffer;
    char *name;
{
    char token[MAX_COMMAND_LEN+1];
    int i;

    /* Read the next token and see if it is a number, or a reach class
     *  name. If neither, it is an error.
     */
    if (pop_token(buffer->fp, token, name) == EOF) { 
        P_WARNING("unexpected end of file\n", "");
        return -1;
    }

	if (is_float(token)) {
        /* Accept number as parameter value for all reach classes. */
        P_WARNING( 
            "obsolete syntax, accepting %s value for all reach classes\n",
            name );
        i = 0;
		push_last_token(buffer->fp);
		return 0;
	}

    /* Check for valid reach class name */
    for (i = 0; i < num_reach_classes; i++)
        if (strcmp( token, reach_class_names[i] ) == 0)
            break;
    if (i == num_reach_classes) {
        /* Accept number as parameter value for all reach classes. */
        p_error(buffer->fp, 
            "unknown reach class %s for %s, using for all\n",
			token,
            name );
        i = 0;
    }

	return i;
}

/* Helper function for parsing species and stock level equations which
 *  are known by reach class. NOTE that first reach class is special:
 *  an equation in that class is applied to all classes. Returned value
 *  is -1 on error, else reach class that was actually read.
 */
int read_reach_class_equation( buffer, equations, name )
    PARSER_FILE *buffer;
    equation *equations;
    char *name;
{
    char token[MAX_COMMAND_LEN+1];
    int id, rc;
    equation temp_eqn;

    /* Read the next token and see if it is a number, or a reach class
     *  name. If neither, it is an error.
     */
    if (pop_token(buffer->fp, token, name) == EOF) { 
        P_WARNING("unexpected end of file\n", "");
        return -1;
    }

    if (is_int( token )) {
        /* Accept number as equation id - apply this equation to all
         *  reach classes. */
        P_WARNING( 
            "obsolete syntax, accepting %s equation for all reach classes\n",
            name );
        rc = 0;
    }
    else {
        /* Check for valid reach class name */
        for (rc = 0; rc < num_reach_classes; rc++)
            if (strcmp( token, reach_class_names[rc] ) == 0)
                break;
        if (rc == num_reach_classes) {
            P_ERROR("invalid reach class %s found, equation discarded\n",
                      token );
            pop_token( buffer->fp, token, name );
            parse_equation( buffer->fp, &temp_eqn, token, name );
            return -1;
        }

        /* get equation id token. */
        if (pop_token(buffer->fp, token, name) == EOF) { 
            P_WARNING("unexpected end of file\n", "");
            return -1;
        }
    }

	/* EQ_MIGR2 equation id has changed */
	id = atoi(token);
	if (buffer->version < 5) {
		if (id == 29) {
		    sprintf(token, "%d", EQ_MIGR2);
		    p_warning(buffer->fp,"migration eqn number has changed.  Updating from %d to %d.\n", id, EQ_MIGR2);
		}
		else if (id == 32) {
		    sprintf(token, "%d", EQ_PRED_TEMP1);
		    p_warning(buffer->fp,"predator temperature response eqn number has changed.  Updating from %d to %d.\n", id, EQ_PRED_TEMP1);
		}
		else if (id == 33) {
		    sprintf(token, "%d", EQ_PRED_TEMP2);
		    p_warning(buffer->fp,"predator temperature response eqn number has changed.  Updating from %d to %d.\n", id, EQ_PRED_TEMP2);
    	}
    }

    /* Actually read equation. */
    parse_equation( buffer, &(equations[rc]), token, name );
    return rc;
}

/*------------------------------------------------------------------------*/
/* parse species puts the appropriate numbers in the appropriate places in
   the record corresponding to the index species.  It is assumed that
   species is an accurate index, or -1 indicating that an unrecognized dummy
   species is to be parsed.  Warning messages are printed for
   syntax errors.
   
   There is no return value */
/*------------------------------------------------------------------------*/
static void parse_species (buffer, species, name, end_token)
    PARSER_FILE *buffer;
    int species;
    char *name;
	const char *end_token;
{
    char token[MAX_COMMAND_LEN+1];
    static COMMAND_LIST spec_list=0;
    struct species *pspec;
    float f0;
    int id, rc;

    if (!spec_list)
    {
	spec_list = com_create_list(spec_list, species_tokens);
    }

    if (species == -1)
    {
	log_msg(L_DBGMSG, "parsing unknown species %s\n", name);
	if (!(pspec = (struct species *) malloc(sizeof(struct species))))
	{			      /* freed on exit from this routine */
	    log_msg(L_ERROR, "no mem to parse unknown species\n");
	    return;
	}
    }
    else
    {
	log_msg(L_DBGMSG, "parsing species %s\n", name);
	pspec = &specs[species];
    }

    while (1)
    {
        if (pop_token(buffer->fp, token, "species command -> ") == EOF)
        {
            if (can_return_from(buffer))
	    {
                buffer = return_after_parse(buffer);
		continue;
	    }
            else
            {
		P_WARNING("encountered EOF before end statement while parsing species %s\n",
			name);
		reset_input(buffer->fp);
		if (species == -1)
		    free(pspec);
		return;
            }
        }
	switch (com_reference(spec_list, lowercase(token, scratch1)))
	{
	case TK_VVAR1:
        PARSE_REACH_CLASS_FLOAT(vvar, &pspec->stock, species_stock_masks[species], "vvar");
	    break;
	case TK_GMORT_EQUATION:
	    if (pop_token(buffer->fp, token, "equation id ->") == EOF)
	    {
	        p_warning(buffer->fp,
			  "unexpected EOF while looking for equation id. species = %s\n",
			  name);
		break;
	    }
	    {
		int eqid = atoi(token);
		if (eqid==EQ_GMORT0 || eqid==EQ_GMORT1 || eqid==EQ_GMORT2) {
		    equation tmp_eqn;
		    P_ERROR("obsolete gas mort eqn for species %s, discarding\n",
			    name);
		    copy_equation(find_default_eq(EQ_GMORT3), &pspec->gmort_eqn);
		    parse_equation(buffer, &tmp_eqn, token, TKS_GMORT_EQUATION);
		    break;
		}
	    }
	    parse_equation(buffer, &pspec->gmort_eqn, token, TKS_GMORT_EQUATION);
	    break;
	case TK_FDENS_EQUATION:
	    if (pop_token(buffer->fp, token, "equation id ->") == EOF)
	    {
	        p_warning(buffer->fp,
	 "unexpected EOF while looking for equation id. species = %s\n",
	 name);
		break;
	    }
	    parse_equation(buffer, &pspec->fdens_eqn, token, TKS_FDENS_EQUATION);
	    break;
	case TK_MIGR_EQUATION:
        version_check( buffer, TKS_MIGR_EQUATION );
        rc = read_reach_class_equation( buffer,
                    pspec->stock.migration_eqn, TKS_MIGR_EQUATION );
        if (rc < 0)
            break;
        id = pspec->stock.migration_eqn[rc].id;

        /* Migration equation #24 has had parameters shuffled. This is to
         *  get all of the real parameters to come before all of the display
         *  only parameters.
         */
        if (buffer->version <= 5 && id == EQ_MIGR) {
            float tval, tmin, tmax;
            equation *eq = &(pspec->stock.migration_eqn[rc]);
            
            /* old param 7 -> param 5;
               old param 5 -> param 6;
               old param 6 -> param 7 */
            tval = eq->param[5].val;
            tmin = eq->param[5].min;
            tmax = eq->param[5].max;

            eq->param[5].val = eq->param[7].val;
            eq->param[5].min = eq->param[7].min;
            eq->param[5].max = eq->param[7].max;

            eq->param[7].val = eq->param[6].val;
            eq->param[7].min = eq->param[6].min;
            eq->param[7].max = eq->param[6].max;

            eq->param[6].val = tval;
            eq->param[6].min = tmin;
            eq->param[6].max = tmax;
        }

		DEFAULT_OVERRIDE(species_stock_masks[species].migration_eqn, rc);
	    break;
	case TK_PRED_TR_EQUATION:
        version_check( buffer, TKS_PRED_TR_EQUATION );
        rc = read_reach_class_equation( buffer,
                    pspec->stock.pred_temp_response_eqn,
                    TKS_PRED_TR_EQUATION );
        if (rc < 0)
            break;

		DEFAULT_OVERRIDE(species_stock_masks[species].pred_temp_response_eqn, rc);
	    break;
	case TK_INRIVER_RETURN_EQUATION:
		if (pop_token(buffer->fp, token, "equation id ->") == EOF)
		{
		    p_warning(buffer->fp,
			  "unexpected EOF while looking for equation id. (%s)\n",
			  pspec->name);
		break;
		}
        parse_equation( buffer,
                    &pspec->stock.inriver_return_eqn,
                    token, TKS_INRIVER_RETURN_EQUATION );
	    break;
	case TK_TRANSPORT_RETURN_EQUATION:
		if (pop_token(buffer->fp, token, "equation id ->") == EOF)
		{
		    p_warning(buffer->fp,
			  "unexpected EOF while looking for equation id. (%s)\n",
			  pspec->name);
		break;
		}
        parse_equation( buffer,
                    &pspec->stock.transport_return_eqn,
                    token, TKS_TRANSPORT_RETURN_EQUATION );
	    break;
	case TK_INRIVER_LATENT_MORT_EQUATION:
		if (pop_token(buffer->fp, token, "equation id ->") == EOF)
		{
		    p_warning(buffer->fp,
			  "unexpected EOF while looking for equation id. (%s)\n",
			  pspec->name);
		break;
		}
        parse_equation( buffer,
                    &pspec->inriver_latent_mort_eqn,
                    token, TKS_INRIVER_LATENT_MORT_EQUATION );
	    break;
	case TK_INRIVER_LATENT_MORT:
		PARSE_FLOAT_OR_DIST(pspec->inriver_latent_mort,
			"latent inriver mortality ->",
			"Parsing float for latent inriver mortality for species %s.\n",
			name)
	    break;
	case TK_TRANSPORT_LATENT_MORT:
		PARSE_FLOAT_OR_DIST(pspec->transport_latent_mort,
			"latent transport mortality ->",
			"Parsing float for latent transport mortality for species %s.\n",
			name)
	    break;
	case TK_DIFFERENTIAL_RETURN:
		PARSE_FLOAT_OR_DIST(pspec->differential_return,
			"differential return ->",
			"Parsing float for differential return for species %s.\n",
			name)
	    break;
	case TK_PRED_CAPTURE_DIST_EQN:
        rc = read_reach_class_equation( buffer,
                    pspec->stock.pred_capture_dist_eqn,
                    TKS_PRED_CAPTURE_DIST_EQN );
        if (rc < 0)
            break;

		DEFAULT_OVERRIDE(species_stock_masks[species].pred_capture_dist_eqn, rc);
	    break;
	case TK_CUSTOM_SURVIVAL_EQN:
        rc = read_reach_class_equation( buffer,
                    pspec->stock.custom_survival_eqn,
                    TKS_CUSTOM_SURVIVAL_EQN );
        if (rc < 0)
            break;

		DEFAULT_OVERRIDE(species_stock_masks[species].custom_survival_eqn, rc);
	    break;
	case TK_VB:
	    P_WARNING("obsolete token {%s}.  Data discarded.\n", TKS_VB);
	    (void) skip_numbers(buffer->fp, STEPS_IN_SEASON);
	    break;
	case TK_VVAR:
	    P_WARNING("obsolete token {%s}.  Data discarded.\n", TKS_VVAR);
	    (void) skip_numbers(buffer->fp, STEPS_IN_SEASON);
	    break;
	case TK_DRIFT:
	    P_WARNING("obsolete token {%s}.  Data discarded.\n", TKS_DRIFT);
	    (void) skip_numbers(buffer->fp, STEPS_IN_SEASON);
	    break;
	case TK_PCOEF_MN:
	    p_warning(buffer->fp,
		"obsolete token {%s}.  Data discarded.\n\t Separate into reach, forebay, and tailrace\n",
		    TKS_PCOEF_MN);
	    (void) skip_numbers(buffer->fp, 1);
	    break;
	case TK_PCOEF_VAR:
	    p_warning(buffer->fp,
		    "obsolete token {%s}.  Data discarded.\n",
		    TKS_PCOEF_VAR);
	    (void) skip_numbers(buffer->fp, 1);
	    break;
	case TK_PCOEF_LOW:
	    p_warning(buffer->fp,
		"obsolete token {%s}.  Data discarded.\n\t Separate into reach, forebay, and tailrace\n",
		    TKS_PCOEF_LOW);
	    (void) skip_numbers(buffer->fp, 1);
	    break;
	case TK_PCOEF_HIGH:
	    p_warning(buffer->fp,
		"obsolete token {%s}.  Data discarded.\n\t Separate into reach, forebay, and tailrace\n",
		    TKS_PCOEF_HIGH);
	    (void) skip_numbers(buffer->fp, 1);
	    break;

    case TK_RPCOEF:
		PARSE_REACH_CLASS_FLOAT_OR_DIST(reach_pcoef,
			pspec,
			species_masks[species],
			name);
	    break;

	case TK_RPCOEF_V15_MEAN:
	    p_warning(buffer->fp,
		"obsolete token {%s}.  Parsing as {%s}.\n",
		    TKS_RPCOEF_V15_MEAN, TKS_RPCOEF_MEAN);
        /* fall-through is intentional. */
    case TK_RPCOEF_MEAN:
        version_check( buffer, TKS_RPCOEF_MEAN );
		PARSE_REACH_CLASS_BROKEN_STICK_DIST_PARAM(reach_pcoef,
			pspec,
			species_masks[species],
			mean,
			"reach pred coefficient mean");
	    break;

	case TK_RPCOEF_V15_LOW:
	    p_warning(buffer->fp,
		"obsolete token {%s}.  Parsing as {%s}.\n",
		    TKS_RPCOEF_V15_LOW, TKS_RPCOEF_LOW);
        /* fall-through is intentional. */
    case TK_RPCOEF_LOW:
        version_check( buffer, TKS_RPCOEF_LOW );
		PARSE_REACH_CLASS_BROKEN_STICK_DIST_PARAM(reach_pcoef,
			pspec,
			species_masks[species],
			low,
			"reach pred coefficient low");
	    break;

	case TK_RPCOEF_V15_HIGH:
	    p_warning(buffer->fp,
		"obsolete token {%s}.  Parsing as {%s}.\n",
		    TKS_RPCOEF_V15_HIGH, TKS_RPCOEF_HIGH);
        /* fall-through is intentional. */
    case TK_RPCOEF_HIGH:
        version_check( buffer, TKS_RPCOEF_HIGH );
		PARSE_REACH_CLASS_BROKEN_STICK_DIST_PARAM(reach_pcoef,
			pspec,
			species_masks[species],
			high,
			"reach pred coefficient high");
	    break;

    case TK_RPCOEF_V16_MEAN:
	    p_warning(buffer->fp,
	          "obsolete token {%s}, ignoring\n", TKS_RPCOEF_V16_MEAN );
	    (void) skip_numbers(buffer->fp, 1);
	    break;
    case TK_RPCOEF_V16_LOW:
	    p_warning(buffer->fp,
	          "obsolete token {%s}, ignoring\n", TKS_RPCOEF_V16_LOW );
	    (void) skip_numbers(buffer->fp, 1);
	    break;
    case TK_RPCOEF_V16_HIGH:
	    p_warning(buffer->fp,
	          "obsolete token {%s}, ignoring\n", TKS_RPCOEF_V16_HIGH );
	    (void) skip_numbers(buffer->fp, 1);
	    break;    
    case TK_PRED_MORT_A:
	    P_WARNING("obsolete token {%s}.  Data discarded.\n", TKS_PRED_MORT_A);
	    (void) skip_numbers(buffer->fp, 1);
        break;
	case TK_PRED_MORT_V16_B:
	    P_WARNING("obsolete token {%s}.  Data discarded.\n", TKS_PRED_MORT_V16_B);
	    (void) skip_numbers(buffer->fp, 1);
        break;
	case TK_PPRIME_A:
        PARSE_REACH_CLASS_FLOAT(pprime_a, pspec, species_masks[species], "p' a coefficient");
        break;
	case TK_PPRIME_B:
        PARSE_REACH_CLASS_FLOAT(pprime_b, pspec, species_masks[species], "p' b coefficient");
        break;
	case TK_FPCOEF:
		PARSE_FLOAT_OR_DIST(pspec->forebay_pcoef,
			"forebay pred coefficient ->",
			"Parsing float for forebay pred coef for species %s.\n",
			name)
	    break;
	case TK_FPCOEF_MEAN:
		PARSE_BROKEN_STICK_DIST_PARAM(pspec->forebay_pcoef, mean,
			"forebay pred coefficient mean ->",
			"Parsing float for forebay pred coef mean for species %s.\n",
			name)
	    break;
	case TK_FPCOEF_LOW:
		PARSE_BROKEN_STICK_DIST_PARAM(pspec->forebay_pcoef, low,
			"forebay pred coefficient low ->",
			"Parsing float for forebay pred coef low for species %s.\n",
			name)
	    break;
	case TK_FPCOEF_HIGH:
		PARSE_BROKEN_STICK_DIST_PARAM(pspec->forebay_pcoef, high,
			"forebay pred coefficient high ->",
			"Parsing float for forebay pred coef high for species %s.\n",
			name)
	    break;
	case TK_TPCOEF:
		PARSE_FLOAT_OR_DIST(pspec->tailrace_pcoef,
			"tailrace pred coefficient ->",
			"Parsing float for tailrace pred coef for species %s.\n",
			name)
	    break;
	case TK_TPCOEF_MEAN:
		PARSE_BROKEN_STICK_DIST_PARAM(pspec->tailrace_pcoef, mean,
			"tailrace pred coefficient mean ->",
			"Parsing float for tailrace pred coef mean for species %s.\n",
			name)
	    break;
	case TK_TPCOEF_LOW:
		PARSE_BROKEN_STICK_DIST_PARAM(pspec->tailrace_pcoef, low,
			"tailrace pred coefficient low ->",
			"Parsing float for tailrace pred coef low for species %s.\n",
			name)
	    break;
	case TK_TPCOEF_HIGH:
		PARSE_BROKEN_STICK_DIST_PARAM(pspec->tailrace_pcoef, high,
			"tailrace pred coefficient high ->",
			"Parsing float for tailrace pred coef high for species %s.\n",
			name)
	    break;
	case TK_GMCOEF_MN:
	    p_warning(buffer->fp,
		    "obsolete token {%s}.  Data discarded.\n",
		    TKS_GMCOEF_MN);
	    (void) skip_numbers(buffer->fp, 1);
	    break;
	case TK_GMCOEF_VAR:
	    p_warning(buffer->fp,
		    "obsolete token {%s}.  Data discarded.\n",
		    TKS_GMCOEF_VAR);
	    (void) skip_numbers(buffer->fp, 1);
	    break;
	case TK_GMCOEF_LOW:
	    p_warning(buffer->fp,
		    "obsolete token {%s}.  Data discarded.\n",
		    TKS_GMCOEF_LOW);
	    (void) skip_numbers(buffer->fp, 1);
	    break;
	case TK_GMCOEF_HIGH:
	    p_warning(buffer->fp,
		    "obsolete token {%s}.  Data discarded.\n",
		    TKS_GMCOEF_HIGH);
	    (void) skip_numbers(buffer->fp, 1);
	    break;
	case TK_GMEXP:
	    p_warning(buffer->fp,
		    "obsolete token {%s}.  Data discarded.\n",
		    TKS_GMEXP);
	    (void) skip_numbers(buffer->fp, 1);
	    break;
	case TK_GMTHRESH:
	    p_warning(buffer->fp,
		    "obsolete token {%s}.  Data discarded.\n",
		    TKS_GMTHRESH);
	    (void) skip_numbers(buffer->fp, 1);
	    break;
	case TK_GM_LO_SLOPE:
	    if (!read_float(buffer->fp,
			    &f0,
			    "gas mortality low slope ->"))
		P_ERROR(
		    "Parsing float for gas mort low slope for species %s.\n",
		    name);
	    log_msg(L_WARNING, "obsolete token {%s}.  Attempting to update.\n",
		    TKS_GM_LO_SLOPE);
	    if (pspec->gmort_eqn.id != EQ_GMORT0)
		copy_equation(find_default_eq(EQ_GMORT0), &pspec->gmort_eqn);
	    pspec->gmort_eqn.gm_mlow = f0*0.0001;
	    break;
	case TK_GM_HI_SLOPE:
	    if (!read_float(buffer->fp,
			    &f0,
			    "gas mortality high slope ->"))
		P_ERROR(
		    "Parsing float for gas mort high slope for species %s.\n",
		    name);
	    log_msg(L_WARNING, "obsolete token {%s}.  Attempting to update.\n",
		    TKS_GM_HI_SLOPE);
	    if (pspec->gmort_eqn.id != EQ_GMORT0)
		copy_equation(find_default_eq(EQ_GMORT0), &pspec->gmort_eqn);
	    pspec->gmort_eqn.gm_mhi = f0*0.001;
	    break;
	case TK_GM_CRITICAL:
	    if (!read_float(buffer->fp,
			    &f0,
			    "gas mortality critical value ->"))
		P_ERROR(
		    "Parsing float for gas mort critical val for species %s.\n",
		    name);
	    log_msg(L_WARNING, "obsolete token {%s}.  Attempting to update.\n",
		    TKS_GM_CRITICAL);
	    if (pspec->gmort_eqn.id != EQ_GMORT0)
		copy_equation(find_default_eq(EQ_GMORT0), &pspec->gmort_eqn);
	    pspec->gmort_eqn.gm_crit = f0;
	    break;
	case TK_NOON_DIST:
	    log_msg( L_WARNING, 
            "obsolete token {%s} for species %s.  Data discarded.\n",
                        TKS_NOON_DIST, name );
	    (void) skip_numbers(buffer->fp, 1);
	    break;
	case TK_NIGHT_DIST:
	    log_msg( L_WARNING, 
            "obsolete token {%s} for species %s.  Data discarded.\n",
                        TKS_NIGHT_DIST, name );
	    (void) skip_numbers(buffer->fp, 1);
	    break;
	case TK_DELAY_CONST:
	    log_msg( L_WARNING, 
            "obsolete token {%s} for species %s.  Data discarded.\n",
                        TKS_DELAY_CONST, name );
	    (void) skip_numbers(buffer->fp, 1);
	    break;
	case TK_DELAY_THRESH:
	    log_msg( L_WARNING, 
            "obsolete token {%s} for species %s.  Data discarded.\n",
                        TKS_DELAY_THRESH, name );
	    (void) skip_numbers(buffer->fp, 1);
	    break;

	case TK_MRCOEF:
		PARSE_REACH_CLASS_FLOAT_OR_DIST(mvcoef,
			&pspec->stock,
			species_stock_masks[species],
			name);
	    break;
	case TK_DVCOEF_MEAN:
	    log_msg(L_WARNING,
		    "obsolete token {%s}.  Parsing as {%s}.\n",
		    TKS_DVCOEF_MEAN, TKS_MRCOEF_MEAN);
        /* Fall through intentionally. */
	case TK_MRCOEF_MEAN:
		PARSE_REACH_CLASS_BROKEN_STICK_DIST_PARAM(mvcoef,
			&pspec->stock,
			species_stock_masks[species],
			mean,
			"migr variance coef mean");
	    break;
	case TK_DVCOEF_LOW:
	    log_msg(L_WARNING,
		    "obsolete token {%s}.  Parsing as {%s}.\n",
		    TKS_DVCOEF_LOW, TKS_MRCOEF_LOW);
        /* Fall through intentionally. */
	case TK_MRCOEF_LOW:
		PARSE_REACH_CLASS_BROKEN_STICK_DIST_PARAM(mvcoef,
			&pspec->stock,
			species_stock_masks[species],
			low,
			"migr variance coef low")
	    break;
	case TK_DVCOEF_HIGH:
	    log_msg(L_WARNING,
		    "obsolete token {%s}.  Parsing as {%s}.\n",
		    TKS_DVCOEF_HIGH, TKS_MRCOEF_HIGH);
        /* Fall through intentionally. */
	case TK_MRCOEF_HIGH:
		PARSE_REACH_CLASS_BROKEN_STICK_DIST_PARAM(mvcoef,
			&pspec->stock,
			species_stock_masks[species],
			high,
			"migr variance coef high")
	    break;
	case TK_TIME_COEFF:
        PARSE_REACH_CLASS_FLOAT(time_coeff, &pspec->stock, species_stock_masks[species], "predator mortality time coefficient");
        break;
	case TK_DISTANCE_COEFF:
        PARSE_REACH_CLASS_FLOAT(distance_coeff, &pspec->stock, species_stock_masks[species], "predator mortality distance coefficient");
        break;
	case TK_SIGMA_D:
        PARSE_REACH_CLASS_FLOAT(sigma_d, &pspec->stock, species_stock_masks[species], "reservoir survival error parameter");
        break;
	case TK_REACH_SURVIVAL_COEF:
        PARSE_REACH_CLASS_FLOAT(reach_survival_coef, &pspec->stock, species_stock_masks[species], "reach survival coefficient");
	    break;
	case TK_Y_PRED_CAL:
	    p_warning(buffer->fp,
		    "obsolete token {%s}.  Data discarded.\n",
		    TKS_Y_PRED_CAL);
	    (void) skip_numbers(buffer->fp, 1);
	    break;
	case TK_DRIFT_EQUATION:
	{
	    equation tmp_eqn;
	    
	    if (pop_token(buffer->fp, token, "equation id ->") == EOF)
	    {
		p_warning(buffer->fp,
			  "unexpected EOF while looking for equation id. species = %s\n",
			  name);
		break;
	    }
	    p_warning(buffer->fp,
		      "obsolete token {%s}.  Data discarded.\n",
		      TKS_DRIFT_EQUATION);
	    parse_equation(buffer, &tmp_eqn, token, TKS_DRIFT_EQUATION);
	    break;
	}
	case TK_INCLUDE:
	    buffer = include_to_parse(buffer);
	    break;
	case TK_END:
		{
			char t[MAX_COMMAND_LEN+1];
			if (pop_token(buffer->fp, t, "ending command ->") == EOF) {
				p_error(buffer->fp, "Unexpected EOF parsing end token\n");
				return;
			}
			if (!strcmp(t, end_token)) {
				reset_input(buffer->fp);
				if (species == -1)
				free(pspec);
				return;
			}
			else {
				reset_input(buffer->fp);
				p_error(buffer->fp, "End token found in %s for %s, ignoring line\n", end_token, t);
			}
		}
		break;
	default:
	    UNKNOWN_COMMAND_ERROR (buffer->fp, token);
	}
    }
}

/*------------------------------------------------------------------------*/
/* parse stock puts the appropriate numbers in the appropriate places in
   the record corresponding to the index stock.  It is assumed that
   stock is an accurate index, or -1 indicating that an unrecognized dummy
   stock is to be parsed.  Warning messages are printed for
   syntax errors.
   
   There is no return value */
/*------------------------------------------------------------------------*/
static void parse_stock (buffer, stock, name, end_token)
    PARSER_FILE *buffer;
    int stock;
    char *name;
	const char *end_token;
{
    char token[MAX_COMMAND_LEN+1];
    static COMMAND_LIST stock_list=0;
    StockT *pstock;
    int id, rc;

    if (!stock_list)
    {
	stock_list = com_create_list(stock_list, stock_tokens);
    }

    if (stock == -1)
    {
	log_msg(L_DBGMSG, "parsing unknown stock %s\n", name);
	if (!(pstock = construct_stock("dummy")))  /* freed on exit */
	{			      
	    log_msg(L_ERROR, "no mem to parse unknown stock\n");
	    return;
	}
    }
    else
    {
	log_msg(L_DBGMSG, "parsing stock %s\n", name);
	pstock = stocks.s[stock];
    }

    while (1)
    {
        if (pop_token(buffer->fp, token, "stock command -> ") == EOF)
        {
            if (can_return_from(buffer))
	    {
                buffer = return_after_parse(buffer);
		continue;
	    }
            else
            {
		P_WARNING("encountered EOF before end statement while parsing stock %s\n",
			name);
		reset_input(buffer->fp);
		if (stock == -1)
		    destroy_stock(&pstock);
		return;
            }
        }
	switch (com_reference(stock_list, lowercase(token, scratch1)))
	{
	case TK_VVAR1:
        PARSE_REACH_CLASS_FLOAT(vvar, pstock, stock_masks[stock], "vvar");
	    break;
	case TK_MIGR_EQUATION:
        version_check( buffer, TKS_MIGR_EQUATION );
        rc = read_reach_class_equation( buffer,
                    pstock->migration_eqn, TKS_MIGR_EQUATION );
        if (rc < 0)
            break;
        id = pstock->migration_eqn[rc].id;
        /* Migration equation #24 has had parameters shuffled. This is to
         *  get all of the real parameters to come before all of the display
         *  only parameters.
         */
        if (buffer->version <= 5 && id == EQ_MIGR) {
            float tval, tmin, tmax;
            equation *eq = &(pstock->migration_eqn[rc]);
            
            /* old param 7 -> param 5;
               old param 5 -> param 6;
               old param 6 -> param 7 */
            tval = eq->param[5].val;
            tmin = eq->param[5].min;
            tmax = eq->param[5].max;

            eq->param[5].val = eq->param[7].val;
            eq->param[5].min = eq->param[7].min;
            eq->param[5].max = eq->param[7].max;

            eq->param[7].val = eq->param[6].val;
            eq->param[7].min = eq->param[6].min;
            eq->param[7].max = eq->param[6].max;

            eq->param[6].val = tval;
            eq->param[6].min = tmin;
            eq->param[6].max = tmax;
        }

		DEFAULT_OVERRIDE(stock_masks[stock].migration_eqn, rc);
	    break;
	case TK_PRED_TR_EQUATION:
        version_check( buffer, TKS_PRED_TR_EQUATION );
        rc = read_reach_class_equation( buffer,
                    pstock->pred_temp_response_eqn,
                    TKS_PRED_TR_EQUATION );
        if (rc < 0)
            break;

		DEFAULT_OVERRIDE(stock_masks[stock].pred_temp_response_eqn, rc);
	    break;
	case TK_INRIVER_RETURN_EQUATION:
		if (pop_token(buffer->fp, token, "equation id ->") == EOF)
		{
		    p_warning(buffer->fp,
			  "unexpected EOF while looking for equation id. (%s)\n",
			  pstock->name);
		break;
		}
        parse_equation( buffer,
                    &pstock->inriver_return_eqn,
                    token, TKS_INRIVER_RETURN_EQUATION );
	    break;
	case TK_TRANSPORT_RETURN_EQUATION:
		if (pop_token(buffer->fp, token, "equation id ->") == EOF)
		{
		    p_warning(buffer->fp,
			  "unexpected EOF while looking for equation id. (%s)\n",
			  pstock->name);
		break;
		}
        parse_equation( buffer,
                    &pstock->transport_return_eqn,
                    token, TKS_TRANSPORT_RETURN_EQUATION );
	    break;
	case TK_PRED_CAPTURE_DIST_EQN:
        rc = read_reach_class_equation( buffer,
                    pstock->pred_capture_dist_eqn,
                    TKS_PRED_CAPTURE_DIST_EQN );
        if (rc < 0)
            break;

		DEFAULT_OVERRIDE(stock_masks[stock].pred_capture_dist_eqn, rc);
	    break;
	case TK_CUSTOM_SURVIVAL_EQN:
        rc = read_reach_class_equation( buffer,
                    pstock->custom_survival_eqn,
                    TKS_CUSTOM_SURVIVAL_EQN );
        if (rc < 0)
            break;

		DEFAULT_OVERRIDE(stock_masks[stock].custom_survival_eqn, rc);
	    break;
	case TK_MRCOEF:
		PARSE_REACH_CLASS_FLOAT_OR_DIST(mvcoef,
			pstock,
			stock_masks[stock],
			name);
	    break;
	case TK_MRCOEF_MEAN:
		PARSE_REACH_CLASS_BROKEN_STICK_DIST_PARAM(mvcoef,
			pstock,
			stock_masks[stock],
			mean,
			"migr variance coef mean");
	    break;
	case TK_MRCOEF_LOW:
		PARSE_REACH_CLASS_BROKEN_STICK_DIST_PARAM(mvcoef,
			pstock,
			stock_masks[stock],
			low,
			"migr variance coef low");
	    break;
	case TK_MRCOEF_HIGH:
		PARSE_REACH_CLASS_BROKEN_STICK_DIST_PARAM(mvcoef,
			pstock,
			stock_masks[stock],
			high,
			"migr variance coef high")
	    break;
	case TK_TIME_COEFF:
        PARSE_REACH_CLASS_FLOAT(time_coeff, pstock, stock_masks[stock], "predator mortality time coefficient");
        break;
	case TK_DISTANCE_COEFF:
        PARSE_REACH_CLASS_FLOAT(distance_coeff, pstock, stock_masks[stock], "predator mortality distance coefficient");
        break;
	case TK_SIGMA_D:
        PARSE_REACH_CLASS_FLOAT(sigma_d, pstock, stock_masks[stock], "reservoir survival error parameter");
        break;
	case TK_REACH_SURVIVAL_COEF:
        PARSE_REACH_CLASS_FLOAT(reach_survival_coef, pstock, stock_masks[stock], "reach survival coefficient");
	    break;
	case TK_INCLUDE:
	    buffer = include_to_parse(buffer);
	    break;
	case TK_END:
		{
			char t[MAX_COMMAND_LEN+1];
			if (pop_token(buffer->fp, t, "ending command ->") == EOF) {
				p_error(buffer->fp, "Unexpected EOF parsing end token\n");
				return;
			}
			if (!strcmp(t, end_token)) {
				reset_input(buffer->fp);
				if (stock == -1)
				destroy_stock(&pstock);
				return;
			}
			else {
				reset_input(buffer->fp);
				p_error(buffer->fp, "End token found in %s for %s, ignoring line\n", end_token, t);
			}
		}
		break;
	default:
	    UNKNOWN_COMMAND_ERROR (buffer->fp, token);
	}
    }
}

/*------------------------------------------------------------------------*/
/* parse_level0 is the base level parser.  It calls the appropriate fcns 
   for the run, stock, and species keywords.  
   Some warning messages are printed, but most checking is done in the
   called functions.
   
   There is no return value */
/*------------------------------------------------------------------------*/
static void parse_level0 (buffer)
    PARSER_FILE *buffer;
{
    char token[MAX_COMMAND_LEN+1];
    static COMMAND_LIST level0_list = 0;

    int		number=0;
    int		dummy, i, j;
    int		run_index = 0;	/* Legacy code */
    int		run_token_count = 0;

    if (!level0_list)
	level0_list = com_create_list(level0_list, level0_tokens);
    
    while (1)
    {
        if (pop_token(buffer->fp, token, "level0 command -> ") == EOF)
        {
            if (can_return_from(buffer))
	    {
                buffer = return_after_parse(buffer);
		continue;
	    }
            else
            {
		log_msg(L_DBGMSG, "end of file\n");
		return;
	    }
        }
        
	switch (com_reference(level0_list, lowercase(token, scratch1)))
	{
	    
	case TK_RUN:
	    
	    if (!read_int(buffer->fp, &dummy, "run ->"))
		P_ERROR("Parsing int for top-level %s.\n", TKS_RUN);
#if 0
	    if (pop_token(buffer->fp, token, "run -> ") != EOF)
	    {
		if (is_int(token))
		    parse_run (buffer, atoi(token));
		else
		{
		    P_ERROR (
	        "looking for a run number, found {%s}: line discarded\n",
		token);
		    reset_input(buffer->fp);
		}
	    }
	    else
		P_WARNING("unexpected EOF while %s\n",
			  "looking for run number.");
#endif
	    log_msg(L_WARNING, "outdated token {%s}, ignoring\n",
		    TKS_RUN);
	    ++run_token_count;
	    break;
	case TK_SPEC:
	    if (pop_token(buffer->fp, token, "species -> ") != EOF)
	    {
		if ((number = ref_spec ( unders_to_spaces ( token ) )) >= 0)
		    parse_species (buffer, number, token, TKS_SPEC);
		else
		{
		    P_WARNING ("unknown species {%s}: skipping\n", token);
		    parse_species (buffer, -1, token, TKS_SPEC);
		}
	    }
	    else
		P_ERROR ("unexpected EOF while %s\n",
			 "looking for species name.");
	    break;
	case TK_STOCK:
	    if (pop_token(buffer->fp, token, "stock -> ") != EOF)
	    {
		if ((number = ref_stock ( unders_to_spaces ( token ) )) >= 0)
		    parse_stock (buffer, number, token, TKS_STOCK);
		else
		{
		    P_WARNING ("unknown stock {%s}: skipping\n", token);
		    parse_stock (buffer, -1, token, TKS_STOCK);
		}
	    }
	    else
		P_ERROR ("unexpected EOF while %s\n",
			 "looking for stock name.");
	    break;
	case TK_PRED_ACT:
	    P_WARNING("obsolete token {%s}.  Data discarded.\n", TKS_PRED_ACT);
	    (void) skip_numbers(buffer->fp, STEPS_IN_SEASON);
	    break;
	case TK_WTEMP:
	    P_WARNING("obsolete token {%s}.  Data discarded.\n", TKS_WTEMP);
	    (void) skip_numbers(buffer->fp, STEPS_IN_SEASON);
	    break;
	case TK_PRED_ACT_EXP: {
	    float val;
	    P_WARNING("obsolete token {%s}.  \n\tUsing value to update all pred temperature response equations.\n", TKS_PRED_ACT);
	    if (read_float(buffer->fp, &val, "pred activity exp ->")) {
    		int i=0;
            equation *defeq;
            defeq = find_default_eq(EQ_PRED_TEMP1);
            for (i=0; i < num_specs; ++i) {
                for (j = 0; j < num_reach_classes; j++) {
                    if (!DEFAULT_OVERRIDDEN(species_stock_masks[i].pred_temp_response_eqn, j))
						copy_equation(defeq, 
                        &(specs[i].stock.pred_temp_response_eqn[j]));
                    specs[i].stock.pred_temp_response_eqn[j].TEMP_RESP_B_VAL
                                      = val;
                }
            }
            for (i=0; i < stocks.num; ++i) {
                for (j = 0; j < num_reach_classes; j++) {
                    if (!DEFAULT_OVERRIDDEN(stock_masks[i].pred_temp_response_eqn, j))
						copy_equation(defeq,
                        &(stocks.s[i]->pred_temp_response_eqn[j]));
		            stocks.s[i]->pred_temp_response_eqn[j].TEMP_RESP_B_VAL
                                      = val;
                }
            }
	    }
	    else {
		P_ERROR("Parsing float for pred act exp for run %d.\n",
			run_index); 
	    }
	    break;
	}
	case TK_NOTES:
		{
			/* Create a scratch variable here since the notes section may be big */
			char *notes_scratch = malloc(sizeof(char) * MAX_NOTES_SIZE);
			if (notes_scratch) {
				if (pop_sized_token(buffer->fp, notes_scratch, "notes ->", MAX_NOTES_SIZE) != EOF) {
					int notes_length = strlen(notes_scratch);
					if (notes_length < MAX_NOTES_SIZE) {
						if (input_notes) free(input_notes);
						/* The notes section may contain certain characters (like ") that we have escaped */
						input_notes = strdup_unescape(notes_scratch);
					}
					else log_msg(L_ERROR, "Notes section too long! Must be smaller than %d", MAX_NOTES_SIZE);
				}
				free(notes_scratch);
			}
		}
		break;
	case TK_FILE_CREATION_DATE:
		{
			if (pop_token(buffer->fp, token, "file creation date ->") != EOF) {
				if (file_creation_date) free(file_creation_date);
				file_creation_date = (char*) malloc(sizeof(char) * strlen(token) + 1);
				strcpy(file_creation_date, token);
			}
		}
		break;
	case TK_FILE_CREATOR:
		{
			if (pop_token(buffer->fp, token, "file creator ->") != EOF) {
				if (file_creator) free(file_creator);
				file_creator = strdup_unescape(token);
			}
		}
		break;
	case TK_FILE_MODIFICATION_DATE:
		{
			if (pop_token(buffer->fp, token, "file modification date ->") != EOF) {
				if (file_modification_date) free(file_modification_date);
				file_modification_date = (char*) malloc(sizeof(char) * strlen(token) + 1);
				strcpy(file_modification_date, token);
			}
		}
		break;
	case TK_FILE_MODIFIER:
		{
			if (pop_token(buffer->fp, token, "file modifier ->") != EOF) {
				if (file_modifier) free(file_modifier);
				file_modifier = strdup_unescape(token);
			}
		}
		break;
	case TK_GAS_DIS_EXP:
	    if (!read_float(buffer->fp,
			      &run[run_index].gas_dissp_exp,
			      "pred activity exp ->"))
		P_ERROR("Parsing float for gas dissp exp for run %d.\n",
			run_index);
	    break;
	case TK_FLOW_PROP:
	    if (!read_float(buffer->fp,
			      &run[run_index].flow_proportion,
			      "flow_proportion ->"))
		P_ERROR("Parsing float for flow proportion for run %d.\n",
			run_index);
	    break;
	case TK_UFREE:
	    if (!read_float(buffer->fp,
			      &run[run_index].Ufree,
			      "Ufree ->"))
		P_ERROR("Parsing float for Ufree for run %d.\n",
			run_index);
	    break;
	case TK_PREY_ENERGY_DENSITY:
	    if (!read_float(buffer->fp,
			      &run[run_index].prey_energy_density,
			      "prey energy density ->"))
		P_ERROR("Parsing float for prey energy density for run %d.\n",
			run_index );
	    break;
	case TK_LENGTH_WEIGHT_B0:
	    if (!read_float(buffer->fp,
			      &run[run_index].length_weight_b0,
			      "length-weight b0 ->"))
		P_ERROR("Parsing float for length-weight b0 for run %d.\n",
			run_index);
	    break;
	case TK_LENGTH_WEIGHT_B1:
	    if (!read_float(buffer->fp,
			      &run[run_index].length_weight_b1,
			      "length-weight b1 ->"))
		P_ERROR("Parsing float for length-weight b1 for run %d.\n",
			run_index);
	    break;
	case TK_FORK_THRESHOLD:
	    if (!read_float(buffer->fp,
			      &run[run_index].fork_threshold,
			      "fork_threshold ->"))
		P_ERROR("Parsing float for fork_threshold for run %d.\n",
			run_index);
	    break;
	case TK_MIN_MIGR_RATE:
	    if (!read_float(buffer->fp,
			      &run[run_index].min_migr_rate,
			      "min migration rate ->"))
		P_ERROR("Parsing minimum migration rate for %d.\n",
			run_index);
	    break;
	case TK_RUN_FLAGS:
	    if (!read_uns(buffer->fp,
			      &run[run_index].flags,
			      "run flags ->"))
		P_ERROR("Parsing int for run flags for run %d.\n",
			run_index);
		if (disable_variance_suppression) run[run_index].flags &= ~VAR_SUPPRESS_FLAG;
	    break;

    /* set how many reach classes we will have. */
    case TK_NUM_REACH_CLASSES:
        if (pop_token(buffer->fp, token, "number of reach classes ->") != EOF) {
            i = atoi( token );
            if (i <= 0 || i > 10) {
                p_error( buffer->fp,
                    "Number of reach classes must be [1..10]\n" );
                break;
            }
            set_num_reach_classes( i );
        }
        break;

    /* Set a given reach class name. */
    case TK_REACH_CLASS_NAME:
        if (!pop_token(buffer->fp, token, "reach class number ->" ) != EOF) {
            i = atoi( token );
            if (i < 0 || i >= num_reach_classes) {
                p_error( buffer->fp, "Reach class number must be [0..%d]\n",
                               num_reach_classes-1 );
                break;
            }
            if (!pop_token( buffer->fp, token, "reach class name ->" ) != EOF) {
                free( reach_class_names[i] );
                reach_class_names[i] = (char *)malloc( strlen( token ) + 2 );
                strcpy( reach_class_names[i], token );
            }
        }
        break;

	case TK_MORTALITY_CLASS:
        if (pop_token(buffer->fp, token, "mortality class ->") != EOF) {
            dummy = -1;
            for (i = 0; i < N_MORTALITY_CLASS; i++) {
                if (!strcasecmp( mortality_class_names[i], token )) {
                    dummy = i;
                    break;
                }
            }
            if (dummy == -1) {
                p_error (buffer->fp, 
                    "Invalid mortality class for run %d - using class %s.\n",
                            run_index, mortality_class_names[0] );
                dummy = 0;
            }
            
            run[run_index].mortality_class = dummy;
        }
	    break;
	case TK_POST_BONNEVILLE_HYPOTHESIS:
        if (pop_token(buffer->fp, token, "post-Bonneville hypothesis ->") != EOF) {
            dummy = -1;
            for (i = 0; i < NUM_POST_BONNEVILLE; i++) {
                if (!strcasecmp( post_bonneville_hypothesis_names[i], token )) {
                    dummy = i;
                    break;
                }
            }
            if (dummy == -1) {
                p_error (buffer->fp, 
                    "Invalid post-Bonneville hypothesis (%s) for run %d - using class %s.\n",
                            token, run_index, post_bonneville_hypothesis_names[0] );

				/* Print out the possible choices so the user knows if they mistyped something */
				log_msg(L_ERROR, "Allowed post-Bonneville hypotheses:\n");
				for (i = 0; i < NUM_POST_BONNEVILLE; i++) log_msg(L_ERROR, "\t%s\n", post_bonneville_hypothesis_names[i]);
                dummy = 0;
            }
            
            run[run_index].post_bonneville_hypothesis = dummy;
        }
	    break;
        case TK_FLOW_SEG:
	    {
		struct river_segment *dam = 0;
		if (pop_token(buffer->fp, token, "dam name ->") != EOF) {
		    if ( !(dam = find_seg_by_name(river_mouth, DAM_SEG,
						  unders_to_spaces(token) ))
			 && !(dam = find_seg_by_name(river_mouth,
						     POWERHOUSE_SEG,
						     unders_to_spaces(token)))
			 && strcmp(token, "none"))
		    {
			P_WARNING ("%s does not exist: proceeding with no flow segment indicator.\n",
				   token);
			current->flow_seg = (struct river_segment *) 0;
		    }
		    else
			current->flow_seg = dam;
		}
	    }
	    break;
        case TK_WATER_TT_UPPER:
	    {
		struct river_segment *seg = 0;
		if (pop_token(buffer->fp, token, "reach name ->") != EOF) {
		    if ( !(seg = find_seg_by_name(river_mouth, REACH_SEG,
						  unders_to_spaces(token) ))
			 && !(seg = find_seg_by_name(river_mouth,
						     DAM_SEG,
						     unders_to_spaces(token))))
		    {
			P_WARNING ("%s not found: proceeding with default water travel segment indicator.\n",
				   token);
		    }
		    else
			current->water_travel.upper = seg;
	    }
		}
	    break;
        case TK_WATER_TT_LOWER:
	    {
		struct river_segment *seg = 0;
		if (pop_token(buffer->fp, token, "reach name ->") != EOF) {
		    if ( !(seg = find_seg_by_name(river_mouth, REACH_SEG,
						  unders_to_spaces(token) ))
			 && !(seg = find_seg_by_name(river_mouth,
						     DAM_SEG,
						     unders_to_spaces(token))))
		    {
			P_WARNING ("%s not found: proceeding with default water travel segment indicator.\n",
				   token);
		    }
		    else
			current->water_travel.lower = seg;
	    }
		}
	    break;
	case TK_WATER_TT_FIRST_DAY:
	    if (!read_int(buffer->fp,
			  &run[run_index].water_travel.first_day,
			      "water travel first day ->"))
		P_ERROR("Parsing int for water tt first day for run %d.\n",
			run_index);
	    break;
	case TK_WATER_TT_LAST_DAY:
	    if (!read_int(buffer->fp,
			  &run[run_index].water_travel.last_day,
			      "water travel last day ->"))
		P_ERROR("Parsing int for water tt last day for run %d.\n",
			run_index);
	    break;
#ifdef REALTIME
	case TK_RT_REPORT_DATE:
	    if (pop_token(buffer->fp, token, "rt report date ->") != EOF)
		run[run_index].rt_report_date = strdup(token);
	    else
		P_ERROR ("unexpected EOF while looking for rt report date\n", 0);
	    break;
#endif	    
	case TK_MOD_RLS: /* modify a release */
	    parse_mod_release (buffer, run_index, TKS_MOD_RLS);
	    break;
	case TK_ADD_RLS: /* add a release */
	    parse_add_release (buffer, run_index, TKS_RLS);
	    break;
	case TK_REACH:
	    if (pop_token(buffer->fp, token, "reach name ->") != EOF)
		parse_reach (buffer, token, run_index, TKS_REACH);
	    else
		P_ERROR ("unexpected EOF while looking for reach name in run %d\n",
			 run_index);
	    break;
	case TK_HDWTR:
	    if (pop_token(buffer->fp, token, "headwater name ->") != EOF)
		parse_hdwtr (buffer, token, run_index, TKS_HDWTR);
	    else
		P_ERROR ("unexpected EOF while looking for headwater name in run %d\n",
			 run_index);
	    break;
	case TK_DAM:
	    if (pop_token(buffer->fp, token, "dam name ->") != EOF)
		parse_dam (buffer, token, run_index, TKS_DAM);
	    else
		P_ERROR ("unexpected EOF while looking for dam name in run %d\n",
			 run_index);
	    break;
	case TK_INCLUDE:
	    buffer = include_to_parse(buffer);
	    break;
	case TK_END:
	    if (run_token_count-- == 0) {
		log_msg(L_DBGMSG, "byebye\nso long\n");
		return;
	    } else {
		reset_input(buffer->fp);
	    }
	    break;
	case TK_VERSION:
	    if (pop_token(buffer->fp, token, "datafile version -> ") != EOF)
	    {
		if (is_int(token))
		{
		    buffer->version = atoi(token);
		    log_msg(L_DBGMSG, "Datafile version %d\n", buffer->version);
		}
		else
		{
		    P_ERROR (
		"looking for a version number, found {%s}: line discarded\n",
		token);
		    reset_input(buffer->fp);
		}
	    }
	    else
		P_ERROR ("unexpected EOF while %s\n",
			 "looking for datafile version.");
	    break;
	default:
	    UNKNOWN_COMMAND_ERROR(buffer->fp, token);
	}
    }
}

int parse_datafile(const char *filename,
				   int replace,	/* bool; delete data first if true */
				   int refresh)	/* bool; refresh wins after if true */
{
    PARSER_FILE *buffer;

    /* Check to see if the file can be opened */
    if (!(buffer = open_to_parse (filename)))
    {
	log_msg(L_ERROR, "Unable to open data file %s\n", filename);
	return 0;
    }
	close_after_parse(buffer);

	/* Reset data as necessary */

	/* create default value masks */
	init_masks();

    /* parse it */
    if (replace) {
		remove_all_data();
		reset_masks(); /* Initialize default value masks */

		/* Remove the notes section instead of just keeping it around */
		if (input_notes) {
			free(input_notes);
			input_notes = NULL;
		}
		if (file_creation_date) {
			free(file_creation_date);
			file_creation_date = NULL;
		}
		if (file_creator) {
			free(file_creator);
			file_creator = NULL;
		}
		if (file_modification_date) {
			free(file_modification_date);
			file_modification_date = NULL;
		}
		if (file_modifier) {
			free(file_modifier);
			file_modifier = NULL;
		}
	}

    /* open the file */
    if (!(buffer = open_to_parse (filename)))
    {
	log_msg(L_ERROR, "Unable to open data file %s\n", filename);
	return 0;
    }
    parse_comments(filename, buffer);
    parse_level0 (buffer);
    close_after_parse(buffer);

	fill_default_reach_class_values(); /* Fill in default values for reach classes */

    if (refresh)
    {
	extern void refresh_windows_from_data();
	
	data_check();			/* report data errors */
    set_planned_spill(); /* Convert old spill schedules to the new format */
	/* compute flow and related stats for all segments before refreshing */
    compute_all_flow();
    }

    return 1;
}
