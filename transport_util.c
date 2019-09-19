#include <stdlib.h>
#include "crisp.h"
#include "log.h"
#include "batch.h"
#include "transport.h"

/* compute the distance between two river segments.  first two args are
 * the segs, third is an offset to be added, usually equally to the length
 * of some portion of one or both segments.  the upstream seg must be the
 * first of the two arguments.
 */

float river_distance(from, to, offset)
    struct river_segment *from;
    struct river_segment *to;
    float  		  offset;
{
    struct river_segment *seg;
    float		  total;

    if (from == to)
	return(0);
    
    total = offset;
    seg = from->down;

    while (seg && seg != to)
    {
	if (seg->seg_type == REACH_SEG)
	    total += seg->current->reach.length;
	seg = seg->down;
    }
    return(total);
}

/* copy a transport record from another if provided, else set to default values.
 * perhaps the default values should be set in a static trns record defined in
 * an init file somewhere.
 */

int copy_trns(old, new)
    struct transport *old, *new;
{
    if (old)
	bcopy((char *)old, (char *)new, sizeof(struct transport));
    else
    {
	new->start_by_date = TRUE;
	new->start_date  = 15;
	new->start_count = 500;
	new->max_restarts = 1;
	new->restart_by_date = FALSE;
	new->restart_date  = 100;
	new->restart_count = 500;
	new->end_date	 = 200;
	new->end_count   = 500;
	new->num_low_days= 5;
	new->hfl_passed_perc = 0.8;
	new->hi_flow	 = 125;
	new->hfl_spec	 = &specs[1];
	new->rate	 = 100.0;  		/* miles/day */
	new->source_seg  = dams[3];
	new->target_seg	 = river_mouth;
    }
	return 0;
}


/* check to see if a trns record is valid.  Right now, just check that
 * the target seg is below the source.
 */

static int trns_valid(trns)
    struct transport *trns;
{
    struct river_segment *seg;

    seg = trns->source_seg;
    while (seg && seg != trns->target_seg)
	seg = seg->down;
    return(seg ? TRUE : FALSE);
}



/* Add transport to a dam.  Return TRUE if successfull.  Takes as input
 * a transport record which will be copied.  This routine may be called
 * externally.
 */

int add_transport(trns)
    struct transport *trns;
{
    struct transport *target_trns;
    
    if (trns_valid(trns))
    {
	target_trns = trns->source_seg->current->dam.transport;
	if ( ! target_trns) 	/* new transport record */
	{			   
	    if ( ! (target_trns =
		    (struct transport *)(malloc(sizeof(struct transport)))))
	    {   log_msg(L_ERROR, "Error allocating mem for transport record\n");
		return(FALSE);
	    }
	    trns->source_seg->current->dam.transport = target_trns;
	}
	copy_trns(trns, target_trns);

	return(TRUE);
    }
    else
	return(FALSE);
}

/* remove transportation from a dam.  remove the corresponding item from
 * the existing transport menu, if it exists.  remove all allocated arrays
 * in release segments associated with this transport, and then remove the
 * transport itself.  this routine may be called externally.
 */

int remove_transport(source_seg)
    struct river_segment *source_seg;
{
    struct river_segment *target_seg;
    struct river_segment *rseg;
    struct release	*release;
    struct rls_seg	*rls_seg;
    int 		i;

    if (!source_seg->current->dam.transport)
	return 0;

    target_seg = source_seg->current->dam.transport->target_seg;

    for (release = run->release_list; release; release = release->next)
    {
	rls_seg = release->rls_seg;
	while (rls_seg)
	{
	    rseg = rls_seg->river_segment;
	    if (rseg == source_seg || rseg == target_seg)
	    {
		if (rls_seg->data)
		{
		    for (i=0; rls_seg->data[i]; i++)
		    {
			if (rls_seg->data[i] != (struct rls_seg_data *) -1)
			{
			    if (rls_seg->data[i]->transport_in)
			    {
				    free(rls_seg->data[i]->transport_in);
				    rls_seg->data[i]->transport_in = NULL;
			    }
			    if (rls_seg->data[i]->transport_in_length)
			    {
				    free(rls_seg->data[i]->transport_in_length);
				    rls_seg->data[i]->transport_in_length = NULL;
			    }
			    if (rls_seg->data[i]->transport_out)
			    {
				    free(rls_seg->data[i]->transport_out);
				    rls_seg->data[i]->transport_out = NULL;
			    }
			}
		    }
		}
	    }
	    rls_seg = rls_seg->down;
	}
    }
    
    free(source_seg->current->dam.transport);
    source_seg->current->dam.transport = NULL;

	return 0;
}


/* remove transportation from all dams in river.  This routine may
 * be called from outside this module.
 */

int remove_all_transport()
{
    struct river_segment *seg;

    for (seg = river_mouth; seg; seg = seg->next)
	if (seg->seg_type == DAM_SEG && seg->current->dam.transport)
	    remove_transport(seg);

	return 0;
}
