/* at_hist.c
 *	Do all the histogram stuff for the analysis tool.
 */

#include <stdio.h>
#ifdef WIN32
#include <stdlib.h>
#endif
#include <string.h>
#include <errno.h>
#include "crisp.h"
#include "log.h"
#include "struct_at.h"
#include "parseutil.h"
#include "writeutil.h"
#include "report.h"

#define MIN_NUM_BINS		1
#define MAX_NUM_BINS		200
#define DEFAULT_NUM_BINS	50
#define ALL_CHARS "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()-_=+\\|'\"`~;:[]{},./<>?\n\r\t "
#define NEAR_ZERO		0.000001
#define MIN_TRAVEL_TIME_PASSAGE 1.0

/*
 * create_summary - create the summary array, and store all averages
 *	we may need.
 */
#ifdef WIN32
float *create_summary(struct rls_stats *rls_data, float *total, int num, 
					  float *mean_day, float *first_day, float *last_day,
			          float *median_day, float *mode_day, float *day_dev, 
					  float *avg_flow, int flags)
#else
float *create_summary(rls_data, total, num, mean_day, first_day, last_day,
			     median_day, mode_day, day_dev, avg_flow, flags)
	struct rls_stats *rls_data;
	int num, flags;
	float *total;
	float *mean_day,*first_day,*last_day,*median_day,*mode_day;
        float *day_dev,*avg_flow;
#endif
{
    float sum_total = 0.0;
    float *ptr = 0;
    int zeros;
    int i;

    *mean_day = *first_day = *last_day = *day_dev = *median_day = *mode_day
	= *avg_flow = 0.0;
    /* This pointer is freed in hist_destroy_func */
    if (!(flags&AT_TRAVEL_FLAG || (ptr=(float *) malloc(num*sizeof(float))))) {
	log_msg(L_ERROR, "Can't allocate memory for histogram data!\n");
	return 0;
    }
    for (i = 0, zeros = 0 ; i < num ; i++, rls_data++) {
	if (rls_data->total.total > *total) {
	    log_msg(L_ERROR, "Data error:survival %f higher than release %f\n",
		    rls_data->total.total, *total);
	    continue;
	}
        if (!(flags & AT_TRAVEL_FLAG)) ptr[i] = rls_data->total.total / *total;

	if (rls_data->total.total < 0.1) {
	    zeros++;
	    continue;
	}
	sum_total += rls_data->total.total;

	*avg_flow   += rls_data->avg_flow;

	/* Weighted averages */
	*mean_day   += rls_data->total.mean    * rls_data->total.total;
	*median_day += rls_data->total.median  * rls_data->total.total;
	*mode_day   += rls_data->total.mode    * rls_data->total.total;
	*first_day  += rls_data->total.first   * rls_data->total.total;
	*last_day   += rls_data->total.last    * rls_data->total.total;
	*day_dev    += rls_data->total.std_dev * rls_data->total.total;
    }

    if (flags & AT_TRAVEL_FLAG && sum_total < 1.0) {
	log_msg(L_ERROR, "Total survival less than the %.1f required for %s\n",
		1.0, "travel time analysis");
	return (float *) 1;
	/* non-zero is the error return for travel time summary */
    }

    if ((num -= zeros) > 0) {
	*avg_flow   /= (float) num;

	/* Weighted averages */
	if (sum_total > 0) {
	    *mean_day   /= sum_total;
	    *median_day /= sum_total;
	    *mode_day   /= sum_total;
	    *first_day  /= sum_total;
	    *last_day   /= sum_total;
	    *day_dev    /= sum_total;
	} else {
	    *mean_day = *median_day = *mode_day = *first_day = *last_day
		= *day_dev = 0.0;
	}
    }
    return ptr;
}

#ifdef ROUTE_TRACKING
/*
 * int calculate_route_avgs()
 *
 *	returns the total passage by all routes, which is zero if the
 *	relevant segment is not a dam (which can be used as a flag to
 *	determine if tracking data is available.)
 */
int calculate_route_avgs(rls_data, num, sluiceway_avg, byp_avg, tur_avg, spi_avg, rsw_avg, tra_avg,
			 premort_transport_total, postmort_transport_total,
			 fpe, sluiceway_in_avg, byp_in_avg, tur_in_avg, spi_in_avg, rsw_in_avg, pro_in_sum)
	struct rls_stats       *rls_data;
	int num;
	float *sluiceway_avg, *byp_avg, *tur_avg, *spi_avg, *rsw_avg, *tra_avg, *premort_transport_total,
	    *postmort_transport_total, *fpe, *sluiceway_in_avg, *byp_in_avg, *tur_in_avg,
	    *spi_in_avg, *rsw_in_avg, *pro_in_sum;
{
    int i;
    float route_total, route_in_total;
    float turbine_in_sum, spill_in_sum, rsw_in_sum, sluiceway_in_sum, bypass_in_sum, project_in_sum;
    float sluiceway_sum, bypass_sum, turbine_sum, spill_sum, rsw_sum, transport_sum;

    turbine_in_sum = spill_in_sum = rsw_in_sum = sluiceway_in_sum = bypass_in_sum = project_in_sum
	= sluiceway_sum = bypass_sum = turbine_sum = spill_sum = rsw_sum = transport_sum = 0.0;
    *premort_transport_total = *postmort_transport_total = 0.0;
    for (i = 0; i < num; i++, rls_data++)
    {
	sluiceway_sum    += rls_data->sluiceway_total;
	bypass_sum    += rls_data->bypass_total;
	turbine_sum   += rls_data->turbine_total;
	spill_sum     += rls_data->spillway_total;
	rsw_sum     += rls_data->rsw_total;
	transport_sum += rls_data->postmort_transport_total;
	*premort_transport_total  += rls_data->premort_transport_total;
	*postmort_transport_total += rls_data->postmort_transport_total;
	project_in_sum 		  += rls_data->project_in_total;
	sluiceway_in_sum  += rls_data->sluiceway_in_total;
	bypass_in_sum  += rls_data->bypass_in_total;
	spill_in_sum   += rls_data->spillway_in_total;
	rsw_in_sum   += rls_data->rsw_in_total;
	turbine_in_sum += rls_data->turbine_in_total;
    }
    route_total = bypass_sum + turbine_sum + spill_sum + transport_sum;
    if (route_total > 0) {
	*sluiceway_avg    = sluiceway_sum     / route_total; /*avg % of total.*/
	*byp_avg    = bypass_sum     / route_total; /*avg % of total.*/
	*tur_avg    = turbine_sum    / route_total;
	*spi_avg    = spill_sum      / route_total;
	*rsw_avg    = rsw_sum      / route_total;
	*tra_avg    = transport_sum  / route_total;
	/* Turn into the percent, rather than the decimal */
	*sluiceway_avg    *= 100.0;
	*byp_avg    *= 100.0;
	*tur_avg    *= 100.0;
	*spi_avg    *= 100.0;
	*rsw_avg    *= 100.0;
	*tra_avg    *= 100.0;
    } else {
	*sluiceway_avg = *byp_avg = *tur_avg = *spi_avg = *rsw_avg = *tra_avg = 0.0;
    }
    route_in_total = sluiceway_in_sum + bypass_in_sum + turbine_in_sum + spill_in_sum + rsw_in_sum;
    if (route_in_total > 0) {
	*sluiceway_in_avg = sluiceway_in_sum  / route_in_total;
	*byp_in_avg = bypass_in_sum  / route_in_total;
	*tur_in_avg = turbine_in_sum / route_in_total;
	*spi_in_avg = spill_in_sum   / route_in_total;
	*rsw_in_avg = rsw_in_sum   / route_in_total;
	*sluiceway_in_avg *= 100.0;
	*byp_in_avg *= 100.0;
	*tur_in_avg *= 100.0;
	*spi_in_avg *= 100.0;
	*rsw_in_avg *= 100.0;
    } else {
	*sluiceway_in_avg = *byp_in_avg = *tur_in_avg = *spi_in_avg = *rsw_in_avg = 0.0;
    }
    if (num > 0) {
	*premort_transport_total  /= (float) num;
	*postmort_transport_total /= (float) num;
	*pro_in_sum		  =  project_in_sum / (float) num;
    } else
	*pro_in_sum = project_in_sum;

    *fpe = 1.0 - ((project_in_sum) ? turbine_in_sum / project_in_sum : 0.0);
    *fpe *= 100.0;

    return ((*sluiceway_avg + *byp_avg + *tur_avg + *spi_avg + *rsw_avg) > 0.0);
}
#endif

/*
 * float compute_weighted_average(float *travel_times, struct rls_stats
 *	*rls_data, int num) -- Returns a weighted (by rls_data[num].total)
 *	travel time mean.
 *
 *	If any travel time value is less than zero, it discards it and
 *	logs a warning message.
 */
float compute_weighted_average(travel_times, rls_data, num)
	float *travel_times;
	struct rls_stats *rls_data;
	int num;
{
    float sum_total = 0.0;
    float weighted_sum = 0.0;

    for (--num ; num >= 0 ; --num) {
	if (rls_data[num].total.total < NEAR_ZERO) {
	    log_msg(L_MSG, "Zero passage for game %d, %s\n", num + 1,
		    "no travel time weight for mean travel time");
	    continue;
	}
	if (travel_times[num] < NEAR_ZERO) {
	    log_msg(L_WARNING,
		 "Impossible travel time value discarded!\n%s",
		 " Weighted average may vary slightly from expected value.\n");
	    log_msg(L_DBGMSG, "Game %d:Passage = %.4f, travel time = %.4f\n",
		    num + 1, rls_data[num].total.total, travel_times[num]);
	    continue;
	}
	weighted_sum += travel_times[num] * rls_data[num].total.total;
	sum_total    += rls_data[num].total.total;
    }
    if (sum_total < MIN_TRAVEL_TIME_PASSAGE)
	return 0.0;
    return weighted_sum / sum_total;
}
