/* decls and routines dealing with time steps */

#ifdef WIN32
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#endif

#include "crisp.h"
#include "crisp_time.h"

/* hour of the beginning of a dam time step */
#define step_time(i)	((int)(24/DAM_TIME_SLC)*(i)*100)


/* routine which returns string representing hours of requested time slice */

char *hours(time)
    int time;
{
    static char hour_list[DAM_TIME_SLC][12];
    static int flag = 0;

    /* init hour list on first time through */
    if (!flag)
    {
	int i;
	
	flag = TRUE;
	for (i=0; i < DAM_TIME_SLC; i++)
	    sprintf(hour_list[i], "%4.4d-%4.4d", step_time(i), step_time(i+1));
    }
    
    return(hour_list[time]);
}


/* routine which determines if dam time step is during night */

int is_night(time)
    int time;
{
    int start = step_time(time);
    int end = step_time(time+1);

    if ((start >= night_start && end <= night_end)
	|| (start < night_end && night_end - start >= step_time(1)/2)
	|| (end > night_start && end - night_start >= step_time(1)/2) )

	return(TRUE);
    else
	return(FALSE);
}
