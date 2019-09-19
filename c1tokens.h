/** \file
 * \brief Input file token definitions.
 *
 * This file defines all the tokens that are used in input files. Note that
 * since this code was written before enumerations were commonly in use in
 * C it just uses macro definitions for the C preprocessor. Of course the
 * problem with doing it this way is that one might accidentally give two
 * different tokens the same number and then the parser can't
 * disambiguate the two.
 *
 * A better design would probably use something like the report_init.c
 * structure to automatically do parsing based on classes of inputs (like
 * float, int, equation, etc.) rather than doing everything by hand as in
 * parse.c.
 *
 * Definitions starting with TK_ represent the ID of the token ad defs
 * starting with TKS_ represent the textual represntation of that token
 * as it would appear in an input file.
 *
 * Search this file for a number before creating a token with that number.
 */
/* definitions of array indices for token location */

/* level 0 tokens */
#define TK_RUN		1	/* monte carlo run */
#define TKS_RUN		"run"
#define TK_SPEC		2	/* species */
#define TKS_SPEC	"species"
#define TK_END		3
#define TKS_END		"end"
#define TK_INCLUDE	4
#define TKS_INCLUDE	"include"
/* equation stuff       5 */
#define TK_VERSION	6
#define TKS_VERSION	"version"
#define TK_STOCK	7	/* stock */
#define TKS_STOCK	"stock"
/* Notes section */
#define TK_NOTES	8	/* notes */
#define TKS_NOTES	"notes"
#define TK_FILE_CREATION_DATE	9	/* scenario/dat file creation date */
#define TKS_FILE_CREATION_DATE	"file_creation_date"
#define TK_FILE_CREATOR	10	/* scenario/dat file creator */
#define TKS_FILE_CREATOR	"file_creator"
#define TK_FILE_MODIFICATION_DATE	1009	/* scenario/dat file modification date */
#define TKS_FILE_MODIFICATION_DATE	"file_modification_date"
#define TK_FILE_MODIFIER	1010	/* scenario/dat file modifier */
#define TKS_FILE_MODIFIER	"file_modifier"

/* run subtokens */
#define TK_PRED_ACT	11	/* predator activity */
#define TKS_PRED_ACT	"pred_activity"
#define TK_ADD_RLS	12	/* release */
#define TK_MOD_RLS	13
#define TKS_RLS		"release"
#define TKS_ADD_RLS	"add_release"
#define TKS_MOD_RLS	"modify_release"
#define TK_REACH	14	/* reach */
#define TKS_REACH	"reach"
#define TK_HDWTR	15	/* headwater */
#define TKS_HDWTR	"headwater"
#define TK_DAM		16	/* dam */
#define TKS_DAM		"dam"
#define TK_PRED_ACT_EXP 17	/* predator activity exponent */
#define TKS_PRED_ACT_EXP "pred_act_exp"
#define TK_GAS_DIS_EXP 	18	/* gas dissipation exponent */
#define TKS_GAS_DIS_EXP "gas_dissp_exp"
#define TK_WTEMP 	19	/* water temperature */
#define TKS_WTEMP 	"water_temp"
#define TK_FLOW_PROP 	110	/* flow proportion */
#define TKS_FLOW_PROP 	"hw_flow_prop"
#define TK_RUN_FLAGS	112
#define TKS_RUN_FLAGS	"runtime_settings"
#define TK_UFREE 	113	/* free flowing stream velocity constant */
#define TKS_UFREE 	"ufree"
#define TK_FLOW_SEG	114
#define TKS_FLOW_SEG 	"flow_indicator_segment"
#define TK_WATER_TT_UPPER 115
#define TKS_WATER_TT_UPPER "water_travel_upper_segment"
#define TK_WATER_TT_LOWER 116
#define TKS_WATER_TT_LOWER "water_travel_lower_segment"
#define TK_WATER_TT_FIRST_DAY 117
#define TKS_WATER_TT_FIRST_DAY "water_travel_first_day"
#define TK_WATER_TT_LAST_DAY 118
#define TKS_WATER_TT_LAST_DAY "water_travel_last_day"
#define TK_MIN_MIGR_RATE 1326
#define TKS_MIN_MIGR_RATE "min_migration_rate"

#ifdef REALTIME
#define TK_RT_REPORT_DATE 119
#define TKS_RT_REPORT_DATE "rt_report_date"
#endif /* REALTIME */

#define TK_MORTALITY_CLASS 120
#define TKS_MORTALITY_CLASS "mortality_class"
#define TK_FORK_THRESHOLD 121
#define TKS_FORK_THRESHOLD "fork_threshold"

/* species subtokens */
#define TK_VB   	21	/* Vb */			/*fish file*/
#define TKS_VB		"vb"
#define TK_VVAR 	22	/* Vvar */			/*fish file*/
#define TKS_VVAR	"vvar"
#define TK_DRIFT   	23	/* drift time */		/*fish file*/
#define TKS_DRIFT	"drift_time"
#define TK_PCOEF_MN   	24	/* predation coeff mean */	/*fish file*/
#define TKS_PCOEF_MN	"pred_coef_mean"
#define TK_PCOEF_VAR   	25	/* predation coeff var */	/*fish file*/
#define TKS_PCOEF_VAR	"pred_coef_var"
#define TK_GMCOEF_MN   	26	/* gas mortality coeff mean */	/*fish file*/
#define TKS_GMCOEF_MN	"gas_mort_mean"
#define TK_GMCOEF_VAR   27	/* gas mortality coeff var */	/*fish file*/
#define TKS_GMCOEF_VAR	"gas_mort_var"
#define TK_GMEXP   	28	/* gas mortality exponent */	/*fish file*/
#define TKS_GMEXP	"gas_mort_exp"
#define TK_GMTHRESH	29	/* gas mortality threshold */	/*fish file*/
#define TKS_GMTHRESH	"gas_mort_thresh"
#define TK_PCOEF_LOW  	210	/* predation coeff low */	/*fish file*/
#define TKS_PCOEF_LOW	"pred_coef_low"
#define TK_PCOEF_HIGH  	211	/* predation coeff high */	/*fish file*/
#define TKS_PCOEF_HIGH	"pred_coef_high"
#define TK_GMCOEF_LOW	212	/* gas mortality coeff low */	/*fish file*/
#define TKS_GMCOEF_LOW	"gas_mort_low"
#define TK_GMCOEF_HIGH	213	/* gas mortality coeff high */	/*fish file*/
#define TKS_GMCOEF_HIGH	"gas_mort_high"
#define TK_RPCOEF_MEAN  214	/* reach predation coeff mean *//*fish file*/
#define TKS_RPCOEF_MEAN "reach_pred_coef_mean"
#define TK_RPCOEF_LOW   215	/* reach predation coeff low */	/*fish file*/
#define TKS_RPCOEF_LOW  "reach_pred_coef_low"	
#define TK_RPCOEF_HIGH  216	/* reach predation coeff high *//*fish file*/
#define TKS_RPCOEF_HIGH "reach_pred_coef_high"
#define TK_RPCOEF_V15_MEAN  2140	/* reach predation coeff mean *//*fish file*/
#define TKS_RPCOEF_V15_MEAN "reach_pred_coef_v15_mean"
#define TK_RPCOEF_V15_LOW   2150	/* reach predation coeff low */	/*fish file*/
#define TKS_RPCOEF_V15_LOW  "reach_pred_coef_v15_low"	
#define TK_RPCOEF_V15_HIGH  2160	/* reach predation coeff high *//*fish file*/
#define TKS_RPCOEF_V15_HIGH "reach_pred_coef_v15_high"
#define TK_RPCOEF_V16_MEAN  2141	/* reach predation coeff mean *//*fish file*/
#define TKS_RPCOEF_V16_MEAN "reach_pred_coef_v16_mean"
#define TK_RPCOEF_V16_LOW   2151	/* reach predation coeff low */	/*fish file*/
#define TKS_RPCOEF_V16_LOW  "reach_pred_coef_v16_low"	
#define TK_RPCOEF_V16_HIGH  2161	/* reach predation coeff high *//*fish file*/
#define TKS_RPCOEF_V16_HIGH "reach_pred_coef_v16_high"
#define TK_PRED_MORT_A  2171	/* predator mortality coeff a *//*fish file*/
#define TKS_PRED_MORT_A "pred_mort_a"
#define TK_PRED_MORT_V16_B  2172	/* predator mortality coeff b *//*fish file*/
#define TKS_PRED_MORT_V16_B "pred_mort_v16_b"
#define TK_FPCOEF_MEAN  217	/* forebay predation coeff mean *//*fish file*/
#define TKS_FPCOEF_MEAN "forebay_pred_coef_mean"
#define TK_FPCOEF_LOW   218	/* forebay predation coeff low *//*fish file*/
#define TKS_FPCOEF_LOW  "forebay_pred_coef_low"
#define TK_FPCOEF_HIGH  219	/* forebay predation coeff high *//*fish file*/
#define TKS_FPCOEF_HIGH "forebay_pred_coef_high"
#define TK_MEAN_FOREBAY_TRANSIT_TIME 2190
#define TKS_MEAN_FOREBAY_TRANSIT_TIME "mean_forebay_transit_time"
#define TK_TPCOEF_MEAN  220	/* tailrace predation coeff mean *//*fish file*/
#define TKS_TPCOEF_MEAN "tailrace_pred_coef_mean"
#define TK_TPCOEF_LOW   221	/* tailrace predation coeff low *//*fish file*/
#define TKS_TPCOEF_LOW  "tailrace_pred_coef_low"
#define TK_TPCOEF_HIGH  222	/* tailrace predation coeff high *//*fish file*/
#define TKS_TPCOEF_HIGH "tailrace_pred_coef_high"
#define TK_VVAR1 	223	/* Vvar */			/*fish file*/
#define TKS_VVAR1	"v_var"
#define TK_DRIFT_EQUATION  224	/* drift equation */		/*fish file*/
#define TKS_DRIFT_EQUATION "drift_eqn"
#define TK_NOON_DIST  	225	/* delay parameter */		/*fish file*/
#define TKS_NOON_DIST 	"noon_distance"
#define TK_NIGHT_DIST  	226	/* delay parameter */		/*fish file*/
#define TKS_NIGHT_DIST 	"night_distance"
#define TK_DELAY_CONST  227	/* delay parameter */		/*fish file*/
#define TKS_DELAY_CONST "delay_const"
#define TK_DELAY_THRESH 228	/* delay parameter */		/*fish file*/
#define TKS_DELAY_THRESH "delay_threshold"
#define TK_DVCOEF_MEAN	229	/* drift variance coeff mean */ /*fish file*/
#define TKS_DVCOEF_MEAN	"drift_var_coef_mean"
#define TK_DVCOEF_LOW	230	/* drift variance coeff low */  /*fish file*/
#define TKS_DVCOEF_LOW "drift_var_coef_low"
#define TK_DVCOEF_HIGH	231	/* drift variance coeff high */ /*fish file*/
#define TKS_DVCOEF_HIGH	"drift_var_coef_high"
#define TK_Y_PRED_CAL	232	/* */				/*fish file*/
#define TKS_Y_PRED_CAL	"y_pred_cal"
#define TK_GM_LO_SLOPE  233	/* gas mortality low slope */	/*fish file*/
#define TKS_GM_LO_SLOPE	"gas_mort_low_slope"
#define TK_GM_HI_SLOPE  234	/* gas mortality high slope */	/*fish file*/
#define TKS_GM_HI_SLOPE	"gas_mort_high_slope"
#define TK_GM_CRITICAL  235	/* gas mort critical value */	/*fish file*/
#define TKS_GM_CRITICAL	"gas_mort_critical"
#define TK_MIGR_EQUATION  236	/* gas mort critical value */	/*fish file*/
#define TKS_MIGR_EQUATION "migration_eqn"
#define TK_GMORT_EQUATION 237	/* gmort equation */		/*fish file*/
#define TKS_GMORT_EQUATION "gas_mort_eqn"
#define TK_FDENS_EQUATION 238	/* fish density/depth equation */ /*fish file*/
#define TKS_FDENS_EQUATION "fish_depth_eqn"
#define TK_MRCOEF_MEAN	239	/* migr rate var coeff mean */ /*fish file*/
#define TKS_MRCOEF_MEAN	"migr_var_coef_mean"
#define TK_MRCOEF_LOW	240	/* migr rate var coeff mean */ /*fish file*/
#define TKS_MRCOEF_LOW "migr_var_coef_low"
#define TK_MRCOEF_HIGH	241	/* migr rate var coeff mean */ /*fish file*/
#define TKS_MRCOEF_HIGH	"migr_var_coef_high"
#define TK_PRED_TR_EQUATION  242 /* pred temperature response */ /*fish file*/
#define TKS_PRED_TR_EQUATION "pred_temp_response_eqn"
/* Below Bonneville tokens */
#define TK_INRIVER_RETURN_EQUATION  243 /* inriver return rate equation */
#define TKS_INRIVER_RETURN_EQUATION "inriver_return_eqn"
#define TK_TRANSPORT_RETURN_EQUATION  244 /* transport return rate equation */
#define TKS_TRANSPORT_RETURN_EQUATION "transport_return_eqn"
#define TK_POST_BONNEVILLE_HYPOTHESIS  2430 /* which post-Bonneville algorithm to use */
#define TKS_POST_BONNEVILLE_HYPOTHESIS "post_bonneville_hypothesis"
#define TK_INRIVER_LATENT_MORT  2431 /* latent mortality for inriver fish */
#define TKS_INRIVER_LATENT_MORT "inriver_latent_mort"
#define TK_TRANSPORT_LATENT_MORT  2432 /* latent mortality for transport fish */
#define TKS_TRANSPORT_LATENT_MORT "transport_latent_mort"
#define TK_DIFFERENTIAL_RETURN  2433 /* differential return (for use in post-Bonneville hypotheses) */
#define TKS_DIFFERENTIAL_RETURN "differential_return"
#define TK_INRIVER_LATENT_MORT_EQUATION  2434 /* Equation to calculate latent mortality of inriver fish (e.g. from WTT) */
#define TKS_INRIVER_LATENT_MORT_EQUATION "inriver_latent_mort_eqn"
/* XT Model parameters, formerly listed under reach tokens */
#define TK_TIME_COEFF 245  /* random velocity component, per species */
#define TKS_TIME_COEFF "time_coef"
#define TK_DISTANCE_COEFF 246  /* distance component, per species */
#define TKS_DISTANCE_COEFF "distance_coef"
/* COMPASS reservoir survival species tokens */
#define TK_PRED_CAPTURE_DIST_EQN  247 /* pred capture distance equation */ /*fish file*/
#define TKS_PRED_CAPTURE_DIST_EQN "pred_capture_dist_eqn"
#define TK_REACH_SURVIVAL_COEF  248 /* reach survival coefficient */ /*fish file*/
#define TKS_REACH_SURVIVAL_COEF "reach_survival_coef"
#define TK_SIGMA_D 249  /* For error in reservoir survival, stock- and reach class-specific */
#define TKS_SIGMA_D "sigma_d"
/* CUSTOM_CLASS mortality class-specific tokens */
#define TK_CUSTOM_SURVIVAL_EQN  250 /* pred capture distance equation */ /*fish file*/
#define TKS_CUSTOM_SURVIVAL_EQN "custom_survival_eqn"

/* release subtokens */
#define TK_RLS_VIT     		131	/* vitality at release */ /*rls file*/
#define TKS_RLS_VIT		"vitality"
#define TK_RLS_VIT_CHANGE   	132	/* vitality change rate */ /*rls file*/
#define TKS_RLS_VIT_CHANGE	"vitality_change"
#define TK_RLS_ADD				1325
#define TKS_RLS_ADD			"add_setting"
#define TK_RLS_AGE          	133	/* age at release */	/*rls file*/
#define TKS_RLS_AGE		"age"
#define TK_RLS_NUM    		134	/* release number */	/*rls file*/
#define TKS_RLS_NUM		"number"
#define TK_RLS_SMOLT_ON        	135	/* smoltification start */ /*rls file*/
#define TKS_RLS_SMOLT_ON	"smolt_onset"
#define TK_RLS_SMOLT_OFF       	136	/* smoltification start */ /*rls file*/
#define TKS_RLS_SMOLT_OFF	"smolt_finish"
#define TK_RT_STOCK		137	/* realtime stock  for this release */
#define TKS_RT_STOCK		"rt_stock"
#define TK_RT_DATA_DATE		138	/* realtime data date */
#define TKS_RT_DATA_DATE	"rt_data_date"
#define TK_RLS_LENGTH		139	/* average fish length */
#define TKS_RLS_LENGTH		"length"
#define TK_INITIAL_SPILL_EXPERIENCE   	1320
#define TKS_INITIAL_SPILL_EXPERIENCE	"initial_spill_experience"

/* reach subtokens */
#define TK_RCH_PRED_DIST    	140	/* predator distribution coef */ /*pred file*/
#define TKS_RCH_PRED_DIST	"pred_dist"
#define TK_RCH_PRED_MEAN    	141	/* predator density mean */ /*pred file*/
#define TKS_RCH_PRED_MEAN	"pred_mean"
#define TK_RCH_PRED_MEAN_V15    	1410	/* predator density mean */ /*pred file*/
#define TKS_RCH_PRED_MEAN_V15	"pred_mean_v15"
#define TK_RCH_PRED_MEAN_V16    	1411	/* predator density mean */ /*pred file*/
#define TKS_RCH_PRED_MEAN_V16	"pred_mean_v16"
#define TK_RCH_PRED_VAR 	142	/* predator density variability *//*pred file*/
#define TKS_RCH_PRED_VAR	"pred_var"
#define TK_RCH_LOSS     	143	/* loss */		/*flow file*/
#define TKS_RCH_LOSS		"loss"
#define TK_RCH_LOSS_MAX 	144	/* loss maximum */	/*flow file*/
#define TKS_RCH_LOSS_MAX 	"loss_max"
#define TK_RCH_LOSS_MIN 	145	/* loss minimum */	/*flow file*/
#define TKS_RCH_LOSS_MIN 	"loss_min"
#define TK_RCH_VELOCITY		146	/* velocity */		/*flow file*/
#define TKS_RCH_VELOCITY	"velocity"
#define TK_RCH_FLOW_MAX		147	/* maximum flow */	/*flow file*/
#define TKS_RCH_FLOW_MAX	"flow_max"
#define TK_RCH_FLOW		148	/* reach flow */	/*flow file*/
#define TKS_RCH_FLOW		"flow"
#define TK_RCH_ELEVATION	149	/* elevation change */	/*flow file*/
#define TKS_RCH_ELEVATION	"elevation_change"
#define TK_RCH_MONTE_OUT	150	/* Monte output tracking flags */
#define TKS_RCH_MONTE_OUT	"monte_output"
#define TK_RCH_GAS_THETA    1501	/* gas dissp theta parm */ 
#define TKS_RCH_GAS_THETA	"gas_theta"
#define TK_DELTA_TEMP 		1502	/* delta water temperature */
#define TKS_DELTA_TEMP 		"delta_water_temp"
#define TK_OUTPUT_SETTINGS	1503	/* Unified output tracking flags */
#define TKS_OUTPUT_SETTINGS	"output_settings"
#define TK_INPUT_TEMP		1504    /* used in reach, and dam */
#define TKS_INPUT_TEMP		"input_temp"
#define TK_INPUT_TURB		1505    /* turbidity;  used in reach, headwater and dam */
#define TKS_INPUT_TURB		"input_turbidity"

/* headwater subtokens */
#define TK_HW_FLOW_COEF    	151	/* coeff for unreg flow *//*flow file*/
#define TKS_HW_FLOW_COEF	"flow_coef"
#define TK_HW_FLOW_MAX  	152	/* headwater flow max */ /*flow file*/
#define TKS_HW_FLOW_MAX 	"flow_max"
#define TK_HW_FLOW    		153	/* headwater flow */	 /*flow file*/
#define TKS_HW_FLOW		"flow"
#define TK_HW_WTEMP 		154	/* water temperature */
#define TKS_HW_WTEMP 		"water_temp"
#define TK_HW_FLOW_MEAN 	1510
#define TKS_HW_FLOW_MEAN	"flow_mean"			/*flow file*/
#define TK_HW_MONTE_OUT		1513	/* Monte output tracking flags */
#define TKS_HW_MONTE_OUT	"monte_output"
#define TK_OUTPUT_GAS		1514    /* used in hw, reach, and dam */
#define TKS_OUTPUT_GAS		"output_gas"

/* dam subtokens */
#define TK_SPL_MRT_MN           161	/* spill mortality mean */
#define TKS_SPL_MRT_MN		"spill_mort_mean" 		/*dam file*/
#define TK_SPL_MRT_VAR          162	/* spill mortality variability */
#define TKS_SPL_MRT_VAR		"spill_mort_var" 		/*dam file*/
#define TK_BY_MRT_MN            163 			/* bypass mortality mean */
#define TKS_BY_MRT_MN		"bypass_mort_mean" 		/*dam file*/
#define TK_BY_MRT_VAR           164			/* bypass mortality variability */
#define TKS_BY_MRT_VAR		"bypass_mort_var" 		/*dam file*/
#define TK_TURB_MRT_MN          165			/* turbine mortality mean */
#define TKS_TURB_MRT_MN		"turbine_mort_mean" 		/*dam file*/
#define TK_TURB_MRT_VAR         166			/* turbine mortality variability */
#define TKS_TURB_MRT_VAR	"turbine_mort_var"		/*dam file*/
#define TK_TRNS_MRT_MN		1634			/* transportation mortality mean */
#define TKS_TRNS_MRT_MN		"transport_mort_mean"		/*dam file*/
#define TK_TRNS_MRT_VAR		1635			/* transportation mortality variability */
#define TKS_TRNS_MRT_VAR	"transport_mort_var"		/*dam file*/
#define TK_SEP_PROB		1636			/* separation probability */
#define TKS_SEP_PROB		"separation_prob"		/*dam file*/
#define TK_DAM_PRED             167			/* predation */
#define TKS_DAM_PRED		"predation" 			/*dam file*/
#define TK_SPL_EQN_CLASS        168			/* spill eqn class */
#define TKS_SPL_EQN_CLASS	"spill_eqn_class" 		/*dam file*/
#define TK_SPL_EQN_ERFN         169			/* spill eqn error function */
#define TKS_SPL_EQN_ERFN	"spill_eqn_err_fn" 		/*dam file*/
#define TK_SPL_EQN_VAL          1610			/* spill eqn value */
#define TKS_SPL_EQN_VAL		"spill_eqn_value" 		/*dam file*/
#define TK_SPL_EQN_RANGE        1611			/* spill eqn range */
#define TKS_SPL_EQN_RANGE	"spill_eqn_range" 		/*dam file*/
#define TK_DAY_NPSS_MN          1612			/* day no pass mean */
#define TKS_DAY_NPSS_MN		"day_nopass_mean" 		/*dam file*/
#define TK_DAY_NPSS_VAR         1613			/* day no pass variability */
#define TKS_DAY_NPSS_VAR	"day_nopass_var" 		/*dam file*/
#define TK_DAY_FGE_MN           1614			/* day fge mean */
#define TKS_DAY_FGE_MN		"day_fge_mean" 			/*dam file*/
#define TK_DAY_FGE_VAR          1615			/* day fge variability */
#define TKS_DAY_FGE_VAR		"day_fge_var" 			/*dam file*/
#define TK_NT_NPSS_MN           1616			/* night no pass mean */
#define TKS_NT_NPSS_MN		"night_nopass_mean" 		/*dam file*/
#define TK_NT_NPSS_VAR          1617			/* night no pass variability */
#define TKS_NT_NPSS_VAR		"night_nopass_var" 		/*dam file*/
#define TK_NT_FGE_MN            1618			/* night fge mean */
#define TKS_NT_FGE_MN		"night_fge_mean" 		/*dam file*/
#define TK_NT_FGE_VAR           1619			/* night fge variability */
#define TKS_NT_FGE_VAR		"night_fge_var" 		/*dam file*/
#define TK_DAM_SPEC             1620			/* dam species designator */
#define TKS_DAM_SPEC		"species" 			/* not used ^..^*/
#define TK_DAM_SPILL            1621			/* spill */
#define TKS_DAM_SPILL		"spill" 			/*spill file*/
#define TK_DAY_SPILL		1623			/* day spill */
#define TKS_DAY_SPILL		"day_spill"			/*spill file*/
#define TK_NT_SPILL		1624			/* night spill */
#define TKS_NT_SPILL		"night_spill"			/*spill file*/
#define TK_TRANSPORT		1637			/* transport subtoken marker */
#define TKS_TRANSPORT		"transport"			/*menu*/
#define TK_DAM_FLOW             1625			/* dam flow */
#define TKS_DAM_FLOW		"flow" 				/*flow file*/
#define TK_DAM_FLOW_MAX         1626			/* dam flow max */
#define TKS_DAM_FLOW_MAX	"flow_max" 			/*flow file*/
#define TK_DAM_LOSS             1627			/* dam flow loss */
#define TKS_DAM_LOSS		"loss" 				/*flow file*/
#define TK_DAM_LOSS_MAX         1628			/* dam flow loss max */
#define TKS_DAM_LOSS_MAX	"loss_max" 			/*flow file*/
#define TK_DAM_LOSS_MIN   	1629			/* dam flow loss min */
#define TKS_DAM_LOSS_MIN	"loss_min" 			/*flow file*/
#define TK_SLC_EQN_CLASS	1630			/* sluice equation class */
#define TKS_SLC_EQN_CLASS	"slc_eqn_class"			/*dam file*/
#define TK_SLC_EQN_ERFN		1631			/* sluice equation error function */
#define TKS_SLC_EQN_ERFN	"slc_eqn_err_fn"		/*dam file*/
#define TK_SLC_EQN_VAL		1632			/* sluice equation values */
#define TKS_SLC_EQN_VAL		"slc_eqn_value"			/*dam file*/
#define TK_SLC_EQN_RANGE	1633			/* sluice equation range */
#define TKS_SLC_EQN_RANGE	"slc_eqn_range"			/*dam file*/
/* used above			1634-1637 */
#define TK_SPILL_EQUATION	1638			/* spill equation stuff */
#define TKS_SPILL_EQUATION	"spill_equation"        	/* dam file */
#define TK_SLUICE_EQUATION	1639			/* sluce equation stuff */
#define TKS_SLUICE_EQUATION	"sluice_equation"        	/* dam file */
#define TK_NSAT_EQUATION	1640			/* nitrogen saturation eqn */
#define TKS_NSAT_EQUATION	"nsat_equation"		/* dam file */
#define TK_PASSAGE		1641			/* passage probability */
#define TKS_PASSAGE		"passage"		/* dam file */
#define TK_PLAN_SPILL		1642			/* planned spill days (legacy) */
#define TKS_PLAN_SPILL		"planned_spill"		/* spill file */
#define TK_FISH_SPILL		1643			/* fish spill hours (legacy) */
#define TKS_FISH_SPILL		"fish_spill"		/* spill file */
#define TK_PLAN_SPILL_DAY		16421			/* planned spill during the day */
#define TKS_PLAN_SPILL_DAY		"planned_spill_day"		/* spill file */
#define TK_PLAN_SPILL_NIGHT		16431			/* planned spill during the night */
#define TKS_PLAN_SPILL_NIGHT		"planned_spill_night"		/* spill file */
#define TK_PWRHS_CAP		1644			/* powerhouse capacity*/
#define TKS_PWRHS_CAP		"powerhouse_capacity"	/* powerhouse capacity*/
#define TK_DELAY_EQUATION	1645			/* delay eqn */
#define TKS_DELAY_EQUATION	"delay_equation"	/* dam file */
#define TK_DAM_PRED_PROB        1646			/* predation */
#define TKS_DAM_PRED_PROB	"predation_prob"	/*dam file*/
#define TK_DAM_PRED_DENS     	1647			/* predator density*/
#define TKS_DAM_PRED_DENS	"predator_density"	/*dam file*/
#define TK_SPL_MRT_LOW         	1648			/* spill mort low */
#define TKS_SPL_MRT_LOW		"spill_mort_low" 	/*dam file*/
#define TK_BY_MRT_LOW          	1649			/* bypass mort var */
#define TKS_BY_MRT_LOW		"bypass_mort_low" 	/*dam file*/
#define TK_TURB_MRT_LOW        	1650			/* turbine mort var */
#define TKS_TURB_MRT_LOW	"turbine_mort_low"	/*dam file*/
#define TK_TRNS_MRT_LOW		1651			/* transport mort var */
#define TKS_TRNS_MRT_LOW	"transport_mort_low"	/*dam file*/
#define TK_DAY_FGE_LOW         	1652			/* day fge var */
#define TKS_DAY_FGE_LOW		"day_fge_low" 		/*dam file*/
#define TK_NT_FGE_LOW          	1653			/* night fge var */
#define TKS_NT_FGE_LOW		"night_fge_low" 	/*dam file*/
#define TK_SPL_MRT_HIGH         1654			/* spill mort high */
#define TKS_SPL_MRT_HIGH	"spill_mort_high" 	/*dam file*/
#define TK_BY_MRT_HIGH          1655			/* bypass mort var */
#define TKS_BY_MRT_HIGH		"bypass_mort_high" 	/*dam file*/
#define TK_TURB_MRT_HIGH        1656			/* turbine mort var */
#define TKS_TURB_MRT_HIGH	"turbine_mort_high"	/*dam file*/
#define TK_TRNS_MRT_HIGH	1657			/* transport mort var */
#define TKS_TRNS_MRT_HIGH	"transport_mort_high"	/*dam file*/
#define TK_DAY_FGE_HIGH         1658			/* day fge var */
#define TKS_DAY_FGE_HIGH	"day_fge_high" 		/*dam file*/
#define TK_NT_FGE_HIGH          1659			/* night fge var */
#define TKS_NT_FGE_HIGH		"night_fge_high" 	/*dam file*/
#define TK_ADDL_PHOUSE          1660			/* extra powerhouse */
#define TKS_ADDL_PHOUSE		"additional_powerhouse" /*dam file*/
#define TK_PWRHS_WEIGHT         1661			/* powerhouse weight */
#define TKS_PWRHS_WEIGHT	"powerhouse_fraction" 	/*dam file*/
#define TK_PWRHS_SCHED          1662			/* powerhouse schedule*/
#define TKS_PWRHS_SCHED		"powerhouse_schedule" 	/*dam file*/
#define TK_PWRHS_PRIORITY       1663			/* powerhouse priority*/
#define TKS_PWRHS_PRIORITY	"powerhouse_priority" 	/*dam file*/
/*#define TK_PWRHS_THRESH		1664
#define TKS_PWRHS_THRESH	"powerhouse_threshold" */
#define TK_DAM_FLOW_MIN		1665			/*flow minimum*/
#define TKS_DAM_FLOW_MIN	"flow_min" 		/*flow file*/
#define TK_PRED_POP_TR		1670			/*tailrace pred pop*/
#define TKS_PRED_POP_TR		"pred_pop_tailrace"	/*dam file*/
#define TK_TR_TIME		1671			/*time in tailrace*/
#define TKS_TR_TIME		"tailrace_time"		/*dam file*/
#define TK_TR_LENGTH		1672			/*length of tailrace*/
#define TKS_TR_LENGTH		"tailrace_length"	/*dam file*/
#define TK_FGE_EQN		1673			/*fge equation*/
#define TKS_FGE_EQN		"fge_equation"		/*dam file*/
#define TK_DAM_PRED_DENS_FB    	1674			/* fb predator density*/
#define TKS_DAM_PRED_DENS_FB	"pred_density_forebay"	/*dam file*/
#define TK_DAM_PRED_DENS_TR    	1675			/* tr predator density*/
#define TKS_DAM_PRED_DENS_TR	"pred_density_tailrace"	/*dam file*/
#define TK_SPILL_CAP    	1676			/* spill cap*/
#define TKS_SPILL_CAP		"spill_cap"		/*dam file*/
#define TK_MONTE_OUT		1677	/* Monte output tracking flags */
#define TKS_MONTE_OUT		"monte_output" 		/* config file */
#define TK_DELAY_SCALAR		1678			/* delay scalar */ 
#define TKS_DELAY_SCALAR	"delay_scalar"		/* dam file */
#define TK_TRANS_EQN		1679			/*trans equation*/
#define TKS_TRANS_EQN		"trans_mort_equation"	/*dam file*/
#define TK_NSAT_DAY_EQUATION	1680			/* nsat day eqn */
#define TKS_NSAT_DAY_EQUATION	"nsat_day_equation"		/* dam file */
#define TK_NSAT_NIGHT_EQUATION	1681			/* nsat night eqn */
#define TKS_NSAT_NIGHT_EQUATION	"nsat_night_equation"	/* dam file */
#define TK_NSAT_BACKUP_EQUATION	1682			/* nsat backup eqn */
#define TKS_NSAT_BACKUP_EQUATION "nsat_backup_equation"	/* dam file */
#define TK_DAM_GAS_THETA	1683			/* gas dissp */ 
#define TKS_DAM_GAS_THETA	"gas_theta"		/* dam file */
#define TK_K_ENTRAIN		1684			/* gas entrainment */ 
#define TKS_K_ENTRAIN		"k_entrain"		/* dam file */
/* RSW */
#define TK_SPILL_RSW_PROP	1685			/* prop of spill to RSW (this is currently obsolete) */ 
#define TKS_SPILL_RSW_PROP	"spill_rsw_prop"		/* dam file */
#define TK_RSW_EQUATION		1686			/* prop of spilled fish to RSW from prop of spill to RSW */ 
#define TKS_RSW_EQUATION	"rsw_equation"		/* dam file */
#define TK_RSW_MRT_MN		1687			/* RSW mortality */ 
#define TKS_RSW_MRT_MN		"rsw_mort_mean"		/* dam file */
#define TK_RSW_MRT_LOW		1688			/* RSW mortality */ 
#define TKS_RSW_MRT_LOW		"rsw_mort_low"		/* dam file */
#define TK_RSW_MRT_HIGH		1689			/* RSW mortality */ 
#define TKS_RSW_MRT_HIGH	"rsw_mort_high"		/* dam file */
#define TK_RSW_SPILL_CAP	1690			/* Max RSW flow */ 
#define TKS_RSW_SPILL_CAP	"rsw_spill_cap"		/* dam file */
/* Sluiceways */
#define TK_SLUICEWAY_MRT_MN		1691			/* sluiceway mortality */ 
#define TKS_SLUICEWAY_MRT_MN	"sluiceway_mort_mean"		/* dam file */
#define TK_SLUICEWAY_MRT_LOW	1692			/* sluiceway mortality */ 
#define TKS_SLUICEWAY_MRT_LOW	"sluiceway_mort_low"		/* dam file */
#define TK_SLUICEWAY_MRT_HIGH	1693			/* sluiceway mortality */ 
#define TKS_SLUICEWAY_MRT_HIGH	"sluiceway_mort_high"		/* dam file */
/* tokens for defining distributions for parameters */
#define TK_TURB_MRT          1710			/* turbine mortality */
#define TKS_TURB_MRT		"turbine_mort" 		/*dam file*/
#define TK_BY_MRT          1711			/* bypass mortality */
#define TKS_BY_MRT		"bypass_mort" 		/*dam file*/
#define TK_SPL_MRT          1712			/* spillway mortality */
#define TKS_SPL_MRT		"spill_mort" 		/*dam file*/
#define TK_SLUICEWAY_MRT          1713			/* sluiceway mortality */
#define TKS_SLUICEWAY_MRT		"sluiceway_mort" 		/*dam file*/
#define TK_RSW_MRT          1714			/* rsw mortality mean */
#define TKS_RSW_MRT		"rsw_mort" 		/*dam file*/
#define TK_SLUICEWAY_PROP          1715			/* rsw mortality */
#define TKS_SLUICEWAY_PROP		"sluiceway_prop" 		/*dam file*/
#define TK_DAY_FGE		1716			/* day FGE */
#define TKS_DAY_FGE		"day_fge"		/*dam file*/
#define TK_NT_FGE		1717			/* night FGE */
#define TKS_NT_FGE		"night_fge"		/*dam file*/
#define TK_FPCOEF		1718			/* forebay pred. coefficient */
#define TKS_FPCOEF		"forebay_pred_coef"		/*dam file*/
#define TK_TPCOEF		1719			/* tailrace pred. coefficient */
#define TKS_TPCOEF		"tailrace_pred_coef"		/*dam file*/
#define TK_RPCOEF		1720			/* reach pred. coefficient */
#define TKS_RPCOEF		"reach_pred_coef"
#define TK_MRCOEF		1721			/* migr rate variance */
#define TKS_MRCOEF		"migr_var_coef"
#define TK_TRNS_MRT		1722			/* transport mortality base */
#define TKS_TRNS_MRT	"transport_mort"		/*dam file*/
/* More mean, low, high tokens */
#define TK_SLUICEWAY_PROP_MN      1760
#define TKS_SLUICEWAY_PROP_MN		"sluiceway_prop_mean"
#define TK_SLUICEWAY_PROP_LOW       1761
#define TKS_SLUICEWAY_PROP_LOW		"sluiceway_prop_low"
#define TK_SLUICEWAY_PROP_HIGH      1762
#define TKS_SLUICEWAY_PROP_HIGH		"sluiceway_prop_high"
/* Dam delay suppot that varies by passage route and day/night */
#define TK_SPILL_DELAY		1780
#define TKS_SPILL_DELAY		"spill_delay"
#define TK_BYPASS_DELAY		1781
#define TKS_BYPASS_DELAY	"bypass_delay"
#define TK_SLUICEWAY_DELAY	1782
#define TKS_SLUICEWAY_DELAY	"sluiceway_delay"
#define TK_TURBINE_DELAY	1783
#define TKS_TURBINE_DELAY	"turbine_delay"
#define TK_RSW_DELAY		1784
#define TKS_RSW_DELAY		"rsw_delay"
#define TK_SPILL_DAY		1785
#define TKS_SPILL_DAY		"spill_day_delay"
#define TK_BYPASS_DAY		1786
#define TKS_BYPASS_DAY		"bypass_day_delay"
#define TK_SLUICEWAY_DAY	1787
#define TKS_SLUICEWAY_DAY	"sluiceway_day_delay"
#define TK_TURBINE_DAY		1788
#define TKS_TURBINE_DAY		"turbine_day_delay"
#define TK_RSW_DAY			1789
#define TKS_RSW_DAY			"rsw_day_delay"


/* dam subtoken subtokens */

#define TK_STORAGE_VOLUME	16370			/* basin volume array*/
#define TKS_STORAGE_VOLUME	"storage_volume"		/*dam file*/

/* transport subtokens -these are subtokens of the dam transport subtoken #1637
 */

#define TK_START_BY_DATE	16371			/* start by date/count*/
#define TKS_START_BY_DATE	"start_by_date"			/*dam file*/
#define TK_START_DATE		16372			/* start date */
#define TKS_START_DATE		"start_date"			/*dam file*/
#define TK_START_COUNT		16373			/* start count */
#define TKS_START_COUNT		"start_count"			/*dam file*/
#define TK_END_DATE		16374			/* end date */
#define TKS_END_DATE		"end_date"			/*dam file*/
#define TK_END_COUNT		16375			/* end count */
#define TKS_END_COUNT		"end_count"			/*dam file*/
#define TK_NUM_LOW_DAYS		16376		/* low days for transport end */
#define TKS_NUM_LOW_DAYS	"num_low_days"			/*dam file*/
#define TK_HFL_PASSED		16377		/* %pssg hfl_spec for full trns*/
#define TKS_HFL_PASSED		"hfl_passed_perc"		/*dam file*/
#define TK_HI_FLOW		16378		/* high flow for partial trans*/
#define TKS_HI_FLOW		"high_flow"			/*dam file*/
#define TK_HFL_SPEC		16379		/* spc to pass during hi flow */
#define TKS_HFL_SPEC		"high_flow_species"		/*dam file*/
#define TK_RATE			163710			/* rate of transport */
#define TKS_RATE		"transport_rate"		/*dam file*/
#define TK_RLS_PT		163711		/* name of rls point (rch seg) */
#define TKS_RLS_PT		"release_point"			/*dam file*/
#define TK_MAX_RESTARTS  167312         /* max # restarts (0, 1, -1=infinite) */
#define TKS_MAX_RESTARTS "max_restarts"
#define TK_RESTART_BY_DATE 167313   /* 1 = by date, 0 = by count */
#define TKS_RESTART_BY_DATE "restart_by_date"
#define TK_RESTART_DATE  167314     /* date to restart on. */
#define TKS_RESTART_DATE "restart_date"
#define TK_RESTART_COUNT 167315     /* min count to trigger restart */
#define TKS_RESTART_COUNT "restart_count"

/* Reach class tokens. */
#define TK_REACH_CLASS  170
#define TKS_REACH_CLASS  "reach_class"
#define TK_REACH_CLASS_NAME 171
#define TKS_REACH_CLASS_NAME "reach_class_name"
#define TK_NUM_REACH_CLASSES 172
#define TKS_NUM_REACH_CLASSES "num_reach_classes"

/* growth related tokens. */
#define TK_PREY_ENERGY_DENSITY 173
#define TKS_PREY_ENERGY_DENSITY "prey_energy_density"
#define TK_LENGTH_WEIGHT_B0 174
#define TKS_LENGTH_WEIGHT_B0 "length_weight_b0"
#define TK_LENGTH_WEIGHT_B1 175
#define TKS_LENGTH_WEIGHT_B1 "length_weight_b1"
#define TK_PPRIME_A 176
#define TKS_PPRIME_A "pprime_a"
#define TK_PPRIME_B 177
#define TKS_PPRIME_B "pprime_b"

/* equation tokens */
#define TKS_PARAMETER		"parameter"
#define TK_PARAMETER		50

/* distribution tokens */
#define TK_DIST_BROKEN_STICK	31
#define	TKS_DIST_BROKEN_STICK	"broken_stick"
/* broken stick distribution tokens */
#define TK_DIST_BS_MEAN	32
#define	TKS_DIST_BS_MEAN	"mean"
#define TK_DIST_BS_LOW	33
#define	TKS_DIST_BS_LOW	"low"
#define TK_DIST_BS_HIGH	34
#define	TKS_DIST_BS_HIGH	"high"
