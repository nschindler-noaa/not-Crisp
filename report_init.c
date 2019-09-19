/** \file
 * \brief Data structures for the report tool.
 *
 *	The global variables for use in all report tools.  Each category
 * defined by a report tool must have a list of structs report_var to
 * define the variable names for the list item, the offset from the start
 * of the report structure (passed in to the output functions of the
 * report), and the type of variable we expect to find at that location.
 */

#include "crisp.h"
#include "c1tokens.h"
#include "report.h"
#include "alt_report.h"
#include <stddef.h>


#define DAM_OFF(xxx) offsetof( struct dam, xxx )
#define DAM_SPEC_OFF(xxx) offsetof( struct dam_species, xxx )
#define SPECIES_OFF(xxx) offsetof( struct species, xxx )
#define STOCK_OFF(xxx) offsetof( struct stock, xxx )
#define RELEASE_OFF(xxx) offsetof( struct release, xxx )
#define TRANSPORT_OFF(xxx) offsetof( struct transport, xxx )
#define HEADWATER_OFF(xxx) offsetof( struct headwater, xxx )
#define REACH_OFF(xxx) offsetof( struct reach, xxx )
#define REACH_SPEC_OFF(xxx) offsetof( struct reach_species, xxx )
#define RUN_OFF(xxx) offsetof( struct run, xxx )
#define ALTERN_OFF(xxx) offsetof( struct alt_data, xxx )

/*
 * This section is initializers for the input report variables
 */
/* The list of reportable species-dependent reach variables */
struct report_var reach_spec_report_vars[] =
{
    { RPT_REACH_SPEC_FLOAT, REACH_SPEC_OFF(pred_pop_mean),
	  TKS_RCH_PRED_MEAN, 1},
    { 0, 0, 0, 0}
};

/* The list of reportable species-dependent dam variables */
struct report_var dam_spec_report_vars[] =
{
    { RPT_DAM_SPEC_FLOAT, DAM_SPEC_OFF(sluiceway_prop),
	  TKS_SLUICEWAY_PROP, 1},
    { RPT_DAM_SPEC_FLOAT, DAM_SPEC_OFF(rsw_mort),
	  TKS_RSW_MRT, 1},
    { RPT_DAM_SPEC_FLOAT, DAM_SPEC_OFF(spill_mort),
	  TKS_SPL_MRT, 1},
    { RPT_DAM_SPEC_FLOAT, DAM_SPEC_OFF(sluiceway_mort),
	  TKS_SLUICEWAY_MRT, 1},
    { RPT_DAM_SPEC_FLOAT, DAM_SPEC_OFF(bypass_mort),
	  TKS_BY_MRT, 1},
    { RPT_DAM_SPEC_FLOAT, DAM_SPEC_OFF(turbine_mort),
          TKS_TURB_MRT, 1},
    { RPT_DAM_SPEC_FLOAT, DAM_SPEC_OFF(transport_mort),
	  TKS_TRNS_MRT, 1},
    { RPT_DAM_SPEC_FLOAT, DAM_SPEC_OFF(separation_prob),
	  TKS_SEP_PROB, 1},
    { RPT_DAM_SPEC_FLOAT, DAM_SPEC_OFF(pred_pop_mean),
      TKS_DAM_PRED_DENS_FB, 1},
    { RPT_DAM_SPEC_FLOAT, DAM_SPEC_OFF(pred_pop_tailrace), 
      TKS_DAM_PRED_DENS_TR, 1},
    { RPT_DAM_SPEC_FLOAT, DAM_SPEC_OFF(mean_forebay_transit_time),
      TKS_MEAN_FOREBAY_TRANSIT_TIME, 1},
    { RPT_DAM_SPEC_FLOAT, DAM_SPEC_OFF(day_fge), TKS_DAY_FGE, 1},
    { RPT_DAM_SPEC_FLOAT, DAM_SPEC_OFF(night_fge), TKS_NT_FGE, 1},

    /* these are plucked from an equation struct.  they do not appear
     * separately in the data file, and so have no tokens of their own.
     */
    { RPT_DAM_SPEC_FLOAT, DAM_SPEC_OFF(fge_eqn.param[0].val), "fge_z0", 1},
    { RPT_DAM_SPEC_FLOAT, DAM_SPEC_OFF(fge_eqn.param[1].val), "fge_z1", 1},
    { RPT_DAM_SPEC_FLOAT, DAM_SPEC_OFF(fge_eqn.param[2].val), "fge_t0", 1},
    { RPT_DAM_SPEC_FLOAT, DAM_SPEC_OFF(fge_eqn.param[2].val), "fge_dt", 1},

    { RPT_DAM_SPEC_INT, DAM_SPEC_OFF(spill_eqn.id), "spill_type", 1},
    { RPT_DAM_SPEC_FLOAT, DAM_SPEC_OFF(spill_eqn.param[0].val), "spill_a", 1},
    { RPT_DAM_SPEC_FLOAT, DAM_SPEC_OFF(spill_eqn.param[1].val), "spill_b", 1},
    { RPT_DAM_SPEC_FLOAT, DAM_SPEC_OFF(spill_eqn.param[2].val), "spill_parm2", 1},
    { RPT_DAM_SPEC_FLOAT, DAM_SPEC_OFF(spill_eqn.param[3].val), "spill_parm3", 1},
    
    { RPT_DAM_SPEC_FLOAT, DAM_SPEC_OFF(trans_eqn.param[0].val), "trans_tt1", 1},
    { RPT_DAM_SPEC_FLOAT, DAM_SPEC_OFF(trans_eqn.param[1].val), "trans_m1", 1},
    { RPT_DAM_SPEC_FLOAT, DAM_SPEC_OFF(trans_eqn.param[2].val), "trans_tt2", 1},
    { RPT_DAM_SPEC_FLOAT, DAM_SPEC_OFF(trans_eqn.param[3].val), "trans_m2", 1},

    { RPT_DAM_SPEC_FLOAT, DAM_SPEC_OFF(delay_eqn.param[0].val), "delay_Aday", 1},
    { RPT_DAM_SPEC_FLOAT, DAM_SPEC_OFF(delay_eqn.param[1].val), "delay_Anight", 1},
    { RPT_DAM_SPEC_FLOAT, DAM_SPEC_OFF(delay_eqn.param[2].val), "delay_Bflow", 1},
    { RPT_DAM_SPEC_FLOAT, DAM_SPEC_OFF(delay_eqn.param[3].val), "delay_Bspill", 1},
    { RPT_DAM_SPEC_FLOAT, DAM_SPEC_OFF(delay_eqn.param[4].val), "delay_Bdate", 1},

    { 0, 0, 0, 0}
};


/* The list of reportable species-independent dam variables */
struct report_var dam_report_vars[] =
{
    { RPT_FLOAT, DAM_OFF(powerhouse_capacity), TKS_PWRHS_CAP, 1},
    { RPT_INT, DAM_OFF(phouse_priority), TKS_PWRHS_PRIORITY, 1},
    { RPT_FLOAT, DAM_OFF(spill_cap), TKS_SPILL_CAP, 1},
    { RPT_FLOAT, DAM_OFF(flow_max), TKS_DAM_FLOW_MAX, 1},
    { RPT_FLOAT, DAM_OFF(flow_min_project), TKS_DAM_FLOW_MIN, 1},
    { RPT_FLOAT, DAM_OFF(gas_theta), TKS_DAM_GAS_THETA, 1},
    { RPT_FLOAT, DAM_OFF(k_entrain), TKS_K_ENTRAIN, 1},
    { RPT_PERIOD_LIST, DAM_OFF(phouse_schedule), TKS_PWRHS_SCHED, 0},
    { RPT_PERIOD_LIST, DAM_OFF(planned_spill_day), TKS_PLAN_SPILL_DAY, 0},
    { RPT_PERIOD_LIST, DAM_OFF(planned_spill_night), TKS_PLAN_SPILL_NIGHT, 0},

    { RPT_INT, DAM_OFF(nsat_eqn.id), "tdg_day_type", 1},
    { RPT_FLOAT, DAM_OFF(nsat_eqn.param[0].val), "tdg_day_parm1", 1},
    { RPT_FLOAT, DAM_OFF(nsat_eqn.param[1].val), "tdg_day_parm2", 1},
    { RPT_FLOAT, DAM_OFF(nsat_eqn.param[2].val), "tdg_day_parm3", 1},
    { RPT_FLOAT, DAM_OFF(nsat_eqn.param[3].val), "tdg_day_parm4", 1},
    { RPT_FLOAT, DAM_OFF(nsat_eqn.param[4].val), "tdg_day_parm5", 1},

    { RPT_INT, DAM_OFF(nsat_night_eqn.id), "tdg_night_type", 1},
    { RPT_FLOAT, DAM_OFF(nsat_night_eqn.param[0].val), "tdg_night_parm1", 1},
    { RPT_FLOAT, DAM_OFF(nsat_night_eqn.param[1].val), "tdg_night_parm2", 1},
    { RPT_FLOAT, DAM_OFF(nsat_night_eqn.param[2].val), "tdg_night_parm3", 1},
    { RPT_FLOAT, DAM_OFF(nsat_night_eqn.param[3].val), "tdg_night_parm4", 1},
    { RPT_FLOAT, DAM_OFF(nsat_night_eqn.param[4].val), "tdg_night_parm5", 1},

    { RPT_INT, DAM_OFF(nsat_backup_eqn.id), "tdg_backup_type", 1},
    { RPT_FLOAT, DAM_OFF(nsat_backup_eqn.param[0].val), "tdg_backup_a", 1},
    { RPT_FLOAT, DAM_OFF(nsat_backup_eqn.param[1].val), "tdg_backup_b", 1},
    { RPT_FLOAT, DAM_OFF(nsat_backup_eqn.param[2].val), "tdg_backup_parm3", 1},

    { 0, 0, 0, 0}
};


/* The list of reportable species variables */
struct report_var species_report_vars[] =
{
    { RPT_RC_FLOAT, SPECIES_OFF(stock.vvar), TKS_VVAR1, 1},
    { RPT_FLOAT, SPECIES_OFF(gmort_eqn.param[0].val), "gmort_Mlow", 1},
    { RPT_FLOAT, SPECIES_OFF(gmort_eqn.param[1].val), "gmort_Mhigh", 1},
    { RPT_FLOAT, SPECIES_OFF(gmort_eqn.param[2].val), "gmort_crit", 1},
    { RPT_RC_FLOAT, SPECIES_OFF(reach_pcoef), TKS_RPCOEF, 1},
    { RPT_RC_FLOAT, SPECIES_OFF(pprime_a), TKS_PPRIME_A, 1},
    { RPT_RC_FLOAT, SPECIES_OFF(pprime_b), TKS_PPRIME_B, 1},
    { RPT_FLOAT, SPECIES_OFF(forebay_pcoef), TKS_FPCOEF, 1},
    { RPT_FLOAT, SPECIES_OFF(tailrace_pcoef), TKS_TPCOEF, 1},
    { RPT_RC_FLOAT, SPECIES_OFF(stock.mvcoef), TKS_MRCOEF, 1},
    { RPT_FLOAT, SPECIES_OFF(fdens_eqn.param[0].val), "FDens_DMode", 1},
    { RPT_FLOAT, SPECIES_OFF(fdens_eqn.param[1].val), "FDens_DBot", 1},

    { RPT_RCEQN_INT, SPECIES_OFF( stock.pred_temp_response_eqn[0].id ),
                              "PredTemp_type", 1 },
    { RPT_RCEQN_FLOAT, SPECIES_OFF(stock.pred_temp_response_eqn[0].param[0].val),
                              "PredTemp_parm1", 1 },
    { RPT_RCEQN_FLOAT, SPECIES_OFF(stock.pred_temp_response_eqn[0].param[1].val),
                              "PredTemp_parm2", 1 },
    { RPT_RCEQN_FLOAT, SPECIES_OFF(stock.pred_temp_response_eqn[0].param[2].val),
                              "PredTemp_parm3", 1 },

    { RPT_RCEQN_INT, SPECIES_OFF( stock.pred_capture_dist_eqn[0].id ),
                              "PredCapt_type", 1 },
    { RPT_RCEQN_FLOAT, SPECIES_OFF(stock.pred_capture_dist_eqn[0].param[0].val),
                              "PredCapt_parm1", 1 },
    { RPT_RCEQN_FLOAT, SPECIES_OFF(stock.pred_capture_dist_eqn[0].param[1].val),
                              "PredCapt_parm2", 1 },
    { RPT_RCEQN_FLOAT, SPECIES_OFF(stock.pred_capture_dist_eqn[0].param[2].val),
                              "PredCapt_parm3", 1 },

    { RPT_RCEQN_INT, SPECIES_OFF( stock.custom_survival_eqn[0].id ),
                              "CustomSurvival_type", 1 },
    { RPT_RCEQN_FLOAT, SPECIES_OFF(stock.custom_survival_eqn[0].param[0].val),
                              "CustomSurvival_parm1", 1 },
    { RPT_RCEQN_FLOAT, SPECIES_OFF(stock.custom_survival_eqn[0].param[1].val),
                              "CustomSurvival_parm2", 1 },
    { RPT_RCEQN_FLOAT, SPECIES_OFF(stock.custom_survival_eqn[0].param[2].val),
                              "CustomSurvival_parm3", 1 },
    { RPT_RCEQN_FLOAT, SPECIES_OFF(stock.custom_survival_eqn[0].param[3].val),
                              "CustomSurvival_parm4", 1 },

    { RPT_RCEQN_INT, SPECIES_OFF( stock.migration_eqn[0].id ),
                              "MigrEqn_type", 1 },
    { RPT_RCEQN_FLOAT, SPECIES_OFF(stock.migration_eqn[0].param[0].val),
                              "MigrEqn_parm1", 1 },
    { RPT_RCEQN_FLOAT, SPECIES_OFF(stock.migration_eqn[0].param[1].val),
                              "MigrEqn_parm2", 1 },
    { RPT_RCEQN_FLOAT, SPECIES_OFF(stock.migration_eqn[0].param[2].val),
                              "MigrEqn_parm3", 1 },
    { RPT_RCEQN_FLOAT, SPECIES_OFF(stock.migration_eqn[0].param[3].val),
                              "MigrEqn_parm4", 1 },
    { RPT_RCEQN_FLOAT, SPECIES_OFF(stock.migration_eqn[0].param[4].val),
                              "MigrEqn_parm5", 1 },
    { RPT_RCEQN_FLOAT, SPECIES_OFF(stock.migration_eqn[0].param[5].val),
                              "MigrEqn_parm6", 1 },
    { 0, 0, 0, 0}
};

/* The list of reportable stock variables */
struct report_var stock_report_vars[] =
{
    { RPT_RC_FLOAT, STOCK_OFF(vvar), TKS_VVAR1, 1},
    { RPT_RC_FLOAT, STOCK_OFF(mvcoef), TKS_MRCOEF, 1},
    { RPT_RC_FLOAT, STOCK_OFF(time_coeff), TKS_TIME_COEFF, 1},
    { RPT_RC_FLOAT, STOCK_OFF(distance_coeff), TKS_DISTANCE_COEFF, 1},
    { RPT_RC_FLOAT, STOCK_OFF(reach_survival_coef), TKS_REACH_SURVIVAL_COEF, 1},


   { RPT_RCEQN_INT, STOCK_OFF( pred_temp_response_eqn[0].id ),
                              "PredTemp_type", 1 },
    { RPT_RCEQN_FLOAT, STOCK_OFF(pred_temp_response_eqn[0].param[0].val),
                              "PredTemp_parm1", 1 },
    { RPT_RCEQN_FLOAT, STOCK_OFF(pred_temp_response_eqn[0].param[1].val),
                              "PredTemp_parm2", 1 },
    { RPT_RCEQN_FLOAT, STOCK_OFF(pred_temp_response_eqn[0].param[2].val),
                              "PredTemp_parm3", 1 },

   { RPT_RCEQN_INT, STOCK_OFF( pred_capture_dist_eqn[0].id ),
                              "PredCapt_type", 1 },
    { RPT_RCEQN_FLOAT, STOCK_OFF(pred_capture_dist_eqn[0].param[0].val),
                              "PredCapt_parm1", 1 },
    { RPT_RCEQN_FLOAT, STOCK_OFF(pred_capture_dist_eqn[0].param[1].val),
                              "PredCapt_parm2", 1 },
    { RPT_RCEQN_FLOAT, STOCK_OFF(pred_capture_dist_eqn[0].param[2].val),
                              "PredCapt_parm3", 1 },

   { RPT_RCEQN_INT, STOCK_OFF( custom_survival_eqn[0].id ),
                              "CustomSurvival_type", 1 },
    { RPT_RCEQN_FLOAT, STOCK_OFF(custom_survival_eqn[0].param[0].val),
                              "CustomSurvival_parm1", 1 },
    { RPT_RCEQN_FLOAT, STOCK_OFF(custom_survival_eqn[0].param[1].val),
                              "CustomSurvival_parm2", 1 },
    { RPT_RCEQN_FLOAT, STOCK_OFF(custom_survival_eqn[0].param[2].val),
                              "CustomSurvival_parm3", 1 },
    { RPT_RCEQN_FLOAT, STOCK_OFF(custom_survival_eqn[0].param[3].val),
                              "CustomSurvival_parm4", 1 },

    { RPT_RCEQN_INT, STOCK_OFF( migration_eqn[0].id ),
                              "MigrEqn_type", 1 },
    { RPT_RCEQN_FLOAT, STOCK_OFF(migration_eqn[0].param[0].val),
                              "MigrEqn_parm1", 1 },
    { RPT_RCEQN_FLOAT, STOCK_OFF(migration_eqn[0].param[1].val),
                              "MigrEqn_parm2", 1 },
    { RPT_RCEQN_FLOAT, STOCK_OFF(migration_eqn[0].param[2].val),
                              "MigrEqn_parm3", 1 },
    { RPT_RCEQN_FLOAT, STOCK_OFF(migration_eqn[0].param[3].val),
                              "MigrEqn_parm4", 1 },
    { RPT_RCEQN_FLOAT, STOCK_OFF(migration_eqn[0].param[4].val),
                              "MigrEqn_parm5", 1 },
    { RPT_RCEQN_FLOAT, STOCK_OFF(migration_eqn[0].param[5].val),
                              "MigrEqn_parm6", 1 },
    { 0, 0, 0, 0}
};

/* The list of reportable release variables. */
struct report_var release_report_vars[] =
{
    { RPT_FLOAT, RELEASE_OFF(vitality_at_release), TKS_RLS_VIT, 1},
    { RPT_FLOAT, RELEASE_OFF(vitality_change_rate), TKS_RLS_VIT_CHANGE, 1},
    { RPT_FLOAT, RELEASE_OFF(initial_spill_experience), TKS_INITIAL_SPILL_EXPERIENCE, 1},
    { RPT_INT, RELEASE_OFF(smolt_onset), TKS_RLS_SMOLT_ON, 1},
    { RPT_INT, RELEASE_OFF(start_date), "start_date", 1},
    { RPT_INT, RELEASE_OFF(smolt_finish), TKS_RLS_SMOLT_OFF, 1},
    { RPT_FLOAT, RELEASE_OFF(length), TKS_RLS_LENGTH, 1},
    { 0, 0, 0, 0}
};

/* The list of reportable transport variables. */
struct report_var transport_report_vars[] =
{
    { RPT_INT, TRANSPORT_OFF(start_by_date), TKS_START_BY_DATE, 1},
    { RPT_INT, TRANSPORT_OFF(start_date), TKS_START_DATE, 1},
    { RPT_INT, TRANSPORT_OFF(start_count), TKS_START_COUNT, 1},
    { RPT_INT, TRANSPORT_OFF(max_restarts), TKS_MAX_RESTARTS, 1},
    { RPT_INT, TRANSPORT_OFF(restart_by_date), TKS_RESTART_BY_DATE, 1},
    { RPT_INT, TRANSPORT_OFF(restart_date), TKS_RESTART_DATE, 1},
    { RPT_INT, TRANSPORT_OFF(restart_count), TKS_RESTART_COUNT, 1},
    { RPT_INT, TRANSPORT_OFF(end_date), TKS_END_DATE, 1},
    { RPT_INT, TRANSPORT_OFF(end_count), TKS_END_COUNT, 1},
    { RPT_INT, TRANSPORT_OFF(num_low_days), TKS_NUM_LOW_DAYS, 1},
    { RPT_FLOAT, TRANSPORT_OFF(hfl_passed_perc), TKS_HFL_PASSED, 1},
    { RPT_FLOAT, TRANSPORT_OFF(hi_flow), TKS_HI_FLOW, 1},
    { RPT_FLOAT, TRANSPORT_OFF(rate), TKS_RATE, 1},
    { 0, 0, 0, 0}
};

/* The list of reportable headwater variables */
struct report_var headwater_report_vars[] =
{
    { RPT_FLOAT, HEADWATER_OFF(flow_coefficient), TKS_HW_FLOW_COEF, 1},
    { RPT_FLOAT, HEADWATER_OFF(flow_max), TKS_HW_FLOW_MAX, 1},
    { RPT_FLOAT, HEADWATER_OFF(flow_mean), TKS_HW_FLOW_MEAN, 1},
    { 0, 0, 0, 0}
};

/* The list of reportable reach variables */
struct report_var reach_report_vars[] =
{
    { RPT_FLOAT, REACH_OFF(pred_dist_coef), TKS_RCH_PRED_DIST, 1},
    { RPT_FLOAT, REACH_OFF(loss_min), TKS_RCH_LOSS_MIN, 1},
    { RPT_FLOAT, REACH_OFF(loss_max), TKS_RCH_LOSS_MAX, 1},
    { RPT_FLOAT, REACH_OFF(flow_max), TKS_RCH_FLOW_MAX, 1},
    { RPT_FLOAT, REACH_OFF(gas_theta), TKS_RCH_GAS_THETA, 1},
    { RPT_INT,   REACH_OFF(rclass), TKS_REACH_CLASS, 1 },
    { 0, 0, 0, 0}
};

/* The list of reportable run variables */
struct report_var run_report_vars[] =
{
/*    { RPT_FLOAT, RUN_OFF(pred_act_exp),    TKS_PRED_ACT_EXP, 1}, */
    { RPT_FLOAT, RUN_OFF(gas_dissp_exp),   TKS_GAS_DIS_EXP, 1},
    { RPT_FLOAT, RUN_OFF(flow_proportion), TKS_FLOW_PROP, 1},
    { RPT_FLOAT, RUN_OFF(Ufree),           TKS_UFREE, 1},
    { RPT_FLOAT, RUN_OFF(prey_energy_density), TKS_PREY_ENERGY_DENSITY, 1},
    { RPT_FLOAT, RUN_OFF(length_weight_b0),TKS_LENGTH_WEIGHT_B0, 1},
    { RPT_FLOAT, RUN_OFF(length_weight_b1),TKS_LENGTH_WEIGHT_B1, 1},
    { RPT_FLOAT, RUN_OFF(fork_threshold),  TKS_FORK_THRESHOLD, 1},
    { RPT_INT,   RUN_OFF(flags),           TKS_RUN_FLAGS, 1},
    { RPT_INT,   RUN_OFF(mortality_class), TKS_MORTALITY_CLASS, 1},
    { RPT_INT,   RUN_OFF(post_bonneville_hypothesis), TKS_POST_BONNEVILLE_HYPOTHESIS, 1},
    { 0, 0, 0, 0}
};

/*
 * This section is initializers for the alternative report variables
 */
struct report_var travel_report_vars[] =
{
    { RPT_ALT_FLOAT, ALTERN_OFF(mean_passage),   ART_PASSAGE_MEAN,     1},
    { RPT_ALT_FLOAT, ALTERN_OFF(stddev_passage), ART_PASSAGE_STDDEV,   1},
    { RPT_ALT_FLOAT, ALTERN_OFF(var_passage),    ART_PASSAGE_COEFFVAR, 1},
    { RPT_ALT_FLOAT, ALTERN_OFF(skew_passage),   ART_PASSAGE_SKEW,     1},
    { RPT_ALT_FLOAT, ALTERN_OFF(kurt_passage),   ART_PASSAGE_KURT,     1},
    { RPT_ALT_FLOAT, ALTERN_OFF(mean_travel),    ART_TRAVEL_MEAN,      1},
    { RPT_ALT_FLOAT, ALTERN_OFF(stddev_travel),  ART_TRAVEL_STDDEV,    1},
    { RPT_ALT_FLOAT, ALTERN_OFF(var_travel),     ART_TRAVEL_COEFFVAR,  1},
    { RPT_ALT_FLOAT, ALTERN_OFF(skew_travel),    ART_TRAVEL_SKEW,      1},
    { RPT_ALT_FLOAT, ALTERN_OFF(kurt_travel),    ART_TRAVEL_KURT,      1},
    { RPT_ALT_FLOAT, ALTERN_OFF(avg_flow),	 ART_AVERAGE_FLOW,     1},
#ifdef ROUTE_TRACKING
    { RPT_ALT_FLOAT, ALTERN_OFF(bypass_avg),	 ART_BYPASS_AVG,       1},
    { RPT_ALT_FLOAT, ALTERN_OFF(turbine_avg),	 ART_TURBINE_AVG,      1},
    { RPT_ALT_FLOAT, ALTERN_OFF(spill_avg),	 ART_SPILL_AVG,	       1},
    { RPT_ALT_FLOAT, ALTERN_OFF(transport_avg),	 ART_TRANSPORT_AVG,    1},
    { RPT_ALT_FLOAT, ALTERN_OFF(premort_transport_total),
						 ART_PREMORT_TOTAL,    1},
    { RPT_ALT_FLOAT, ALTERN_OFF(postmort_transport_total),
						 ART_POSTMORT_TOTAL,   1},
    { RPT_ALT_FLOAT, ALTERN_OFF(fpe),		 ART_FPE,	       1},
    { RPT_ALT_FLOAT, ALTERN_OFF(bypass_in_avg),  ART_BYPASS_IN_AVG,    1},
    { RPT_ALT_FLOAT, ALTERN_OFF(turbine_in_avg), ART_TURBINE_IN_AVG,   1},
    { RPT_ALT_FLOAT, ALTERN_OFF(spillway_in_avg),
						 ART_SPILL_IN_AVG,     1},
    { RPT_ALT_FLOAT, ALTERN_OFF(project_in_total),
						 ART_PROJECT_IN_TOTAL, 1},
#endif
    { 0, 0, 0, 0},
};
