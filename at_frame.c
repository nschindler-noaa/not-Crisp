/* at_frame.c
 *
 * Analysis tool histogram thingie.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include "crisp.h"
#include "log.h"
#include "struct_at.h"
#include "tree.h"

#ifndef MAXPATHLEN
#define MAXPATHLEN (1024)
#endif

/*
 * summary_day_comparison - Compare two summary pointers by comparing
 *	their travel_time field.
 */
int summary_day_comparison(val1, val2)
	struct summary *val1, *val2;
{
    if (val2->travel_time < val1->travel_time) return -1;
    else return (val2->travel_time > val1->travel_time);
}

/*
 * summary_total_comparison - Compare two summary pointers by comparing
 *	their total field.
 */
int summary_total_comparison(val1, val2)
	struct summary *val1, *val2;
{
    if (val2->info->total.total < val1->info->total.total) return -1;
    else return (val2->info->total.total > val1->info->total.total);
}

/*
 * void summary_struct_free(struct summary *ptr) -- Free a dynamically
 *	allocated summary struct.
 */
static void summary_struct_free(ptr)
	struct summary *ptr;
{
    free(ptr->info);
    free(ptr);
}

/*
 * get_data - A routine to create the appropriate array to use in the
 *	histogram.  It builds a list of segments with name seg_name from
 *	the active list of releases cur_rls, and sums the data for
 *	all games from those releases, averaging all data except total
 *	passage over the number of games.  It then returns this sum in
 *	a malloc'ed array of length games.
 *
 *	Also returns the number of items in the array, and the total
 *	number of fish released over all releases.
 */
struct summary *get_data(games, cur_rls, seg_name, flags, total, num)
    int games;
    struct rls *cur_rls;
    char *seg_name;
    int flags;
    float *total;
    int *num;
{

    TREE *tree;
    struct rls *rls;
    struct river_seg_info *seg, *seg_list, *nseg;
    struct rls_stats *ptr;
    struct summary *sum;
    float travel_time = 0.0;
    int i, segment_count;

    if (!(cur_rls && seg_name)) return 0;
    seg_list = (struct river_seg_info *) 0;
    /* Everything is in order.  Do initialization. */
    *total = 0.0;
    *num = games;
    for (rls = cur_rls, segment_count = 0 ; rls ; rls = rls->next) {
	for (seg = rls->river_segment; seg ; seg = seg->next) {
	    if (strcmp(seg_name, seg->name)) continue;
	    *total += rls->hist_summary.total.total;
	    travel_time += rls->hist_summary.total.median;

	    /* Freed at the end of this function */
	    if (!(nseg = (struct river_seg_info *)
		malloc(sizeof(struct river_seg_info)))) {
		log_msg(L_ERROR, "Can't allocate memory in get_data!\n");
		break;
	    }
	    bcopy(seg, nseg, sizeof(struct river_seg_info));
	    nseg->next = seg_list;
	    seg_list = nseg;
	    ++segment_count;
	    break;
	}
    }
    if (!segment_count) {
	log_msg(L_MSG, "Analysis: No releases selected!\n");
	return (struct summary *) 0;
    }
    travel_time /= segment_count;
    tree = tree_init((flags & AT_TRAVEL_FLAG) ? summary_day_comparison
		: summary_total_comparison, 0, TRUE, sizeof(struct summary));
    for (i = 0 ; i < *num ; i++) {

	if (!(ptr = (struct rls_stats *) malloc(sizeof(struct rls_stats)))) {
	    log_msg(L_ERROR, "Can't allocate memory for get_data!\n");
	    break;
	}
	/* Freed in tree_free */
	if (!(sum = (struct summary *) malloc(sizeof(struct rls_stats)))) {
	    log_msg(L_ERROR, "Can't allocate memory for summary!\n");
	    free(ptr);
	    break;
	}
	bzero(ptr, sizeof(struct rls_stats));
	for (seg = seg_list ; seg ; seg=seg->next) {
	    ptr->total.total 		 += seg->summaries[i].total.total;

#ifdef ROUTE_TRACKING
	    ptr->sluiceway_total	 += seg->summaries[i].sluiceway_total;
	    ptr->bypass_total	 += seg->summaries[i].bypass_total;
	    ptr->turbine_total	 += seg->summaries[i].turbine_total;
	    ptr->spillway_total	 += seg->summaries[i].spillway_total;
	    ptr->rsw_total	 += seg->summaries[i].rsw_total;
	    ptr->postmort_transport_total +=
		seg->summaries[i].postmort_transport_total;
	    ptr->premort_transport_total +=
		seg->summaries[i].premort_transport_total;
	    ptr->turbine_in_total  += seg->summaries[i].turbine_in_total;
	    ptr->spillway_in_total += seg->summaries[i].spillway_in_total;
	    ptr->rsw_in_total += seg->summaries[i].rsw_in_total;
	    ptr->sluiceway_in_total   += seg->summaries[i].sluiceway_in_total;
	    ptr->bypass_in_total   += seg->summaries[i].bypass_in_total;
	    ptr->project_in_total  += seg->summaries[i].project_in_total;
#endif	    

	    /* This average should not be weighted. */
	    ptr->avg_flow	 += seg->summaries[i].avg_flow;

	    /* All of these means need to be weighted by number of fish
	     * in order to eliminate problems with zero passage games.
	     */
	    ptr->total.median		 += seg->summaries[i].total.median * seg->summaries[i].total.total;
	    ptr->total.first		 += (int) (seg->summaries[i].total.first * seg->summaries[i].total.total);
	    ptr->total.last		 += (int) (seg->summaries[i].total.last * seg->summaries[i].total.total);
	    ptr->total.mean  		 += seg->summaries[i].total.mean * seg->summaries[i].total.total;
	    ptr->total.mode  		 += seg->summaries[i].total.mode * seg->summaries[i].total.total;
	    ptr->total.std_dev	 += seg->summaries[i].total.std_dev * seg->summaries[i].total.total;
	}
	ptr->avg_flow /= (float) segment_count;

	if (ptr->total.total) {
	    ptr->total.first    /= (int) ptr->total.total;
	    ptr->total.last     /= (int) ptr->total.total;
	    ptr->total.mean     /= ptr->total.total;
	    ptr->total.median   /= ptr->total.total;
	    ptr->total.mode     /= ptr->total.total;
	    ptr->total.std_dev  /= ptr->total.total;
	} else {
	    ptr->total.first = ptr->total.last = 0;
		ptr->total.mean = ptr->total.median = ptr->total.mode	= ptr->total.std_dev = 0.0;
	}
	sum->info = ptr;
	sum->travel_time = ptr->total.median - travel_time;
	if (tree_insert(tree, sum) < 0) {
	    log_msg(L_ERROR, "Error inserting value into tree.\n");
		summary_struct_free(sum);
	}
    }
    seg = seg_list;
    while (seg) {
	nseg = seg;
	seg = seg->next;
	free(nseg);
    }

    /* Freed in split_data */
    sum = (struct summary *) tree_array(tree);
    tree_free(tree);
    return sum;
}

/*
 * split_data - Divide the summary array into two separate arrays.
 */
int split_data(in_ptr, size, out_ptr, out_alt)
	struct summary *in_ptr;
	int size;
	struct rls_stats **out_ptr;
	float **out_alt;
{
    struct rls_stats *ptr;
    float *alt;

    if (!(*out_ptr=(struct rls_stats *) malloc(size*sizeof(struct rls_stats))))
	return 0;
    else if (out_alt && !(*out_alt=(float *) malloc(size * sizeof(float)))) {
	free(*out_ptr);
	*out_ptr = 0;
	return 0;
    }

    /* Loop replicated for efficiency purposes: this can be a big loop */

    if (!out_alt) {
	for (ptr = *out_ptr ; size != 0 ; --size, ++ptr, ++in_ptr)
	    bcopy(in_ptr->info, ptr, sizeof(struct rls_stats));
    } else {
	alt = *out_alt;
	for (ptr = *out_ptr ; size != 0 ; --size, ++ptr, ++in_ptr, ++alt) {
	    bcopy(in_ptr->info, ptr, sizeof(struct rls_stats));
	    *alt = in_ptr->travel_time;
	}
    }
    return 1;
}

