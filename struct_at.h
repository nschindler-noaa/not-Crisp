/** \file
 * \brief Misc. informational data structures.
 */

#ifndef __STRUCT_AT_H__
#define __STRUCT_AT_H__

#include "crisp.h"

extern int summary_day_comparison( /* val1, val2 */ );
extern int summary_total_comparison( /* val1, val2 */ );

/** River segment information */
struct river_seg_info {
    struct river_seg_info 	*next;
    char *name;					/** name of river segment */
    struct rls_stats 		*summaries; 	/** sum for specific release */
};

/** Release structure information */
struct rls {
    struct rls 			*next;
    char			*species;
    char 			*location;
    unsigned int		date;
    float			smolt_onset;
    struct rls_stats		hist_summary;  	/** Hist sum for release input */
    char			*flow_seg; 	/** flow indicator segment */
    struct river_seg_info	*river_segment; /** all the info for this
						  segment, this year */
};
       
/** Release years */
struct rls_years {
    unsigned int		year;
    struct rls			*releases;
};

/** I don't know what this is */
struct at_struct {
    char *input_file;
    char *alternative;
    int years;
    int games;
    
    struct rls_years *release_year;	/* malloc to at_struct.years */
};

/** Monte Carlo histogram information */
struct monte_hist_info {
    float *data_array;
    int data_count;
    float bin_size;
    int flags;
};

/** Summary data */
struct summary {
    struct rls_stats *info;
    float travel_time;
};

/* Flags for histogram display */
#define AT_CUMUL_FLAG	0x01
#define AT_TRAVEL_FLAG	0x02

/** at display struct */

struct at_disp {
    struct at_struct *alt;
    struct rls_years *year;
    struct rls *rls;
    struct rls one_release;	/* storage if only one release selected */
};

#endif

