#include <stdio.h>
#include <stdlib.h>

#include "crisp.h"
#include "log.h"
#include "batch.h"
#include "realtime.h"
#include "release.h"

/* these items defined in "release_frame.c" */
extern int 		release_mod_flag,
		release_exist_flag;

/* This section contains the function definitions for creating,
 * destroying, and managing releases.  These fns must be used for
 * proper memory management to occur.
 */

/* allocate a release and fill with default data.
 */
struct release *construct_release()
{
    struct release *rls = (struct release *) malloc(sizeof(struct release));
    if (!rls)
	return 0;
#ifdef REALTIME
    rls->rt = 0;
#endif
    copy_release(0, rls);
    return rls;
}

/* copy the data from one release to another.  if "from" is null, fill
 * the "new" with default data.
 */

void copy_release(old, new)
	struct release *old, *new;
{
    if (old) {
#ifdef REALTIME
	if (new->rt)
	    destroy_rtinfo(&new->rt);
#endif
	
	bcopy ((char *) old, (char *) new, sizeof(struct release));

#ifdef REALTIME
	if (old->rt) {
	    if (!(new->rt = clone_rtinfo(old->rt)))
		log_msg(L_FATAL, "out of mem for new realtime data!\n");
	}
#endif	
    }
    else {
#ifdef REALTIME
	if (new->rt)
	    destroy_rtinfo(&new->rt);
#endif	    
	bzero((char *) new, sizeof(struct release));
	new->site = release_site;
	new->number[0] = 100000.0;
	new->species = &specs[0];
	new->stock = &new->species->stock;

	/* Copy from default values */
	new->start_date = release_defaults.start_date;
	new->smolt_onset = release_defaults.smolt_onset;
	new->smolt_finish = release_defaults.smolt_finish;
	new->length = release_defaults.length;
	new->vitality_at_release = release_defaults.vitality_at_release;
	new->vitality_change_rate = release_defaults.vitality_change_rate;
	new->initial_spill_experience = release_defaults.initial_spill_experience;
    }
}

/* free all data associated with a release.  external modules other than
 * "release_frame.c" should use delete_release() below.
 */

void free_release(rls)
    struct release *rls;
{
#ifdef REALTIME
    destroy_rtinfo(&rls->rt);
#endif	    
    free(rls);
}

/* dump all releases in the system. */

void remove_all_releases()
{
	while(current->release_list) {
		delete_release(current->release_list);
	}
}

/* insert an allocated release into the system release list.  construct
 * subsidary data areas (e.g. rls_seg) as necessary.
 */
void activate_release(rls)
	struct release *rls;
{
    struct rls_seg *rls_seg;
    struct river_segment *river_seg;
    void update_release_frame();
    void add_to_menu();
	rls->next = current->release_list;
    if (rls->next) {
	rls->next->prev = rls;
    }
    current->release_list = rls;
    rls->prev = NULL; 
    rls_seg = (struct rls_seg *)0;

    for (river_seg = rls->site->seg; river_seg; river_seg = river_seg->down) {
	if (rls_seg) {
	    if (! (rls_seg->down =
		   (struct rls_seg *)malloc(sizeof(struct rls_seg)))) {
		log_msg(L_ERROR, "no rls seg mem for release %s\n",
			rls->site->name);
		return;
	    }
	    bzero((char *)rls_seg->down, sizeof(struct rls_seg));
	    rls_seg->down->up = rls_seg;
	    rls_seg = rls_seg->down;
	} else {
	    if (! (rls_seg =
		   (struct rls_seg *)malloc(sizeof(struct rls_seg)))) {
		log_msg(L_ERROR, "no rls seg mem for release %s\n",
			rls->site->name);
		return;
	    }		
	    bzero((char *)rls_seg, sizeof(struct rls_seg));
	    rls_seg->up = (struct rls_seg *)0;
	    rls->rls_seg = rls_seg;
	}
	rls_seg->data = NULL;
	rls_seg->down = (struct rls_seg *)0;
	rls_seg->river_segment = river_seg;
    }
}

void delete_release(rls)
	struct release *rls;
{
	struct rls_seg *rls_seg;
	struct release *find_release();
	void update_release_values();
	void set_release_label();
	void set_frame_species();
	void update_release_frame();
	void add_to_menu();
	
	if (rls->prev)
		rls->prev->next = rls->next;
	else
		current->release_list = rls->next;

	if (rls->next)
		rls->next->prev = rls->prev;
	while ((rls_seg = rls->rls_seg) != NULL) {
	    if (rls_seg->data) {
		int i = 0;
		while (rls_seg->data[i]) {
		    if (rls_seg->data[i] != (struct rls_seg_data *) -1)
		    {
			if (rls_seg->data[i]->output)
			    free(rls_seg->data[i]->output);
			if (rls_seg->data[i]->length)
			    free(rls_seg->data[i]->length);
			if (rls_seg->data[i]->transport_in)
			    free(rls_seg->data[i]->transport_in);
			if (rls_seg->data[i]->transport_in_length)
			    free(rls_seg->data[i]->transport_in_length);
			if (rls_seg->data[i]->transport_out)
			    free(rls_seg->data[i]->transport_out);
#ifdef ROUTE_TRACKING
			if (rls_seg->data[i]->project_in)
			    free(rls_seg->data[i]->project_in);
			if (rls_seg->data[i]->turbine_in)
			    free(rls_seg->data[i]->turbine_in);
			if (rls_seg->data[i]->sluiceway_in)
			    free(rls_seg->data[i]->sluiceway_in);
			if (rls_seg->data[i]->bypass_in)
			    free(rls_seg->data[i]->bypass_in);
			if (rls_seg->data[i]->spillway_in)
			    free(rls_seg->data[i]->spillway_in);
			if (rls_seg->data[i]->rsw_in)
			    free(rls_seg->data[i]->rsw_in);

			if (rls_seg->data[i]->sluiceway_pass)
			    free(rls_seg->data[i]->sluiceway_pass);
			if (rls_seg->data[i]->bypass_pass)
			    free(rls_seg->data[i]->bypass_pass);
			if (rls_seg->data[i]->turbine_pass)
			    free(rls_seg->data[i]->turbine_pass);
			if (rls_seg->data[i]->spillway_pass)
			    free(rls_seg->data[i]->spillway_pass);
			if (rls_seg->data[i]->rsw_pass)
			    free(rls_seg->data[i]->rsw_pass);
			if (rls_seg->data[i]->transport_pass)
			    free(rls_seg->data[i]->transport_pass);
			if (rls_seg->data[i]->transport_arrived)
			    free(rls_seg->data[i]->transport_arrived);
#endif			
			if (rls_seg->data[i]->d) {
				delayed_mortality_table_free(rls_seg->data[i]->d);
				rls_seg->data[i]->d = NULL;
			}
			free(rls_seg->data[i]);
		    }
		    i++;
		}
		free(rls_seg->data);
	    }
	    rls->rls_seg = rls_seg->down;
	    free(rls_seg);
	}
	if (rls->stats.d) {
		delayed_mortality_table_free(rls->stats.d);
		rls->stats.d = NULL;
	}

    free_release(rls);
}

/* allocate the game pointers for this release for the specified number
 * of games.  make sure the list of pointers to rls_seg_data items
 * [ngames] is big enough.
 *	return: 0 on error
 *		1 on success
 */

int alloc_seg_data(release, games)
    struct release *release;
    int games;
{
    struct rls_seg *rls_seg = release->rls_seg;
    int i;

    while (rls_seg)
    {
	if (! rls_seg->data)		/* alloc the game pointer list,
					   null terminated */
	{
	    if ( ! (rls_seg->data =
		(struct rls_seg_data **)
		    malloc(sizeof(struct rls_seg_data *) * (games+1))))
	    {
		log_msg(L_FATAL, "no mem for data ptrs for release %s\n",
			release->site->name);
		return(0);
	    }
	    /* use -1 as place holder, NULL as terminator */
	    for (i = 0; i < games; i++)
		rls_seg->data[i] = (struct rls_seg_data *) -1;
	    rls_seg->data[games] = NULL;
	}
	else
	{
	    /* more game pointers needed? */
	    i = 0;
	    while (i < games && rls_seg->data[i] != NULL)
		i++;

	    if (i < games)
	    {
		struct rls_seg_data **tmp;
		
		if (! (tmp = (struct rls_seg_data **)
		       realloc(rls_seg->data,
			       sizeof(struct rls_seg_data *) * (games+1))))
		{
		    log_msg(L_FATAL,
			    "cannot realloc data ptrs for release %s\n",
			    release->site->name);
		    return(0);
		}
		rls_seg->data = tmp;
		for ( ; i < games; i++)
		    rls_seg->data[i] = (struct rls_seg_data *) -1;
		rls_seg->data[games] = NULL;
	    }
	}
	
    	/* alloc data segs for all games if needed */
	for (i=0; i < games; i++)
	{
	    if (rls_seg->data[i] == (struct rls_seg_data *) -1)
	    {
		if ( !(rls_seg->data[i] = (struct rls_seg_data *)
		       malloc(sizeof(struct rls_seg_data))))
		{
		    log_msg(L_FATAL,
			    "no mem for game %d data for release %s\n",
			    i, release->site->name);
		    return(0);
		}
		else
		{
		    rls_seg->data[i]->output = NULL;
		    rls_seg->data[i]->length = NULL;
		    rls_seg->data[i]->dam_spill = NULL;
		    rls_seg->data[i]->output_transport = NULL;
		    rls_seg->data[i]->delayed_survival_rate = NULL;
		    rls_seg->data[i]->transport_out = NULL;
		    rls_seg->data[i]->transport_in = NULL;
		    rls_seg->data[i]->transport_in_length = NULL;
#ifdef ROUTE_TRACKING
		    rls_seg->data[i]->project_in = NULL;
		    rls_seg->data[i]->turbine_in = NULL;
		    rls_seg->data[i]->sluiceway_in = NULL;
		    rls_seg->data[i]->bypass_in = NULL;
		    rls_seg->data[i]->spillway_in = NULL;
		    rls_seg->data[i]->rsw_in = NULL;

		    rls_seg->data[i]->sluiceway_pass = NULL;
		    rls_seg->data[i]->bypass_pass = NULL;
		    rls_seg->data[i]->turbine_pass = NULL;
		    rls_seg->data[i]->spillway_pass = NULL;
		    rls_seg->data[i]->rsw_pass = NULL;
		    rls_seg->data[i]->transport_pass = NULL;
		    rls_seg->data[i]->transport_arrived = NULL;
#endif
		    rls_seg->data[i]->d = NULL;
		}
	    }
	}
	rls_seg = rls_seg->down;
    }
    return(1);
}


/* clear the data area for this release and game.  call alloc_seg_data to
 * realloc the game pointers if necessary.  rls_seg_data and the output array
 * will be allocated here if necessary.
 *	return: 0 on error
 *		1 on success
 */


int clear_seg_data(release, game)
    struct release *release;
    int game;
{
    struct rls_seg *rls_seg = release->rls_seg;

    /* alloc/realloc the game pointers if necessary, being sure to pass
     * in the number of games, which is game+1, since game starts at 0
     */
    if (! alloc_seg_data(release, game+1))
	return(0);

    while (rls_seg)
    {
	/* zero data areas.  */
	rls_seg->data[game]->nopass = 0;
	bzero((char *)&rls_seg->data[game]->stats, sizeof(struct rls_stats));
	if ( !rls_seg->data[game]->output)
	    if ( !(rls_seg->data[game]->output = (float *) malloc(STEPS_IN_RUN * sizeof(float))))
	    {
		log_msg(L_DBGMSG, "Out of heap memory...realloc'ing\n");
		if (!(rls_seg->data[game]->output = (float *) malloc(STEPS_IN_RUN * sizeof(float)))) {
		    log_msg(L_FATAL, "No memory for output heap!\n");
		    return(0);
		}
	    }	    
	if ( !rls_seg->data[game]->length)
	    if ( !(rls_seg->data[game]->length = (float *) malloc(STEPS_IN_RUN * sizeof(float))))
	    {
		log_msg(L_DBGMSG, "Out of heap memory...realloc'ing\n");
		if (!(rls_seg->data[game]->length = (float *) malloc(STEPS_IN_RUN * sizeof(float)))) {
		    log_msg(L_FATAL, "No memory for length heap!\n");
		    return(0);
		}
	    }	    
	if ( !rls_seg->data[game]->dam_spill)
	    if ( !(rls_seg->data[game]->dam_spill = (float *) malloc(STEPS_IN_RUN * sizeof(float))))
	    {
		log_msg(L_DBGMSG, "Out of heap memory...realloc'ing\n");
		if (!(rls_seg->data[game]->dam_spill = (float *) malloc(STEPS_IN_RUN * sizeof(float)))) {
		    log_msg(L_FATAL, "No memory for dam_spill heap!\n");
		    return(0);
		}
	    }	    
	if ( !rls_seg->data[game]->output_transport)
	    if ( !(rls_seg->data[game]->output_transport = (float *) malloc(STEPS_IN_RUN * sizeof(float))))
	    {
		log_msg(L_DBGMSG, "Out of heap memory...realloc'ing\n");
		if (!(rls_seg->data[game]->output_transport = (float *) malloc(STEPS_IN_RUN * sizeof(float)))) {
		    log_msg(L_FATAL, "No memory for output_transport heap!\n");
		    return(0);
		}
	    }	    
	bzero((char *)rls_seg->data[game]->output,
	      sizeof(float) * STEPS_IN_RUN);
    bzero((char *)rls_seg->data[game]->length,
		  sizeof(float) * STEPS_IN_RUN);
    bzero((char *)rls_seg->data[game]->dam_spill,
		  sizeof(float) * STEPS_IN_RUN);
    bzero((char *)rls_seg->data[game]->output_transport,
		  sizeof(float) * STEPS_IN_RUN);
	if (rls_seg->data[game]->transport_out)
	    bzero((char *)rls_seg->data[game]->transport_out,
		  sizeof(float) * STEPS_IN_RUN);
	if (rls_seg->data[game]->transport_in)
	    bzero((char *)rls_seg->data[game]->transport_in,
		  sizeof(float) * STEPS_IN_RUN);
	if (rls_seg->data[game]->transport_in_length)
	    bzero((char *)rls_seg->data[game]->transport_in_length,
		  sizeof(float) * STEPS_IN_RUN);
#ifdef ROUTE_TRACKING
	if (rls_seg->data[game]->project_in)
	    bzero((char *)rls_seg->data[game]->project_in,
		  sizeof(float) * STEPS_IN_RUN);
	if (rls_seg->data[game]->turbine_in)
	    bzero((char *)rls_seg->data[game]->turbine_in,
		  sizeof(float) * STEPS_IN_RUN);
	if (rls_seg->data[game]->sluiceway_in)
	    bzero((char *)rls_seg->data[game]->sluiceway_in,
		  sizeof(float) * STEPS_IN_RUN);
	if (rls_seg->data[game]->bypass_in)
	    bzero((char *)rls_seg->data[game]->bypass_in,
		  sizeof(float) * STEPS_IN_RUN);
	if (rls_seg->data[game]->spillway_in)
	    bzero((char *)rls_seg->data[game]->spillway_in,
		  sizeof(float) * STEPS_IN_RUN);
	if (rls_seg->data[game]->rsw_in)
	    bzero((char *)rls_seg->data[game]->rsw_in,
		  sizeof(float) * STEPS_IN_RUN);

	if (rls_seg->data[game]->sluiceway_pass)
	    bzero((char *)rls_seg->data[game]->sluiceway_pass,
		  sizeof(float) * STEPS_IN_RUN);
	if (rls_seg->data[game]->bypass_pass)
	    bzero((char *)rls_seg->data[game]->bypass_pass,
		  sizeof(float) * STEPS_IN_RUN);
	if (rls_seg->data[game]->turbine_pass)
	    bzero((char *)rls_seg->data[game]->turbine_pass,
		  sizeof(float) * STEPS_IN_RUN);
	if (rls_seg->data[game]->spillway_pass)
	    bzero((char *)rls_seg->data[game]->spillway_pass,
		  sizeof(float) * STEPS_IN_RUN);
	if (rls_seg->data[game]->rsw_pass)
	    bzero((char *)rls_seg->data[game]->rsw_pass,
		  sizeof(float) * STEPS_IN_RUN);
	if (rls_seg->data[game]->transport_pass)
	    bzero((char *)rls_seg->data[game]->transport_pass,
		  sizeof(float) * STEPS_IN_RUN);
	if (rls_seg->data[game]->transport_arrived)
	    bzero((char *)rls_seg->data[game]->transport_arrived,
		  sizeof(float) * STEPS_IN_RUN);

	if (rls_seg->data[game]->stats.d) {
		delayed_mortality_table_free(rls_seg->data[game]->stats.d);
		rls_seg->data[game]->stats.d = NULL;
	}
#endif	
	rls_seg = rls_seg->down;
    }
    return(1);
}	

struct release *find_release(site, species)
	struct release_site *site;
	struct species *species;
{
	struct release *rls;

	for (rls = current->release_list ; rls ; rls = rls->next) {
		if (site && rls->site != site) continue;
		if (species && rls->species != species) continue;
		return rls;
	}
	return 0;
}

