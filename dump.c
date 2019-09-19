/* dump.c - dump some raw data to the message window and summary.dat */

#include <stdio.h>
#include "crisp.h"
#include "log.h"
#include "dump.h"
#include "output_flags.h"


/* utility proc to dump a string of data both to the message window
 * and the designated file.  returns 0 on failure.
 */
static int dump_str(  FILE *fp,  char *str)
{
    log_msg(L_DUMP,  "%s", str);
    if (fp && !fprintf(fp, " %s ", str))
	return 0;
    else
	return 1;
}

/* one time dump of header.  prints release names.  returns 0 on error.
 */
static int dump_header( FILE *fp)
{
    struct release *rls = current->release_list;

    /* only bother checking for write failure on the first output */
    if (!dump_str(fp, "\nData dump for the following (all) releases:\n")) {
	log_msg(L_ERROR, "problem writing summary data raw dump\n");
	return 0;
    }

    while (rls) {
	dump_str(fp, rls->site->name);
	dump_str(fp, "\n");
	rls = rls->next;
    }
    return 1;
}

/* walk all releases and call the dump proc for each.
 * params:  outfile, river segment, proc pointer to get proper data.
 */
static void dump_releases( FILE *fp, struct river_segment *seg, float (*getdata_proc)( /* struct rls_seg *rseg */ ))
{
    struct release *rls = current->release_list;

    while (rls) {
	struct rls_seg *rls_seg = rls->rls_seg;
	while (rls_seg && rls_seg->river_segment != seg)
	    rls_seg = rls_seg->down;
	if (rls_seg) {
	    char msg[32];
	    float data = (*getdata_proc)(rls_seg);
	    sprintf(msg, "%.0f", data);
	    dump_str(fp, msg);
	}
	else
	    dump_str(fp, "0");

	rls = rls->next;
    }
}

/* some getdata procs to pluck a piece of data from a rls_seg */

float get_passage(  struct rls_seg *rls_seg)
{   return rls_seg->data[0]->stats.total.total;  }

float get_mean_day(  struct rls_seg *rls_seg)
{   return rls_seg->data[0]->stats.total.mean;  }

#ifdef ROUTE_TRACKING
float get_sluiceway_in(  struct rls_seg *rls_seg)
{   return rls_seg->data[0]->stats.sluiceway_in_total;  }

float get_bypass_in(  struct rls_seg *rls_seg)
{   return rls_seg->data[0]->stats.bypass_in_total;  }

float get_turbine_in(  struct rls_seg *rls_seg)
{   return rls_seg->data[0]->stats.turbine_in_total;  }

float get_spillway_in( struct rls_seg *rls_seg)   
{   return rls_seg->data[0]->stats.spillway_in_total;  }
float get_rsw_in( struct rls_seg *rls_seg)   
{   return rls_seg->data[0]->stats.rsw_in_total;  }

float get_transport_in( struct rls_seg *rls_seg)   
{   return rls_seg->data[0]->stats.premort_transport_total;  }

float get_sluiceway_out(struct rls_seg *rls_seg)  
{   return rls_seg->data[0]->stats.sluiceway_total;  }

float get_bypass_out(struct rls_seg *rls_seg)  
{   return rls_seg->data[0]->stats.bypass_total;  }

float get_turbine_out(struct rls_seg *rls_seg)
{   return rls_seg->data[0]->stats.turbine_total;  }

float get_spillway_out(struct rls_seg *rls_seg)   
{   return rls_seg->data[0]->stats.spillway_total;  }
float get_rsw_out(struct rls_seg *rls_seg)   
{   return rls_seg->data[0]->stats.rsw_total;  }

float get_transport_out(struct rls_seg *rls_seg)   
{   return rls_seg->data[0]->stats.postmort_transport_total;  }

#endif /* ROUTE TRACKING */

/* dump routines for various passage information.
 */
static void passage_dump( FILE *fp,  struct river_segment *seg)
{
    dump_str(fp, "\nTotal Passage\n");
    dump_releases(fp, seg, get_passage);
    dump_str(fp, "\nMean Passage Day\n");
    dump_releases(fp, seg, get_mean_day);
    dump_str(fp, "\n");
}

#ifdef ROUTE_TRACKING
static void sluiceway_dump( FILE *fp,  struct river_segment *seg)
{
    dump_str(fp, "\nSluiceway Input\n");
    dump_releases(fp, seg, get_sluiceway_in);
    dump_str(fp, "\nSluiceway Output\n");
    dump_releases(fp, seg, get_sluiceway_out);
    dump_str(fp, "\n");
}

static void bypass_dump( FILE *fp,  struct river_segment *seg)
{
    dump_str(fp, "\nBypass Input\n");
    dump_releases(fp, seg, get_bypass_in);
    dump_str(fp, "\nBypass Output\n");
    dump_releases(fp, seg, get_bypass_out);
    dump_str(fp, "\n");
}

static void turbine_dump( FILE *fp,  struct river_segment *seg)
{
    dump_str(fp, "\nTurbine Input\n");
    dump_releases(fp, seg, get_turbine_in);
    dump_str(fp, "\nTurbine Output\n");
    dump_releases(fp, seg, get_turbine_out);
    dump_str(fp, "\n");
}

static void spillway_dump( FILE *fp,  struct river_segment *seg)
{
    dump_str(fp, "\nSpillway Input\n");
    dump_releases(fp, seg, get_spillway_in);
    dump_str(fp, "\nSpillway Output\n");
    dump_releases(fp, seg, get_spillway_out);
    dump_str(fp, "\nRSW Input\n");
    dump_releases(fp, seg, get_rsw_in);
    dump_str(fp, "\nRSW Output\n");
    dump_releases(fp, seg, get_rsw_out);
    dump_str(fp, "\n");
}

static void transport_dump( FILE *fp, struct river_segment *seg)
{
    dump_str(fp, "\nPremort Transport\n");
    dump_releases(fp, seg, get_transport_in);
    dump_str(fp, "\nPostmort Transport\n");
    dump_releases(fp, seg, get_transport_out);
    dump_str(fp, "\n");
}
#endif 
/* ROUTE_TRACKING */

/* recursive routine to do a summary data dump.  for any river segment
 * with some sort of passage hist displayed, that type of passage data
 * will be dumped for all releases.  returns 0 on failure.  takes the
 * name of an open output summary file and current river segment as
 * parameters.
 */
static int summary_dump_seg( FILE *fp,  struct river_segment *seg)
{
    static int header_flag = 0;	/* has the header been dumped yet? */

    /* dont bother if the dump flag isn't set or if no releases */
    if (!log_raw || !current->release_list)
	return 1;
       
    if (seg == river_mouth)	/* reset at the river mouth */
	header_flag = 0;
    
    /* use recursion to generate the proper ordering */
    if (seg->up) {
	if (!summary_dump_seg(fp, seg->up))
	    return 0;
    }
    if (seg->fork) {
	if (!summary_dump_seg(fp, seg->fork))
	    return 0;
    }

	/* ZApp app uses seg->output_settings to control dump. */
	if (seg->output_settings != 0)
    {
	char msg[1024];

	/* dump the header.  check here for file write failure */
	if (!header_flag) {
	    header_flag = 1;
	    if (!dump_header(fp))
		return 0;	/* return on failure */
	}
	sprintf(msg, "\nRaw Dump for %s\n", seg->seg_name);
	dump_str(fp, msg);

	/* ZApp app uses seg->output_settings to control dump. */
	if ((seg->seg_type == DAM_SEG || seg->seg_type == REACH_SEG) && 
				(seg->output_settings & PASSAGE_FLAG) != 0)
	    passage_dump(fp, seg);
#ifdef ROUTE_TRACKING
	if (seg->seg_type == DAM_SEG && 
				(seg->output_settings & ROUTING_FLAG) != 0)
	    sluiceway_dump(fp, seg);
	if (seg->seg_type == DAM_SEG && 
				(seg->output_settings & ROUTING_FLAG) != 0)
	    bypass_dump(fp, seg);
	if (seg->seg_type == DAM_SEG && 
				(seg->output_settings & ROUTING_FLAG) != 0)
	    turbine_dump(fp, seg);
	if (seg->seg_type == DAM_SEG && 
				(seg->output_settings & ROUTING_FLAG) != 0)
	    spillway_dump(fp, seg);
	if (seg->seg_type == DAM_SEG && 
				(seg->output_settings & TRANSPORT_FLAG) != 0)
	    transport_dump(fp, seg);
#endif /*ROUTE_TRACKING*/
    }
    return 1;
	/* success */
}

/** \brief Top level routine to do a full summary dump */

int summary_dump(  FILE *fp)
{
    return summary_dump_seg(fp, river_mouth);
}
