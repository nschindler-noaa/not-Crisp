/* flow.c - downstream and upstream propagation routines, top level
 *  calls to flow modulation routines, plus all flow related menus
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "crisp.h"
#include "river.h"
#include "flow.h"
#include "log.h"
#include "nsat.h"
#include "phouse.h"
#include "run_mode.h"
#include "spill.h"

extern int calculate_flow(struct river_segment *dam, int day);

/** routine to return pointer to water temperature array for any seg */

float *get_temp_ptr(seg)
    struct river_segment *seg;
{
    switch(seg->seg_type)
    {
    case HEADWATER_SEG:
        return(seg->current->headwater.water_temp);
    
    case REACH_SEG:
        return(seg->current->reach.water_temp);

    case DAM_SEG:
        return(seg->current->dam.water_temp);

    default:
        log_msg(L_ERROR, "cannot get temperature of seg %s\n",
            seg->seg_name);
        return((float *) NULL);
    }
    
}

/** routine to return pointer to turbidity array for any seg */

float *get_turbidity_ptr(seg)
    struct river_segment *seg;
{
    switch(seg->seg_type)
    {
    case HEADWATER_SEG:
        return(seg->current->headwater.turbidity);
    
    case REACH_SEG:
        return(seg->current->reach.turbidity);

    case DAM_SEG:
        return(seg->current->dam.turbidity);

    default:
        log_msg(L_ERROR, "cannot get turbidity of seg %s\n",
            seg->seg_name);
        return((float *) NULL);
    }
    
}
/** utility routine to determine upon which "side" of a river a fork is
 * entering given three points of reference.  orientation is facing
 * downstream.  parameters are three latlons: the point of
 * intersection, an upstream point on the fork, and a downstream point
 * on the mainstem.
 */

FlowLocation
determine_side(intersection, downstream, fork)
    struct point *intersection, *downstream, *fork;
{
    /* compute new x and y coordinates for the downstream point and
     * the fork by shifting the intersection to the origin.
     */
    double dx = downstream->lon - intersection->lon;
    double dy = downstream->lat - intersection->lat;
    double fx = fork->lon - intersection->lon;
    double fy = fork->lat - intersection->lat;

    /* compute polar coordinate angles theta and phi for the points.
     * results are in range [-PI, PI].
     */
    double theta = atan2(dy, dx);
    double phi = atan2(fy, fx);

    /* modify phi by rotating the system such that the downstream
     * point lies on the positive x axis
     */
    phi -= theta;

    /* the fork is on the "right" if phi now represents a point in the
     * 3rd or 4th quadrants.
     */
    if ((phi < 0.0 && phi > -M_PI)
            || (phi > 0.0 && phi > M_PI))
        return FLOW_RIGHT;
    else
        return FLOW_LEFT;
}

/** return the side of the fork flow given a seg and a fork */
static FlowLocation
find_fork_side(seg, fork)
    struct river_segment *seg;
    struct river_segment *fork;
{
    /* points for intersection, downstream on the river, and upstream on the
       fork */
    struct point *intp, *rivp, *forkp;

    intp = fork->bottom;
    /* if the fork is just a single point, choose a side arbitrarily */
    if (!(forkp = next_upstream_point(fork, intp))) 
        return FLOW_RIGHT;

    /* forks may only occur at the upstream end of reaches */
    intp = seg->top;
    if (!(rivp = next_downstream_point(seg, intp))) {
        /* someone has connected a fork right at the mouth of the
         * river.  yuck.  anyway, we'll use this for the downstream
         * point, and pick the intersection to be the very next point
         * upstream.  That ought to do.  If there isn't one, then it's
         * a single point river, and we get to choose arbitrarily.
         */
        rivp = intp;
        if (!(intp = next_upstream_point(seg, rivp)))
            return FLOW_RIGHT;
    }

    /* we found the three points, now just call the nifty little
       routine which analyzes them! */
    return determine_side(intp, rivp, forkp);
}

/** recursive routine to perform the downstream flow propagation operation.
 *  return 0 on success, non-zero on error.
 */

static int compute_flow(seg, outflow)
    struct river_segment *seg;
    float       outflow[ DAYS_IN_SEASON ];
{
    int     i, j;
    int     erc = 0;
    float   flow[ DAYS_IN_SEASON ];
    float   avg_flow;
    float   *wtemp = NULL;
    float   *uptemp = NULL;
    float   *deltatemp = NULL;
	float   *turb = NULL;
	float   *upturb = NULL;
    static float secs_per_step = 3600.0 * 24.0/STEPS_PER_DAY;

    if (run_mode.compute_flow_break)
        return 1;

    bzero((char *)flow, sizeof(flow));

    /* perform basic recursion */
    if (seg->up)
    {
        erc += compute_flow(seg->up, flow);
        
        wtemp = get_temp_ptr(seg);
        uptemp = get_temp_ptr(seg->up);

		/* if init_temp_flag == 1, do not model temperature at this segment
		 * -reading values from .dat file instaed.                          */
		if ( (seg->seg_type == REACH_SEG && !seg->current->reach.init_temp_flag) ||
			(seg->seg_type == DAM_SEG && !seg->current->dam.init_temp_flag) )
		{
        if (seg->seg_type == REACH_SEG)
            deltatemp = seg->current->reach.delta_temp;

        /* copy upstream temperature now if no fork */
        if (!seg->fork) {
            for (i=0; i < STEPS_IN_SEASON; i++) {
                wtemp[i] = uptemp[i];
                if (seg->seg_type == REACH_SEG)
                    wtemp[i] += deltatemp[i];
            }
        }
		}

		/*  propagate turbidity  */
		turb = get_turbidity_ptr(seg);
        upturb = get_turbidity_ptr(seg->up);

		/* if turbidity_flag == 1, do not model turbidity at this segment
		 * -reading values from .dat file instaed.                         */
		if ( (seg->seg_type == REACH_SEG && !seg->current->reach.turbidity_flag) ||
			(seg->seg_type == DAM_SEG && !seg->current->dam.turbidity_flag) )
		{
			/* copy upstream turbidity now if no fork */
			if (!seg->fork) {
				for (i=0; i < STEPS_IN_SEASON; i++) {
					turb[i] = upturb[i];
				}
			}

		}
    }
    else
    {
    /* at headwaters */
    if (seg->seg_type != HEADWATER_SEG)
    {
        log_msg(L_FATAL, "%s is not a headwater\n", seg->seg_name);
        return ++erc;       /* return on err */
    }
    }
    if (seg->fork)
    {
    float   fork_flow[ DAYS_IN_SEASON ];
    float   *fork_uptemp;
    float   *fork_upturb;

    bzero((char *)fork_flow, sizeof(fork_flow));
    erc += compute_flow(seg->fork, fork_flow);
    fork_uptemp = get_temp_ptr(seg->fork);
	fork_upturb = get_turbidity_ptr(seg->fork);

        /* calculate proper nsat and water temp based on upstream vals */
    for (i=0; i < DAYS_IN_SEASON; i++)
    {
		/* if init_temp_flag == 1, do not model temperature at this segment
		 * -reading values from .dat file instaed.                           */
		if ( (seg->seg_type == REACH_SEG && !seg->current->reach.init_temp_flag) ||
			(seg->seg_type == DAM_SEG && !seg->current->dam.init_temp_flag) )
		{
        if (flow[i] != 0.0 || fork_flow[i] != 0.0)
        {
            for (j=0; j < STEPS_PER_DAY; j++)
            {
                wtemp[i*STEPS_PER_DAY+j] =
                        (flow[i] * uptemp[i*STEPS_PER_DAY+j]
                         + fork_flow[i] * fork_uptemp[i*STEPS_PER_DAY+j])
                        / (flow[i] + fork_flow[i]);
                if (seg->seg_type == REACH_SEG) {
                    wtemp[i*STEPS_PER_DAY+j] += deltatemp[i*STEPS_PER_DAY+j];
                }
            }
        }
        else
        {
            for (j=0; j < STEPS_PER_DAY; j++)
            {
                wtemp[i*STEPS_PER_DAY+j] = 0.0;
            }
        }
		}

		/* if turbidity_flag == 1, do not model turbidity at this segment
		 * -reading values from .dat file instaed.                         */
		if ( (seg->seg_type == REACH_SEG && !seg->current->reach.turbidity_flag) ||
			(seg->seg_type == DAM_SEG && !seg->current->dam.turbidity_flag) )
		{
        if (flow[i] != 0.0 || fork_flow[i] != 0.0)
        {
            for (j=0; j < STEPS_PER_DAY; j++)
            {
                turb[i*STEPS_PER_DAY+j] =
                        (flow[i] * upturb[i*STEPS_PER_DAY+j]
                         + fork_flow[i] * fork_upturb[i*STEPS_PER_DAY+j])
                        / (flow[i] + fork_flow[i]);
            }
        }
        else
        {
            for (j=0; j < STEPS_PER_DAY; j++)
            {
                turb[i*STEPS_PER_DAY+j] = 0.0;
            }
        }
		}
        
        /* total flow */
        flow[i] += fork_flow[i];
    }
    }
    /* make/link up pointers in the gas distributions.  this is an
     * initialization which will only happen once in each segment.  gas
     * input is allocated if the segment is a fork, otherwise it's a
     * link to the upstream gas output.  gas output for this segment
     * will contain allocated vectors for gas; the flow will be a
     * pointer to the flow for this seg; flow_fraction is a pointer to
     * the spill fraction if this seg is a dam, a pointer to the
     * upstream flow fraction for reaches, and allocated for
     * headwaters.
     *
     * note that the fraction_side values and the flow_fraction
     * storage and pointers are all determined here, and should not be
     * altered elsewhere.
     *
     * stuff allocated here becomes part of the static (in the logical
     * sense) river struct, and is never deleted.
     */
       
    if (!get_gas_dist(seg)) {
    GasDistribution *gas;
    if (!(gas = (GasDistribution *) calloc(1, sizeof(GasDistribution)))
        || !(gas->gas[FLOW_RIGHT] =
         (float *) calloc(DAM_SLICES_IN_SEASON, sizeof(float)))
        || !(gas->gas[FLOW_LEFT] =
         (float *) calloc(DAM_SLICES_IN_SEASON, sizeof(float)))) {
        log_msg(L_ERROR, "no mem for gas vectors in %s\n", seg->seg_name);
        return ++erc;
    }
    /* gas input first */
    if (seg->fork) {
        GasDistribution *gas_in;
        FlowLocation fork_side;

        if (!(gas_in = (GasDistribution *)
          calloc(1, sizeof(GasDistribution)))
        || !(gas_in->gas[FLOW_RIGHT] 
            = (float *) calloc(DAM_SLICES_IN_SEASON, sizeof(float)))
        || !(gas_in->gas[FLOW_LEFT]
            = (float *) calloc(DAM_SLICES_IN_SEASON, sizeof(float)))
        || !(gas_in->flow
            = (float *) calloc(DAYS_IN_SEASON, sizeof(float)))
        || !(gas_in->flow_fraction
            = (float *) calloc(DAM_SLICES_IN_SEASON, sizeof(float)))) {

        log_msg(L_ERROR, "no mem for gas vectors in %s\n", seg->seg_name);
        return ++erc;
        }
        /* set the fraction side to be that of the main stem */
        fork_side = find_fork_side(seg, seg->fork);
        gas_in->fraction_side = FLOW_OTHER_SIDE(fork_side);

        /* forks have to be reaches */
        seg->current->reach.gas_in = gas_in;
    }
    else if (seg->up) {
        if (seg->seg_type == DAM_SEG)
        seg->current->dam.gas_in = get_gas_dist(seg->up);
        else if (seg->seg_type == REACH_SEG)
        seg->current->reach.gas_in = get_gas_dist(seg->up);
    }
    /* then gas output */
    if (seg->seg_type == DAM_SEG) {
        gas->flow = seg->current->dam.flow;
        gas->flow_fraction = seg->current->dam.spill;
        gas->fraction_side = seg->current->dam.spill_side;
        seg->current->dam.gas_out = gas;
    }
    else if (seg->seg_type == REACH_SEG) {
        gas->flow = seg->current->reach.flow;
        gas->flow_fraction = seg->current->reach.gas_in->flow_fraction;
        gas->fraction_side = seg->current->reach.gas_in->fraction_side;
        seg->current->reach.gas_out = gas;
    }
    else if (seg->seg_type == HEADWATER_SEG) {
        gas->flow = seg->current->headwater.flow;
        gas->fraction_side = FLOW_RIGHT;
        /* allocate space for the flow fraction in headwaters */
        if (!(gas->flow_fraction
          = (float *) calloc(DAM_SLICES_IN_SEASON, sizeof(float)))) {
        log_msg(L_ERROR, "no mem for gas vectors in %s\n",
            seg->seg_name);
        return ++erc;
        }
        seg->current->headwater.gas_out = gas;
    }
    else {
        log_msg(L_FATAL, "bad seg type %d in gas init\n", seg->seg_type);
        return ++erc;
    }
    }   

    /* if seg is a fork, calculate proper gas input values */
    if (seg->fork) {
    /* location to store final input calculations */
    GasDistribution *gas_in = seg->current->reach.gas_in;

    /* the fraction side is that of the main stem */
    FlowLocation up_side = gas_in->fraction_side;
    GasDistribution *up_gas = get_gas_dist(seg->up);
    float *up_temp = get_temp_ptr(seg->up); 

    FlowLocation fork_side = FLOW_OTHER_SIDE(up_side);
    GasDistribution *fork_gas = get_gas_dist(seg->fork);
    float *fork_temp = get_temp_ptr(seg->fork); 

    /* mix upstream gas on each branch */
    mix_gas_instantaneously(gas_in->gas[fork_side], fork_gas, 0, 0);
    mix_gas_instantaneously(gas_in->gas[up_side], up_gas, 0, 0);

    /* set the flow and flow fraction */
    for (i=0; i < DAYS_IN_SEASON; ++i) {
        float frac;
        
        gas_in->flow[i] = fork_gas->flow[i] + up_gas->flow[i];

        /* fraction side is the "up" side */
        frac = (gas_in->flow[i]) ? up_gas->flow[i] / gas_in->flow[i] : 0;
        
        for (j=0; j < DAM_TIME_SLC; ++j) {
        gas_in->flow_fraction[i*DAM_TIME_SLC + j] = frac;
        }
    }
    /* now mix the two upstream gas values into a single one.
         * this may change later.  the gas is mixed in place in one
         * side input vector, then copied to the other vector.
     */
    mix_gas_instantaneously(gas_in->gas[FLOW_RIGHT],
                gas_in,
                (up_side==FLOW_LEFT)  ? up_temp : fork_temp,
                (up_side==FLOW_RIGHT) ? up_temp : fork_temp);
    bcopy((char *) gas_in->gas[FLOW_RIGHT], (char *) gas_in->gas[FLOW_LEFT],
          sizeof(float) * DAM_SLICES_IN_SEASON);
    }
    
    /* do calculations for physical parameters based on seg type */
    if (seg->seg_type == REACH_SEG)
    {
    extern float water_tt();
    
    register struct reach *rchptr;
    float       flow_max;
    float       slopetan;
    float       volume;
    float       velocity;
    float       adj;

    rchptr = &(seg->current->reach);
    
    flow_max = rchptr->flow_max;
    for (i = 0; i < DAYS_IN_SEASON; i++)
    {
        /* adjust loss if resulting flow is < flow_min */
        if ((adj = flow[i] - rchptr->loss[i]) < rchptr->flow_min)
        {
        /* suppress warning message on rounding errors */
        if (rchptr->flow_min - adj > 0.0001) 
            log_msg(L_WARNING, "%s%.2f%s%d%s%s%s%.2f%s%.2f%s%.2f%s",
            "insufficient flow (",
            flow[i],
            ") on day ",
            i,
            " at ",
            seg->seg_name,
            ";\n ...adjusting (old loss: ",
                rchptr->loss[i],
            ", new loss: ",
            flow[i] - rchptr->flow_min,
            ", flow min: ",
            rchptr->flow_min,
            ")\n" );
        rchptr->loss[i] = flow[i] - rchptr->flow_min;
        }
        flow[i] -= rchptr->loss[i];
        rchptr->flow[i] = flow[i];
        if (flow[i] > flow_max)
        flow_max = flow[i];
    }
    rchptr->flow_max = flow_max;

    /* put the velocity in miles per time step, averaged over input
     * and output flows.  velcocity is based on upper and lower depths
     * and slopetan.  volume is computed as an intermediate step, and
     * saved for use in pred density calculations.
     * note: volumes aren't really, because we have the length factored
     * out.  this works because length drops out in all of our subsequent
     * calculations.
     */
    
    slopetan = tan(rchptr->slope);  /* already in radians */

    /* full pool volume first, once only */
    if (rchptr->volume < 0)
        rchptr->volume = REACH_VOLUME(0.0, rchptr->upper_depth, 
           rchptr->lower_depth, rchptr->width, slopetan);
    if (rchptr->volume < 0)
    {
        log_msg(L_ERROR,
            "volume < 0 in %s;\n\tcheck river description file\n",
            seg->seg_name);
        return ++erc;
    }

    for (i = 0; i < DAYS_IN_SEASON; ++i)
    {
        /* adjust elevation if out of range; use min depth of 5%.
         * in forebay, as represented by lower_depth
         */
        if (-rchptr->elev_change[i]
        > rchptr->lower_depth * DEPTH_PADDING)
        {
        rchptr->elev_change[i]= -rchptr->lower_depth * DEPTH_PADDING;
        log_msg(L_WARNING,
            "elevation drop below depth in reach %s\n\tadjusting...\n",
            seg->seg_name);
        }
        avg_flow = flow[i]
            + rchptr->loss[i]/2;
        if (avg_flow < 0.0)
        avg_flow = 0.0;
        
        /* volume and velocity computations */

        volume = REACH_VOLUME(-(rchptr->elev_change[i]),
             rchptr->upper_depth, rchptr->lower_depth,
             rchptr->width, slopetan);

        if (-(rchptr->elev_change[i]) < rchptr->upper_depth)
        {
        velocity = (volume == 0) ? 0.0 : avg_flow / volume;
        }
        else
        {
        velocity = (avg_flow == 0) ? 0.0 
            : 1.0 / (volume / avg_flow +
                 (1 - (rchptr->lower_depth
                   + rchptr->elev_change[i])
                  / (rchptr->lower_depth - rchptr->upper_depth))
                 / (current->Ufree * 0.1));
        }
        /* final assignments and conversion */
        for (j = 0; j < STEPS_PER_DAY; j++)
        {
        rchptr->cur_volume[i*STEPS_PER_DAY+j] = volume;
        rchptr->velocity[i*STEPS_PER_DAY+j] =
            velocity * 0.19 * secs_per_step;
        }
    }
    
    /* override the river gas with specific output value if specified */
    if (rchptr->initial_gas) {
        for (i=0; i < DAM_SLICES_IN_SEASON; i++) {
        rchptr->gas_out->gas[FLOW_RIGHT][i]
            = rchptr->gas_out->gas[FLOW_LEFT][i]
            = rchptr->initial_gas[i];
        }
    }
    else
        /* calculate nitrogen supersaturation dissipation */
        compute_reach_gas(rchptr->gas_out, rchptr->gas_in, seg);
    
    /* average water particle travel time */
    rchptr->water_particle_tt = water_tt(seg,
                         current->water_travel.first_day,
                         current->water_travel.last_day);
                         
    /* print some stats for day 1 */
    log_msg(L_DBGMSG,
        "reach %s length(mi) %6.2f,\n\tday 1:  vel(mi/hr) %6.2f, vel_conv(mi/day) %f, \n\tvol(acre-ft) %6.0f, nsat %6.2f, temp %6.2f\n",
        seg->seg_name, rchptr->length,
        rchptr->velocity[STEPS_PER_DAY]/(24/STEPS_PER_DAY),
        0.19*secs_per_step*STEPS_PER_DAY/rchptr->volume,
        rchptr->cur_volume[STEPS_PER_DAY] * rchptr->length * 640 / 5280,
        rchptr->gas_out->gas[rchptr->gas_out->fraction_side][DAM_TIME_SLC],
        rchptr->water_temp[STEPS_PER_DAY]);

    }
    else if (seg->seg_type == DAM_SEG)
    {
    register struct dam *damptr = &(seg->current->dam);
    struct river_segment *phouse;
    float   flow_max;
    float   *foreddwn;
    float   *tailddwn;

    if ((seg->up->seg_type !=REACH_SEG && seg->up->seg_type !=HEADWATER_SEG)
        || seg->down->seg_type != REACH_SEG)
    {
        log_msg(L_ERROR, "dam not enclosed by reaches at %s\n",
            seg->seg_name);
        return ++erc;
    }

    clear_powerhouse_fractions(seg);
    bzero((char *) damptr->spill, sizeof(damptr->spill));
    
    /* find the drawdowns from the surrounding reaches */
    
    if (seg->up->seg_type == HEADWATER_SEG)
        foreddwn = seg->up->current->headwater.elev_change;
    else 
        foreddwn = seg->up->current->reach.elev_change;
    tailddwn = seg->down->current->reach.elev_change;

    /* now the big loop over the season, inside which we do it all.
     * the interior of this loop should ideally be split into many
     * function calls to do the work, but it is slow enough already,
     * so we will sacrifice cleanliness for speed.  we still use a
     * few function calls, for the things that are really hairy.
     */
    
    flow_max = damptr->flow_max;
    for (i=0; i < DAYS_IN_SEASON; ++i)
    {
        /* set forebay and tailrace depths using draw downs in the
         * surrounding reaches
         */
        damptr->forebay_depth[i] = damptr->fp_fb_depth + foreddwn[i];
        damptr->tailrace_depth[i] =
        (damptr->tr_elev - damptr->base_elev) + tailddwn[i];
        if (damptr->forebay_depth[i] < 0)
        {
        damptr->forebay_depth[i] = 0;
        log_msg(L_ERROR, "negative forebay depth at %s\n",
            seg->seg_name);
        ++erc;
        }
        if (damptr->tailrace_depth[i] < 0)
        {
        damptr->tailrace_depth[i] = 1;
        log_msg(L_WARNING, "free running tailrace (depth 1) at %s\n",
            seg->seg_name);
        }
        damptr->drop_ratio[i] =
        (damptr->fp_fb_depth - damptr->forebay_depth[i])
            / damptr->fp_fb_depth;
        damptr->tr_drop_ratio[i] =
        ((damptr->tr_elev - damptr->base_elev)
         - damptr->tailrace_depth[i])
            / (damptr->tr_elev - damptr->base_elev);
        
        /* now on to the flow stuff */
        
        if (flow[i] < 0.0)
        {
        flow[i] = 0.0;
        log_msg(L_ERROR,
            "negative flow at %s; setting to 0\n",
            seg->seg_name);
        }

        /* set the flow in main powerhouse.  note that in all
         * powerhouses, we keep track of the total project flow.
         * the fractional flow is only multiplied in later, during
         * the passage calculations.
         */

        if (!damptr->storage_basin || i == DAYS_IN_SEASON - 1)
        damptr->flow[i] = flow[i];
        else {
        /* Storage basin at dam */
        calculate_flow(seg, i);
        flow[i] = damptr->flow[i];
        }
        if (damptr->flow[i] > flow_max)
        flow_max = damptr->flow[i];

        /* set flow for other powerhouses */
        
        for (phouse = damptr->additional_pwrhs; phouse;
         phouse = phouse->current->dam.additional_pwrhs)
        {
        phouse->current->dam.flow[i] = flow[i];
        }

        /* compute forced spill and flow fractions for all phouses */
        set_dam_flow_routing(seg, i);
    }
    damptr->flow_max = flow_max;

    /* override the river gas with specific output value if specified */
    if (damptr->initial_gas) {
        for (i=0; i < DAM_SLICES_IN_SEASON; i++) {
        damptr->gas_out->gas[FLOW_RIGHT][i]
            = damptr->gas_out->gas[FLOW_LEFT][i]
            = damptr->initial_gas[i];
        }
    }
    else
        /* compute gas supersaturation */
        erc += compute_dam_gas(seg);
    }
    else if (seg->seg_type == HEADWATER_SEG) {
    GasDistribution *gas = get_gas_dist(seg);

    /* use the specific output value if specified */
    if (seg->current->headwater.initial_gas) {
        for (i=0; i < DAM_SLICES_IN_SEASON; i++) {
        gas->gas[FLOW_RIGHT][i] = gas->gas[FLOW_LEFT][i]
            = seg->current->headwater.initial_gas[i];
        }
    }
    else {
        bzero((char *) gas->gas[FLOW_LEFT],
          sizeof(float)*DAM_SLICES_IN_SEASON);
        bzero((char *) gas->gas[FLOW_RIGHT],
          sizeof(float)*DAM_SLICES_IN_SEASON);
    }

    wtemp = get_temp_ptr(seg);
    for (i = 0; i < DAYS_IN_SEASON; ++i) {
        if (!(flow[i] = seg->current->headwater.flow[i])) {
        wtemp[i*STEPS_PER_DAY] = 0.0;   /* Zero flow?  Zero temp! */
        wtemp[i*STEPS_PER_DAY+1] = 0.0;
        }
    }
    }
    
    if (outflow)
    {
    for (i = 0; i < DAYS_IN_SEASON; ++i)
        outflow[i] += flow[i];
    }

    return erc;
}

/** \brief Compute flow at all segments */
int compute_all_flow()
{
    extern void set_system_water_tt();
    int erc;

    run_mode.compute_flow_break = 0;
    
    erc = compute_flow(river_mouth, (float *) 0);
    set_system_water_tt();
    return erc;
}

/** \brief Routine to fill all regulated headwaters above a given
 * segment with flow.  Recursive.  
 *
 * A regulated headwater is defined as one for which the
 * nearest downstream regulation point is nearer than the nearest
 * downstream confluence.  The flow from that regulation point (dam 
 * or reach) is copied into the headwater, and the headwater is marked 
 * as regulated.  Unregulated headwaters are also tagged at this time. 
 * Unregulated flow coefficients must be recomputed after this routine 
 * executes.
 *
 * Note that only active dams and reaches are regulation points. Dams and
 *  reaches become active if they are mentioned in a flow archive file,
 *  for example.
 */
#define SCALE_FACTOR    1.98347 /* Scaling factor for kcfs (flow) to give
                                    kiloacre-feet (volume). */

static void label_and_fill_regulated_headwaters(seg)
    struct river_segment *seg;
{
    int i;
    struct river_segment *downseg;
    
    while (!seg->fork && seg->up)
    seg = seg->up;

    if (seg->fork)
    label_and_fill_regulated_headwaters(seg->fork);

    if (seg->up)
    {
    label_and_fill_regulated_headwaters(seg->up);
    return;
    }
    
    if (seg->seg_type != HEADWATER_SEG)
    log_msg(L_ERROR, "%s is not a headwater\n", seg->seg_name);

    downseg = seg->down;
    while (downseg && downseg->inactive && !downseg->fork)
        downseg = downseg->down;
    
    if (!downseg || downseg->fork)      
    seg->current->headwater.flow_coefficient = -1.0; /* unregulated hw */
    else
    {
    float   flow_max;
    
    seg->current->headwater.flow_coefficient = 0.0;
    if (downseg->seg_type == DAM_SEG)
        for (i=0; i < DAYS_IN_SEASON; i++)
            seg->current->headwater.flow[i] = downseg->current->dam.flow[i];
    else /* if (downseg->seg_type == REACH_SEG) */
        for (i=0; i < DAYS_IN_SEASON; i++)
            seg->current->headwater.flow[i] = downseg->current->reach.flow[i];

    /* If headwater is directly above a dam which has a storage basin,
     *  correct headwater flow for changes in basin volume.
     */
    if (seg->down == downseg && downseg->seg_type == DAM_SEG &&
             downseg->current->dam.storage_basin != NULL) {
        for (i = 0; i < DAYS_IN_SEASON-1; i++) {
            seg->current->headwater.flow[i] 
                += (downseg->current->dam.storage_basin->volume[i+1]
                        - downseg->current->dam.storage_basin->volume[i])
                                    / SCALE_FACTOR;
            if (seg->current->headwater.flow[i] < 0.0)
                log_msg( L_ERROR, 
                        "day %d: basin volume change drove hw flow negative at %s\n",
                                i, seg->seg_name );
        }
    }

    /* Correct headwater flow for losses in intervening reaches.
     */
    while (downseg != seg)
    {
        if (downseg->seg_type == REACH_SEG)
            for (i=0; i < DAYS_IN_SEASON; i++)
                seg->current->headwater.flow[i] 
                            += downseg->current->reach.loss[i];
        downseg = downseg->up;
    }
    flow_max = seg->current->headwater.flow_max;
    for (i=0; i < DAYS_IN_SEASON; i++)
    {
        if (seg->current->headwater.flow[i]
        < seg->current->headwater.flow_min)
        {
        log_msg(L_WARNING, "%s%s%s%d%s%.2f%s",
            "insufficient flow at ",
            seg->seg_name,
            " on day ",
            i,
            ", setting to min (",
            seg->current->headwater.flow_min,
            ")\n" );
        seg->current->headwater.flow[i] =
            seg->current->headwater.flow_min;
        }
        
        if (seg->current->headwater.flow[i] > flow_max)
        flow_max = seg->current->headwater.flow[i];
    }
    seg->current->headwater.flow_max = flow_max;
    }
}


/** \brief Routine to examine reaches immediately above a seg and perform some action.
 *
 * Passed in parameter fn_code determines what action(s) to take during the
 * course of the traversal; choices are described in 'flow.h'.
 * Returns true if completely enclosed by regulated flow, else false.
 * Only active dams and reaches are considered regulation points, except 
 * when computing total upstream regulated flow, in which case flows at 
 * unregulated headwaters are added to the tally.
 */

int examine_enclosed_reaches(seg, fn_code, count, loss, rflow, excess_flow)
    struct river_segment *seg;          /* starting seg */
    unsigned int      fn_code;      /* function code */
    int          *count;        /* number affected segs */
    float         loss[DAYS_IN_SEASON]; /* total loss in reaches */
    float         rflow[DAYS_IN_SEASON];/* total regulated flow */
    float         excess_flow[DAYS_IN_SEASON]; /* tot flow above min */
{
    int i, j;
    int upok, forkok;
    
    if (seg->seg_type == HEADWATER_SEG)
    {
        if (fn_code & REFN_TOTREG)
            for (i = 0; i < DAYS_IN_SEASON; i++)
            rflow[i] += seg->current->headwater.flow[i];
        return(FALSE);
    }
    if (!seg->inactive)  /* May be dam or reach */
    {
        if (fn_code & REFN_TOTREG) {
            if (seg->seg_type == DAM_SEG)
                for (i = 0; i < DAYS_IN_SEASON; i++)
                    rflow[i] += seg->current->dam.flow[i];
            else /* if (seg->seg_type == REACH_SEG) */
                for (i = 0; i < DAYS_IN_SEASON; i++)
                    rflow[i] += seg->current->reach.flow[i];
        }
        if (fn_code & REFN_COUNT_DAMS)
            (*count)++;
        if (fn_code & REFN_TOTEXCESS) {
            if (seg->seg_type == DAM_SEG)
                for (i=0; i < DAYS_IN_SEASON; ++i)
                    excess_flow[i] += (seg->current->dam.flow[i]
                           - seg->current->dam.flow_min_river);
            else /* if (seg->seg_type == REACH_SEG) */
                for (i=0; i < DAYS_IN_SEASON; ++i)
                    excess_flow[i] += (seg->current->reach.flow[i]
                           - seg->current->reach.flow_min);
        }
        if (fn_code & REFN_ADJ_UNREG_LOSS)
        {
            /* adjust loss in reach immediately below this dam, if any */
            if (seg->down->seg_type != REACH_SEG)
                log_msg( L_ERROR,
                     "Regulated reach (%s) must be above reach, but is above (%s)\n",
                     seg->seg_name, seg->down->seg_name );
            else if (*count)
                for (i=0; i < DAYS_IN_SEASON; i++)
                    seg->down->current->reach.loss[i] += loss[i] / *count;
        }
        if (fn_code & REFN_ADJLOSS) /* adjust regulated loss */
        {
            struct river_segment *downseg;
            int reaches;
            float newloss[DAYS_IN_SEASON];

            bzero((char *)newloss, DAYS_IN_SEASON * sizeof(float));

            /* calculate new loss contribution from this reg. point */
            for (i=0; i < DAYS_IN_SEASON; ++i)
            {
            if (loss[i] != 0)
            {
                float weight;
                if (*count == 1)    /* this is the only upstream rg. pt. */
                weight = 1.0;
                else
                {
                /* if no excess flow, divide the loss evenly.  the
                   violation of the flow minimum will be reported
                   during the downstream propagation.
                 */
                if (! excess_flow[i] > 0.0)
                    weight = 1.0 / ((float) *count);
                else
                {
                    /* new loss is weighted by dam flow above minimum */
                    if (seg->seg_type == DAM_SEG)
                        weight = ((seg->current->dam.flow[i]
                               - seg->current->dam.flow_min_river)
                              / excess_flow[i]);
                    else /* if (seg->seg_type == REACH_SEG) */
                        weight = ((seg->current->reach.flow[i]
                               - seg->current->reach.flow_min)
                              / excess_flow[i]);

                    /* reverse weighting if loss is negative
                       (net flow gain)
                     */
                    if (loss[i] < 0)
                    weight = (1.0 - weight) / (*count - 1);
                }
                }
                newloss[i] = weight * loss[i];
            }
            }
            /* count downstream reaches */
            reaches = 0;
            downseg = seg->down;
            while (downseg->inactive && downseg->seg_type == REACH_SEG)
            {
            ++reaches;
            downseg = downseg->down;
            }
            if (downseg->seg_type == REACH_SEG)
                reaches++;   /* active reach can receive losses too. */

            /* distribute new loss over downstream reaches */
            for (j = 0, downseg = seg->down; j < reaches; j++)
            {
            float loss_max = seg->current->reach.loss_max; /* for slider */
            for (i=0; i < DAYS_IN_SEASON; ++i)
            {
                downseg->current->reach.loss[i] += newloss[i] / reaches;

                if (seg->current->reach.loss[i] > loss_max)
                loss_max = seg->current->reach.loss[i];
                else if (-seg->current->reach.loss[i] > loss_max)
                loss_max = -seg->current->reach.loss[i];
            }
            downseg = downseg->down;
            }
        }
        return(TRUE);
    }
    if (seg->inactive && seg->seg_type == REACH_SEG)
    {
        if (fn_code & REFN_COUNT)
            *count = *count + 1;
        if (fn_code & REFN_TOTLOSS)
            for (i = 0; i < DAYS_IN_SEASON; i++)
            loss[i] += seg->current->reach.loss[i];
    }

    upok = forkok = TRUE;
    if (seg->fork)
        forkok = examine_enclosed_reaches(seg->fork, fn_code,
                      count, loss, rflow, excess_flow);
    if (seg->up)
        upok = examine_enclosed_reaches(seg->up, fn_code, count, loss,
                    rflow, excess_flow);
    return(upok && forkok);
}


/** \brief Routine to adjust the loss between regulated flows.
 *
 * Examines each
 * active dam, determines if the reaches above are enclosed by regulation
 * points, and adjusts the loss if they are.  The loss adjustment is
 * weighted by the flow maxima in the reaches.
 */

static void adjust_regulated_loss(seg)
    struct river_segment *seg;
{
    unsigned int    fn_code;
    int         i;
    int         count;
    int         doup, dofork;
    float       loss[DAYS_IN_SEASON];
    float       rflow[DAYS_IN_SEASON];
    float       excess_flow[DAYS_IN_SEASON];
    
    while (seg)
    {
    if (!seg->inactive)  /* dam or reach */
    {
        doup = dofork = FALSE;
        if (examine_enclosed_reaches(seg->up, REFN_NULL, (int *)0,
                     (float *)0, (float*)0, (float *) 0))
        doup = TRUE;
        if (seg->fork)
        if (examine_enclosed_reaches(seg->fork, REFN_NULL, (int *)0,
                     (float *)0, (float*)0, (float *) 0))
            dofork = TRUE;
        if (doup && ((!seg->fork) || dofork))
        {
        bzero((char *) loss, sizeof(loss));
        bzero((char *) rflow, sizeof(rflow));
        bzero((char *) excess_flow, sizeof(excess_flow));
        count = 0;
        fn_code = REFN_COUNT_DAMS | REFN_TOTLOSS | REFN_TOTREG
            | REFN_TOTEXCESS;

        if (doup)
            examine_enclosed_reaches(seg->up, fn_code, &count, loss,
                         rflow, excess_flow);
        if (dofork)
            examine_enclosed_reaches(seg->fork, fn_code, &count, loss,
                         rflow, excess_flow);
           
        if (seg->seg_type == DAM_SEG) {
            for (i = 0; i < DAYS_IN_SEASON; i++)
                loss[i] = (rflow[i] - (loss[i]+seg->current->dam.flow[i]));
        } else /* if (seg->seg_type == REACH_SEG) */ {
            for (i = 0; i < DAYS_IN_SEASON; i++)
                loss[i] = (rflow[i] - (loss[i]+seg->current->reach.flow[i]
                                         +seg->current->reach.loss[i]));
        }

        if (doup)
            examine_enclosed_reaches(seg->up, REFN_ADJLOSS, &count,
                         loss, (float *)0,
                         excess_flow);
        if (dofork)
            examine_enclosed_reaches(seg->fork, REFN_ADJLOSS,
                         &count, loss, (float *)0,
                         excess_flow);
        }
    }
    seg = seg->next;
    }
}
    

/** \brief Routine to count local unregulated headwaters above a given segment.
 * 
 * Also computes the maximum flow and total regulated flow.
 */

static void count_unreg_hws(seg, countp, unreg_flow_max, total_regulated_flow)
    struct river_segment *seg;
    int          *countp;
    float        *unreg_flow_max;
    float         total_regulated_flow[DAYS_IN_SEASON];
{
    int i;
    
    while (!seg->fork && seg->inactive && seg->up)
        seg = seg->up;

    /* If there is a regulation point on this branch, add in it's flow
     *  and return.
     */
    if (!seg->inactive && seg->seg_type == DAM_SEG)
    {
        for (i=0; i < DAYS_IN_SEASON; i++)
            total_regulated_flow[i] += seg->current->dam.flow[i];
        return;
    }
    else if (!seg->inactive && seg->seg_type == REACH_SEG) {
        for (i=0; i < DAYS_IN_SEASON; i++)
            total_regulated_flow[i] += seg->current->reach.flow[i];
        return;
    }
    
    if (seg->fork)
        count_unreg_hws(seg->fork, countp, unreg_flow_max,
                total_regulated_flow);

    if (seg->up)
    {
        count_unreg_hws(seg->up, countp, unreg_flow_max, total_regulated_flow);
        return;
    }
    
    if (seg->seg_type != HEADWATER_SEG)
    log_msg(L_ERROR, "%s is not a headwater\n", seg->seg_name);

    if (seg->current->headwater.flow_coefficient)
    {
        for (i=0; i < DAYS_IN_SEASON; i++)  /* count flow min as regulated*/
            total_regulated_flow[i] += seg->current->headwater.flow_min;
        (*countp)++;
        (*unreg_flow_max) += seg->current->headwater.flow_max;
    }
    else log_msg(L_ERROR, "regulated hw %s found above confluence!\n",
         seg->river_name);
    
}


/** \brief Routine to compute unregulated headwater coefficients
 * above a given segment.
 *
 * Assumes unreg hws are currently
 * tagged (coefficient value != 0).
 */

static void compute_unreg_coeffs(seg, flow_max)
    struct river_segment *seg;
    float         flow_max;
{
    while (!seg->fork && seg->inactive && seg->up)
        seg = seg->up;

    if (!seg->inactive)
        return;
    
    if (seg->fork)
    compute_unreg_coeffs(seg->fork, flow_max);

    if (seg->up)
    {
    compute_unreg_coeffs(seg->up, flow_max);
    return;
    }
    
    if (seg->seg_type != HEADWATER_SEG)
    log_msg(L_WARNING, "%s is not a headwater\n", seg->seg_name);

    if (seg->current->headwater.flow_coefficient)
    seg->current->headwater.flow_coefficient =
        seg->current->headwater.flow_max / flow_max;

	if (seg->current->headwater.flow_max == 0.0) {
		log_msg(L_ERROR, "%s's flow_max is 0!\n", seg->seg_name);
		log_msg(L_ERROR, "%s's flow_coefficient is now %f\n",
			seg->seg_name, seg->current->headwater.flow_coefficient);
	}
}


/* Routine to fill unregulated headwaters above a
 * given seg.  Takes as parameters the seg and the flow
 * to divide amongst them.
 */


static void fill_unreg_hws(seg, flow)
    struct river_segment *seg;
    float   flow[DAYS_IN_SEASON];
{
    int i;
    
    while (!seg->fork && seg->inactive && seg->up)
        seg = seg->up;

    if (!seg->inactive)
        return;
    
    if (seg->fork)
        fill_unreg_hws(seg->fork, flow);

    if (seg->up)
    {
        fill_unreg_hws(seg->up, flow);
        return;
    }
    
    if (seg->seg_type != HEADWATER_SEG)
    log_msg(L_WARNING, "%s is not a headwater\n", seg->seg_name);

    if (!seg->current->headwater.flow_coefficient)
    return;

/* DEBUG */
    if (seg->current->headwater.flow_coefficient < 0.0)
    log_msg(L_ERROR, "ACK! negative flow - headwater %s\n",seg->river_name);
    else log_msg(L_DBGMSG, "-->unreg hw %s\n", seg->river_name); 

    {
    float   flow_max;

    flow_max = seg->current->headwater.flow_max;
    
    for (i = 0; i < DAYS_IN_SEASON; i++)
    {
        seg->current->headwater.flow[i] = seg->current->headwater.flow_min +
        (flow[i] * seg->current->headwater.flow_coefficient);
        if (seg->current->headwater.flow[i] > flow_max)
        flow_max = seg->current->headwater.flow[i];
    }

    /* Possibly correct for volume changes in storage basin directly
     *  below headwater.
     */
    if (seg->down->seg_type == DAM_SEG &&
             seg->down->current->dam.storage_basin != NULL) {
        for (i = 0; i < DAYS_IN_SEASON-1; i++) {
            seg->current->headwater.flow[i] 
                += (seg->down->current->dam.storage_basin->volume[i+1]
                        - seg->down->current->dam.storage_basin->volume[i])
                                    / SCALE_FACTOR;
            if (seg->current->headwater.flow[i] < 0.0)
                log_msg( L_ERROR, 
                     "day %d: basin volume change drove hw flow negative at %s\n",
                                i, seg->seg_name );
        }
    }

    seg->current->headwater.flow_max = flow_max;
    }
}


/* Routine which counts all the unregulated headwaters above
 * a regulation point, sets the coefficients, and fills them.
 * It then does an excess loss adjustment if necessary, visiting
 * regulation points above the current one and increasing the loss there.
 * Primary purpose of this intermediate routine is to remove
 * local variables from the recursive routine which calls it
 * (fill_all_unregulated_headwaters()).
 */

static void count_and_fill_unregs_above_dam(seg)
    struct river_segment *seg;
{
    int     num_unreg_hws = 0;
    float   unreg_flow_max = 0;
    int     num_regs = 0;
    int     adjloss_flag = 0;
    float   flow[DAYS_IN_SEASON];
    float   loss[DAYS_IN_SEASON];
    int     i;
    
    /* count the local unregulated headwaters and note flow_max
     * as well as the total regulated flow input.
     */
    bzero((char *)flow, sizeof(flow));
    bzero((char *)loss, sizeof(loss));
    count_unreg_hws(seg->up, &num_unreg_hws, &unreg_flow_max, flow);
    if (seg->fork)
    count_unreg_hws(seg->fork, &num_unreg_hws, &unreg_flow_max, flow);
    
    /* fill in the flow_coefficients */
    compute_unreg_coeffs(seg->up, unreg_flow_max);
    if (seg->fork)
    compute_unreg_coeffs(seg->fork, unreg_flow_max);

    examine_enclosed_reaches(seg->up, REFN_TOTLOSS, (int *) 0, loss,
                 (float *) 0, (float *) 0);
    if (seg->fork)
    examine_enclosed_reaches(seg->fork, REFN_TOTLOSS, (int *) 0, loss,
                 (float *) 0, (float *) 0);
    
    /* determine the unregulated flow and readjust loss*/
    for (i=0; i < DAYS_IN_SEASON; i++)
    {
        if (seg->seg_type == DAM_SEG)
            flow[i] = (seg->current->dam.flow[i] + loss[i]) - flow[i];
        else /* if (seg->seg_type == REACH_SEG) */
            flow[i] = (seg->current->reach.flow[i] + loss[i]) - flow[i];
        if (flow[i] < 0.0)
        {
            loss[i] = -flow[i];
            flow[i] = 0.0;
            adjloss_flag = 1;
        }
        else loss[i] = 0.0;
    }

    log_msg(L_DBGMSG,
        "\nfilling %d unreg hws above %s\n",num_unreg_hws, seg->seg_name);
    
    /* fill the unregulated headwaters local to this dam */
    fill_unreg_hws(seg->up, flow);
    if (seg->fork)
        fill_unreg_hws(seg->fork, flow);

    /* if there was more loss than can be handled (i.e, unreg headwater flow
     * would fall below the minimum) then try to take it below upstream
     * regulation points (dams).
     */
    if (adjloss_flag)
    {
        /* first call counts upstream regulation points */
        examine_enclosed_reaches(seg->up, REFN_COUNT_DAMS, &num_regs,
                     loss, (float *) 0, (float *) 0);
        if (seg->fork)
            examine_enclosed_reaches(seg->fork, REFN_COUNT_DAMS, &num_regs,
                         loss, (float *) 0, (float *) 0);
    
        /* second call allocates the loss */
        examine_enclosed_reaches(seg->up, REFN_ADJ_UNREG_LOSS, &num_regs,
                     loss, (float *) 0, (float *) 0);
        if (seg->fork)
            examine_enclosed_reaches(seg->fork, REFN_ADJ_UNREG_LOSS, &num_regs,
                         loss, (float *) 0, (float *) 0);

    }
}

/* Routine to visit every unregulated headwater above seg and fill
 * it.
 */

static void fill_all_unregulated_headwaters(seg)
    struct river_segment *seg;
{

    while (!seg->fork && seg->up)
    seg = seg->up;

    if (seg->fork)
    fill_all_unregulated_headwaters(seg->fork);

    if (seg->up)
    {
    fill_all_unregulated_headwaters(seg->up);
    return;
    }
    
    if (seg->seg_type != HEADWATER_SEG)
    log_msg(L_WARNING, "%s is not a headwater\n", seg->seg_name);

    if (seg->current->headwater.flow_coefficient >= 0.0)
    /* regulated hw or unregulated but already filled */
    return;

    /* find the regulatoin point which this headwater feeds */
    while (seg &&  seg->inactive)
    seg = seg->down;
    if (!seg)
    {
    log_msg(L_WARNING, "no dam below unregulated headwater!!\n");
    return;
    }

    count_and_fill_unregs_above_dam(seg);
}



/** Routine to fill headwaters with flow based on flows
 * already existing at dams.  Fills regulated, then
 * unregulated headwaters.  
 */

int fill_headwaters_from_dams(start_seg)
    struct river_segment *start_seg;
{
    label_and_fill_regulated_headwaters(start_seg);
    fill_all_unregulated_headwaters(start_seg);
    adjust_regulated_loss(start_seg);
    return 0;
}
 

/** Routine which marks as active a list of dams and reaches, while making
 * the rest inactive.  used by the monte carlo module.  lists must be
 * NULL-terminated (and -1's indicate empty slots in the arrays).
 */

static void mark_active_dam_list(dam_list, reach_list)
    struct river_segment **dam_list;
    struct river_segment **reach_list;
{
    struct river_segment *seg = river_mouth;
    
    /* first mark all segments inactive */
    while (seg)
    {
        seg->inactive = TRUE;
        seg = seg->next;
    }

    /* now make the dams in the list active */
    while (*dam_list)
    {
        if (*dam_list != (struct river_segment *) -1)
            (*dam_list)->inactive = FALSE;
        dam_list++;
    }

    /* now make the reaches in the list active */
    while (*reach_list)
    {
        if (*reach_list != (struct river_segment *) -1)
            (*reach_list)->inactive = FALSE;
        reach_list++;
    }
}

/** Routine to mark all dams as active and all other segments as inactive.
 *  Used to subvert flow regulation system to use for model initialization.
 */

void mark_only_dams_active( void ) {
    struct river_segment *seg = river_mouth;
    
    while (seg)
    {
        seg->inactive = seg->seg_type != DAM_SEG;
        seg = seg->next;
    }
}

/** Routine to remove all loss from all segments */

static void remove_all_loss()
{
    float *loss;
    struct river_segment *seg = river_mouth;

    while (seg)
    {
    switch (seg->seg_type)
    {
    case REACH_SEG:
        loss = seg->current->reach.loss;
        break;
    default:
        loss = NULL;
        break;
    }
    if (loss)
        bzero((char *) loss, DAYS_IN_SEASON * sizeof(float));
    
    seg = seg->next;
    }
}


/** Routine called from monte carlo module to do upstream propagation.
 * first marks the active dams (those for which flow data is present).
 * the active dams are considered regulation points.  then the standard
 * upstream propagation routine is called.
 */

void monte_prop_upstream(dam_list, reach_list)
    struct river_segment **dam_list;
    struct river_segment **reach_list;
{
    remove_all_loss();
    mark_active_dam_list(dam_list, reach_list);
    fill_headwaters_from_dams(river_mouth);
}


/** Routine to modulate the flows for a list of dams.  marks the dams as
 * active then calls a recursive routine to walk the tree in postorder
 * keeping track of upstream flows.
 */

void modulate_flows(struct river_segment **dam_list, struct river_segment **reach_list)
{
    float step_flow[DAYS_IN_SEASON];
    float mod_flow[DAYS_IN_SEASON];

    step_flow[0] = mod_flow[0] = -1;    /* mark flows as uninitialized */
    
    mark_active_dam_list(dam_list, reach_list);
}

/** Function to propogate elevation changes upstream from dams. NOTE
 *  that they are propogated up to the next dam or headwater; forks are
 *  propogated if their proportion of max_flow is more than 
 *  current->fork_threshold. NOTE this is all done in the current list
 *  of river segments.
 *
 * This routine is recursive to handle forks. Calling with a NULL argument
 *  starts it at the river mouth and searches upriver for the first dam.
 *  Calling with a non-zero argument propogates the denoted segment upstream.
 */

static void find_and_prop_upstream_dam( struct river_segment *seg ) {
    switch (seg->seg_type) {
    case DAM_SEG:
        propogate_elev_change( seg->up, seg->current->dam.fb_elev );
        break;

    case REACH_SEG:
        find_and_prop_upstream_dam( seg->up );
        if (seg->fork != NULL)
            find_and_prop_upstream_dam( seg->fork );
        break;

    case HEADWATER_SEG:
        break;
    }
}

            /** Propogate elevation changes upstream if (a) there is
             *  a reach up there, and (b) the flow-max of that reach
             *  is big enough, and (c) the bottom of the river at the
             *  lower end of that reach is below the top of the dam
             *  we are propogating from.
             */
void propogate_elev_change( struct river_segment *seg, double dam_top ) {
    int i;
    float e, d;

    if (seg == NULL) {
        find_and_prop_upstream_dam( river_mouth );
    }
    else {
        switch (seg->seg_type) {
        case REACH_SEG:
            /* Propogate elevation changes upstream if (a) there is
             *  a reach up there, and (b) the flow-max of that reach
             *  is big enough, and (c) the bottom of the river at the
             *  lower end of that reach is below the top of the dam
             *  we are propogating from.
             */
            if (seg->up->seg_type == REACH_SEG &&
                seg->up->current->reach.lower_elev < -9999.0) {
                log_msg(L_ERROR,
                    "reach %s, did not read lower_elev in river desc. file\n",
                    seg->seg_name);
            }

            if (seg->up->seg_type == REACH_SEG &&
                seg->up->current->reach.flow_max 
                            >= seg->current->reach.flow_max
                                        * current->fork_threshold &&
                seg->up->current->reach.lower_elev < dam_top) {
                /* Propogate elevation change upriver. NOTE compute_flow()
                 *  enforces a limit on elevation change, which we arrange
                 *  to just barely satisfy.
                 */
                d = seg->up->current->reach.lower_depth * DEPTH_PADDING;
                for (i = 0; i < DAYS_IN_SEASON; i++) {
                    e = seg->current->reach.elev_change[i];
                    if (-e >= d)
                        e = -d + 0.01;
                    seg->up->current->reach.elev_change[i] = e;
                }
                propogate_elev_change( seg->up, dam_top );
            }
            else
                find_and_prop_upstream_dam( seg->up );

            if (seg->fork != NULL) {
                if (seg->fork->seg_type == REACH_SEG &&
                    seg->fork->current->reach.lower_elev < -9999.0) {
                    log_msg(L_ERROR,
                        "reach %s, did not read lower_elev in river desc. file\n",
                        seg->seg_name);
                }

                if (seg->fork->seg_type == REACH_SEG &&
                    seg->fork->current->reach.flow_max 
                            >= seg->current->reach.flow_max
                                        * current->fork_threshold &&
                    seg->fork->current->reach.lower_elev < dam_top) {
                    /* Propogate elevation change up fork. NOTE compute_flow()
                     *  enforces a limit on elevation change, which we arrange
                     *  to just barely satisfy.
                     */
                    d = seg->fork->current->reach.lower_depth * DEPTH_PADDING;
                    for (i = 0; i < DAYS_IN_SEASON; i++) {
                        e = seg->current->reach.elev_change[i];
                        if (-e >= d)
                            e = -d + 0.01;
                        seg->fork->current->reach.elev_change[i] = e;
                    }
                    propogate_elev_change( seg->fork, dam_top );
                }
                else
                    find_and_prop_upstream_dam( seg->fork );
            }
            break;
        case DAM_SEG:
            propogate_elev_change( seg->up, seg->current->dam.fb_elev );
            break;

        case HEADWATER_SEG:
            break;
        }
    }
}

