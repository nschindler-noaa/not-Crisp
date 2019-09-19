/** \file
 * Realtime function implementations */

#include "crisp.h"
#include "monte.h"
#include "run_mode.h"
#include "log.h"
#include "realtime.h"
#include "batch.h"
#include "token.h"
#include "c1tokens.h"
#include "fpe.h"
#include "nsat.h"
#include "release_util.h"
#include "dir.h"
#include <stdlib.h>

#ifdef WIN32
#include <direct.h>
#include <io.h>
#else
#include <sys/wait.h>
#include <sys/resource.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>
#endif

#include <errno.h>

#include <stdio.h>
#include <string.h>

#include "file.h"

/* PC needs this, on SUN it does no harm. */
#ifndef R_OK
#define R_OK 4
#endif

#ifdef REALTIME

void 			run_realtime();
void			realtime_monte_callback();
int			realtime_output_datafile();

static int		run_realtime_monte();
static void 		fput_special_array();
static void		fput_float_array();
static int		selected_segment();

/** the input pathname is a static local.  also provided is a fn to set it. */

static char *input_pathname = 0;
static char *datafile = 0;

/** set the input pathname to a new value by copying the string. */

void rt_set_input_pathname(path)
    char *path;
{
    if (input_pathname)
	free(input_pathname);
    input_pathname = strdup(path);
}

/** set the input datafile name to a new value */

void rt_set_datafile(name)
    char *name;
{
    if (datafile)
	free(datafile);
    datafile = strdup(name);
}

/** this routine runs the realtime, either in batch or user mode */	
void run_realtime(mode)
    int mode;			/* RT_SCENARIO or RT_MONTE (run_mode.h) */
{
    datafile = (datafile) ? datafile : REALTIME_DATAFILE;
    if (access(datafile, R_OK) < 0) {
	    fprintf(stderr, "Can't access data file \"%s\" for realtime run!\n",
		    datafile);
		return;
    }
    if (run_realtime_monte(datafile, mode) < 0) return;
}

/**
 *	Outputs:
 *        - daily passage
 *        - daily % survival
 *        - daily % transport
 *        - daily spill (%)
 *        - daily spill (kcfs)
 *        - daily fpe (fish passage efficiency)
 *        - daily flow (kcfs)
 *        - daily %nsat in forebay 
 *        - daily %nsat in tailrace
 */
int realtime_output_datafile(current_run)
	struct run	*current_run;
{
    char			outputpath[MAXPATHLEN];
    char			tempstore[256];
    FILE			*output;
    struct river_segment	*segment;
    LIST *assoc_list, *assocp;

    sprintf(outputpath, "%s/%s.%s", REALTIME_ALTERNATIVE, REALTIME_ALTERNATIVE,
	    REALTIME_OUTPUT_EXT);
    if (!(output = fopen(outputpath, "w")))
	return -1;

    assoc_list = rt_associate_releases(current_run->release_list);
    fprintf(output, "# Easy-parse Real-time Data (as used in hospitals)\n");
    fprintf(output, "%s %s\n", TKS_RT_REPORT_DATE, run[0].rt_report_date);

    for (segment = river_mouth ; segment ; segment = segment->next) {

	if (!selected_segment(/*segment*/))
	    continue;

	fprintf(output, "Segment %s\n",
		string_to_word(segment->seg_name, tempstore));

 	if (segment->seg_type == DAM_SEG) {
	    FlowLocation spill_side = DAMPTR(segment)->spill_side;
	    FlowLocation phouse_side =
		(spill_side == FLOW_LEFT) ? FLOW_RIGHT : FLOW_LEFT;

	    fput_special_array(output, "Spill_gas", 1, 0, DAM_TIME_SLC,
			       DAMPTR(segment)->gas_out->gas[spill_side], 1);
	    fput_special_array(output, "Phouse_gas", 1, 0, DAM_TIME_SLC,
			       DAMPTR(segment)->gas_out->gas[phouse_side], 1);
	    fput_special_array(output, "Temperature", 1, 0,
			       STEPS_PER_DAY, DAMPTR(segment)->water_temp, 1);
	}
	else if (segment->seg_type == REACH_SEG) {
	    fput_special_array(output, "Gas_left", 1, 0, DAM_TIME_SLC,
			       RCHPTR(segment)->gas_out->gas[FLOW_LEFT], 1);
	    fput_special_array(output, "Gas_right", 1, 0, DAM_TIME_SLC,
			       RCHPTR(segment)->gas_out->gas[FLOW_RIGHT], 1);
	    fput_special_array(output, "Temperature", 1, 0,
			       STEPS_PER_DAY, RCHPTR(segment)->water_temp, 1);
	}
	
/* 	else if (segment->seg_type == HEADWATER_SEG) */
/* 	    fput_float_array(output, "Nsat", 1, DAYS_IN_SEASON, */
/* 			     HDWPTR(segment)->nsat); */
	
	for (assocp = assoc_list; assocp; assocp = assocp->next) {
	    LIST *release_list = (LIST *) assocp->item;
	    int first_day;
	    float rls_total;
	    struct rls_seg_data *data
		= collapse_release_data(release_list, segment,
					&first_day, &rls_total);

	    /* grab the first release for header info */
	    struct release *release = (struct release *) release_list->item;

	    if (data) {
		fprintf(output, "\n\tRelease %s",
			string_to_word(release->site->name, tempstore));
		fprintf(output, " Species %s",
			string_to_word(release->species->name, tempstore));
		fprintf(output, " Stock %s\n",
			(release->rt) ?
			    string_to_word(release->rt->name, tempstore) : "?");
		fprintf(output,
			"\t\tStartDate %d DataDate %d Count %.0f \n",
			first_day,
			(release->rt) ? release->rt->data_date : 0,
			rls_total);

		fput_special_array(output, "Passage", 1, first_day,
				   STEPS_PER_DAY, data->output, 0);
		fput_special_array(output, "Transport_in", 1,
				   first_day,
				   STEPS_PER_DAY, data->transport_in, 0);
		fput_special_array(output, "Transport_out", 1,
				   first_day,
				   STEPS_PER_DAY, data->transport_out, 0);
#ifdef ROUTE_TRACKING		
		fput_special_array(output, "Project_in", 1,
				   first_day,
				   STEPS_PER_DAY, data->project_in, 0);
		fput_special_array(output, "Bypass_in", 1,
				   first_day,
				   STEPS_PER_DAY, data->bypass_in, 0);
		fput_special_array(output, "Bypass_out", 1,
				   first_day,
				   STEPS_PER_DAY, data->bypass_pass, 0);
		if (data->sluiceway_pass && data->bypass_pass && data->turbine_pass && data->spillway_pass && data->rsw_pass) {
		    /* project_out represents pre-tailrace mort passage */
		    int i;
		    float project_out[STEPS_IN_SEASON];
		    bzero((char *)project_out, sizeof(float)*STEPS_IN_SEASON);
		    for (i=first_day*STEPS_PER_DAY; i < STEPS_IN_SEASON; ++i) {
			project_out[i] = data->sluiceway_pass[i] + data->bypass_pass[i] + data->turbine_pass[i]
			    + data->spillway_pass[i] + data->rsw_pass[i];
		    }
		    fput_special_array(output, "Project_out", 1,
				       first_day,
				       STEPS_PER_DAY, project_out, 0);
		}
#endif		
		{
		    float fpe_vec[STEPS_IN_SEASON];
		    bzero((char *)fpe_vec, sizeof(float)*STEPS_IN_SEASON);
		    if (compute_fpe(fpe_vec, data))
			fput_special_array(output, "FPE", 1,
					   first_day,
					   STEPS_PER_DAY, fpe_vec, 1);
		}
	    }
	}

	putc('\n', output);
    }

    fclose(output);
    free_assoc_list(assoc_list);
    return 0;
}

/** some arrays require special treatment, consisting of collapsing the
 * timesteps into days.  this is done by dumping to temp array before
 * output.  it is assumed that the arrays passed in are of size
 * DAYS_IN_SEASON * slice_size.  after collapsing to daily values, the
 * range from start date to DAYS_IN_SEASON is output.
 */
static void fput_special_array(fp, title, indent_count, start_date, slice_size,
			       array, percentiles)
	FILE		*fp;
	char		*title;
	unsigned char	indent_count;
	int		start_date;
	int		slice_size;
	float		*array;
	int 		percentiles; /* bool: array contains percentile vals */
{
    static float *outvec = 0;
    int i=0;
    
    if (!array) return;
    if (!outvec) 
	outvec = (float *) malloc(sizeof(float) * DAYS_IN_SEASON);

    bzero((char *)outvec, sizeof(float) * DAYS_IN_SEASON);

    /* percentiles are combined by averaging, others by summing */
    if (percentiles)
	for (i=0; i < DAYS_IN_SEASON * slice_size; ++i) 
	    outvec[i / slice_size] += (array[i] / slice_size);
    else
        for (i=0; i < DAYS_IN_SEASON * slice_size; ++i) 
	    outvec[i / slice_size] += array[i];

    fput_float_array(fp, title, indent_count,
		     DAYS_IN_SEASON - start_date,
		     &outvec[start_date]);
}

#define VALUES_PER_LINE		5
#define FLOAT_FORMAT		"%.2f"

static void fput_float_array(fp, title, indent_count, array_count, array)
	FILE		*fp;
	char		*title;
	unsigned char	indent_count;
	unsigned	array_count;
	float		*array;
{
    unsigned		loop_count;
    unsigned		i;

    if (!array) return;
    /* trim trailing zeros, but always print at least one */
    while (array_count > 1 && array[array_count-1] < 0.005)
	--array_count;
    
    if (indent_count > 1) {
	i = indent_count - 1;
	while (i--)
	    putc('\t', fp);
    }

    if (!(title && *title))
	title = "Array";

    fputs(title, fp);

    fprintf(fp, " %d\n", array_count);
    loop_count = 0;
    while (1) {

	if (loop_count % VALUES_PER_LINE == 0) {
	    i = indent_count;
	    while (i--)
		putc('\t', fp);
	}

	fprintf(fp, FLOAT_FORMAT, array[loop_count]);

	if (++loop_count >= array_count) break;	/* Real exit point */

	if (loop_count % VALUES_PER_LINE == 0)
	    putc('\n', fp);
	else
	    putc(' ', fp);
    }
    putc('\n', fp);
}

static int selected_segment(/*struct river_segment *segment*/)
{
    return 1;		/* For now, all are selected */
}

static int run_realtime_monte(filename, mode)
    char *filename;
    int mode;
{
    struct monte_carlo_data	monte_data;
    struct altern_data		alternative;
    struct input_file		input_file;

#ifndef WIN32
    struct stat			sbuf;
    int				sreturn;
#endif

    char dbuf[MAXPATHLEN];

    monte_data.nalternatives 	= 1;
    monte_data.alts 		= &alternative;
    
    if (mode==RT_MONTE)
	monte_data.flags =
	    MF_REALTIME_MONTE | ARCHIVE_SPILL | ARCHIVE_ELEV | ARCHIVE_FLOW;
    else
	monte_data.flags = MF_REALTIME_MONTE;
    alternative.load_directory   = strcpy(dbuf, current_dir());
    alternative.nyears		 = 1;
    alternative.ngames 		 = 1;
    alternative.alternative_name = REALTIME_ALTERNATIVE;
    alternative.flow_archive     = (mode==RT_MONTE) ? REALTIME_FLOW_ARCHIVE : NULL;
    alternative.use_flow_from    = (mode==RT_MONTE) ? ARCHIVE_FLOW : 0;
    alternative.use_spill_from   = (mode==RT_MONTE) ? ARCHIVE_SPILL : 0;
    alternative.use_elev_from    = (mode==RT_MONTE) ? ARCHIVE_ELEV : 0;
    alternative.multi            = 0;
    alternative.input_files      = &input_file;

    input_file.file_name	= filename ? filename : REALTIME_DATAFILE;
    input_file.list		= (char **) 0;

#ifdef WIN32
	/* On the PC, just command the directory to be made and hope for the best. */
	/* TODO check to see if directory got made somehow. */
	mkdir( REALTIME_ALTERNATIVE );
#else
	if ((sreturn = stat(REALTIME_ALTERNATIVE, &sbuf)) < 0
	|| !S_ISDIR(sbuf.st_mode)) {

	if (sreturn == 0) {	/* File is not a directory! */

	    log_msg(L_ERROR,
		"Real-time: Can't create real-time alternative, File exists");
	    return -1;
	}

	if (errno != ENOENT) {	/* Unanticipated error! */
	    log_msg(L_ERROR, "Real-time: Error in stat %s\n",
		    strerror(errno));
	    return -1;
	}

	log_msg(L_MSG, "Creating alternative directory \"%s\"\n",
		REALTIME_ALTERNATIVE);
	if (mkdir(REALTIME_ALTERNATIVE, 0777) < 0) {
	    log_msg(L_ERROR, "Can't create realtime alternative directory!\n");
	    return -1;
	}
    }
#endif /* WIN32 */
    return run_monte_carlo(&monte_data);
}


/** some routines for handling the realtime data stored in releases.  all
 * rtinfo structs must be created and deleted using these routines, for
 * proper operation of the module.  an ivariant on the rtinfo struct is
 * that an allocated "name" always exists.
 */

/** copy the data from one rtinfo to another.  if the from "param" is null,
 * fill with generic data.  returns 0 on failure, or the address of the
 * "to" struct on success.
 */
static RtInfo *copy_rtinfo(from, to)
    RtInfo *from;
    RtInfo *to;
{
    char *name=0;
    
    if (!to) return 0;
    if (from == to) return to;

    if (!from) {
	bzero((char *)to, sizeof(RtInfo));
	name = "";
    }
    else {
	name = from->name;
	to->data_date = from->data_date;
    }
    if (rt_reset_name(to, name))
	return to;
    else return 0;
}

/** allocate and return an rtinfo with the requested name if specified.
 * makes a copy of the name string.  returns 0 on failure.
 */
RtInfo *construct_rtinfo(name)
    char *name;
{
    RtInfo *rtinfo = (RtInfo *) malloc(sizeof(RtInfo));
    if (!rtinfo)
	return 0;
    bzero((char *) rtinfo, sizeof(RtInfo));
    if (!name)
	name = "";

    if (!(rtinfo->name = strdup(name))) {
	    free(rtinfo);
	    return 0;
    }
    return rtinfo;
}

/** deletes an rtinfo and zeros the pointer to it.  
 */
void destroy_rtinfo(rtinfopp)
    RtInfo **rtinfopp;
{
    RtInfo *ptr = *rtinfopp;
    if (!ptr)
	return;
    free(ptr->name);
    free(ptr);
    *rtinfopp = 0;
}

/** clone an rtinfo.  this is like a copy constructor. return 0 on failure */

RtInfo *clone_rtinfo(from)
    RtInfo *from;
{
    RtInfo *new;

    if (!(new = construct_rtinfo(0)))
	return 0;
    return (copy_rtinfo(from, new));
}

/** reset the name of an rtinfo struct.  copies the string.  returns 0 on
 * failure, non-zero on success.
 */
int rt_reset_name(rtinfo, name)
    RtInfo *rtinfo;
    char *name;
{
    if (!name) return 0;
    if (name == rtinfo->name)
	return 1;
    free(rtinfo->name);

    if ((rtinfo->name = strdup(name)))
	return 1;
    else return 0;	
}
   
#endif	/* REALTIME */

