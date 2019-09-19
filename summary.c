/* output.c:  contains routines to do model output and summary statistics */

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include "crisp.h"
#include "log.h"
#include "writeutil.h"
#include "species.h"
#include "monte_output.h"
#include "dump.h"
#include "output_flags.h"
#include "unused.h"

#define FN_BASE "summary"		/* output file name base */
#define FN_EXT_SCN "dat"		/* scenario mode extension */
#define FN_EXT_MNT "alt"		/* monte mode extension */
#define FN_LEN	16			/* maximum filename length */

#define VALUES_IN_HIST(hist) (int) ((hist).conv_factor * DAYS_IN_SEASON)

/* this routine opens the summary file and writes the output data for one game.
 * if both the year and game are <=0, then the summary file is
 * overwritten; otherwise it is appended to.  if the game or year is less
 * than 0, then scenario mode is assumed, and no year or game labels are
 * included in the output.  the filename for scenario mode is "summary.dat",
 * for monte mode it is "summary.altn" where n is the alternative number.
 */
   
int summarize_histograms(outfile, game)
	FILE *outfile;
	int game;
{
    struct river_segment *seg;
	struct release *release;
	struct rls_seg *rls_seg;
	char strbuff[256];
    
	/* use seg->output_settings bits to control summary output.
	 */
	for (release = current->release_list; release; release = release->next)
    {
		for (rls_seg = release->rls_seg; rls_seg; rls_seg = rls_seg->down)
		{
			
			/* Fish Length and Passage data is stored by time step for REACH_SEG's only.
			 * Both Length and Passage are needed to compute growth and so both
			 * are printed when Length data is requested.
			 */
			if ((rls_seg->river_segment->output_settings & LENGTH_FLAG) != 0) {
				log_msg(L_MSG, "\n%s Mean Fish Length vs Julian Day:",
									rls_seg->river_segment->seg_name);
				
				/*this "if" statement should not be necessary, but could quietly prevent problems*/
				if (rls_seg->river_segment->seg_type == REACH_SEG) {
					sprintf(strbuff,": Mean Fish Length vs Julian Day for stock: %s  Start Date: %d  ",
						release->stock->name, release->start_date);
					log_raw_float_array( STEPS_IN_SEASON,
						rls_seg->data[game]->length );
					OUTPUT_FLOAT_ARRAY(rls_seg->river_segment->seg_name,
						strbuff,DAYS_IN_SEASON, FLOAT_DEFAULT,
							rls_seg->data[game]->length,
							OCT_AVERAGE, STEPS_PER_DAY, NULL);

					sprintf(strbuff,": Fish Passage vs Julian Day for stock: %s  Start Date: %d  ",
						release->stock->name, release->start_date);
					log_raw_float_array( STEPS_IN_SEASON,
						rls_seg->data[game]->output );
					OUTPUT_FLOAT_ARRAY(rls_seg->river_segment->seg_name,
						strbuff,DAYS_IN_SEASON, FLOAT_DEFAULT,
							rls_seg->data[game]->output,
							OCT_SUM, STEPS_PER_DAY, NULL);
				}  

			}
			else if ((rls_seg->river_segment->output_settings & PASSAGE_FLAG) != 0) {
				log_msg(L_MSG, "\n%s Fish Passage vs Julian Day:",
									rls_seg->river_segment->seg_name);
				
				/*this "if" statement should not be necessary, but could quietly prevent problems*/
				if (rls_seg->river_segment->seg_type == REACH_SEG) {
					sprintf(strbuff,": Fish Passage vs Julian Day for stock: %s  Start Date: %d  ",
						release->stock->name, release->start_date);
					log_raw_float_array( STEPS_IN_SEASON,
						rls_seg->data[game]->output );
					OUTPUT_FLOAT_ARRAY(rls_seg->river_segment->seg_name,
						strbuff,DAYS_IN_SEASON, FLOAT_DEFAULT,
							rls_seg->data[game]->output,
							OCT_SUM, STEPS_PER_DAY, NULL);
				}  
			}

			if ((rls_seg->river_segment->output_settings & SPILL_EXPERIENCED_FLAG) != 0) {
				log_msg(L_MSG, "\n%s Mean Spill Experienced vs Julian Day:",
									rls_seg->river_segment->seg_name);
				
				/*this "if" statement should not be necessary, but could quietly prevent problems*/
				if (rls_seg->river_segment->seg_type == REACH_SEG) {
					sprintf(strbuff,": Mean Spill Experienced vs Julian Day for stock: %s  Start Date: %d  ",
						release->stock->name, release->start_date);
					log_raw_float_array( STEPS_IN_SEASON,
						rls_seg->data[game]->dam_spill );
					OUTPUT_FLOAT_ARRAY(rls_seg->river_segment->seg_name,
						strbuff,DAYS_IN_SEASON, FLOAT_DEFAULT,
							rls_seg->data[game]->dam_spill,
							OCT_AVERAGE, STEPS_PER_DAY, NULL);

				}
			}
		}
	}
		


    for (seg = river_mouth; seg; seg = seg->next)
	{
		/* Flow data is straightforward - one value per day. */
	    if ((seg->output_settings & FLOW_FLAG) != 0) {
			log_msg(L_MSG, "\nFlow for %s (kcfs):", seg->seg_name);
			if (seg->seg_type == DAM_SEG) {
				log_msg_float_array( DAYS_IN_SEASON, 
									  seg->current->dam.flow );
				OUTPUT_FLOAT_ARRAY("Flow for ", seg->seg_name, 
							DAYS_IN_SEASON, FLOAT_DEFAULT,
							seg->current->dam.flow, OCT_NO_CONV, 1, NULL);
			}
			else if (seg->seg_type == REACH_SEG) {
				log_msg_float_array( DAYS_IN_SEASON, 
									  seg->current->reach.flow );
				OUTPUT_FLOAT_ARRAY("Flow for ", seg->seg_name, 
							DAYS_IN_SEASON, FLOAT_DEFAULT,
							seg->current->reach.flow, OCT_NO_CONV, 1, NULL);
			}
			else if (seg->seg_type == HEADWATER_SEG) {
				log_msg_float_array( DAYS_IN_SEASON, 
									  seg->current->headwater.flow );
				OUTPUT_FLOAT_ARRAY("Flow for ", seg->seg_name, 
							DAYS_IN_SEASON, FLOAT_DEFAULT,
							seg->current->headwater.flow, OCT_NO_CONV, 1, NULL);
			}
	    }

		/* Water temperature data is stored by time step (several values
		 *  per day) and is logged both by day and by step. */
	    if ((seg->output_settings & WATERTEMP_FLAG) != 0) {
			log_msg(L_MSG, "\n%s Water Temperature (C) vs Julian Day:",
										seg->seg_name);
			if (seg->seg_type == DAM_SEG) {
				log_float_array( L_MSG, DAYS_IN_SEASON, 
							seg->current->dam.water_temp, 
							OCT_AVERAGE, STEPS_PER_DAY );
			}
			else if (seg->seg_type == REACH_SEG) {
				log_float_array( L_MSG, DAYS_IN_SEASON, 
							seg->current->reach.water_temp,
							OCT_AVERAGE, STEPS_PER_DAY );
			}
			else if (seg->seg_type == HEADWATER_SEG) {
				log_float_array( L_MSG, DAYS_IN_SEASON, 
							seg->current->headwater.water_temp,
							OCT_AVERAGE, STEPS_PER_DAY );
			}

			log_msg(L_DUMP, "\n%s Water Temperature (C) vs time step:",
									seg->seg_name );
			if (seg->seg_type == DAM_SEG) {
				log_raw_float_array( STEPS_IN_SEASON,
							seg->current->dam.water_temp );
				OUTPUT_FLOAT_ARRAY(seg->seg_name,
							" Water Temperature (C) vs Julian Day",
							DAYS_IN_SEASON, FLOAT_DEFAULT,
							seg->current->dam.water_temp, OCT_AVERAGE, 
							STEPS_PER_DAY, NULL);
			}
			else if (seg->seg_type == REACH_SEG) {
				log_raw_float_array( STEPS_IN_SEASON, 
							seg->current->reach.water_temp );
				OUTPUT_FLOAT_ARRAY(seg->seg_name,
							" Water Temperature (C) vs Julian Day",
							DAYS_IN_SEASON, FLOAT_DEFAULT,
							seg->current->reach.water_temp, OCT_AVERAGE, 
							STEPS_PER_DAY, NULL);
			}
			else if (seg->seg_type == HEADWATER_SEG) {
				log_raw_float_array( STEPS_IN_SEASON, 
							seg->current->headwater.water_temp );
				OUTPUT_FLOAT_ARRAY(seg->seg_name,
							" Water Temperature (C) vs Julian Day",
							DAYS_IN_SEASON, FLOAT_DEFAULT,
							seg->current->headwater.water_temp, OCT_AVERAGE, 
							STEPS_PER_DAY, NULL);
			}

	    }

		/* Turbidity data is stored by time step (several values
		 *  per day) and is logged both by day and by step. */
	    if ((seg->output_settings & TURBIDITY_FLAG) != 0) {
			log_msg(L_MSG, "\n%s Turbidity vs Julian Day:",
										seg->seg_name);
			if (seg->seg_type == DAM_SEG) {
				log_float_array( L_MSG, DAYS_IN_SEASON, 
							seg->current->dam.turbidity, 
							OCT_AVERAGE, STEPS_PER_DAY );
			}
			else if (seg->seg_type == REACH_SEG) {
				log_float_array( L_MSG, DAYS_IN_SEASON, 
							seg->current->reach.turbidity,
							OCT_AVERAGE, STEPS_PER_DAY );
			}
			else if (seg->seg_type == HEADWATER_SEG) {
				log_float_array( L_MSG, DAYS_IN_SEASON, 
							seg->current->headwater.turbidity,
							OCT_AVERAGE, STEPS_PER_DAY );
			}

			log_msg(L_DUMP, "\n%s Turbidity vs time step:",
									seg->seg_name );
			if (seg->seg_type == DAM_SEG) {
				log_raw_float_array( STEPS_IN_SEASON,
							seg->current->dam.turbidity );
				OUTPUT_FLOAT_ARRAY(seg->seg_name,
							" Turbidity vs Julian Day",
							DAYS_IN_SEASON, FLOAT_DEFAULT,
							seg->current->dam.turbidity, OCT_AVERAGE, 
							STEPS_PER_DAY, NULL);
			}
			else if (seg->seg_type == REACH_SEG) {
				log_raw_float_array( STEPS_IN_SEASON, 
							seg->current->reach.turbidity );
				OUTPUT_FLOAT_ARRAY(seg->seg_name,
							" Turbidity vs Julian Day",
							DAYS_IN_SEASON, FLOAT_DEFAULT,
							seg->current->reach.turbidity, OCT_AVERAGE, 
							STEPS_PER_DAY, NULL);
			}
			else if (seg->seg_type == HEADWATER_SEG) {
				log_raw_float_array( STEPS_IN_SEASON, 
							seg->current->headwater.turbidity );
				OUTPUT_FLOAT_ARRAY(seg->seg_name,
							" Turbidity vs Julian Day",
							DAYS_IN_SEASON, FLOAT_DEFAULT,
							seg->current->headwater.turbidity, OCT_AVERAGE, 
							STEPS_PER_DAY, NULL);
			}

	    }



		/* Nitrogen supersaturation is stored DAM_TIME_SLICE values per day,
         *  and is stored separately for the right and left sides of the river.
         */
	    if ((seg->output_settings & NSAT_FLAG) != 0) {
			if (seg->seg_type == DAM_SEG) {
				log_msg( L_MSG, 
                         "\n%s Left-side TDG Saturation > 100%c vs Julian Day:", 
						 seg->seg_name, '%' );
			    log_float_array(L_MSG, DAYS_IN_SEASON,
				                seg->current->dam.gas_out->gas[FLOW_LEFT], 
                                OCT_AVERAGE, DAM_TIME_SLC);
				log_msg( L_DUMP, 
                         "\n%s Left-side TDG Saturation > 100%c vs time slice:", 
						 seg->seg_name, '%' );
			    log_raw_float_array( DAM_SLICES_IN_SEASON,
				                seg->current->dam.gas_out->gas[FLOW_LEFT] );

				log_msg( L_MSG, 
                         "\n%s Right-side TDG Saturation > 100%c vs Julian Day:", 
						 seg->seg_name, '%' );
			    log_float_array(L_MSG, DAYS_IN_SEASON,
				                seg->current->dam.gas_out->gas[FLOW_RIGHT], 
                                OCT_AVERAGE, DAM_TIME_SLC);
				log_msg( L_DUMP, 
                         "\n%s Right-side TDG Saturation > 100%c vs time slice:", 
						 seg->seg_name, '%' );
			    log_raw_float_array( DAM_SLICES_IN_SEASON,
				                seg->current->dam.gas_out->gas[FLOW_RIGHT] );

				OUTPUT_FLOAT_ARRAY(seg->seg_name,
							    " Left-side TDG Saturation > 100% vs Julian Day",  
							    DAYS_IN_SEASON, FLOAT_DEFAULT,
							    seg->current->dam.gas_out->gas[FLOW_LEFT],
                                OCT_AVERAGE, DAM_TIME_SLC, NULL);
				OUTPUT_FLOAT_ARRAY(seg->seg_name,
							    " Right-side TDG Saturation > 100% vs Julian Day",  
							    DAYS_IN_SEASON, FLOAT_DEFAULT,
							    seg->current->dam.gas_out->gas[FLOW_RIGHT],
                                OCT_AVERAGE, DAM_TIME_SLC, NULL);
			}
			else if (seg->seg_type == REACH_SEG) {
				log_msg( L_MSG, 
                         "\n%s Left-side TDG Saturation > 100%c vs Julian Day:", 
						 seg->seg_name, '%' );
			    log_float_array(L_MSG, DAYS_IN_SEASON,
				                seg->current->reach.gas_out->gas[FLOW_LEFT], 
                                OCT_AVERAGE, DAM_TIME_SLC);
				log_msg( L_DUMP, 
                         "\n%s Left-side TDG Saturation > 100%c vs time slice:", 
						 seg->seg_name, '%' );
			    log_raw_float_array( DAM_SLICES_IN_SEASON,
				                seg->current->reach.gas_out->gas[FLOW_LEFT] );

				log_msg( L_MSG, 
                         "\n%s Right-side TDG Saturation > 100%c vs Julian Day:", 
						 seg->seg_name, '%' );
			    log_float_array(L_MSG, DAYS_IN_SEASON,
				                seg->current->reach.gas_out->gas[FLOW_RIGHT], 
                                OCT_AVERAGE, DAM_TIME_SLC);
				log_msg( L_DUMP, 
                         "\n%s Right-side TDG Saturation > 100%c vs time slice:", 
						 seg->seg_name, '%' );
			    log_raw_float_array( DAM_SLICES_IN_SEASON,
				                seg->current->reach.gas_out->gas[FLOW_RIGHT] );

				OUTPUT_FLOAT_ARRAY(seg->seg_name,
							    " Left-side TDG Saturation > 100% vs Julian Day",  
							    DAYS_IN_SEASON, FLOAT_DEFAULT,
							    seg->current->reach.gas_out->gas[FLOW_LEFT],
                                OCT_AVERAGE, DAM_TIME_SLC, NULL);
				OUTPUT_FLOAT_ARRAY(seg->seg_name,
							    " Right-side TDG Saturation > 100% vs Julian Day",  
							    DAYS_IN_SEASON, FLOAT_DEFAULT,
							    seg->current->reach.gas_out->gas[FLOW_RIGHT],
                                OCT_AVERAGE, DAM_TIME_SLC, NULL);
			}
	    }

        /* Velocity is in miles per time step, stored by time step. */
	    if ((seg->output_settings & VELOCITY_FLAG) != 0
							&& seg->seg_type == REACH_SEG) {
			log_msg(L_MSG, "\n%s Water Velocity (mi/day) vs Julian Day:",
						seg->seg_name);
			log_float_array(L_MSG, DAYS_IN_SEASON,
				    seg->current->reach.velocity, OCT_SUM, STEPS_PER_DAY);
			log_msg(L_DUMP, "\n%s, Water Velocity (mi/step) vs time step:",
							seg->seg_name);
			log_raw_float_array( STEPS_IN_SEASON, seg->current->reach.velocity );
			OUTPUT_FLOAT_ARRAY("Water Velocity (mi/day) vs Julian Day ", 
						seg->seg_name, DAYS_IN_SEASON, FLOAT_DEFAULT,
						seg->current->reach.velocity, OCT_SUM, STEPS_PER_DAY, NULL);
	    }

	    /* loss, day_spill, elev also */
	    if ((seg->output_settings & LOSS_FLAG) != 0
			 && seg->seg_type == REACH_SEG) {
			log_msg(L_MSG, "\nLoss for %s (kcfs):", seg->seg_name);
			log_msg_float_array(DAYS_IN_SEASON, seg->current->reach.loss);
			OUTPUT_FLOAT_ARRAY("Loss (kcfs) for ", seg->seg_name, 
						DAYS_IN_SEASON, FLOAT_DEFAULT,
						seg->current->reach.loss, OCT_NO_CONV, 1, NULL);
	    }

	    if ((seg->output_settings & SPILL_FLAG) != 0
			 && seg->seg_type == DAM_SEG) {
			log_msg(L_MSG, "\nspill fraction vs. Julian day for %s:", 
						seg->seg_name);
			log_float_array( L_MSG, DAYS_IN_SEASON, seg->current->dam.spill,
								OCT_AVERAGE, DAM_TIME_SLC );
			log_msg(L_DUMP, "\nspill fraction vs. time step for %s:", 
						seg->seg_name);
			log_raw_float_array( DAYS_IN_SEASON * DAM_TIME_SLC,
									seg->current->dam.spill );
			OUTPUT_FLOAT_ARRAY("spill fraction vs. Julian day for ", 
							seg->seg_name, DAYS_IN_SEASON, FLOAT_DEFAULT,
							seg->current->dam.spill,
							OCT_AVERAGE, DAM_TIME_SLC, NULL);
	    }

		if ((seg->output_settings & DAY_SPILL_FLAG) != 0
		 	 && seg->seg_type == DAM_SEG) {

			log_msg(L_MSG, "\nday spill fraction vs. Julian day for %s:", 
						seg->seg_name);
			log_float_array( L_MSG, DAYS_IN_SEASON, seg->current->dam.spill,
								OCT_DAM_DAY, DAM_TIME_SLC );
			OUTPUT_FLOAT_ARRAY("day spill fraction vs. Julian day for ", 
							seg->seg_name, DAYS_IN_SEASON, FLOAT_DEFAULT,
							seg->current->dam.spill,
							OCT_DAM_DAY, DAM_TIME_SLC, NULL);

		}

		if ((seg->output_settings & NIGHT_SPILL_FLAG) != 0
		 	 && seg->seg_type == DAM_SEG) {

			log_msg(L_MSG, "\nnight spill fraction vs. Julian day for %s:", 
						seg->seg_name);
			log_float_array( L_MSG, DAYS_IN_SEASON, seg->current->dam.spill,
								OCT_DAM_NIGHT, DAM_TIME_SLC );
			OUTPUT_FLOAT_ARRAY("night spill fraction vs. Julian day for ", 
							seg->seg_name, DAYS_IN_SEASON, FLOAT_DEFAULT,
							seg->current->dam.spill,
							OCT_DAM_NIGHT, DAM_TIME_SLC, NULL);

		}

	    if ((seg->output_settings & ELEVATION_FLAG) != 0
						 && seg->seg_type == REACH_SEG) {
			log_msg(L_MSG, "\nElevation Change for %s (ft):", seg->seg_name );
			log_msg_float_array( DAYS_IN_SEASON, 
								seg->current->reach.elev_change );
			OUTPUT_FLOAT_ARRAY("Elevation Change (ft)for ", seg->seg_name, 
							DAYS_IN_SEASON, FLOAT_DEFAULT,
							seg->current->reach.elev_change, OCT_NO_CONV, 1, NULL);
	    }
	}
	log_msg(L_MSG, "\n" );
	log_msg(L_DUMP, "\n" );
    return 0;
}

/* This local routine calculates the survival of non-transported
 *  fish "Vn" for a given release, game, and transport dam.
 * According to Josh Hayes (6/11/98):

Nr = number released
Nd1, Nd2, Nd3 = number arriving at dam 1, 2, or 3
Nt1, Nt2, Nt3 = number transported out at dam 1, 2, or 3
Nd = number arriving at downstream point
 
Now, Vn for this release is given by:
 
                     [Nd/Nr]
-----------------------------------------------
[(1-(Nt1/Nd1)) * (1-(Nt2/Nd2)) * (1-(Nt3/Nd3))]

  (except that we use number arriving at current transport dam as "Nr" and
    calculate Vn incrementally as we search down the river).
 NOTE: we do our own adding up of passage arrays, because it is too tricky
  to try to force compute_stats to be called in all the necessary cases.
 */
#ifdef ROUTE_TRACKING
static float compute_Vn( struct release *UNUSED(release), 
                         struct rls_seg *rls_seg, int game ) {
    struct rls_seg *rs;
    struct rls_seg_data *dat;
    float Vn, t, s; 
    int i;

    /* Initialize Vn with number of fish not transported at this dam.
     *  NOTE this dam may not have a project_in
     *  array; in which case use the output of the upstream segment.
     *  Or the project_in array may be empty, in which case we still
     *  use the output array of the upstream segment.
     */
    dat = rls_seg->data[game];
    if (dat->project_in != NULL)
        for (i = 0, t = 0.0; i < STEPS_IN_SEASON; i++)
            t += dat->project_in[i];
    if (dat->project_in == NULL || t < 1.0) {
        dat = rls_seg->up->data[game];
        for (i = 0, t = 0.0; i < STEPS_IN_SEASON; i++)
            t += dat->output[i];
        }

    /* This segment always has a transport_out array because of how this
     *  routine was called. */
    dat = rls_seg->data[game];
    for (i = 0; i < STEPS_IN_SEASON; i++)
        t -= dat->transport_out[i];
    if (t < 1.0)
        return -1.0;
    Vn = 1 / t;

    /* Run down the river looking for the target segment of this transport,
     *  or for intervening transports which will complicate matters.
     */
    for (rs = rls_seg->down; rs != NULL; rs = rs->down) {
        if (rs->river_segment == 
            rls_seg->river_segment->current->dam.transport->target_seg) {
            /* Calculate fish who got to transport target point; finalize
             *  calculation of Vn.
             */
            dat = rs->up->data[game];
            for (i = 0, t = 0.0; i < STEPS_IN_SEASON; i++)
                t += dat->output[i];
            Vn *= t;
            return Vn;
        }
        else if (rs->river_segment->seg_type == DAM_SEG &&
            rs->river_segment->current->dam.transport != NULL) {
            /* Intervening dam with active transport found. Calculate and
             *  apply correction. NOTE this dam may not have a project_in
             *  array (or the project_in array may be empty); in which case
             *  use the output of the upstream segment.
             */
            dat = rs->data[game];
            if (dat->project_in != NULL)
                for (i = 0, t = 0.0; i < STEPS_IN_SEASON; i++)
                    t += dat->project_in[i];
            if (dat->project_in == NULL || t < 1.0) {
                dat = rs->up->data[game];
                for (i = 0, t = 0.0; i < STEPS_IN_SEASON; i++)
                    t += dat->output[i];
            }
            if (t < 1.0)
                return -1.0;
            dat = rs->data[game];
            for (i = 0, s = 0.0; i < STEPS_IN_SEASON; i++)
                s += dat->transport_out[i];
            Vn /= 1.0 - s / t;
        }
    }

    /* got to river mouth; must have messed up somehow. */
    return -1.0;
}
#endif

int write_output_summary(game, year, alt)
    int game;
    int year;
    int alt;
{
    FILE *outfile;
    struct release *release;
    struct rls_seg *rls_seg;
    int i;
    int mode;			/* 0 for scenario, 1 for monte */
    time_t clock;
    char filename[FN_LEN];
    float passed, transported_in, transported_out, censored,
    	tot_passed, tot_trans_in, tot_trans_out, tot_released,
	tot_censored, length;
#ifdef ROUTE_TRACKING
    float Vn;
#endif //ROUTE_TRACKING
    int stat_header_printed = 0;

    mode = (game >= 0 && year >= 0);	/* monte mode */
    sprintf(filename, "%s.%s", FN_BASE, mode ? FN_EXT_MNT :FN_EXT_SCN);
    if (mode)				/* if monte mode */
	sprintf(filename, "%s%d", filename, alt+1);
    
    /* output some summary stuff */
    if (game <= 0 && year <= 0)
	outfile = fopen(filename, "w");
    else
	outfile = fopen(filename, "a");
    if (!outfile)
    {
	if (mode)			/* monte mode */
	{
	    log_msg(L_ERROR, "unable to write summary file %s",
		    filename);
	}
	else
	{
	    log_msg(L_ERROR, "unable to write output file %s",
		    filename);
	}
	return(-1);
    }
    else
    {
	fprintf(outfile, "COMPASS Summary Output File\n");
	log_msg(L_MSG, "\nCOMPASS Summary Output File\n");
    }
    
    tot_passed = tot_trans_in = tot_trans_out = tot_released
	= tot_censored = 0.0;
    
    time(&clock);
    fprintf(outfile, "%s\n",ctime(&clock));
    log_msg(L_MSG, "%s\n",ctime(&clock));
    if (mode)	{			/* if monte mode */
	fprintf(outfile, "\tYear %d  Game %d\n", year, game);
	log_msg(L_MSG, "\tYear %d  Game %d\n", year, game);
    } else {
	fprintf(outfile, "\tScenario Mode\n");
	log_msg(L_MSG, "\tScenario Mode\n");
    }
    for (release = current->release_list; release; release = release->next)
    {
	int release_name_printed = 0;
	censored = 0.0;

	/* move pointer through rls_segs, counting censored fish
	   along the way*/

	for (rls_seg = release->rls_seg; rls_seg;
					 rls_seg = rls_seg->down)
	{
	    int frame_showing = FALSE;
	    censored += rls_seg->data[game]->nopass;

		/* use seg->output_settings to control output. */
		if ((rls_seg->river_segment->seg_type == DAM_SEG 
					|| rls_seg->river_segment->seg_type == REACH_SEG) && 
				(rls_seg->river_segment->output_settings
                          & (PASSAGE_FLAG | LENGTH_FLAG)) != 0)
			frame_showing = TRUE;
#ifdef ROUTE_TRACKING
		if (rls_seg->river_segment->seg_type == DAM_SEG && 
				(rls_seg->river_segment->output_settings & ROUTING_FLAG) != 0)
			frame_showing = TRUE;
		if (rls_seg->river_segment->seg_type == DAM_SEG && 
				(rls_seg->river_segment->output_settings & TRANSPORT_FLAG) != 0)
			frame_showing = TRUE;
#endif //ROUTE_TRACKING

	    /* we always gather data for the river mouth, and for other
	     * segments with a hist showing.
	     */
	    if (frame_showing || rls_seg->river_segment == river_mouth)
	    {
		/* first gather data */
		passed = transported_in = transported_out = length = 0;
		for (i = 0; i < STEPS_IN_RUN; i++)
	        {
		    passed += rls_seg->data[game]->output[i];
            length += rls_seg->data[game]->output[i]
                               * rls_seg->data[game]->length[i];
		    if (rls_seg->data[game]->transport_in)
			transported_in +=
			    rls_seg->data[game]->transport_in[i];
		    if (rls_seg->data[game]->transport_out)
			transported_out +=
			    rls_seg->data[game]->transport_out[i];
		}
	    length /= passed;
		if (rls_seg->river_segment == river_mouth)
		{
		    tot_passed += passed;
		    tot_released += release->total_released;
		    tot_censored += censored;
		    
/* not sure these total transport stats make any sense, since they
 * are only computed at the river mouth.
 *		    tot_trans_in += transported_in;
 *		    tot_trans_out += transported_out;
 */
		}

		compute_stats(rls_seg->data[game],
			      rls_seg->river_segment,
			      release->start_date,
			      (int) release->stats.total.mean);

		/* now output data */

		/* kludge to skip river mouth if no frame showing */
		if (!frame_showing)
		    continue;
		
		if (!stat_header_printed) {
		    stat_header_printed = 1;
		    write_stat_col_hdr(outfile, current->flow_seg);
		}
		if (!release_name_printed) {
		    release_name_printed = 1;
		    fprintf(outfile,
		     "\nRelease:  %s\tSpecies:  %s\tStock:  %s\tStart date:  %d\tReleased:  %.0f\nInriver return:  %f\tTransport return:  %f\tOverall return:  %f\nReturn:  %f\n",
			    release->site->name,
			    release->species->name,
                release->stock->name,
			    release->start_date,
			    release->total_released,
          release->stats.inriver_return,
          release->stats.transport_return,
		  release->stats.overall_return,
		  release->stats.post_bonneville_total);
		    log_msg(L_MSG,
		     "\nRelease:  %s\tSpecies:  %s\tStock:  %s\tStart date:  %d\tReleased:  %.0f\nInriver return:  %f\tTransport return:  %f\tOverall return:  %f\nReturn:  %f\n",
			    release->site->name,
			    release->species->name, 
                release->stock->name,
			    release->start_date,
			    release->total_released,
          release->stats.inriver_return,
          release->stats.transport_return,
		  release->stats.overall_return,
		  release->stats.post_bonneville_total);
		}
		fprintf(outfile, "\n Survival Below %s\n",
			rls_seg->river_segment->seg_name);
		log_msg(L_MSG, "\n Survival Below %s\n",
			rls_seg->river_segment->seg_name);

		fprintf(outfile,  "\tPassed:        \t%.0f\n", passed);
		log_msg(L_MSG,  "\tPassed:        \t%.0f\n", passed);
        if (rls_seg->river_segment->seg_type == REACH_SEG && 
				(rls_seg->river_segment->output_settings
                          & LENGTH_FLAG) != 0) {
            fprintf(outfile,  "\tMean Length:   \t%.0f\n", length);
    		log_msg(L_MSG,  "\tMean Length:   \t%.0f\n", length);
        }
		if (transported_in > 0.5) {
		    fprintf(outfile, "\tTransported In:\t%.0f\n",
			    transported_in);
		    log_msg(L_MSG, "\tTransported In:\t%.0f\n",
			    transported_in);
		}
		if (transported_out > 0.5) {
		    fprintf(outfile, "\tTransported Out:%.0f\n",
			    transported_out);
		    log_msg(L_MSG, "\tTransported Out:%.0f\n",
			    transported_out);

#ifdef ROUTE_TRACKING
            /* Compute "Vn" and "D" for this transport. */
            if (rls_seg->river_segment->output_settings & TRANSPORT_FLAG) {
                Vn = compute_Vn( release, rls_seg, game );
		        fprintf(outfile, "\tVn:             %.3f\n",
			        Vn);
		        log_msg(L_MSG, "\tVn:             %.3f\n",
			        Vn);
            }
#endif
		}
		if (censored > 0.5) {
		    fprintf(outfile,
			    "\tCensored:      \t%.0f\n", censored);
		    log_msg(L_MSG,
			    "\tCensored:      \t%.0f\n", censored);
		}
		fprintf(outfile,  "\tSurvival:      \t%.1f%%\n",
			(release->total_released)
			? 100*passed/release->total_released : 0);
		log_msg(L_MSG,  "\tSurvival:      \t%.1f%%\n",
			(release->total_released) ?
			100*passed / release->total_released : 0);

		write_seg_stats(outfile,
				&rls_seg->data[game]->stats,
				TRUE);
	    }
	}
    }

    if (tot_released > 0) {
	fprintf(outfile, 	"\n\nSummary for all Releases\n");
	log_msg(L_MSG, 	"\n\nSummary for all Releases\n");
	fprintf(outfile,  	"\tReleased:      \t%.0f\n",
		tot_released);
	log_msg(L_MSG,  	"\tReleased:      \t%.0f\n",
		tot_released);
	fprintf(outfile,  	"\tPassed:        \t%.0f\n",
		tot_passed);
	log_msg(L_MSG,  	"\tPassed:        \t%.0f\n",
		tot_passed);
    
/* see above comment about the total transports
 *    if (tot_trans_in > 0.5) {
 *	fprintf(outfile,"\tTransported In:\t%.0f\n", tot_trans_in);
 *	log_msg(L_MSG,"\tTransported In:\t%.0f\n", tot_trans_in);
 *   }
 *   if (tot_trans_out > 0.5) {
 *	fprintf(outfile,"\tTransported Out:%.0f\n", tot_trans_out);
 *	log_msg(L_MSG,"\tTransported Out:%.0f\n", tot_trans_out);
 *   }
 */
	if (tot_censored > 0.5) {
	    fprintf(outfile,"\tCensored:      \t%.0f\n", tot_censored);
	    log_msg(L_MSG,"\tCensored:      \t%.0f\n", tot_censored);
	}
	fprintf(outfile,  	"\tSurvival:      \t%.1f%%\n\n",
		(tot_released) ? 100*tot_passed/tot_released : 0);
	log_msg(L_MSG,  	"\tSurvival:      \t%.1f%%\n\n",
		(tot_released) ? 100*tot_passed/tot_released : 0);
    }

    summarize_histograms(outfile, game);
    summary_dump(outfile);
    
    fclose(outfile);
    return(0);
}
