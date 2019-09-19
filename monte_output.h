/* \file
 * \brief Write out Monte Carlo data after a run.
 */

#ifndef __MONTE_OUTPUT_H__
#define __MONTE_OUTPUT_H__

#include "monte.h"
#include "crisp.h"

extern void compute_passage_stats(struct passage_stats *stats, float *data, int length, int start_date);

extern void compute_stats(struct rls_seg_data *seg_data, 
						  struct river_segment *seg, 
						  int start_date, int first_flow_day);
extern void close_output_file();
extern int write_seg_stats(FILE *outfile, struct rls_stats *stats, 
							int print_summary);
extern void write_stat_col_hdr(FILE *outfile, 
							   struct river_segment *flow_seg);
extern int new_outfile(int alt, struct monte_carlo_data *monte_data);
extern void calculate_stats_and_prune_tree(int game, int prune);
extern int write_output_data(int year);

#endif
