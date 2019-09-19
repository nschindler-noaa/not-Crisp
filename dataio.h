/* \file
 * \brief Code for writing out input files.
 */

#ifndef __DATAIO_H__
#define __DATAIO_H__

#include "writeutil.h"

#ifdef __cplusplus
extern "C" {
#endif

extern void reset_all_spill();
extern void reset_all_dams();
extern void reset_all_releases();    
extern int write_river_environment_file( char *fname );
extern int write_operations_file( char *fname );
extern int write_calibration_file( char *fname );
extern int write_post_bonneville_file( char *fname );
extern int write_etc_file( char *fname );
extern int write_data_file( char *fname );
extern int write_output_file( char *fname );
extern int output_version(FILE *outfile);
extern int output_crisp_data (FILE *outfile);
extern int output_file_data (FILE *outfile);
extern int output_all_runs (FILE *outfile);
extern int output_all_rls (FILE *outfile);
extern int output_reach_classes (FILE *outfile);
extern int output_config (FILE *outfile);
extern int output_river (FILE *outfile);
extern int output_all_dam (FILE *outfile);
extern int output_all_flow (FILE *outfile);
extern int output_all_spill (FILE *outfile);
extern int output_all_reaches (FILE *outfile);
extern int output_all_specs_and_stocks(FILE *outfile);
extern int output_float(FILE *outfile, int indent_level, 
						enum FloatType float_type, int output_terminal,
						char *name, double value, float *default_value);

#ifdef __cplusplus
}
#endif
    
#endif /* __DATAIO_H__ */
