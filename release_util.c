/** \file 
 * \brief  Some utils associated with releases. */

#include <string.h>
#include <stdlib.h>

#include "list.h"
#include "crisp.h"
#include "release_util.h"

#ifdef WIN32
#include <malloc.h>
#endif

#ifdef REALTIME

static void sum_float_vec();	

/** free a release association list, whose format is described above. */

void free_assoc_list(assoc)
    LIST *assoc;
{
    LIST *ptr = assoc;
    while (ptr) {
	free_list((LIST *) ptr->item, NULL);
	ptr = ptr->next;
    }
    free_list(assoc, NULL);
}

/** take a list of releases and a target segment, and collapses the
 * release data.  result is a pointer to a statically defined
 * rls_seg_data, or NULL if these releases do not pass the requested
 * segment.  that means the results must be used before the next call
 * to this routine.  also note that the rls_seg_data returned is
 * unusual in that the arrays are of size STEPS_IN_SEASON instead of
 * the usual size STEPS_IN_RUN, and they begin at julian day 0.
 * finally, the earliest day of data and the release totals are
 * returned in the supplied parameters.
 */

struct rls_seg_data *collapse_release_data(rlsp, segment, first_day, rls_total)
    LIST *rlsp;
    struct river_segment *segment;
    int *first_day;
    float *rls_total;
{
   static struct rls_seg_data *data = 0;
   static struct rls_seg_data *result = 0;

   *first_day = DAYS_IN_SEASON;
   *rls_total = 0;
   if (!data) {
       /* data stores all the arrays.  result stores the pointers to
        * the subset relevant for this segment.
	*/
       data = (struct rls_seg_data *) malloc(sizeof(struct rls_seg_data));
       result = (struct rls_seg_data *) malloc(sizeof(struct rls_seg_data));

       bzero((char *)data, sizeof(struct rls_seg_data));
       data->output = (float *) malloc(sizeof(float) * STEPS_IN_SEASON);
       data->length = (float *) malloc(sizeof(float) * STEPS_IN_SEASON);
       data->dam_spill = (float *) malloc(sizeof(float) * STEPS_IN_SEASON);
       data->output_transport = (float *) malloc(sizeof(float) * STEPS_IN_SEASON);
       data->transport_out = (float *) malloc(sizeof(float) * STEPS_IN_SEASON);
       data->transport_in  = (float *) malloc(sizeof(float) * STEPS_IN_SEASON);
       data->transport_in_length
                  = (float *) malloc(sizeof(float) * STEPS_IN_SEASON);
#ifdef ROUTE_TRACKING       
       data->project_in    = (float *) malloc(sizeof(float) * STEPS_IN_SEASON);
       data->turbine_in    = (float *) malloc(sizeof(float) * STEPS_IN_SEASON);
       data->sluiceway_in     = (float *) malloc(sizeof(float) * STEPS_IN_SEASON);
       data->bypass_in     = (float *) malloc(sizeof(float) * STEPS_IN_SEASON);
       data->spillway_in   = (float *) malloc(sizeof(float) * STEPS_IN_SEASON);
       data->rsw_in   = (float *) malloc(sizeof(float) * STEPS_IN_SEASON);
       data->sluiceway_pass   = (float *) malloc(sizeof(float) * STEPS_IN_SEASON);
       data->bypass_pass   = (float *) malloc(sizeof(float) * STEPS_IN_SEASON);
       data->turbine_pass  = (float *) malloc(sizeof(float) * STEPS_IN_SEASON);
       data->spillway_pass = (float *) malloc(sizeof(float) * STEPS_IN_SEASON);
       data->rsw_pass = (float *) malloc(sizeof(float) * STEPS_IN_SEASON);
       data->transport_pass= (float *) malloc(sizeof(float) * STEPS_IN_SEASON);
       data->transport_arrived= (float *) malloc(sizeof(float) * STEPS_IN_SEASON);
#endif
   }
   bzero((char *)result, sizeof(struct rls_seg_data));
   bzero((char *)data->output, sizeof(float) * STEPS_IN_SEASON);
   bzero((char *)data->length, sizeof(float) * STEPS_IN_SEASON);
   bzero((char *)data->transport_in, sizeof(float) * STEPS_IN_SEASON);
   bzero((char *)data->transport_in_length, sizeof(float) * STEPS_IN_SEASON);
   bzero((char *)data->transport_out, sizeof(float) * STEPS_IN_SEASON);
#ifdef ROUTE_TRACKING
   bzero((char *)data->project_in, sizeof(float) * STEPS_IN_SEASON);
   bzero((char *)data->turbine_in, sizeof(float) * STEPS_IN_SEASON);
   bzero((char *)data->sluiceway_in, sizeof(float) * STEPS_IN_SEASON);
   bzero((char *)data->bypass_in, sizeof(float) * STEPS_IN_SEASON);
   bzero((char *)data->spillway_in, sizeof(float) * STEPS_IN_SEASON);
   bzero((char *)data->rsw_in, sizeof(float) * STEPS_IN_SEASON);
   bzero((char *)data->sluiceway_pass, sizeof(float) * STEPS_IN_SEASON);
   bzero((char *)data->bypass_pass, sizeof(float) * STEPS_IN_SEASON);
   bzero((char *)data->turbine_pass, sizeof(float) * STEPS_IN_SEASON);
   bzero((char *)data->spillway_pass, sizeof(float) * STEPS_IN_SEASON);
   bzero((char *)data->rsw_pass, sizeof(float) * STEPS_IN_SEASON);
   bzero((char *)data->transport_pass, sizeof(float) * STEPS_IN_SEASON);
   bzero((char *)data->transport_arrived, sizeof(float) * STEPS_IN_SEASON);
#endif

   for ( ; rlsp; rlsp = rlsp->next) {
       struct release *rls = (struct release *) rlsp->item;
       struct rls_seg *rseg;
       
       for (rseg = rls->rls_seg ;
	    rseg && rseg->river_segment != segment;
	    rseg = rseg->down)
		;
       if (!rseg)
	   return 0;
       else {
	   struct rls_seg_data *segdat = rseg->data[0];
	   int first_step = rls->start_date * STEPS_PER_DAY;
	   int num_steps = STEPS_IN_RUN;

	   if (first_step + num_steps > STEPS_IN_SEASON)
	       num_steps = STEPS_IN_SEASON - first_step;
	   
	   if (segdat->output) {
	       sum_float_vec(&data->output[first_step],
			     segdat->output, num_steps);
	       result->output = data->output;
	   }
       if (segdat->length)
           result->length = data->length;
       if (segdat->dam_spill)
           result->dam_spill = data->dam_spill;
       if (segdat->output_transport)
           result->output_transport = data->output_transport;
	   if (segdat->transport_in) {
	       sum_float_vec(&data->transport_in[first_step],
			     segdat->transport_in, num_steps);
	       result->transport_in = data->transport_in;
	   }
       if (segdat->transport_in_length)
	       result->transport_in_length = data->transport_in_length;
	   if (segdat->transport_out) {
	       sum_float_vec(&data->transport_out[first_step],
			     segdat->transport_out, num_steps);
	       result->transport_out = data->transport_out;
	   }
#ifdef ROUTE_TRACKING
	   if (segdat->project_in) {
	       sum_float_vec(&data->project_in[first_step],
			     segdat->project_in, num_steps);
	       result->project_in = data->project_in;
	   }
	   if (segdat->turbine_in) {
	       sum_float_vec(&data->turbine_in[first_step],
			     segdat->turbine_in, num_steps);
	       result->turbine_in = data->turbine_in;
	   }
	   if (segdat->sluiceway_in) {
	       sum_float_vec(&data->sluiceway_in[first_step],
			     segdat->sluiceway_in, num_steps);
	       result->sluiceway_in = data->sluiceway_in;
	   }
	   if (segdat->bypass_in) {
	       sum_float_vec(&data->bypass_in[first_step],
			     segdat->bypass_in, num_steps);
	       result->bypass_in = data->bypass_in;
	   }
	   if (segdat->spillway_in) {
	       sum_float_vec(&data->spillway_in[first_step],
			     segdat->spillway_in, num_steps);
	       result->spillway_in = data->spillway_in;
	   }
	   if (segdat->rsw_in) {
	       sum_float_vec(&data->rsw_in[first_step],
			     segdat->rsw_in, num_steps);
	       result->rsw_in = data->rsw_in;
	   }
	   if (segdat->sluiceway_pass) {
	       sum_float_vec(&data->sluiceway_pass[first_step],
			     segdat->sluiceway_pass, num_steps);
	       result->sluiceway_pass = data->sluiceway_pass;
	   }
	   if (segdat->bypass_pass) {
	       sum_float_vec(&data->bypass_pass[first_step],
			     segdat->bypass_pass, num_steps);
	       result->bypass_pass = data->bypass_pass;
	   }
	   if (segdat->turbine_pass) {
	       sum_float_vec(&data->turbine_pass[first_step],
			     segdat->turbine_pass, num_steps);
	       result->turbine_pass = data->turbine_pass;
	   }
	   if (segdat->spillway_pass) {
	       sum_float_vec(&data->spillway_pass[first_step],
			     segdat->spillway_pass, num_steps);
	       result->spillway_pass = data->spillway_pass;
	   }
	   if (segdat->rsw_pass) {
	       sum_float_vec(&data->rsw_pass[first_step],
			     segdat->rsw_pass, num_steps);
	       result->rsw_pass = data->rsw_pass;
	   }
	   if (segdat->transport_pass) {
	       sum_float_vec(&data->transport_pass[first_step],
			     segdat->transport_pass, num_steps);
	       result->transport_pass = data->transport_pass;
	   }
	   if (segdat->transport_arrived) {
	       sum_float_vec(&data->transport_arrived[first_step],
			     segdat->transport_arrived, num_steps);
	       result->transport_arrived = data->transport_arrived;
	   }
#endif
	   if (rls->start_date < *first_day)
	       *first_day = rls->start_date;
	   *rls_total += rls->total_released;
       }
   }
   return result;
}

/** routine to search a null-terminated "used" array of release
 * pointers for an existing entry.  returns 0 if no match found,
 * otherwise non-zero. */
static int rls_used(rls, used)
    struct release *rls, **used;
{
    struct release **ptr = used;

    while (*ptr) {
	if (*ptr++ == rls)
	    return 1;
    }
    return 0;
}

/** determine if two releases are a "match" from the realtime point of view */

static int rt_release_match(rls1, rls2)
    struct release *rls1, *rls2;
{
    if (rls1->site == rls2->site
	&& rls1->species == rls2->species
	&& rls1->stock == rls2->stock
	&& rls1->rt && rls2->rt 
	&& !strcmp(rls1->rt->name, rls2->rt->name)
	&& rls1->rt->data_date == rls2->rt->data_date)

	return 1;
    else
	return 0;
}

/** routine to add the contents of v2 into v1. */
static void sum_float_vec(v1, v2, len)
    float *v1, *v2;
    int len;
{
    int i;
    for (i=0; i < len; ++i)
	v1[i] += v2[i];
}

/** rt_associate_releases() - creates a list structure which associates
 * corresponding realtime releases.  uses the passed release list as
 * data.  returns a pointer to the head of the association list.
 *
 * the association list is a list where each node contains a pointer
 * to the head of another the list, the "release list".  each node of
 * the release list contains a pointer to a single release.  all of
 * the releases in a given release list are "associated" on the basis
 * of having been originally derived from a single release.
 *
 * the original release list (passed as parameter) is not altered by
 * this routine.
 */


LIST *rt_associate_releases(releases)
    struct release *releases;
{
    int num_rels = 0;
    int used_index = 0;
    struct release **used;
    struct release *cur_rls, *rls_cmp;
    LIST *assoc = 0;
    LIST *rlsptr, *assocptr;
    
    /* first count all the releases */
    cur_rls = releases;
    while (cur_rls) {
	++num_rels;
	cur_rls = cur_rls->next;
    }

    /* allocate null-terminated array of used release ptrs */
    used = (struct release **) malloc(sizeof(struct release *) * (num_rels+1));
    bzero((char *) used, sizeof(struct release *) * (num_rels+1));

    /* walk the release list and create the associations */
    for (cur_rls = releases; cur_rls; cur_rls = cur_rls->next) {
	if (rls_used(cur_rls, used))
	    continue;
	used[used_index++] = cur_rls;

	/* make a node for this release */
	rlsptr = make_list_entry();
	rlsptr->item = (void *) cur_rls;

	/* make a node for the assoc list and append it */
	assocptr = make_list_entry();
	assocptr->item = (void *) rlsptr;
	list_append(&assoc, assocptr);

	/* find other releases matching this one, and build the
         * release list.  note that we only need to look further
         * "down" the original release list
	 */
	for (rls_cmp = cur_rls->next; rls_cmp; rls_cmp = rls_cmp->next) {
	    if (rt_release_match(cur_rls, rls_cmp)) {
		/* update used list, make a release node and append it */
		used[used_index++] = rls_cmp;
		rlsptr = make_list_entry();
		rlsptr->item = (void *) rls_cmp;
		list_append((LIST **) &assocptr->item, rlsptr);
	    }
	}
    }
    free(used);
    return assoc;
}

#endif /*REALTIME*/    

