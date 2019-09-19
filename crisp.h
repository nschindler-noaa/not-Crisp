/** \file
 * \brief Main header file for model data structures.
 *
 * This file contains the main data structures of the model, especially those
 * that deal with the river layout, model parameters, species,
 * releases, transport projects, dams, reaches, headwaters, and so on.
 *
 * Note that stocks are described in the separate stock.h header file, but
 * doesn't seem to be a clear reason why stocks as separated from everything
 * else.
 */

#ifndef CRISP_H
#define CRISP_H

/* TRUE and FALSE needed on the pc */
#ifndef FALSE
/** FALSE is set to zero */
#define FALSE 0
#endif
#ifndef TRUE
/** TRUE is set to 1 although any non-zero expression is considered true in C */
#define TRUE 1
#endif

#ifdef WIN32
/* TODO: handle this issue more elegantly. */
#pragma warning( disable: 4244 )
/* warning C4244: '=' : conversion from 'double ' to 'float ', possible loss of data */
#pragma warning( disable: 4305 )
/* warning C4305: '=' : truncation from 'const double ' to 'float ' */
#endif

#include "strtools.h"
    
#include "crisp_defs.h"

/** convenient shorthand pointer into river segments for dams */
#define DAMPTR(seg) (&(seg)->current->dam)
/** convenient shorthand pointer into river segments for reaches */
#define RCHPTR(seg) (&(seg)->current->reach)
/** convenient shorthand pointer into river segments for headwaters */
#define HDWPTR(seg) (&(seg)->current->headwater)


#include "eqns.h"
#include "period.h"
#include "list.h"
#include "run_state.h"
#include "realtime_types.h"
#include "stock.h"
#include "nsat.h"
#include "var.h"
#include "delayed_mort.h"


#ifdef __cplusplus
extern "C" {
#endif

/** \brief Point structure that represents a geographic point using latitude
 * and longitude. The points can be joined into linked lists.*/
struct point {
	/** Next point in the list */
    struct point	*next;
    /** Previous point in the list */
    struct point   	*prev;
    /** Latitude */
    float		lat;
    /** Longitude */
    float		lon;
};

/** Type representing a dam river segment */
#define	DAM_SEG		1
/** Type representing a reach river segment */
#define	REACH_SEG	2
/** Type representing a headwater river segment */
#define	HEADWATER_SEG	3
/** Type representing an additional dam powerhouse river segment */
#define POWERHOUSE_SEG  4

/* Forward reference */
struct tributary;

/** \brief A segment of the river in the model.
 *
 * River segments are probably the most important data structure in the entire
 * model since they represent the river and all of its tributaries as well as
 * headwaters. Segments may be dams, extra powerhouses, reaches, or headwaters.
 *
 * All river segments form a tree with the global river_mouth segment being the
 * root of the tree. Segments may have up to 2 children using the up and fork
 * fields. All segments in the model are linked together in no partiular order
 * using the next field.
 */
struct river_segment {
    struct river_segment   *up;		/** Main branch upstream for the river */
    struct river_segment   *fork;	/** Fork (if any) to a second upstream segment */
    struct river_segment   *down;	/** Link back downstream */
    struct river_segment   *next;	/** For scanning purposes this connects all segments in the model in no particular order */

    /** The tributaries that attach to this segment, in order from downstream
     *  to upstream, linked by their "trib_list" pointers.
     */
    struct tributary       *tributaries;

    /** Name of the river of which this segment is a part */
    char       *river_name;
    /** Name of this segment */
    char       *seg_name;
    /** Abbreviated name of this segment */
    char       *abbrev;

    /** Type of this segment using the following:
     * - DAM_SEG
     * - POWERHOUSE_SEG
     * - REACH_SEG
     * - HEADWATER_SEG
     */
    int		seg_type;

    /** Runtime output flag field */
    unsigned int output_flags;
    /** Stored output settings (for selecting sections of output. see summary.c and \ref output_settings for more information) */
    unsigned int output_settings;

    /** This union stores the type-specific information about a river segment. */
    union {
	/** Dam-specific segment data (i.e. the dam parameters) */
	struct dam {
	    /** Species-specific information at a dam (i.e. the dam species parameters) */
	    struct dam_species {
		float	spill_mort;	/** Mortality in conventional spillways */
		float	bypass_mort;	/** Mortality in the bypass */
		float	sluiceway_mort;	/** Mortality in the sluiceway */
		float	turbine_mort;	/** Mortality in the turbines */
		float	transport_mort;	/** Mortality in transport */
		float	rsw_mort;	/** Mortality in the RSWs */
	        float   mean_forebay_transit_time;  /** In hours, for forebay pred mort */
		float	separation_prob; /** Chance of non-collection during a transport operation */
		float	pred[DAM_TIME_SLC];	/** Predation given at each dam time slice */
				
		float	pred_pop_mean; /** Predation population in the forebay */
		float	pred_pop_tailrace; /** Predation population in the tailrace */

	    equation	spill_eqn; /** Spill equation to determine the proportion of fish routed to spillways (both conventional and RSW) */
		equation	fge_eqn; /** Fish guidance efficiency for determining the proportion of non-spilled fish routed through the bypass system */
		equation 	trans_eqn; /** Transport mortality equation to estimate transport mortality based on water particle travel time */
		/** Delay equation that determines delay of fish at a dam depending on
		 * species, time of day, season, and flow (relative to hydraulic
		 * capacity). Yield a passage probability. Note that this has been mostly
		 * superseded by the newer dam delay model that allows for simulating diel
		 * passage. See \ref new_dam_delay. */
		equation    	delay_eqn;
		equation	rsw_eqn; /** RSW efficiency which determines the proportion of fish that go through RSW */

		/** Proportion of non-spilled fish that pass through SBC or sluicway */
		float	sluiceway_prop;

		float day_fge; /** Day time FGE */
		float night_fge; /** Night time FGE */

		/** \section new_dam_delay Dam delay and diel passage
		 * New dam delay support that varies by passage route and day/night. The new
		 * dam delay mechanism allows extra delay for each route to be specified and
		 * then delay for each route that is only incurred during the day. This can
		 * be used to simulate diel passage.
		 *
		 * This works by delaying a certain fraction of fish by whole number of days
		 * in order to have the whole group average a (probably fractional) amount of
		 * delay. */
		/* Extra delay for passage routes */
		float	spill_delay; /** Extra delay for fish entering conventional spillways */
		float	bypass_delay; /** Extra delay for fish entering bypass system */
		float	sluiceway_delay; /** Extra delay for fish entering sluiceway */
		float	turbine_delay; /** Extra delay for fish entering turbines */
		float	rsw_delay; /** Extra delay for fish entering RSW */
		/* Probability that a fish arriving during the day will pass at night */
		float	spill_day; /** Extra delay for fish arriving during the day when entering conventional spillways*/
		float	bypass_day; /** Extra delay for fish arriving during the day when entering bypass system*/
		float	sluiceway_day; /** Extra delay for fish arriving during the day when entering sluiceway*/
		float	turbine_day; /** Extra delay for fish arriving during the day when entering turbines*/
		float	rsw_day; /** Extra delay for fish arriving during the day when entering RSW */

	    } *species, *spec;

	    /* physical characteristics; some may be computed from others,
	     * depending on which were found in the river description file.
	     */

	    int			num_powerhouses; /** Total number of powerhouses. */
	    
	    float		powerhouse_capacity;  /** Capacity in KCFS of this powerhouse. */
	    float		width;		/** Tailrace width (ft) */
	    float		tailrace_length;/** Tailrace length in ft */
	    float		base_elev; 	/** Dam floor elevation (ft) */
	    float		fb_elev; 	/** Forebay elevation (ft) */
	    float		tr_elev; 	/** Tailrace elevation (ft) */
	    float		fp_head; 	/** Full pool head */
	    float		fp_fb_depth;	/** Full pool forebay depth */
	    float		bypass_height;	/** Height of bypass orifice */

		int			collector; /** Whether or not this dam is a collector dam.
								* (i.e. it has PIT tag detectors and a
								* different kind of bypass system which can
								* produce latent effects) */

	    float		ngates;		/** Number of spill gates */
	    float		gate_width;	/** Width of each gate (ft) */
	    float		pergate; 	/** Max spill per gate */
	    float		spillway_width; /** Spilway width */
	    float		basin_length;	/** Stilling basin depth */
	    float		sgr;		/** Specific grav of roller */

	    /* spill and flow stuff */
	    float	spill[DAM_SLICES_IN_SEASON]; /** Spill fraction given at each time slice */

	    /** Planned spill vector is computed from planned_spill days/nights,
               then serves as inpute to compute_flow() */
	    float	planned_spill[DAYS_IN_SEASON*DAM_TIME_SLC];

	    PERIOD_LIST	*planned_spill_day; /** Planned spill during the day */
	    PERIOD_LIST	*planned_spill_night; /** Planned spill during the night */

		/* These two variables are used to implement support for legacy tokens
		 * planned_spill and fish_spill. Only planned_spill_day and
		 * planned_spill_night should be used by new users and these legacy
		 * tokens only set planned_spill_day and planned_spill_night. */
	    PERIOD_LIST	*legacy_planned_spill; /** Planned spill during the day. This is used to implement support for legacy planned_spill and fish_spill tokens. */
	    PERIOD_LIST	*legacy_fish_spill; /** Planned spill during the day. This is used to implement support for legacy planned_spill and fish_spill tokens. */

	    float 	spill_cap;	/** Maximum unforced spill (kcfs) */
	    FlowLocation spill_side;     /** Left bank or right bank */
			
	    float	flow_max;	/** For slider top-end */
	    float	flow_min_project; /** User data; used by modulators*/
	    float 	flow_min_river; /** Calculated from river desc */
	    float	flow[DAYS_IN_SEASON]; /** Flow in KCFS given at each day */

	    /* multiple powerhouse stuff */
	    
	    struct river_segment *additional_pwrhs; /** Pointer to additional powerhouse if it exists */
	    int		phouse_priority; /** Ranking of each phouse. The best priority is 0 followed by increasing positive numbers. */
	    
	    /* Note that flow_min controls when the next powerhouse turns on */
	    PERIOD_LIST	*phouse_schedule; /** Powerhouse operation schedule (in case some powerhouses are only available at certain times) */

	    /** The threshold and schedule are used at run time to determine
	     * the actual percentage flow through this powerhouse, tracked
	     * here as flow_fraction
	     */
	    float	flow_fraction[DAM_SLICES_IN_SEASON];

	    /* other optional associated info */
            struct basin *storage_basin; /** Storage basin (may be NULL). */
	    struct transport *transport; /** Transport operation (this may be NULL) information. */

	    /* nsat stuff */
	    GasDistribution *gas_in; /** Gas distribution input */
	    GasDistribution *gas_out; /** Gas distribution output */
	    float *initial_gas; /** Only defined if there is an initial gas
				 * vector which supercedes any in-river gas */
	    
	    equation	nsat_eqn; /** Nitrogen supersaturation equation that determines the production of gas during day hours */
	    equation	nsat_night_eqn; /** Nitrogen supersaturation equation during night hors */
	    equation	nsat_backup_eqn; /** Nitrogen supersaturation equation for when the spill fraction drops out of the range in which the day and night equations reasonably model behavior */
	    float	gas_theta; 	/** Mixing parameter */
	    float 	k_entrain; 	/** Powerhouse side gas entrainment */

	    /* actual depths, calculated from adjacent reaches, with drawdown */
	    float	forebay_depth[DAYS_IN_SEASON]; /** Forebay depth at each day */
	    float	tailrace_depth[DAYS_IN_SEASON]; /** Tailrace depth at each day */

	    /* useful params, calculated during each simulation */
	    float	drop_ratio[DAYS_IN_SEASON]; /** Drop ratio. I'm not actually sure what this is. */
	    float	tr_drop_ratio[DAYS_IN_SEASON]; /** Drop ratio for the tailrace. I'm not actually sure what this is. */
	    float 	daylight_proportion[DAM_SLICES_IN_SEASON]; /** Proportion of the day that is light given at each dam time slice. */
	    
	    float	water_temp[STEPS_IN_SEASON]; /** Water temperature given at each model time step. */
		int     init_temp_flag;  /** 1 if input_temp = "0N" (reading temperatures), 0 if "Off" */

		float   turbidity[STEPS_IN_SEASON]; /** Turbidity at each model time step. */
		int     turbidity_flag;  /** 1 if input_turb = "0N" (reading turbidity), 0 if "Off" (propegate from above)*/

		/* Removable spill weirs */

		float	rsw_spill_cap;	/** Maximum flow (partitioned from spill) through RSW (setting this to 0 essentially turns off RSWs) */
			
	} dam;

	/** Reach/reservoir-specific parameters. A reach is a segment of the actual river (where water flows). */
	struct reach {
	    /** Species-specific parameters for reaches */
	    struct reach_species {
	        float	pred_pop_mean;	/** Predator population. */
	    } *species, *spec;

        int     rclass;      /** Reach class number which should be on  [0..num_reach_classes-1] */
	    float	length;		/** Length of segment in miles */
	    float	volume;		/** Volume at max reservoir level; ft^3*/
	    float	surface_area;	/** Average; ft^2 */
	    float	width;		/** Average; ft */
	    float	depth;		/** Average max; ft */
	    float	upper_depth;	/** Max at upper end of reach; ft */
	    float	lower_depth;	/** Max at lower end of reach; ft */
        float   lower_elev;  /** Height above sea level, or bottom of
                              *  river at the lower end of reach; ft. */
	    float	slope;		/** Angle of sides, from vertical; deg */
	    float	elev_change[DAYS_IN_SEASON]; /** Delta from max */

	    float	cur_volume[STEPS_IN_SEASON];   	/** Current volume */
	    float	velocity[STEPS_IN_SEASON]; 	/** Current velocity */
	    float	flow_max;	/** For slider top-end */
	    float	flow_min;	/** Summed from headwaters */
	    float	flow[DAYS_IN_SEASON]; /** Flow given at each day */

	    GasDistribution *gas_in; /** Input gas distribution */
	    GasDistribution *gas_out; /** Output gas distribution */
	    float	gas_theta; 	/** Mixing parameter */
	    float *initial_gas; /** Only defined if there is an initial gas
				 * vector which supercedes any in-river gas */
	    
        /** This equation is not actually stored in data files; it is
         *  included only for insight into gas distribution mechanisms. */
	    equation	gas_dist_eqn;

	    float	loss_max; /** Maximum loss at this reach */
	    float	loss_min; /** Minimum loss at this reach */
	    float	loss[DAYS_IN_SEASON]; /** Loss given at each day */
	    float	water_temp[STEPS_IN_SEASON]; /** Water temperature at each model time step */
	    float	delta_temp[STEPS_IN_SEASON]; /** Modifies water_temp */
		int     init_temp_flag;  /** 1 if input_temp = "0N" (reading temperatures), 0 if "Off"*/

		float   turbidity[STEPS_IN_SEASON]; /** Turbidity given at each model time step */
		int     turbidity_flag;  /** 1 if input_turb = "0N" (reading turbidity), 0 if "Off" (propegate from above)*/

        float   pred_dist_coef; /** "Predator distribution coefficient" -
                                 *  used in V1.6 pred/vol interaction. */

	    float	water_particle_tt; /** particle travel time; computed */
	} reach;
		
	/** Headwater-specific parameters */
	struct headwater {
	    float	flow_coefficient; /** For unregulated flow */
	    float	flow_max;	/** For slider top-end */
	    float	flow_min;	/** For modulation or propagation */
	    float	flow[DAYS_IN_SEASON]; /** Flow given at each day */
	    float	elev_change[DAYS_IN_SEASON]; /** Delta from max */
	    GasDistribution *gas_out; /** Output gas distribution */
	    float *initial_gas; /** Only defined if there is an initial gas
				 * vector which supercedes any in-river gas */
	    
	    float	water_temp[STEPS_IN_SEASON]; /** Water temperature at each model time step */

		float   turbidity[STEPS_IN_SEASON]; /** Turbidity at each model time step */
		int     turbidity_flag;  /** 1 if input_turb = "0N" (reading turbidity), 0 if "Off" (propegate from above)*/

	    float       flow_mean; /** Mean flow at this headwater */
	} headwater;

    } *current, *comparison, run[ NRUNS ];

    /* list of map coordinates (exactly one for dam segment) */
    struct point   *bottom, *top;

    int		temp1;		/** Temporary (used in flow.c) */

};

/** Transport operation parameters */
struct transport {
    int		start_by_date;		/** Bool: start trans by date or count */
    int		start_date;		/** For transport start by date */
    int		start_count;		/** For transport start by count */
    int		max_restarts;		/** Max restarts; -1 for infinite */
    int		restart_by_date;	/** Bool: restart trns by date or count*/
    int		restart_date;		/** For transport restart by date */
    int		restart_count;		/** For transport restart by count */
    int		end_date;		/** Hard date for transport end */
    int		end_count;		/** Low count for transport end */
    int		num_low_days;		/** Number of consecutive low count 
					 * days required to end transport
					 */ //kjhkj
    float	hfl_passed_perc;	/** Percentage passage of hfl_spec
					 * for full transport
					 */
    float	hi_flow;		/** High flow val for partial transport*/
    struct species *hfl_spec;		/** Species to return to river during
					 * high flow
					 */
    
    float	rate;			/** Rate of transport (i.e. truck or barge?) */

    struct river_segment *source_seg;	/** Dam transported around */
    struct river_segment *target_seg;	/** Release point */
};

/** \brief Species parameters.
 *
 * Major species groups are:
 * - Chinook 0 (sub-yearlings)
 * - Chinook 1 (yearlings)
 * - Coho
 * - Sockeye
 * - Steelhead
 */

struct species {
    int 	index;			/** Index into global array */
    char	*name;			/** Name of species/cohort */

    StockT	stock;			/** Generic stock params this species */
    /* All float []'s are dimensioned by number of reach classes. */
    float	reach_pcoef[10];  /** Reach predation coefficients */
    float   pprime_a[10];     /** Used to calculate p' for growth alg. */
    float   pprime_b[10];     /** Used to calculate p' for growth alg. */
    float	tailrace_pcoef; /** Used in dam predation mortality. */
    float	forebay_pcoef; /** Used in dam predation mortality. */
/* even newer gmort stuff */
    equation	gmort_eqn; /** Gas bubble disease mortality equation */
    equation	fdens_eqn; /** Used in gas mortality calculations */

	float inriver_latent_mort; /** Latent mortality for post-Bonneville calculations (inriver) */
	float transport_latent_mort; /** Latent mortality for post-Bonneville calculations (transported) */

	/** Differential return for post-Bonneville calculations */
	float differential_return;

	/** This equation is used to estimate latent mortality of inriver fish and is used in the S3 vs. WTT post-Bonneville algorithm */
	equation	inriver_latent_mort_eqn;

/* new now old gmort stuff 
 *   float	gm_low_slope;
 *   float	gm_high_slope;
 *   float	gm_critical;
 */
/* old gmort stuff
 *   float	gmcoef_mean;
 *   float	gmcoef_low;
 *   float	gmcoef_high;
 *   float	gmexp;
 *   float	gmthresh;
 */

    /* health vs. experience curve? */
    /* any other parameters of the model that might vary by species */
};

/**
 * \brief A structure to represent a storage basin associated with a dam.
 */

struct basin {
    float volume[DAYS_IN_SEASON];	/* Per day storage basin volumes */
    float min_volume;			/* Minimum allowable volume */
    float max_volume;			/* Maximum allowable volume */
};

/** \brief A structure to describe tributaries.
 *
 * These are simply points which
 *  decorate reaches - they make no flow contribution, and are in fact
 *  completely ignored by the downstream juvenile model. The upstream adult
 *  model uses them as places where straying may occur.
 */
struct tributary {
    struct tributary *next;
    struct tributary *trib_list;  /* the tribs hanging off a given reach */
    char		*name;
    struct point   	*latlon;
    struct river_segment *seg;
    float		seg_mile;
};

/* Various structures to handle release sites and release tracking statistics.
 */

/** Release site structure */
struct release_site {
    struct release_site *next;

    char		*name;
    struct point   	*latlon;
    struct river_segment *seg;
    float		seg_mile;
};

/** Release summary statistics structure. These are statistics for the entire release at a segment */
struct rls_stats {			/* summary statistics for a seg */
    int		done;			/** Compute done flag */

	/** Passage statistics (separated by total, inriver, and transport) */
	struct passage_stats {
		int		first;	/** First day of passage */
		int		last;	/** Last day of passage */
		float	total;	/** Total passage */
		float	mean;	/** Mean passage */
		float	median;	/** Median passage */
		float	mode;	/** Mode passage */
		float	std_dev;	/** Standard deviation of passage */
	} total, inriver, transport;

	/* Route tracking */
    float 	project_in_total;	/** Total number of fish entering the segment */
    float	turbine_in_total;	/** Total number of fish entering the turbines */
    float	sluiceway_in_total;	/** Total number of fish entering the sluiceway */
    float	bypass_in_total;	/** Total number of fish entering the bypass system */
    float	spillway_in_total;	/** Total number of fish entering the conventional spillways */
    float	rsw_in_total;	/** Total number of fish entering RSW */
    
    float	sluiceway_total;	/** Total passed through sluiceways */
    float	bypass_total;	/** Total passed through bypass system */
    float	turbine_total;	/** Total passed through turbines */
    float       spillway_total;	/** Total passed through conventional spillways */
    float	rsw_total;	/** Total passed through RSW */
    float 	premort_transport_total; /** Total being transported out (corresponds to transport_out data) */
    float	postmort_transport_total;/** Total that arrive at the destination (corresponds to transport_pass) */
	float	transport_in_total; /** Total entering transport */

	float	spill_efficiency; /** Spill efficiency ((proportion of fish spilled) / (proportion of water spilled)) */

	delayed_mortality_table *d; /** Table for calculating delayed mortality ("D"). This is used at the default_transport_target segment */
	float	inriver_latent_mort;	/** Inriver latent mortality */
	float	transport_latent_mort;	/** Transported latent mortality */
	float	differential_return;	/** Differential return */
	float	inriver_return;	/** Return rate for inriver */
	float	transport_return; /** Return rate for transported fish */
	float	overall_return; /** Overall return rate for fish (weighted average of inriver/transported return rates) */
	float	post_bonneville_total; /** Fish estimated to survive (out of those that arrive at this segment, i.e. project_in_total) */

	float	night_passage;	/** The proportion of fish that pass at night */
    float	avg_flow;		/** avg flow at indicator over run */
};

/** Structure for release statistics at a segment that are calculated and then converted into summary statistics. */
struct rls_seg_data {
    float	*output;		/** Number that left that day */
    float   *length;        /** Mean length of fish that left that day.
					 * size is [STEPS_IN_RUN]
					 */
	float	*dam_spill;		/** Mean spill fraction experienced by fish that left that day,
							 * used for the Z15 model. Size: STEPS_IN_RUN */

	float	*output_transport;		/** The number of fish that pass which were transported in (this is only used at a transport destination).
									 * This is needed so that statistics (like mean, median, mode, etc.) can be calculated separately for inrvier vs. transported fish */

	float	*delayed_survival_rate;		/** Rate of survival for fish that are delayed while passing through a downstream dam.
										 * Note that this is only calculated in segments that are directly upstream from a dam
										 * and the size is STEPS_IN_RUN */

    float	*transport_out;		/** Number transported out (dam only)*/
    float	*transport_in;		/** Number transported in (reach only)*/
    float   *transport_in_length; /** Mean length of the above. */
    float	*project_in;		/** After forebay pred; used for FPE */
#ifdef ROUTE_TRACKING
    /* these vars track passage routes at dams only.  they represent
     * post-mortality passage through the various dam routes.
     */
    float	*turbine_in;		/** Entering turbine; also for FPE */
    float	*sluiceway_in;		/** Entering bypass */
    float	*bypass_in;		/** Entering bypass */
    float	*spillway_in;		/** Entering conventional spillway */
    float	*rsw_in;		/** Entering RSW */
    
    float	*sluiceway_pass;		/** Number surviving sluiceway */
    float	*bypass_pass;		/** Number surviving bypass */
    float 	*turbine_pass;		/** Number surviving turbine */
    float 	*spillway_pass;		/** Number surviving conventional spillway */
    float	*rsw_pass;		/** Number surviving RSW */
    float	*transport_pass;	/** Number surviving transport index by date of start of transport from collector dam */
    float	*transport_arrived;	/** Number surviving transport indexed by date of arrival at the destination */

	delayed_mortality_table *d; /** This gets copied into stats.d when computing statistics but should be freed only from here (the pointer in stats gets NULLed when computing statistics) */

	float	inriver_latent_mort; /** Copied into stats */
	float	transport_latent_mort; /** Copied into stats */
	float	differential_return; /** Copied into stats */
	float	inriver_return;	/** Return rate for inriver (copied into stats) */
	float	transport_return; /** Return rate for transported fish (copied into stats) */
	float	overall_return; /** Copied into stats */
	float	post_bonneville_total; /** Copied into stats */

#endif    
    float	nopass;			/** Censored fish (fish that don't leave the model before simulation is complete) */
    struct rls_stats stats;		/** Summary statistics for this seg */
};
    
/** Release segment (where calculated data is stored for a release at a segment). */
struct rls_seg {
    struct rls_seg 	*up;		/** Next one upstream */
    struct rls_seg 	*down;		/** 	Next one downstream */
    struct river_segment *river_segment; /** The river segment that this data corresponds to */

    struct rls_seg_data **data;		/** Dynamic array of pointers to
					 *  seg data [games], null terminated
					 */
};

/** Release parameters */
struct release {
    struct release *next;	/** Next release (for scanning) */
    struct release *prev;	/** Previous release (for scanning) */

    struct rls_seg *rls_seg;		/** First one MUST be a reach_seg */

    struct release_site *site;		/** Where the release is released */
    struct species *species;		/** The species of fish in this release */
    StockT *stock;			/** The stock of fish that is released */

#ifdef REALTIME
    RtInfo *rt;				/** Realtime info (optional) */
#endif    

	int		add_setting;	//For determining how to add the release to the system.
    int     dirty_flag;     /** For use by GUI release editor. */
    int		start_date;		/** Offset into season */
    int		smolt_onset;		/** Start of smoltification */
    int		smolt_finish;		/** End of smoltification */
    float	length;			/** Average length at release */
    float	vitality_at_release;	/** Vitality at release (this isn't used) */
    float	vitality_change_rate;	/** Vitality change rate (this isn't used) */
	float	initial_spill_experience; /** If the release is made in a segment that isn't immediately downstream from a dam, then the dam spill experience will be filled in with this value */
    float	number[STEPS_IN_RUN];	/** The release distribution */

    float	total_released;		/** In last run */
    struct rls_stats stats;
    float 	migr_onset_median;	/** Migration onset, median value
					   in days (computed) */
    char	*temp;			/** Temporary data field */
};

/** Entire run parameters */
struct run {
    struct release *release_list;	/** Releases */

    int		iterations;		/** Monte carlo iterations */

    float	gas_dissp_exp;		/** z */
    float	flow_proportion;	/** For unreg hdw flow modulation */
    float	Ufree;			/** Free flowing vel const (kfs) */
    float   fork_threshold; /** Proportion of total flow a fork needs to be
                             *  "significant" - range [0.0-0.4] */
    float prey_energy_density; /** Value == 0 => no growth calculated */
    float length_weight_b0;  /** Also used in growth calculations. */
    float length_weight_b1;  /** Also used in growth calculations. */

	float min_migr_rate;  /**  Minimum migration rate for the entire run */

    struct river_segment *flow_seg;	/** Segment with indicative flow	 */
#ifdef REALTIME    
    char 	*rt_report_date; 	/** Realtime report date */
#endif
    
   /** Average system-wide water particle travel time for transport mort */
    struct water_travel {		
	struct river_segment *upper;	/** Upper seg for computation */
	struct river_segment *lower;	/** Lower seg for computation */
	int first_day;			/** First day for computation */
	int last_day;			/** Last day for computation */

	float time;			/** Computed water travel time */
    } water_travel;
    
    unsigned int flags;			/** Run time flags */
    unsigned int mortality_class; /** Which mortalities are calculated -
                                   *  only a few combinations are allowed. */
    /* mortality_class: GAS_AND_PREDATION_CLASS = gas & predation
			XT_CLASS = XT reservoir survival model */

	int post_bonneville_hypothesis; /** Which hypothesis should be used for post-Bonneville calculations */
};

/** How many valid mortality classes are defined. run.mortality_class must
 *  be in range [0..N_MORTALITY_CLASS-1] */
#define N_MORTALITY_CLASS 3
/** Names of mortality classes for use as input tokens */
extern char *mortality_class_names[N_MORTALITY_CLASS];

/** Gas and predation class */
#define GAS_AND_PREDATION_CLASS	0
/** Distance and time class */
#define XT_CLASS	1
/** Custom class (also uses distance and time, see custom_survival.h) */
#define CUSTOM_CLASS	2

/** Performing XT calculations */
#define DO_XT	(current->mortality_class == XT_CLASS)
/** Performing custom calculations */
#define DO_CUSTOM	(current->mortality_class == CUSTOM_CLASS)

/** Post-Bonneville hypotheses/algorithms */
enum {
	POST_BONNEVILLE_SAR_VS_DATE,
	POST_BONNEVILLE_LATENT_MORTALITY,
	POST_BONNEVILLE_CONSTANT_D,
	POST_BONNEVILLE_S3_VS_WTT,
	NUM_POST_BONNEVILLE
};
extern const char *post_bonneville_hypothesis_names[NUM_POST_BONNEVILLE]; /** These are for the actual input tokens for post-Bonneville hypothesis selection. */
extern const char *post_bonneville_hypothesis_names_formatted[NUM_POST_BONNEVILLE]; /** These may contain spaces and other characters not allowed in input tokens and are the "pretty" hypothesis names. */



/*===================================================================*/
/* Information in crisp.c                                            */
/*===================================================================*/

/** Release sites */
extern struct release_site  *release_site;

/** Tributary */
extern struct tributary *tributary;

/** River mouth, the root of the river segment tree. */
extern struct river_segment *river_mouth;

/** Map limits */
extern float	lat_min, lat_max, lat_range;	/* map limits */
/** Map limits */
extern float	lon_min, lon_max, lon_range;

/** Number of species */
extern int num_specs;
/** Species structures */
extern struct species *specs;
/** Species names */
extern char **spec_names;
/** Stock data */
extern struct stock_info stocks;

/** Run information */
extern struct run *current, *comparison, run[ NRUNS ];

/** If set, this flag prevents output settings from being modified by the
 *  input data file parser (only). */
extern int lock_output_settings;
/** If set, this flag prevents variance suppression from being modified by the
 *  input data file parser (only). */
extern int disable_variance_suppression;


/** Dynamically allocated array of pointers to all dams (but not extra powerhouses) */
extern struct river_segment	**dams;
/** Dynamically allocated array of pointers to all dams and powerhouses */
extern struct river_segment	**phouses;

/** Number of dams and powerhouses as calculated at run time*/
extern int num_dams,
    	   num_powerhouses;

/** Default list of segments for a variety of purposes */

extern LIST *default_segment_list;

/** Program version retrieval routine (but you should probably use version.h) */

extern char *get_program_version();

/* Array of flags which turn individual types of mortality on and off.
 *  These are set from run.mortality_class during simulation_init()
 */
#define N_MORTALITY_TYPE 3
/** Turn on and off mortality types */
extern int mortality_type[N_MORTALITY_TYPE];
#define GAS_MORT 0
#define PRED_MORT 1
#define DO_GAS_MORT (mortality_type[GAS_MORT])
#define DO_PRED_MORT (mortality_type[PRED_MORT])

/** \section reach_classes Reach classes
 *
 * New for CRiSP v1.7 was the concept of "reach classes". Each reach has a small,
 *  non-negative number defining it's class. Here we give the number
 *  of allowed classes and the name of each. There is an absolute limit
 *  of 10 reach classes, which we expect will never be reached in
 *  practice.
 *
 * These classes are used to define parameters that are the same for a given
 * reach/species/stock but which vary depending on how far down in the river
 * it is (so the river is split up into several different reach classes).
 */
/** Number of reach classes */
extern int num_reach_classes;
/** Reach class names (e.g. "Class_0") */
extern char *reach_class_names[10];

/* New support for a simple notes section */
/** Maximum length of the notes section */
#define	MAX_NOTES_SIZE	16384
/** Notes on the scenario file */
extern char *input_notes;
extern char *file_creation_date; /** File's creation date */
extern char *file_creator; /** File's creator */
extern char *file_modification_date; /** Modification date */
extern char *file_modifier; /** The person that modified the file last */

/* Default input values. These disable as many features as possible
 * and when an input file is written out, only values that differ from
 * these defaults are written out. Thus, old parameters that are
 * generally disabled are hidden. */

extern struct species species_defaults;		/** Species defaults */
extern struct headwater headwater_defaults;	/** Headwater defaults */
extern struct release release_defaults;		/** Release defaults */
extern struct reach_species reach_species_defaults;	/** Reach species defaults */
extern struct reach reach_defaults;		/** Reach defaults */
extern struct dam_species dam_species_defaults;	/** Dam species defaults */
extern struct dam dam_defaults;			/** Dam defaults */

/** There is a segment that is the default transport target for the
 * I/O interface. Note that this is currently also where the
 * delayed mortality calculations are made. */

extern struct river_segment *default_transport_target;

#ifdef WIN32
/** Location in the Windows registry for the model's subkey
 * (currently only used for finding the installation path). */
#define	WINDOWS_REGISTRY_SUBKEY	"Software\\COMPASS"
#endif

#ifdef __cplusplus
}
#endif

#endif
