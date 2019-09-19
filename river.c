/** \file
 * \brief Implementation of functions manipulating the river. */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "crisp.h"
#include "river.h"
#include "parseutil.h" /* for defn of lowercase() */
#include "transport.h"
#include "species.h"
#include "basin.h"

static struct river_segment *last=0;

struct river_segment *
new_river_segment(seg_type)
	int		seg_type;
{
    struct river_segment *new;

    if ( ! (new = (struct river_segment *)
	    malloc(sizeof(struct river_segment))))
	return (struct river_segment *)0;

    bzero((char *)new, sizeof(struct river_segment));
    if (last)
	last->next = new;
    else
	river_mouth = new;
    last = new;
    new->seg_type = seg_type;
    new->current = &new->run[0];

    /* defaults */
    switch(seg_type)
    {
    case REACH_SEG:
	
	/* we need to know later if certain characteristics appeared in the
	 * file, so we set the values to nonsense first.
	 */
	new->current->reach.slope = -1;
	new->current->reach.volume = -1;
	new->current->reach.surface_area = -1;
	new->current->reach.width = -1;
	new->current->reach.depth = -1;
	new->current->reach.upper_depth = -1;
	new->current->reach.lower_depth = -1;
    allocate_reach_species(new);
	break;
    case DAM_SEG:
    case POWERHOUSE_SEG:
	allocate_dam_species(new);
	break;
    case HEADWATER_SEG:
	new->current->headwater.flow_min = 0.02;
	break;
    default:
	break;
    }
    return new;
}

int destroy_river_segment (seg)
    struct river_segment *seg;
{
    struct river_segment *cur_seg, *old_seg=0;

    if (seg->up || seg->fork || seg->down)
    {
	log_msg(L_ERROR,"YO: Someone somewhere is being stupid. Removing a \n");
	log_msg(L_ERROR,"\triver segment that has already been sorted into \n");
	log_msg(L_ERROR,"\tthe river sequence causes massive problems. This\n");
	log_msg(L_ERROR,"\tattempt failed. River segment name is {%s}.\n",
		seg->seg_name);
	return(1);
    }

    /* traverse the data structure and excise the segment */
    for ( cur_seg = river_mouth ; cur_seg ; cur_seg = cur_seg->next )
    {
	if (seg == cur_seg)
	{
	    if (old_seg) old_seg->next = cur_seg->next;
	    if (last == seg)
		last = old_seg;
	    continue; 
	}
	else
	    old_seg = cur_seg;
    }
    
    if (seg->seg_type == DAM_SEG)
    {
	remove_transport(seg);
	remove_basin(seg);
    }
    if (seg->seg_type == DAM_SEG || seg->seg_type == POWERHOUSE_SEG)
    {
	destroy_period_struct(seg->current->dam.planned_spill_day);
	destroy_period_struct(seg->current->dam.planned_spill_night);
	destroy_period_struct(seg->current->dam.phouse_schedule);
	if (seg->current->dam.additional_pwrhs)
	    destroy_river_segment(seg->current->dam.additional_pwrhs);
	if (seg->current->dam.species)
	    free(seg->current->dam.species);
    }
    if (seg->seg_name)
	(void) free(seg->seg_name);
    if (seg->river_name)
	(void) free(seg->river_name);
    if (seg->abbrev)
	(void) free(seg->abbrev);

    (void) free(seg);

    return(0);
}

/*------------------------------------------------------------------------*/
/* these scratch variables are defined here to avoid the overhead of having
 * them initialized everytime a recursive fcn that uses them is called.   */
/*------------------------------------------------------------------------*/
static char scratch1[MAX_COMMAND_LEN + 1], scratch2[MAX_COMMAND_LEN + 1];
/*------------------------------------------------------------------------*/

/**
 find_seg_by_name looks for the river segment matching the name passed to
   it.
   \return  a pointer the that river_segment or 0 if it is not found.
*/

struct river_segment *find_seg_by_name(river, seg_type, name)
    struct river_segment *river;
    int seg_type;
    char *name;
{
    if (!river)
	return ((struct river_segment *)0);
    else if (river->seg_type == seg_type &&
	     strcmp( lowercase(river->seg_name,scratch1), lowercase(name,scratch2)) == 0 )
	return (river);
    else
	return (find_seg_by_name (river->next, seg_type, name));
}

/*
 find_seg_by_abbrev looks for the river segment matching the abbreviation
   passed to it.
   \return a pointer the that river_segment or
   0 if it is not found
*/

struct river_segment *find_seg_by_abbrev(river, abbr)
    struct river_segment *river;
    char *abbr;
{
    if (!river)
	return ((struct river_segment *)0);
    else if (!river->abbrev)
	return (find_seg_by_abbrev (river->next, abbr));
    else if (strcmp( lowercase(river->abbrev,scratch1), lowercase(abbr,scratch2)) == 0 )
	return (river);
    else
	return (find_seg_by_abbrev (river->next, abbr));
}


/* a couple of utility routines to find the next upstream or
 * downstream point on a river segment.
 * preconditions: properly formed seg;
 *		  point exists in seg point list
 * postconditions: no state change
 * return: pointer to the appropriate next point, or 0 if not found
 */
struct point *
next_downstream_point(seg, pt)
    struct river_segment *seg;
    struct point *pt;
{
    if (!pt)
	return 0;
    else if (pt->prev)
	return pt->prev;
    else if (seg->down)
	return next_downstream_point(seg->down, seg->down->top);
    else
	return 0;
}

struct point *
next_upstream_point(seg, pt)
    struct river_segment *seg;
    struct point *pt;
{
    if (!pt)
	return 0;
    else if (pt->next)
	return pt->next;
    else if (seg->up)
	return next_upstream_point(seg->up, seg->up->bottom);
    else
	return 0;
}

