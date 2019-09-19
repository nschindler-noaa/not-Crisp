#include <string.h>
#ifdef WIN32
#include <stdio.h>
#endif

#include <stdlib.h>
#include <math.h>
#include "crisp.h"
#include "log.h"
#include "strtools.h"

#include "phouse.h"
#include "run_mode.h"
#include "batch.h"
#include "gas_mort.h"
#include "migration.h"
#include "fdens.h"
#include "delay.h"
#include "release.h"
#include "transport.h"
#include "daylight.h"
#include "spill.h"
#include "args.h"
#include "main.h"
#include "file.h"
#include "parse.h"

extern void make_transport_frame();
extern void remove_all_phouse_scheds();
extern int fill_basin(struct river_segment *dam, float volume);
int param_error(double low, double mean, double high, char *msg);
extern void init_release_stats();


/* One or the other of these must be defined or we refuse to compile.
 *  This allows the adult and juvenile models to share source files.
 */
#ifndef UPSTREAM
#ifndef DOWNSTREAM
%%%%%%%
#endif
#endif

/* The following routines are to initialize or redimension the
 *  reach class name array and the species and stock values which
 *  are arrays dimensioned by number of reach classes.
 */

/* Helper routines for set_num_reach_classes() */
void new_float( int newn, int oldn, float *fptr ) {
    int i;
    for (i = oldn; i < newn; i++)
        fptr[i] = fptr[0];
}

void new_equation( int newn, int oldn, equation *eptr ) {
    int i;
    for (i = oldn; i < newn; i++)
        bcopy( &(eptr[0]), &(eptr[i]), sizeof( equation ) );
}

void new_float_star( int newn, int oldn, float **fsptr ) {
    int i;

    /* NOTE these are temporary arrays which do not need to be initialized
     *  here. */
    if (newn > oldn) {
        for (i = oldn; i < newn; i++)
            fsptr[i] = (float *)malloc( STEPS_IN_SEASON * sizeof(float) );
    }
    else if (newn < oldn) {
        for (i = newn; i < oldn; i++) {
            free( fsptr[i] );
            fsptr[i] = NULL;
        }
    }
}

/** \brief Set a new number of reach classes.
 *
 * Make up some default names for
 *  any new reach classes created. Redimension the species and stock
 *  arrays which depend on this number.
 */
void set_num_reach_classes( int new_nrc ) {
    int i;
    struct river_segment *seg;
    struct reach *rchptr;

    if (new_nrc <= 0)
        return;

    if (new_nrc == num_reach_classes)
        return;

    /* Redimension array of reach class names. */
    if (new_nrc < num_reach_classes) {
        for (i = new_nrc; i < num_reach_classes; i++) {
            free( reach_class_names[i] );
            reach_class_names[i] = NULL;
        }
    }
    else /* if (new_nrc > num_reach_classes) */ {
        for (i = num_reach_classes; i < new_nrc; i++) {
            reach_class_names[i] = (char *)malloc( 12 );
            sprintf( reach_class_names[i], "Class_%d", i );
        }
    }

    /* Redimension arrays in species. */
    for (i = 0; i < num_specs; i++) {
        /* These parameters are known by species and reach class. */
        new_float( new_nrc, num_reach_classes,
                        specs[i].reach_pcoef );
        new_float( new_nrc, num_reach_classes, specs[i].pprime_a );
        new_float( new_nrc, num_reach_classes, specs[i].pprime_b );

        /* These parameters are known by stock by reach class (here,
         *  we update the default stock for the given species.). */
        new_float( new_nrc, num_reach_classes, specs[i].stock.mvcoef );
        new_float( new_nrc, num_reach_classes, specs[i].stock.vvar );
        new_float( new_nrc, num_reach_classes, specs[i].stock.distance_coeff );
        new_float( new_nrc, num_reach_classes, specs[i].stock.time_coeff );
        new_float( new_nrc, num_reach_classes, specs[i].stock.reach_survival_coef );

        /* These equations are known by stock by reach class (here,
         *  we update the default stock for the given species.). */
        new_equation( new_nrc, num_reach_classes,
                        specs[i].stock.migration_eqn );
        new_equation( new_nrc, num_reach_classes,
                        specs[i].stock.pred_temp_response_eqn );
        new_equation( new_nrc, num_reach_classes,
                        specs[i].stock.custom_survival_eqn );

        /* This temporary array is two dimensional (by number of
         *  reach classes, then by STEPS_IN_SEASON */
        new_float_star( new_nrc, num_reach_classes,
                        specs[i].stock.migr_b1fac );
    }

    /* Redimension arrays in stocks. */
    for (i = 0; i < stocks.num; i++) {
        /* These parameters are known by stock by reach class. */
        new_float( new_nrc, num_reach_classes, stocks.s[i]->mvcoef );
        new_float( new_nrc, num_reach_classes, stocks.s[i]->vvar );

        /* These equations are known by stock by reach class (here,
         *  we update the default stock for the given species.). */
        new_equation( new_nrc, num_reach_classes,
                        stocks.s[i]->migration_eqn );
        new_equation( new_nrc, num_reach_classes,
                        stocks.s[i]->pred_temp_response_eqn );
        new_equation( new_nrc, num_reach_classes,
                        stocks.s[i]->custom_survival_eqn );

        /* This temporary array is two dimensional (by number of
         *  reach classes, then by STEPS_IN_SEASON */
        new_float_star( new_nrc, num_reach_classes,
                        stocks.s[i]->migr_b1fac );
    }

    /* Make sure all reaches have a valid reach class number. */
    for (seg = river_mouth; seg; seg = seg->next) {
	    if (seg->seg_type == REACH_SEG) {
	        rchptr = &(seg->current->reach);
            if (rchptr->rclass >= new_nrc)
                rchptr->rclass = 0;
        }
    }

    /* This update must come last. */
    num_reach_classes = new_nrc;
}

/** \brief Initialize default release parameters */
void init_releases() {
	release_defaults.number[0] = 0;
	release_defaults.species = NULL;
	release_defaults.stock = NULL;
	release_defaults.start_date = 0;
	release_defaults.smolt_onset = 0;
	release_defaults.smolt_finish = 0;
	release_defaults.length = 0;
	release_defaults.vitality_at_release = 0;
	release_defaults.vitality_change_rate = 0;
	release_defaults.initial_spill_experience = 0;
}
 
/* The following routines are used by init_dams and to reset to defaults
 * in ctrl_frame.  Don't want to have to change them in more than one place.
 */

/** Used by init_dams() */
void init_spill_cap(damptr)
	struct dam * damptr;
{
    damptr->spill_cap = 1000;
}

/** Used by init_dams() */
void init_dam_species(damptr)
	struct dam *damptr;
{
    int k, l;

	/* Initialize defaults and then copy them in */
	dam_species_defaults.pred[0] = 1.0;
	dam_species_defaults.separation_prob = 0.0;
    dam_species_defaults.mean_forebay_transit_time = 0;
    dam_species_defaults.pred_pop_mean = 0;
    dam_species_defaults.pred_pop_tailrace = 0;
	dam_species_defaults.day_fge = 0.0;
	dam_species_defaults.night_fge = 0.0;

	dam_species_defaults.spill_delay = 0.0;
	dam_species_defaults.bypass_delay = 0.0;
	dam_species_defaults.sluiceway_delay = 0.0;
	dam_species_defaults.turbine_delay = 0.0;
	dam_species_defaults.rsw_delay = 0.0;

	dam_species_defaults.spill_day = 0.0;
	dam_species_defaults.bypass_day = 0.0;
	dam_species_defaults.sluiceway_day = 0.0;
	dam_species_defaults.turbine_day = 0.0;
	dam_species_defaults.rsw_day = 0.0;
	
	copy_equation(find_default_eq(EQ_LIN0),
		      &dam_species_defaults.spill_eqn);
	copy_equation(find_default_eq(EQ_LIN0),
		      &dam_species_defaults.fge_eqn);
	copy_equation(find_default_eq(EQ_LIN0),
		      &dam_species_defaults.trans_eqn);
	copy_equation(find_default_eq(EQ_LIN0),
		      &dam_species_defaults.delay_eqn);
    copy_equation(find_default_eq(EQ_LIN0),
		&dam_species_defaults.rsw_eqn);
    
    for (k=0; k < num_specs; k++)
    {
	for (l=0; l < DAM_TIME_SLC; l++)
	{
	    damptr->species[k].pred[l] = 1;
	}

	/* TODO: it would be nice to not have to support these old defaults from the old model */
	if (!strcasecmp("steelhead", spec_names[k]))
	    damptr->species[k].separation_prob = 0.20; /* steelhead */
	else
	    damptr->species[k].separation_prob = 0.80; /* others */
	
    damptr->species[k].mean_forebay_transit_time = dam_species_defaults.mean_forebay_transit_time;
    damptr->species[k].pred_pop_mean = dam_species_defaults.pred_pop_mean;
    damptr->species[k].pred_pop_tailrace = dam_species_defaults.pred_pop_tailrace;
	damptr->species[k].day_fge = 0.5;
	damptr->species[k].night_fge = 0.5;

	dam_species_defaults.spill_delay = dam_species_defaults.spill_delay;
	dam_species_defaults.bypass_delay = dam_species_defaults.bypass_delay;
	dam_species_defaults.sluiceway_delay = dam_species_defaults.sluiceway_delay;
	dam_species_defaults.turbine_delay = dam_species_defaults.turbine_delay;
	dam_species_defaults.rsw_delay = dam_species_defaults.rsw_delay;

	dam_species_defaults.spill_day = dam_species_defaults.spill_day;
	dam_species_defaults.bypass_day = dam_species_defaults.bypass_day;
	dam_species_defaults.sluiceway_day = dam_species_defaults.sluiceway_day;
	dam_species_defaults.turbine_day = dam_species_defaults.turbine_day;
	dam_species_defaults.rsw_day = dam_species_defaults.rsw_day;

	copy_equation(&dam_species_defaults.spill_eqn,
		      &damptr->species[k].spill_eqn);
	copy_equation(&dam_species_defaults.fge_eqn,
		      &damptr->species[k].fge_eqn);
	copy_equation(&dam_species_defaults.trans_eqn,
		      &damptr->species[k].trans_eqn);
	copy_equation(&dam_species_defaults.delay_eqn,
		      &damptr->species[k].delay_eqn);
    copy_equation(&dam_species_defaults.rsw_eqn,
		&damptr->species[k].rsw_eqn);
    }
}

static void	init_dams()
{
    int	 	i, j, k, l;
    int		dam_num;
    int		phouse_num;
    int		river_num;
    int		phouses_this_group;
    char	*current_river_name;
    struct river_segment *seg;

	/* Defaults */
	bzero(&dam_defaults, sizeof(struct dam));
	/* These values should essentially disable these parameters */
	dam_defaults.tailrace_length = 0;
	dam_defaults.gas_theta = 0;
	dam_defaults.k_entrain = 0;
	dam_defaults.flow_min_project = 0; /* Note: flow_min for dams is always output */
	dam_defaults.rsw_spill_cap = 0;

	copy_equation(find_default_eq(EQ_LIN0),  &dam_defaults.nsat_eqn);
	copy_equation(find_default_eq(EQ_LIN0),  &dam_defaults.nsat_night_eqn);
	copy_equation(find_default_eq(EQ_LIN0),  &dam_defaults.nsat_backup_eqn);

	/* Spill schedules */
	dam_defaults.planned_spill_day = NULL;
	dam_defaults.planned_spill_night = NULL;
	dam_defaults.legacy_planned_spill = NULL;
	dam_defaults.legacy_fish_spill = NULL;

    /* count dams and rivers with dams */
    num_dams = num_powerhouses = 0;
    phouses_this_group = 0;
    current_river_name = NULL;
    for (seg = river_mouth; seg != NULL; seg = seg->next)
    {
	/* initialize additional powerhouses and dams */
	if (seg->seg_type == DAM_SEG || seg->seg_type == POWERHOUSE_SEG)
	{
	    phouses_this_group++;
	    num_powerhouses++;
	    if (seg->seg_type == DAM_SEG)
		num_dams++;
	}
    }

    /* allocate and initialize the arrays dam and dam_group */
    dams = (struct river_segment **)
	malloc(sizeof(struct river_segment *) * num_dams);
    phouses = (struct river_segment **)
	malloc(sizeof(struct river_segment *) * num_powerhouses);
    dam_num = -1;
    phouse_num = -1;
    river_num = -1;
    current_river_name = NULL;
    for (seg = river_mouth; seg != NULL; seg = seg->next)
    {
	if (seg->seg_type == DAM_SEG || seg->seg_type == POWERHOUSE_SEG)
	{
	    phouses[++phouse_num] = seg;
	    if (seg->seg_type == DAM_SEG)
		dams[++dam_num] = seg;
	}
    }

    for (i=0; i < num_dams; i++)
    {
	for (j = 0; j < NRUNS; j++)
	{
	    struct river_segment *seg = dams[i];
	    struct dam *damptr = &(seg->run[j].dam);

	    init_dam_species(damptr);
	    /* copy defaults */
		damptr->tailrace_length = dam_defaults.tailrace_length;
	    damptr->gas_theta = dam_defaults.gas_theta;
	    damptr->k_entrain = dam_defaults.k_entrain;
	    damptr->flow_min_project = dam_defaults.flow_min_project;

	    copy_equation(&dam_defaults.nsat_eqn,  &damptr->nsat_eqn);
	    copy_equation(&dam_defaults.nsat_night_eqn,  &damptr->nsat_night_eqn);
	    copy_equation(&dam_defaults.nsat_backup_eqn,  &damptr->nsat_backup_eqn);

		/* Spill schedules */
		damptr->planned_spill_day = dam_defaults.planned_spill_day;
		damptr->planned_spill_night = dam_defaults.planned_spill_night;
		damptr->legacy_planned_spill = dam_defaults.legacy_planned_spill;
		damptr->legacy_fish_spill = dam_defaults.legacy_fish_spill;

	    compute_daylight_proportions(seg);

	    /* fill in defaults for items not found in river description */
	    if (damptr->powerhouse_capacity == 0)
		damptr->powerhouse_capacity = damptr->flow_max;

	    /* get forebay depth and head from elevations, or vice versa */
	    if (damptr->fb_elev != 0)
	    {
		damptr->fp_fb_depth = damptr->fb_elev - damptr->base_elev;
	    }
	    else
	    {
		if (damptr->fp_fb_depth == 0)
		    damptr->fp_fb_depth = 112;
		damptr->fb_elev = damptr->fp_fb_depth + damptr->base_elev;
	    }

	    if (damptr->tr_elev != 0)
	    {
		damptr->fp_head = damptr->fb_elev - damptr->tr_elev;
	    }
	    else
	    {
		if (damptr->fp_head == 0)
		    damptr->fp_head = 75;
		damptr->tr_elev = damptr->fb_elev - damptr->fp_head;
	    }
	    
	    if (   damptr->base_elev < -50 /* arbitrary min floor elev */
		|| damptr->fb_elev <= 0
		|| damptr->tr_elev <= 0
		|| damptr->bypass_height > damptr->fp_fb_depth
		|| damptr->fp_head < 0
		|| damptr->fp_fb_depth <= 0)
	    {
		log_msg(L_ERROR, "bad physical specs at %s\n",
			dams[i]->seg_name);
	    }

	    /* do some spillway width defaults if necessary */
	    if (damptr->pergate == 0 || damptr->ngates == 0
		|| damptr->gate_width == 0)
	    {
		if (damptr->spillway_width == 0)
		    damptr->spillway_width = 1320;
		damptr->ngates = 1;
		damptr->gate_width = damptr->spillway_width;
		damptr->pergate = 1; /* this won't matter with ngates == 1 */
	    }
	    init_spill_cap(damptr);
	    if (damptr->basin_length == 0)
		damptr->basin_length = 180;
	    if (damptr->sgr == 0)
		damptr->sgr = 1.0;

	    damptr->flow_min_project = 0;

	    if (damptr->storage_basin) {
		(void) fill_basin(dams[i], damptr->storage_basin->max_volume);
	    }

	    for (l = 0; l < DAYS_IN_SEASON*DAM_TIME_SLC; l++)
		damptr->flow_fraction[l] = 1.0;
	    damptr->phouse_priority = 0;
	    damptr->flow_min_project = 0;
	    damptr->phouse_schedule = make_default_phouse_schedule();
	    
	    /* check for additional powerhouses; do all of them */

	    if ((seg = damptr->additional_pwrhs))
	    {
		int num_powerhouses = 1;

		while (seg)
		{
		    num_powerhouses++;
		    seg->current->dam.phouse_priority = num_powerhouses - 1;
		    seg = seg->current->dam.additional_pwrhs;
		}

		damptr->num_powerhouses = num_powerhouses;
		for (seg = damptr->additional_pwrhs; seg;
		     seg = seg->current->dam.additional_pwrhs)
		{
		    struct dam *phouse = &seg->current->dam;
		    
		    bzero((char *) phouse->flow_fraction,
			  sizeof(phouse->flow_fraction));
		    phouse->num_powerhouses = num_powerhouses;
		    phouse->flow_min_project = 0;
		    phouse->phouse_schedule = make_default_phouse_schedule();
		    for (k=0; k < num_specs; k++)
		    {
			phouse->species[k].day_fge = 0.5;
			phouse->species[k].night_fge = 0.5;
			copy_equation(&damptr->species[k].fge_eqn,
				      &phouse->species[k].fge_eqn);
		    }
		}
	    }
	    else 
		damptr->num_powerhouses = 1;

	}
    }
}


void init_pred_pop_mean_reach(rchptr)
	struct reach * rchptr;
{
    int k;
    
    for (k=0; k < num_specs; k++)
    {
        rchptr->species[k].pred_pop_mean = reach_species_defaults.pred_pop_mean;
    }
}

void reset_water_temp(seg)
	struct river_segment *seg;
{
    int i;
    for (i=0; i < STEPS_IN_SEASON; i++)
	seg->current->headwater.water_temp[i] = 15.0;
}

/** init non-dam segs */
void init_other_segs()
{
    struct river_segment *seg;

	/* Fill in defaults */

	bzero(&headwater_defaults, sizeof(struct headwater));
	headwater_defaults.flow_mean = 0;
	headwater_defaults.flow_coefficient = -1;

	/* Fill in reach defaults */
	bzero(&reach_defaults, sizeof(struct reach));
	reach_species_defaults.pred_pop_mean = 0;
    reach_defaults.pred_dist_coef = 1.0;
	reach_defaults.gas_theta = 0;
	copy_equation(find_default_eq(EQ_LIN0),  &reach_defaults.gas_dist_eqn);
	reach_defaults.water_temp[0] = 0;
	reach_defaults.delta_temp[0] = 0;
	reach_defaults.turbidity[0] = 0;
    for (seg = river_mouth; seg; seg = seg->next)
    {
	if (seg->seg_type == REACH_SEG)
	{
	    struct reach *rchptr = &(seg->current->reach);

        rchptr->rclass = 0;
	    init_pred_pop_mean_reach(rchptr);
        rchptr->pred_dist_coef = reach_defaults.pred_dist_coef;
	    rchptr->gas_theta = reach_defaults.gas_theta;
	    copy_equation(&reach_defaults.gas_dist_eqn,  &rchptr->gas_dist_eqn);
		rchptr->water_temp[0] = reach_defaults.water_temp[0];
		rchptr->delta_temp[0] = reach_defaults.delta_temp[0];
		rchptr->turbidity[0] = reach_defaults.turbidity[0];

	    /* the model needs width and depth information.  this is supplied
	     * in the river description file, either directly, or in terms
	     * of volume and surface area, upper and lower depths,
	     * or some combination thereof.  we fill in the gaps where necessary
	     * (if possible).  if no values were read in, then the vars have
	     * value -1 as a marker.
	     */
	
	    /* first do depth, the most complicated */
	    if (rchptr->upper_depth > 0 && rchptr->lower_depth >= 0)
		rchptr->depth = (rchptr->upper_depth + rchptr->lower_depth) / 2;
	
	    else if (rchptr->depth >= 0)
		rchptr->upper_depth = rchptr->lower_depth = rchptr->depth;

	    else if (rchptr->volume >= 0 && rchptr->surface_area >= 0)
		rchptr->upper_depth = rchptr->lower_depth = rchptr->depth
		    = rchptr->volume / rchptr->surface_area;

	    else if (rchptr->volume >= 0 && rchptr->width >= 0)
		rchptr->upper_depth = rchptr->lower_depth = rchptr->depth
		    = rchptr->volume / (rchptr->width * rchptr->length * 5280);

	    else			/* no depth available */
	    {
		rchptr->depth = rchptr->upper_depth = rchptr->lower_depth = 0;
		log_msg(L_ERROR, "missing depth information for %s\n",
			seg->seg_name);
	    }

	    /* check to see that it came out ok */
	    if (rchptr->upper_depth > rchptr->lower_depth)
	    {
		log_msg(L_ERROR,
			"%s upper depth %.2f > lower depth %.2f\n\tsetting both to %.2f\n",
			seg->seg_name, rchptr->upper_depth,
			rchptr->lower_depth, rchptr->lower_depth);
		rchptr->upper_depth = rchptr->lower_depth;
	    }
	    /* do width */
	    if (rchptr->width < 0)
	    {
		if (rchptr->surface_area >= 0)
		    rchptr->width = rchptr->surface_area
			/ (rchptr->length * 5280);
	    
		else if (rchptr->depth >= 0 && rchptr->volume >= 0)
		    rchptr->width =
			rchptr->volume / (rchptr->depth * rchptr->length *5280);
	    
		else			/* no width available */
		{
		    rchptr->width = 0;
		    log_msg(L_ERROR,"missing width information for %s\n",
			    seg->seg_name);
		}
	    }

	    /* now that we've got all that straight, force flow.c to
	     * recompute the volume the fancy way.
	     */
	    rchptr->volume = -1;

	    /* default val for slope */
	    if (rchptr->slope < 0)
			rchptr->slope = 40 * M_PI / 180.0;

	    /* now do sanity checks on the results */
	    if (! (rchptr->lower_depth > 0
		   && rchptr->upper_depth >= 0
		   && rchptr->upper_depth <= rchptr->lower_depth
		   && rchptr->slope >= 0
		   && rchptr->slope < M_PI / 2
		   && tan(rchptr->slope)
		   	<= rchptr->width /
		   		(rchptr->upper_depth + rchptr->lower_depth)))
	    {
		rchptr->slope = atan(rchptr->width / (rchptr->upper_depth
						      + rchptr->lower_depth));
		log_msg(L_ERROR, "%s%s\n\t%s\n\t%s%.2f.\n",
			"invalid physical specifications for ",
			seg->seg_name,
			"Depth, width, or slope is invalid.",
			"Resetting for bottom width == 0 using slope ",
			rchptr->slope * 180.0 / M_PI);
	    }
	}
	
	/* init headwaters */
	if (seg->seg_type == HEADWATER_SEG)
	{
		/* Initialize to default */
		seg->current->headwater.flow_coefficient =  headwater_defaults.flow_coefficient;
		seg->current->headwater.flow_mean =  headwater_defaults.flow_mean;
	    reset_water_temp(seg);
	}
    }
}

/**
 *	\brief this routine comes last in the initialization sequence.  it
 *	initializes everything which is dependent upon other initialized
 *	variables.
 */

void init_dependent_variables()
{
    int i;

    for (i = 0; i < num_dams; i++)
    {
	/* get dam tailrace width from downstream reach, if present */
	if (dams[i]->down && dams[i]->down->seg_type == REACH_SEG)
	    dams[i]->current->dam.width = dams[i]->down->current->reach.width;

	/* otherwise try upstream */
	else if (dams[i]->up && dams[i]->up->seg_type == REACH_SEG)
	    dams[i]->current->dam.width = dams[i]->up->current->reach.width;

	else
	    log_msg(L_ERROR, "unable to get width information for %s\n",
		    dams[i]->seg_name);
    }
}


void init_river_parameters()
{
    struct river_segment *seg;
	struct river_segment *last_reach = NULL;
    
    current->gas_dissp_exp = 0.2;
    current->flow_proportion = 1.0;
    current->Ufree = 0.045;		/* kfs x10-1*/
    current->fork_threshold = 0.1;
#ifdef REALTIME
    current->rt_report_date = "Unknown";
#endif    

    /* water particle travel time stuff */
    current->water_travel.first_day = 1;
    current->water_travel.last_day = 365;
    current->water_travel.time = 0; /* calculated in compute_flow() */

    current->water_travel.lower = seg = river_mouth;
    /* find default upper seg for water travel computation */
    while (seg && strcmp(seg->seg_name, "Little Goose Pool")) {
	if (seg->seg_type == REACH_SEG)
	    last_reach = seg;
	seg = seg->next;
    }

    current->water_travel.upper = (seg) ? seg : last_reach;

    current->prey_energy_density = 0.0;
    current->length_weight_b0 = 0.0;
    current->length_weight_b1 = 0.0;
}

void init_runtime_flags()
{
    current->flags = 0;
    current->mortality_class = 0;
	current->post_bonneville_hypothesis = 0;
	current->min_migr_rate = .5;
}

/** init run info */

void init_runs()
{
    init_river_parameters();
    init_runtime_flags();
}

/** init stock info */

/* assuming the global list of stocks has been established, fill each
 * with default data.
 */
static void init_stock_defaults() {
	copy_equation(find_default_eq(EQ_LIN0),
		      &(stock_defaults.migration_eqn[0]));
	copy_equation(find_default_eq(EQ_LIN0),
		      &(stock_defaults.pred_temp_response_eqn[0]));
	copy_equation(find_default_eq(EQ_RETURN_LOG),
		      &(stock_defaults.inriver_return_eqn));
	copy_equation(find_default_eq(EQ_RETURN_LOG),
		      &(stock_defaults.transport_return_eqn));
	copy_equation(find_default_eq(EQ_LIN0),
		      &(stock_defaults.pred_capture_dist_eqn[0]));
	copy_equation(find_default_eq(EQ_SURVIVAL_Z15),
		      &(stock_defaults.custom_survival_eqn[0]));
	stock_defaults.mvcoef[0] = 1.0;
	stock_defaults.vvar[0] = 0.0;
	stock_defaults.time_coeff[0] = 1.0;
	stock_defaults.distance_coeff[0] = 0.0;
	stock_defaults.reach_survival_coef[0] = 1.0;
}
static void init_stock(StockT *stock) {
	copy_equation(&stock_defaults.migration_eqn[0],
		      &(stock->migration_eqn[0]));
	copy_equation(&stock_defaults.pred_temp_response_eqn[0],
		      &(stock->pred_temp_response_eqn[0]));
	copy_equation(&stock_defaults.inriver_return_eqn,
		      &(stock->inriver_return_eqn));
	copy_equation(&stock_defaults.transport_return_eqn,
		      &(stock->transport_return_eqn));
	copy_equation(&stock_defaults.pred_capture_dist_eqn[0],
		      &(stock->pred_capture_dist_eqn[0]));
	copy_equation(&stock_defaults.custom_survival_eqn[0],
		      &(stock->custom_survival_eqn[0]));
	stock->mvcoef[0] = stock_defaults.mvcoef[0];
	stock->vvar[0] = stock_defaults.vvar[0];
	stock->time_coeff[0] = stock_defaults.time_coeff[0];
	stock->distance_coeff[0] = stock_defaults.distance_coeff[0];
	stock->reach_survival_coef[0] = stock_defaults.reach_survival_coef[0];
}
static void init_stocks()
{
    int i;

	/* Set values for stock_defaults and copy them in */
	init_stock_defaults();

    for (i=0; i < stocks.num; ++i) {
		init_stock(stocks.s[i]);
    }
}

/** init species info */

void init_specs_data()
{
	struct species *species;
	int j;

	/* Initialize species defaults and then copy the defaults */
	init_stock_defaults();
	/* These values should disable these parameters */
	copy_equation(find_default_eq(EQ_LIN0), &species_defaults.gmort_eqn);
	copy_equation(find_default_eq(EQ_LIN0),&species_defaults.fdens_eqn);
	species_defaults.reach_pcoef[0] = 0.0;
	species_defaults.pprime_a[0] = 0.0;
	species_defaults.pprime_b[0] = 0.0;
	species_defaults.tailrace_pcoef = 0.0;
	species_defaults.forebay_pcoef = 0.0;
	/* Note that latent mortality values aren't suppressed */
	species_defaults.inriver_latent_mort = 0.0;
	species_defaults.transport_latent_mort = 0.0;
	species_defaults.differential_return = 1.0;
	copy_equation(find_default_eq(EQ_LATENT_MORT_WTT),
		      &(species_defaults.inriver_latent_mort_eqn));

    for (j = 0; j < num_specs; ++j) {
		species = &specs[j];

		/*bzero((char *)species, sizeof(struct species));*/

		species->index = j;
		species->name = spec_names[j];
		/* all specs have a generic stock */
		species->stock.name = "Generic"; 

		copy_equation(&species_defaults.gmort_eqn, &species->gmort_eqn);
		copy_equation(&species_defaults.fdens_eqn, &species->fdens_eqn);
		species->reach_pcoef[0] = species_defaults.reach_pcoef[0];
		species->pprime_a[0] = species_defaults.pprime_a[0];
		species->pprime_b[0] = species_defaults.pprime_b[0];
		species->tailrace_pcoef = species_defaults.tailrace_pcoef;
		species->forebay_pcoef = species_defaults.forebay_pcoef;
		species->inriver_latent_mort = species_defaults.inriver_latent_mort;
		species->transport_latent_mort = species_defaults.transport_latent_mort;
		species->differential_return = species_defaults.differential_return;
		copy_equation(&species_defaults.inriver_latent_mort_eqn,
				  &(species->inriver_latent_mort_eqn));

		/* Init the generic stock */
		init_stock(&species->stock);
	}
}

/** initialize the UI stuff and the underlying data */
static void init_specs()
{
    init_specs_data();
}

/** set up the list of default segments */

static void init_default_list()
{
	if (default_segment_list) free(default_segment_list);

    if (! (default_segment_list = (LIST *) malloc(sizeof(LIST))))
    {
	log_msg(L_ERROR, "no mem for default seg list!\n");
	return;
    }

    default_segment_list->next = default_segment_list->prev = NULL;
    
    /* list starts with just the river mouth */
    default_segment_list->item = (void *) river_mouth;
}

/** \brief general routine to do most inits */
int init_compass()
{
    extern void	init_seed();
    extern void init_reservoirs();
    extern void init_runs();
    extern void remember_current_directory();

    remember_current_directory(); 
    init_seed();
    init_dams();		/* This must come before init_flow_menus() */

	init_releases();

    set_num_reach_classes( 1 );
    init_specs();
    init_stocks();
    init_other_segs();
    init_runs();
    init_default_list();
    init_dependent_variables(); /* this must be last, to initialize vars
				 * dependent on other initializations.
			         */
	return 0;
}

/** \brief Remove optional flow/gas information that shouldn't be kept around */
void remove_all_flow() {
	struct river_segment *seg;
	for (seg = river_mouth; seg; seg = seg->next) {
		if (seg->seg_type == REACH_SEG) {
			bzero(seg->current->reach.loss, STEPS_IN_SEASON * sizeof(float));
			bzero(seg->current->reach.turbidity, STEPS_IN_SEASON * sizeof(float));
			bzero(seg->current->reach.delta_temp, STEPS_IN_SEASON * sizeof(float));
			bzero(seg->current->reach.water_temp, STEPS_IN_SEASON * sizeof(float));
			bzero(seg->current->reach.velocity, STEPS_IN_SEASON * sizeof(float));
			bzero(seg->current->reach.cur_volume, STEPS_IN_SEASON * sizeof(float));
			bzero(seg->current->reach.elev_change, DAYS_IN_SEASON * sizeof(float));
			if (seg->current->reach.initial_gas) {
				free(seg->current->reach.initial_gas);
				seg->current->reach.initial_gas = NULL;
			}
		}
		else if (seg->seg_type == DAM_SEG) {
			if (seg->current->dam.initial_gas) {
				free(seg->current->dam.initial_gas);
				seg->current->dam.initial_gas = NULL;
			}
		}
		else if (seg->seg_type == HEADWATER_SEG) {
			bzero(seg->current->headwater.water_temp, STEPS_IN_SEASON * sizeof(float));
			bzero(seg->current->headwater.turbidity, STEPS_IN_SEASON * sizeof(float));
			bzero(seg->current->headwater.elev_change, DAYS_IN_SEASON * sizeof(float));
			if (seg->current->headwater.initial_gas) {
				free(seg->current->headwater.initial_gas);
				seg->current->headwater.initial_gas = NULL;
			}
		}
	}
}

/**
 * \brief 
 *	Removes data which would not necessarily be overwritten upon
 *	new data entry (like read) but which should usually be cleared.
 *
 *	Eventually, when the user has a 'reset defaults' option, this
 *	routine will be called first, followed by a global initialize,
 *	which is why it is in this file.
 */

void remove_all_data()
{
    char initial_file[MAXPATHLEN];

    remove_all_spill_scheds();
    remove_all_phouse_scheds();
    remove_all_releases();
    remove_all_transport();
	remove_all_flow();

	free_variable_parameters(); /* remove all stochastic parameter distributions */
   
    current->flow_seg = NULL;

	/* Reinitialize and then reload the default input file */
    init_compass();
 
    /* If the output control file is specified, read it and then lock the
     *  output settings.
     */
    if (user_args.output_control) {
        if (find_compass_file( user_args.output_control, initial_file ))
            parse_datafile( initial_file, 0, 0 );
        else
            parse_datafile( user_args.output_control, 0, 0 );
        lock_output_settings = 1;
    }

    /* read in initial datafile, if specified
     */
    if (user_args.initial_data) {
        if (find_compass_file(user_args.initial_data, initial_file)) {
            parse_datafile(initial_file, 0, 0);
        }
        else
            /* call parser anyway, to generate error message
             */
            parse_datafile(user_args.initial_data, 0, 0);
    }
}

/**
 * \brief executed before every game run, if new data may have been read in.
 *	checks all stochastic parameters for error conditions.
 *
 *	\return: 0	no error, 1	error
 *
 *	The reason for this routine is twofold.  First, in the case of
 *	of an error, the specific offending parameter may be identified
 *	in an error message, and the run aborted.  Second, this removes a
 *	few checks from the broken_stick() routine, speeding it up slightly.
 *
 *	NOTE:  this parameter check is no longer performed during the actual
 *	sampling during the model run.  thus, every parameter had better
 *	be checked here.  omissions could be disastrous.
 */

static int stochastic_param_check()
{
	/* Check all distributions for errors. This code has been replaced in
	 * the stochastic parameter support files var.c and var.h. Names for
	 * parameters are now determined without hard-wiring by using the
	 * report_var structure and arrays from report.h and report_init.c */
    return check_variable_parameters();
}

/*
 * \breif 
 *	routine to do various checks to make sure that parameter data is
 *	basically sensible.
 *
 * calls stochastic_param_check() above, and
 *	performs checks on other types of params locally.  this should be
 *	called before a model run, and after every data read from file.
 *
 *	\return	0  	no error, 1  	error
 */

int data_check()
{
    struct river_segment *seg;
    struct release *release;
    float *elev;
    int erc = 0;
    int i, j;

    if (stochastic_param_check())
	++erc;
    
    for (seg = river_mouth; seg; seg = seg->next)
    {
	if (seg->seg_type == DAM_SEG || seg->seg_type == POWERHOUSE_SEG)
	{
	    if (check_daily_period_list(DAMPTR(seg)->planned_spill_day)
		|| check_daily_period_list(DAMPTR(seg)->planned_spill_night)
		|| check_daily_period_list(DAMPTR(seg)->legacy_planned_spill)
		|| check_daily_period_list(DAMPTR(seg)->legacy_fish_spill)
		|| check_daily_period_list(DAMPTR(seg)->phouse_schedule))
	    {
		++erc;
		log_msg(L_ERROR, "bad period list at %s\n",
			seg->seg_name);
	    }
	}
		
	/* check the elevation change array for out of range error */
	elev = NULL;
	if (seg->seg_type == REACH_SEG)
	    elev = seg->current->reach.elev_change;
	else if (seg->seg_type == HEADWATER_SEG)
	    elev = seg->current->headwater.elev_change;
	if (elev)
	{
	    int elev_err = 0;

	    for (i = 0; i < DAYS_IN_SEASON; i++)
		if (elev[i] > 0)
		{
		    elev[i] = 0;
		    elev_err = 1;
		}
	    if (elev_err)
	    {
		++erc;
		log_msg(L_ERROR, "%s%s%s\n",
			"elevation change out of range for ",
			seg->seg_name,
			";\n\tsetting to zero");
	    }
	}
    }

    /* check releases for bad smolt dates */
    for (release = run->release_list; release; release = release->next)
    {
	if (release->smolt_finish < release->smolt_onset)
	{
	    ++erc;
	    log_msg(L_ERROR, "%s%s\n",
		    "bad smolt dates for release ",
 		    release->site->name);
	}
    }

    /* check species for bad vals */
    for (i=0; i < num_specs; ++i)
    {
	    if (specs[i].fdens_eqn.fd_dmode > specs[i].fdens_eqn.fd_dbot)
	    {
	        ++erc;
	        log_msg(L_ERROR, "%s%s\n\t%s\n",
		        "bad population density equation params for ",
		        specs[i].name, "dmode > dbot");

	    }

        for (j = 0; j < num_reach_classes; j++) {
	        if ((specs[i].stock.migration_eqn[j].id == EQ_MIGR ||
                 specs[i].stock.migration_eqn[j].id == EQ_MIGR2) &&
                (specs[i].stock.migration_eqn[j].migr_bmin
	            > specs[i].stock.migration_eqn[j].migr_bmax))
	        {
	            ++erc;
	            log_msg(L_ERROR, "%s%s\n\t%s\n",
		            "bad migration equation params for ",
		            specs[i].name, "bmin > bmax");
	        }
        }
    }

    /* check stocks for bad vals */
    for (i=0; i < stocks.num; ++i)
    {
        for (j = 0; j < num_reach_classes; j++) {
	        if ((stocks.s[i]->migration_eqn[j].id == EQ_MIGR ||
                 stocks.s[i]->migration_eqn[j].id == EQ_MIGR2) &&
                (stocks.s[i]->migration_eqn[j].migr_bmin
	            > stocks.s[i]->migration_eqn[j].migr_bmax))
	        {
	            ++erc;
	            log_msg(L_ERROR, "%s%s\n\t%s\n",
		            "bad migration equation params for ",
		            stocks.s[i]->name, "bmin > bmax");
	        }
        }
    }

    return erc;
}

/*
 * \brief 
 *	routine called to do appropriate inits before a simulation.
 *	also checks to make sure that user parameters have sensible values.
 *	it is necessary to call this routine whenever user data
 *	parameters may have changed.  thus, it is called before every
 *	game in scenario mode, or after every new data file is read
 *	(each year) in monte mode.
 *
 *	\return: 0	no error
 *		1	error
 */

int simulation_init(mode)
    int mode;			/* scenario or monte mode */
{
#ifdef DOWNSTREAM
    int i, j, k;
#endif
    int erc = 0;
    struct river_segment *seg;
    extern float compute_drift();
    extern float compute_pred_radical();
    extern void set_scenario_output_flags();
    extern void set_monte_flow_seg();
    extern void refresh_windows_from_data();
    
    log_msg(L_DBGMSG, "\n");
    log_msg(L_DBGMSG,
	    "initializing simulation\n\t%s\t%s\n\t%s\t%s\n\t%s\t%s\n",
	    "pred dens/vol flag ",
	    (current->flags & PRED_VOL_FLAG) ? "On" : "Off",
	    "age dep fge flag ",
	    (current->flags & AGE_DEP_FGE_FLAG) ? "On" : "Off",
#ifdef NOVAR
	    "suppress variance flag ",
	    (current->flags & VAR_SUPPRESS_FLAG) ? "On" : "Off");
#else
    	    "","");
    /* if novar not compiled and suppression thrown, log error */
    if (current->flags & VAR_SUPPRESS_FLAG)
        log_msg(L_WARNING, "variance suppression not available.\n");
#endif

    /* Decide what types of mortality to calculate in this run.
     */
    mortality_type[GAS_MORT] = current->mortality_class == GAS_AND_PREDATION_CLASS;
    mortality_type[PRED_MORT] = current->mortality_class == GAS_AND_PREDATION_CLASS;
    log_msg( L_DBGMSG, "using mortality class %d\n", 
                                current->mortality_class );

    init_release_stats();

    /* perform various initializations if there are any fish */    
    if (run->release_list)
    {
#ifdef DOWNSTREAM
        for (i = 0; i < num_specs; i++)
	    {
            for (k = 0; k < num_reach_classes; k++) {
	            for (j = 0; j < STEPS_IN_SEASON; j++)
	                specs[i].stock.migr_b1fac[k][j] =
			            mb1factor(&specs[i].stock.migration_eqn[k],
			                   (float) j/STEPS_PER_DAY);
            }
	    }

	    for (i = 0; i < stocks.num; i++)
	    {
            for (k = 0; k < num_reach_classes; k++) {
                for (j = 0; j < STEPS_IN_SEASON; j++)
		            stocks.s[i]->migr_b1fac[k][j] =
		                mb1factor(&stocks.s[i]->migration_eqn[k],
			               (float) j/STEPS_PER_DAY);
            }
	    }
#endif
        /* initialize the output tracking flags */
	    for (seg = river_mouth; seg; seg = seg->next)
	    {
	        if (mode == SCENARIO)
    		    set_scenario_output_flags(seg);	/* also sets flow indicator (ifdef DISPLAY) */
	        else  
    		    seg->output_flags = seg->output_settings;
	    }
    }
    
    if (erc || data_check())
    {
	    erc = 1;
	    log_msg(L_ERROR,"aborting run due to bad parameter values.\n");

    }    
    
    return erc;
}

