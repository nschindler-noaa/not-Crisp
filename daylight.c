#include "crisp.h"
#include "crisp_time.h"

#include <math.h>

/* get the sunrise and sunset for a particular day */
static void get_sun(int day, float lat, float* sunrise, float* sunset)
{
    /* Tilt is the tilt of the earth, which varies through the season */
    double tilt;
    double daylength;
    /* arbitrarily assume noon is at 12:00; should do longitudinal adjustment */
    double noon = 12.0;
    double pi_180 = M_PI / 180.0;
    
    /* nicko's daylength calculation (approximate) */

    /* arguments to math calls get converted to radians */
    lat *= pi_180;
    tilt = -cos((pi_180)*(360.0/365.0)*(day+10))*23.5;
    tilt *= pi_180;
    daylength = 24*(180.0 + 2*(180/M_PI)*asin(tan(lat)*tan(tilt)))/360.0;

    *sunrise = noon - daylength/2;
    *sunset  = noon + daylength/2;
}

/* compute the proportion of daylight for a given dam time slice in day */
static float daylight_proportion(int slice, float sunrise, float sunset)
{
    float slice_length = 24.0/DAM_TIME_SLC;
    float start_time = slice_length * slice;
    float end_time = start_time + slice_length;

    /* start with whole slice in daylight */
    float daylight = slice_length;

    /* check for whole slice of darkness */
    if (end_time <= sunrise || start_time >= sunset)
	return 0.0;

    /* otherwise subtract off any darkness */
    if (start_time < sunrise)
	daylight -= sunrise - start_time;
    if (end_time > sunset)
	daylight -= end_time - sunset;
    return daylight / slice_length;
}
	

/* this routine sets the daylight proportions at a dam for all dam time slices*/
void compute_daylight_proportions(struct river_segment *seg)
{
/* extremely simplistic version as a place holder */
    int day, slice;
    /* get the latitude of the first point on this river seg */
    float lat = seg->top->lat;

    for (day=0; day < DAYS_IN_SEASON; ++day) {
	float sunrise, sunset;
	get_sun(day, lat, &sunrise, &sunset);
	
	for (slice=0; slice < DAM_TIME_SLC; ++slice) {
	    seg->current->dam.daylight_proportion[day*DAM_TIME_SLC + slice]
		= daylight_proportion(slice, sunrise, sunset);
	}
    }
}

/* This routine gets the proportion of daylight in a given timestep.
     (1 = all day, 0 = all night). */
double get_daylight_fraction( double latitude, int timestep )
{
    double step_length = 24.0 / STEPS_PER_DAY;
	int day = timestep / STEPS_PER_DAY;
	float sunrise, sunset;

    double start_time = step_length * (timestep - day * STEPS_PER_DAY);
    double end_time = start_time + step_length;
	double daylight;

	get_sun(day, latitude, &sunrise, &sunset);

    /* check for whole slice of darkness */
    if (end_time <= sunrise || start_time >= sunset)
		return 0.0;

    /* start with whole step in daylight */
    daylight = step_length;

    /* subtract off any darkness */
    if (start_time < sunrise)
		daylight -= sunrise - start_time;
	if (end_time > sunset)
		daylight -= end_time - sunset;
    return daylight / step_length;
}
