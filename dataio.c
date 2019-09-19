/* dataio.c
 *
 *	These functions represent the actual workhorses of the file output
 * routines; the functions that actually perform the writes of all data
 * associated with a certain partition of the model (i.e. equations,
 * behavior, reservoirs, dams, flow, etc.)
 */
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
    
#include "filerext.h"
#include "crisp.h"
#include "c1tokens.h"
#include "log.h"
#include "writeutil.h"
#include "period.h"
#include "dataio.h"
#include "init.h"
#include "transport.h"
#include "release.h"
#include "file.h"

/* These three routines are only needed during file input.
 */
void reset_all_spill()
{
    struct river_segment *seg;
 
    for (seg = river_mouth; seg; seg = seg->next)
    {
        if (seg->seg_type == DAM_SEG)
        {
            init_spill_cap((struct dam *)(&(seg->current->dam)));
            bzero((char *) seg->current->dam.spill, 
				           sizeof(seg->current->dam.spill));
            destroy_period_struct(seg->current->dam.planned_spill_day);
			seg->current->dam.planned_spill_day 
								= (PERIOD_LIST *) 0;
            destroy_period_struct(seg->current->dam.planned_spill_night);
			seg->current->dam.planned_spill_night 
								= (PERIOD_LIST *) 0;
        }
    }
}

void reset_all_dams()
{
    struct river_segment *seg;
 
    for (seg = river_mouth; seg; seg = seg->next)
        if (seg->seg_type == DAM_SEG && seg->current->dam.transport)
            remove_transport(seg);
}
 
void reset_all_releases()
{
    int run_count;
 
    for (run_count=0; /* run_count < NRUNS */ run_count < 1; run_count++)
        while(run[run_count].release_list)
            delete_release(run[run_count].release_list);
}

/* These routines are included so that writing output files can be done
 *  from several levels.
 */

/* This is the main routine for writing output files, in various formats
 *  which are selected by the file name extension. "fname" is assumed
 *  to be a buffer allocated to MAXPATHLEN. Since this routine may be
 *  called in batch mode, all error reporting is external. Returns 0 on
 *  success, or a non-zero error code.
 */
int write_output_file( char *fname ) {
	char *nameonly;
    int err, len;
    FILE *fp;
    
    len = strlen( fname );

    if (!strcasecmp( fname+len-5, ".ctrl" )) {
		/* Control file has includes for the other file types.
		 *  Also, force writes of all the other file types.
		 */
		/* First, write out all file types.
		 */
		strcpy( fname+len-4, "riv" );
		write_river_environment_file( fname );
		strcpy( fname+len-4, "ops" );
		write_operations_file( fname );
		strcpy( fname+len-4, "clb" );
		write_calibration_file( fname );
		strcpy( fname+len-4, "pbn" );
		write_post_bonneville_file( fname );
		strcpy( fname+len-4, "etc" );
		write_etc_file( fname );
		strcpy( fname+len-4, "ctrl" );

		/* Now, write the control file itself.
		 */
		fp = fopen( fname, "w" );
		if (fp == NULL){
            err = -1;
        }
		else {
			char nfname[MAXPATHLEN];

			nameonly = strrchr( fname, PATHSEPERATOR );
			if (nameonly)
				strcpy( nfname, nameonly+1 );
			else
				strcpy( nfname, fname );
			len = strlen( nfname ) - 4;
			write_header( fp, "COMPASS Control File", nfname );
			fprintf( fp, "%s %d\n", 
				TKS_VERSION, CURRENT_OUTFILE_VERSION );

			/* Write file creation/modification data */
			err = 0;
			err |= output_file_data(fp);

			strcpy( nfname+len, "pbn" );
			fprintf( fp, "include %s\n", nfname );
			strcpy( nfname+len, "clb" );
			fprintf( fp, "include %s\n", nfname );
			strcpy( nfname+len, "riv" );
			fprintf( fp, "include %s\n", nfname );
			strcpy( nfname+len, "ops" );
			fprintf( fp, "include %s\n", nfname );
			strcpy( nfname+len, "etc" );
			fprintf( fp, "include %s\n", nfname );
			fclose( fp );
		}
	}
	/* Here write part of the database, selected by the file
	 *  extension, or the whole thing if the extension is not
	 *  recognized.
	 */
	else if (!strcasecmp( fname+len-4, ".riv" ))
		err = write_river_environment_file( fname );
	else if (!strcasecmp( fname+len-4, ".ops" ))
		err = write_operations_file( fname );
	else if (!strcasecmp( fname+len-4, ".clb" ))
		err = write_calibration_file( fname );
	else if (!strcasecmp( fname+len-4, ".pbn" ))
		err = write_post_bonneville_file( fname );
	else if (!strcasecmp( fname+len-4, ".etc" ))
		err = write_etc_file( fname );
	else 
		err = write_data_file( fname );

    return err;
}

/* Lower level output routines. */
int output_indent(FILE *outfile, int indent_level)
{
    if (indent_level == INDENT_CONTINUATION) {
	if (putc(' ', outfile) == EOF) OUTPUT_ERROR;
    } else
	while (indent_level--)
	    if (putc('\t', outfile) == EOF) OUTPUT_ERROR;

	return 0;
}
int output_string_w_extra(FILE *outfile, int indent_level, char *name, char *extra)
{
	int error_code;	
	if ((error_code = output_indent(outfile, indent_level))) return error_code;

    if (name) {
	if (*name && fputs(name, outfile) == EOF) OUTPUT_ERROR;
	if (putc(' ', outfile) == EOF) OUTPUT_ERROR;
    }

    if (extra) {
	if (*extra && fputs(extra, outfile) == EOF) OUTPUT_ERROR;
	if (putc(' ', outfile) == EOF) OUTPUT_ERROR;
    }

	return error_code;
}
int output_float_w_extra(FILE *outfile, int indent_level, 
						enum FloatType float_type, int output_terminal,
						char *name, char *extra, double value, float *default_value)
{
    int		error_return;

	/* Skip if it's set to the default */
	if (!output_all && default_value) {
		/* If we're using FLOAT_PRECISE then the default value must actually be a double and not a float */
		double real_default_value = float_type == FLOAT_PRECISE ? *((double*)default_value) : *default_value;
		if (value == real_default_value) return 0;
	}

	if ((error_return = output_string_w_extra(outfile, indent_level, name, extra))) return error_return;

    switch (float_type) {
      case FLOAT_DEFAULT:
      case FLOAT_FIXED:
	error_return = fprintf(outfile, "%.2f", value);
	break;
      case FLOAT_INTEGER:
	error_return = fprintf(outfile, "%.0f", value);
	break;
      case FLOAT_SCIENTIFIC:
	error_return = fprintf(outfile, "%.6f", value);
	break;
      case FLOAT_PRECISE:
	error_return = fprintf(outfile, "%.12f", value);
	break;
      default:
	log_msg(L_ERROR, "Programmer error in output_float!\n");
	return 1;
	break;
    }
    if (error_return == EOF)
	OUTPUT_ERROR;
    if (output_terminal)
	if (putc('\n', outfile) == EOF)
	    OUTPUT_ERROR;
    return 0;
}

int output_float(FILE *outfile, int indent_level, 
						enum FloatType float_type, int output_terminal,
						char *name, double value, float *default_value)
{
    return output_float_w_extra( outfile, indent_level, float_type,
                                 output_terminal, name, NULL, value, default_value );
}

/* Writes out a stochastic distribution (i.e. variable_parameter)
 * Note that since the users prefer using turbine_mort_mean instead of the
 * broken_stick token version, we assume that the parser has such tokens
 * implemented! */
int output_variable_parameter_w_extra(FILE *outfile, int indent_level,
	enum FloatType float_type, char* name, char *extra, variable_parameter *vp)
{
    int error_code=0;   /* used to check for errors in subprocesses */
#ifdef FULL_BROKEN_STICK_OUTPUT
	int indent_params = INDENT_CONTINUATION;
#endif


/* Only output turbine_mort broken_stick ... end () if the user really wants it
 * (by default this is not defined, so we use turbine_mort_mean, etc.) */
#ifdef FULL_BROKEN_STICK_OUTPUT
	if ((error_code = output_string_w_extra(outfile, indent_level, name, extra))) return error_code;
	if (indent_level != INDENT_CONTINUATION) {
		indent_level += 2;
		indent_params = indent_level + 1;

		/* Add new line for broken_stick token */
		if (putc('\n', outfile) == EOF) OUTPUT_ERROR;
	}
#else
	char name_mean[MAX_TOKEN_LEN + 1];
	char name_low[MAX_TOKEN_LEN + 1];
	char name_high[MAX_TOKEN_LEN + 1];

	name_mean[0] = '\0';
	name_low[0] = '\0';
	name_high[0] = '\0';

	strcat(name_mean, name);
	strcat(name_mean, "_mean");
	strcat(name_low, name);
	strcat(name_low, "_low");
	strcat(name_high, name);
	strcat(name_high, "_high");
#endif

	switch (vp->type) {
	case DIST_BROKEN_STICK:
#ifdef FULL_BROKEN_STICK_OUTPUT
		error_code |= output_string_w_extra(outfile, indent_level, TKS_DIST_BROKEN_STICK, "\n");
		error_code |= output_float(outfile, indent_params, float_type, TRUE, TKS_DIST_BS_MEAN,
			 vp->parameters.broken_stick.mean);
		error_code |= output_float(outfile, indent_params, float_type, TRUE, TKS_DIST_BS_LOW,
			 vp->parameters.broken_stick.low);
		error_code |= output_float(outfile, indent_params, float_type, TRUE, TKS_DIST_BS_HIGH,
			 vp->parameters.broken_stick.high);
		error_code |= output_indent(outfile, indent_level);
	    OUTPUT_STRING("%s %s ()\n", TKS_END, TKS_DIST_BROKEN_STICK);
#else
		error_code |= output_float_w_extra(outfile, indent_level, float_type, TRUE, name_mean, extra,
			 vp->parameters.broken_stick.mean, NULL);
		error_code |= output_float_w_extra(outfile, indent_level, float_type, TRUE, name_low, extra,
			 vp->parameters.broken_stick.low, NULL);
		error_code |= output_float_w_extra(outfile, indent_level, float_type, TRUE, name_high, extra,
			 vp->parameters.broken_stick.high, NULL);
#endif
	break;
	default:
		log_msg(L_ERROR, "Unknown distribution type that cannot be output\n");
		error_code |= 1;
	}

    return(error_code);
}
int output_variable_parameter(FILE *outfile, int indent_level,
			enum FloatType float_type, char* name, variable_parameter *vp) {
	return output_variable_parameter_w_extra(outfile, indent_level, float_type, name, NULL, vp);
}

int output_float_or_dist_w_extra(FILE *outfile, int indent_level, 
						enum FloatType float_type, int output_terminal,
						char *name, char *extra, float *variable, float *default_value)
{
	variable_parameter *vp = get_variable_parameter(variable);

	if (vp) return output_variable_parameter_w_extra(outfile, indent_level, float_type, name, extra, vp);

	else return output_float_w_extra( outfile, indent_level, float_type,
                                 output_terminal, name, extra, *variable, default_value );
}
int output_float_or_dist(FILE *outfile, int indent_level, 
						enum FloatType float_type, int output_terminal,
						char *name, float *variable, float *default_value)
{
	return output_float_or_dist_w_extra(outfile, indent_level, float_type, output_terminal, name, NULL, variable, default_value);
}


/* Middle level output routine to output an array of parameter values
 *  dimensioned by num_reach_classes. The non-standard part is that the
 *  first value is always output, whereas the rest of them are only
 *  output if they disagree with the first value.
 */
int output_reach_class_float(FILE *outfile, int indent_level, 
						enum FloatType float_type, int output_terminal,
						char *name, float *values, float *default_value)
{
    int i, result;
    result = output_float_w_extra( outfile, indent_level, float_type,
                                 output_terminal, name, 
                                 reach_class_names[0], values[0], default_value );

    for (i = 1; i < num_reach_classes; i++)
        if (values[i] != values[0])
            result |= output_float_w_extra( outfile, indent_level, 
                                 float_type, output_terminal, name, 
                                 reach_class_names[i], values[i], default_value );

    return result;
}

/* Note that this version must check to see if the default reach class
 * has a distribution or not or the current reach class has one or not
 * and THEN must establish equality or inequality before it knows
 * whether or not to output the distribution or float */
int output_reach_class_float_or_dist(FILE *outfile, int indent_level, 
						enum FloatType float_type, int output_terminal,
						char *name, float *values, float *default_value)
{
	variable_parameter *vp0;
    int i, result;

	vp0 = get_variable_parameter(&values[0]);
	if (vp0) result = output_variable_parameter_w_extra(outfile, indent_level, float_type, name, reach_class_names[0], vp0);
	else result = output_float_w_extra(outfile, indent_level, float_type, output_terminal, name, reach_class_names[0], values[0], default_value);

    for (i = 1; i < num_reach_classes; i++) {
		variable_parameter *vp = get_variable_parameter(&values[i]);
		if (vp) {
			int should_output = 0;
			if (!vp0 || (vp0 && vp0->type != vp->type)) should_output = 1;
			else {
				switch (vp->type) {
				case DIST_BROKEN_STICK:
					if (vp0->parameters.broken_stick.mean != vp->parameters.broken_stick.mean
						|| vp0->parameters.broken_stick.low != vp->parameters.broken_stick.low
						|| vp0->parameters.broken_stick.high != vp->parameters.broken_stick.high)
					{
						should_output = 1;
					}
				break;
				default:
					should_output = 1;
				}
			}

			if (should_output) 
				result |= output_variable_parameter_w_extra( outfile, indent_level, 
									 float_type, name, 
									 reach_class_names[i], vp );
		}
		else {
			if (vp0 || (!vp0 && values[i] != values[0]))
				result |= output_float_w_extra( outfile, indent_level, 
									 float_type, output_terminal, name, 
									 reach_class_names[i], values[i], default_value );
		}
	}

    return result;
}

/* Helper function returns TRUE if two equations are equal in every
 *  important way.
 */
static int eq_equal( equation eq1, equation eq2 ) {
    int i;

    if (eq1.id != eq2.id)
        return FALSE;

    for (i = 0; i < eqn_dflt_tab[eq1.id].saved_params; i++) {
        if (eq1.param[i].val != eq2.param[i].val)
            return FALSE;
        if (eq1.param[i].min != eq2.param[i].min)
            return FALSE;
        if (eq1.param[i].max != eq2.param[i].max)
            return FALSE;
    }

    return TRUE;
}

/* fcn to output an equation, starting with the equation id.
 * Format:  ... id
 *          \t\t\t parameter # val min max
 *	    .
 *	    .
 *	    .
 * It is assumed that the begining and ending tokens are output before and after
 * calling this routine.
 */

static int output_equation(FILE *outfile, int indent_level,
	const char *name, const char *extra, equation *eq, equation *default_eq)
{
    int i;

	if (eq == NULL || name == NULL) return 1;

	/* Don't output the equation if it's identical to the default */
	if (!output_all && default_eq) {
		if (eq_equal(*eq, *default_eq)) return 0;
	}

	/* Output start of an equation */
	output_indent(outfile, indent_level);
	OUTPUT_STRING("%s%s", "", name);
	if (extra != NULL) {
		OUTPUT_STRING("%s%s", "\t", extra);
	}
	/* Equation number */
	OUTPUT_ITEM("%s%d\n", "\t", eq->id);

	/* Output the equation contents */
    for(i=0; i < eqn_dflt_tab[eq->id].saved_params; i++)
    {
		/* Only write out the parameter if it's different from the default */
		if (output_all || eq->param[i].val != eqn_dflt_tab[eq->id].param[i].val) {
			output_indent(outfile, indent_level + 1);
			OUTPUT_ITEM("%s %d", TKS_PARAMETER, i);
			output_float(outfile, 0, eq->precision, FALSE, "",
					 eq->param[i].val, NULL);
			/* Don't bother printing out the GUI min/max values; just use the defaults */
			/*output_float(outfile, 0, FLOAT_SCIENTIFIC, FALSE, "",
					 eq->param[i].min);
			output_float(outfile, 0, FLOAT_SCIENTIFIC, FALSE, "",
					 eq->param[i].max);*/
			putc('\n', outfile);
		}
    }

	/* Output end of an equation */
	output_indent(outfile, indent_level);
	OUTPUT_END("", name, extra);

    return (0);
}

/* Fcn to output an array of equations, dimensioned by reach class.
 *  The first equation is always output, and subsequent ones are
 *  output if they differ in any way from the first. The first line
 *  is made to look like "migration_eqn Freeflowing 24" or some such.
 */
static int output_reach_class_equation(FILE *outfile, int indent_level, const char *name, equation *eq, equation *default_eq)
{
    int i, error_code;

    for (i = 0; i < num_reach_classes; i++) {
        if (i == 0 || !eq_equal( eq[0], eq[i] )) {
            if ((error_code=output_equation(outfile, indent_level, name, reach_class_names[i], &eq[i], default_eq)))
	            return(error_code);
        }
    }

    return 0;
}

/*
 * int output_periods(FILE *outfile, PERIOD_LIST *plist, int value_exists) --
 *	Recursive routinte to write a period list.  Assumes that the token
 * string has already been written, and that a newline will be appended.
 */

static int output_periods(outfile, plist, value_exists)
    FILE *outfile;
    PERIOD_LIST *plist;
    int value_exists;
{
    static int depth=0;

    ++depth;
    OUTPUT_ITEM("%s%d", "", plist->start);
    OUTPUT_ITEM("%s%d", ":", plist->end);

    if (plist->sub_periods)
    {
	OUTPUT_ITEM("%s%s", "", " (");
	output_periods(outfile, plist->sub_periods, value_exists);
	OUTPUT_ITEM("%s%s", "", ")");
    }
    else if (value_exists)
	output_float(outfile, 0, FLOAT_DEFAULT, FALSE, " = ", plist->value, NULL);

    if (plist->next)
    {
	OUTPUT_ITEM("%s%s", "", ", ");
	if (depth % 4 == 0)
	    OUTPUT_ITEM("\n\t%s%s", "", "");
	
	output_periods(outfile, plist->next, value_exists);
    }

    --depth;
    return(0);
}

/*
 * int output_period_list(FILE *outfile, PERIOD_LIST *plist) -- Routine to
 *	call output_periods() without the value field to be output.
 */
static int output_period_list(outfile, plist) 
    FILE *outfile;
    PERIOD_LIST *plist;
{
    collapse_period_list(plist);
    return output_periods(outfile, plist, FALSE);
}

/*
 * int output_period_value_list(FILE *outfile, PERIOD_LIST *plist) -- Routine
 *	to call output_periods() with the value field to be output.
 */
static int output_period_value_list(outfile, plist) 
    FILE *outfile;
    PERIOD_LIST *plist;
{
    collapse_period_list(plist);
    return output_periods(outfile, plist, TRUE);
}


/*---------------------------------------------------------------------------*
 *									     *
 * Low-Level Data Section						     *
 *									     *
 * The following are the low level output routines that may be used in either*
 * the multi-file format or the format that keeps the data for a section of  *
 * the river in one spot.                              			     *
 *									     *
 * Every data item in the model which needs to be written to disk must       *
 * appear once and only once amongst all of these routines.                  *
 *									     *
 * ^..^ explain the format for these things.				     *
 *---------------------------------------------------------------------------*/

/* 
 * int output_spec(FILE *outfile, struct species *spec) -- The routine which
 *	writes all data for a species.
 */
static int output_spec(outfile, spec)
    FILE *outfile;
    struct species *spec;
{
    int error_code=0;   /* used to check for errors in subprocesses */

	output_reach_class_float_or_dist(outfile, 1, FLOAT_SCIENTIFIC, TRUE,TKS_RPCOEF,
		 spec->reach_pcoef, &species_defaults.reach_pcoef[0]);

    output_reach_class_float(outfile, 1, FLOAT_SCIENTIFIC, TRUE,TKS_PPRIME_A,
		 spec->pprime_a, &species_defaults.pprime_a[0]);
    output_reach_class_float(outfile, 1, FLOAT_SCIENTIFIC, TRUE,TKS_PPRIME_B,
		 spec->pprime_b, &species_defaults.pprime_b[0]);

	output_float_or_dist(outfile, 1, FLOAT_SCIENTIFIC, TRUE, TKS_FPCOEF, &spec->forebay_pcoef, &species_defaults.forebay_pcoef);
	output_float_or_dist(outfile, 1, FLOAT_SCIENTIFIC, TRUE, TKS_TPCOEF, &spec->tailrace_pcoef, &species_defaults.tailrace_pcoef);

    output_reach_class_float(outfile, 1, FLOAT_SCIENTIFIC, TRUE,TKS_VVAR1,
        spec->stock.vvar, &stock_defaults.vvar[0]);

	output_reach_class_float_or_dist(outfile, 1, FLOAT_SCIENTIFIC, TRUE,TKS_MRCOEF,
		 spec->stock.mvcoef, &stock_defaults.mvcoef[0]);

    output_reach_class_float(outfile, 1, FLOAT_SCIENTIFIC, TRUE, TKS_TIME_COEFF,
		 spec->stock.time_coeff, &stock_defaults.time_coeff[0]);
    output_reach_class_float(outfile, 1, FLOAT_SCIENTIFIC, TRUE, TKS_DISTANCE_COEFF,
		 spec->stock.distance_coeff, &stock_defaults.distance_coeff[0]);
    output_reach_class_float(outfile, 1, FLOAT_SCIENTIFIC, TRUE, TKS_SIGMA_D,
		 spec->stock.sigma_d, &stock_defaults.sigma_d[0]);
    output_reach_class_float(outfile, 1, FLOAT_SCIENTIFIC, TRUE, TKS_REACH_SURVIVAL_COEF,
		 spec->stock.reach_survival_coef, &stock_defaults.reach_survival_coef[0]);
    if ((error_code=output_equation(outfile, 1, TKS_GMORT_EQUATION, NULL, &spec->gmort_eqn, &species_defaults.gmort_eqn)))
	return(error_code);
    if ((error_code=output_equation(outfile, 1, TKS_FDENS_EQUATION, NULL, &spec->fdens_eqn, &species_defaults.fdens_eqn)))
	return(error_code);
    
    if ((error_code=output_reach_class_equation(
                    outfile, 1, TKS_MIGR_EQUATION, spec->stock.migration_eqn, &stock_defaults.migration_eqn[0])))
	return(error_code);
    if ((error_code=output_reach_class_equation(outfile, 1,
                         TKS_PRED_TR_EQUATION,
                         spec->stock.pred_temp_response_eqn, &stock_defaults.pred_temp_response_eqn[0])))
	return(error_code);
    if ((error_code=output_equation(outfile, 1, TKS_INRIVER_RETURN_EQUATION, NULL, &spec->stock.inriver_return_eqn, &stock_defaults.inriver_return_eqn)))
	return(error_code);
    if ((error_code=output_equation(outfile, 1, TKS_TRANSPORT_RETURN_EQUATION, NULL, &spec->stock.transport_return_eqn, &stock_defaults.transport_return_eqn)))
	return(error_code);

    if ((error_code=output_reach_class_equation(outfile, 1,
                         TKS_PRED_CAPTURE_DIST_EQN,
                         spec->stock.pred_capture_dist_eqn, &stock_defaults.pred_capture_dist_eqn[0])))
	return(error_code);
    if ((error_code=output_reach_class_equation(outfile, 1,
                         TKS_CUSTOM_SURVIVAL_EQN,
                         spec->stock.custom_survival_eqn, &stock_defaults.custom_survival_eqn[0])))
	return(error_code);
    return(0);
}

/*
 * int output_reach_class_names(FILE *outfile) - output number of reach
 *  classes currently defined, and their names. This will go at the
 *  very top of the .dat and .beh files.
 */
static int output_reach_class_names(FILE *outfile) {
    int i, error_return;
    char scratch[NAME_LEN];

    sprintf( scratch, "%d", num_reach_classes );
    OUTPUT_STRING( "%s %s\n", TKS_NUM_REACH_CLASSES, scratch );

    for (i = 0; i < num_reach_classes; i++) {
        error_return = fprintf( outfile, "\t%s %d %s\n",
                         TKS_REACH_CLASS_NAME, i, reach_class_names[i] );
        if (error_return == EOF)
	        OUTPUT_ERROR;
    }
    error_return = fprintf( outfile, "\n" );
    if (error_return == EOF)
	    OUTPUT_ERROR;

    return 0;
}

/*
 * int output_all_species(FILE *outfile) -- Loop through the global species
 *	list, outputting each species in order.
 */
static int output_all_species(outfile)
    FILE *outfile;
{
    extern struct species *specs;

    int count=0; 	/* a misc. counter */
    int error_code=0;   /* used to check for errors in subprocesses */
    char scratch[NAME_LEN];

    for (count=0; count < num_specs; count++)
    {
	OUTPUT_STRING("%s %s\n", TKS_SPEC,
		      string_to_word(specs[count].name, scratch));
	if ( (error_code=output_spec(outfile, &specs[count])) )
	    return (error_code);
	OUTPUT_END("", TKS_SPEC, string_to_word(specs[count].name, scratch));
    } /* species loop */

    return (0);
}

/* 
 * int output_stock(FILE *outfile, StockT *stock) -- The routine which
 *	writes all data for a stock.
 */
static int output_stock(outfile, stock)
    FILE *outfile;
    StockT *stock;
{
    int error_code=0;   /* used to check for errors in subprocesses */
    
    output_reach_class_float(outfile, 1, FLOAT_SCIENTIFIC, TRUE, TKS_VVAR1,
		 stock->vvar, &stock_defaults.vvar[0]);

	output_reach_class_float_or_dist(outfile, 1, FLOAT_SCIENTIFIC, TRUE,TKS_MRCOEF,
		 stock->mvcoef, &stock->mvcoef[0]);

    output_reach_class_float(outfile, 1, FLOAT_SCIENTIFIC, TRUE, TKS_TIME_COEFF,
		 stock->time_coeff, &stock_defaults.time_coeff[0]);
    output_reach_class_float(outfile, 1, FLOAT_SCIENTIFIC, TRUE, TKS_DISTANCE_COEFF,
		 stock->distance_coeff, &stock_defaults.distance_coeff[0]);
    output_reach_class_float(outfile, 1, FLOAT_SCIENTIFIC, TRUE, TKS_SIGMA_D,
		 stock->sigma_d, &stock_defaults.sigma_d[0]);
    output_reach_class_float(outfile, 1, FLOAT_SCIENTIFIC, TRUE, TKS_REACH_SURVIVAL_COEF,
		 stock->reach_survival_coef, &stock_defaults.reach_survival_coef[0]);
    if ((error_code=output_reach_class_equation(
                   outfile, 1, TKS_MIGR_EQUATION, stock->migration_eqn, &stock_defaults.migration_eqn[0])))
	return(error_code);
    if ((error_code=output_reach_class_equation(
                   outfile, 1, TKS_PRED_TR_EQUATION, stock->pred_temp_response_eqn, &stock_defaults.pred_temp_response_eqn[0])))
	return(error_code);
    if ((error_code=output_equation(outfile, 1, TKS_INRIVER_RETURN_EQUATION, NULL, &stock->inriver_return_eqn, &stock_defaults.inriver_return_eqn)))
	return(error_code);
    if ((error_code=output_equation(outfile, 1, TKS_TRANSPORT_RETURN_EQUATION, NULL, &stock->transport_return_eqn, &stock_defaults.transport_return_eqn)))
	return(error_code);
    if ((error_code=output_reach_class_equation(
                   outfile, 1, TKS_PRED_CAPTURE_DIST_EQN, stock->pred_capture_dist_eqn, &stock_defaults.pred_capture_dist_eqn[0])))
	return(error_code);
    if ((error_code=output_reach_class_equation(
                   outfile, 1, TKS_CUSTOM_SURVIVAL_EQN, stock->custom_survival_eqn, &stock_defaults.custom_survival_eqn[0])))
	return(error_code);
    return(0);
}

/*
 * int output_all_stocks(FILE *outfile) -- Loop through the global stock
 *	list, outputting each stock in order.
 */
static int output_all_stocks(outfile)
    FILE *outfile;
{
    int count=0; 	/* a misc. counter */
    int error_code=0;   /* used to check for errors in subprocesses */
    char scratch[NAME_LEN];

    for (count=0; count < stocks.num; count++)
    {
	OUTPUT_STRING("%s %s\n", TKS_STOCK,
		      string_to_word(stocks.s[count]->name, scratch));
	if ( (error_code=output_stock(outfile, stocks.s[count])) )
	    return (error_code);
	OUTPUT_END("", TKS_STOCK, string_to_word(stocks.s[count]->name, scratch));
    } /* stock loop */

    return (0);
}

/*
 * int output_all_specs_and_stocks(FILE *outfile) -- Output the version
 * 	and then all species, and then all stocks.
 */
int output_all_specs_and_stocks(outfile)
	FILE *outfile;
{
    int erc=0;
    erc += output_reach_class_names( outfile );
    erc += output_all_species(outfile);
    erc += output_all_stocks(outfile);
    return(erc);
}

static int output_flow_reach (outfile, reach)
    FILE *outfile;
    struct reach *reach;
{
    /* output flow */
/* disabled
 *   OUTPUT_ITEM("\t\t%s %f\n", TKS_RCH_FLOW_MAX, reach->flow_max);
 *   OUTPUT_FLOAT_ARRAY("\t\t", TKS_RCH_FLOW, DAYS_IN_SEASON, reach->flow,
 *	OCT_NO_CONV, 1);
 */

    /* output loss */
    output_float(outfile, 2, FLOAT_DEFAULT, TRUE, TKS_RCH_LOSS_MAX,
		 reach->loss_max, &reach_defaults.loss_max);
    output_float(outfile, 2, FLOAT_DEFAULT, TRUE, TKS_RCH_LOSS_MIN,
		 reach->loss_min, &reach_defaults.loss_min);
    OUTPUT_FLOAT_ARRAY("\t\t", TKS_RCH_LOSS, DAYS_IN_SEASON, FLOAT_DEFAULT,
		       reach->loss, OCT_NO_CONV, 1, NULL);

    /* elevation change */
    OUTPUT_FLOAT_ARRAY("\t\t", TKS_RCH_ELEVATION, DAYS_IN_SEASON,
		       FLOAT_DEFAULT, reach->elev_change, OCT_NO_CONV, 1, NULL);
    
    if (reach->initial_gas) {
	    OUTPUT_STRING("\t\t%s %s ", TKS_OUTPUT_GAS, "On");
	    OUTPUT_FLOAT_ARRAY("", "", DAM_SLICES_IN_SEASON, FLOAT_DEFAULT,
		           reach->initial_gas, OCT_NO_CONV, 1, NULL);
    }
    /*else
	    OUTPUT_STRING("\t\t%s %s\n", TKS_OUTPUT_GAS, "Off");*/

    /* output velocity */
/* disabled
 *   OUTPUT_FLOAT_ARRAY("\t\t", TKS_RCH_VELOCITY,
 *		DAYS_IN_SEASON, reach->velocity, OCT_AVERAGE, STEPS_PER_DAY);
 */

	/* Output temperature and turbidity */

    /* delta temperature */   /* ...Only print if not using input_temp  */
    /*OUTPUT_FLOAT_ARRAY("\t\t", TKS_DELTA_TEMP, DAYS_IN_SEASON, FLOAT_DEFAULT,
		       reach->delta_temp, OCT_AVERAGE, STEPS_PER_DAY); */
	
	/* input_temp */
	if (reach->init_temp_flag) {
		/*OUTPUT_STRING("\t\t%s %s ", TKS_DELTA_TEMP, "[*] 0\n");*/
	    OUTPUT_STRING("\t\t%s %s ", TKS_INPUT_TEMP, "On");
	    OUTPUT_FLOAT_ARRAY("", "", DAYS_IN_SEASON, FLOAT_DEFAULT,
		           reach->water_temp, OCT_AVERAGE, STEPS_PER_DAY, &reach_defaults.water_temp[0]);
    }
    else {
		OUTPUT_FLOAT_ARRAY("\t\t", TKS_DELTA_TEMP, DAYS_IN_SEASON, FLOAT_DEFAULT,
		       reach->delta_temp, OCT_AVERAGE, STEPS_PER_DAY, &reach_defaults.delta_temp[0]);
	    /*OUTPUT_STRING("\t\t%s %s\n", TKS_INPUT_TEMP, "Off");*/
	}

	/* input turbidity */
	if (reach->turbidity_flag) {
	    OUTPUT_STRING("\t\t%s %s ", TKS_INPUT_TURB, "On");
	    OUTPUT_FLOAT_ARRAY("", "", DAYS_IN_SEASON, FLOAT_DEFAULT,
		           reach->turbidity, OCT_AVERAGE, STEPS_PER_DAY, &reach_defaults.turbidity[0]);
    }
    else {
	    /*OUTPUT_STRING("\t\t%s %s\n", TKS_INPUT_TURB, "Off");*/
	}
    
    return(0);
}

static int output_flow_dam (outfile, dam)
    FILE *outfile;
    struct dam *dam;
{
    if (dam->storage_basin) {
	OUTPUT_FLOAT_ARRAY("\t\t", TKS_STORAGE_VOLUME, DAYS_IN_SEASON,
		FLOAT_DEFAULT, dam->storage_basin->volume, OCT_NO_CONV, 1, NULL);
    }

	/* intput_temp */
	if (dam->init_temp_flag) {
	    OUTPUT_STRING("\t\t%s %s ", TKS_INPUT_TEMP, "On");
	    OUTPUT_FLOAT_ARRAY("", "", DAYS_IN_SEASON, FLOAT_DEFAULT,
		           dam->water_temp, OCT_AVERAGE, STEPS_PER_DAY, NULL);
    }
    /*else
	    OUTPUT_STRING("\t\t%s %s\n", TKS_INPUT_TEMP, "Off");*/

	/* output_gas */
    if (dam->initial_gas) {
	    OUTPUT_STRING("\t\t%s %s ", TKS_OUTPUT_GAS, "On");
	    OUTPUT_FLOAT_ARRAY("", "", DAM_SLICES_IN_SEASON, FLOAT_DEFAULT,
		           dam->initial_gas, OCT_NO_CONV, 1, NULL);
    }
    /*else
	    OUTPUT_STRING("\t\t%s %s\n", TKS_OUTPUT_GAS, "Off");*/

		/* input turbidity */
	if (dam->turbidity_flag) {
	    OUTPUT_STRING("\t\t%s %s ", TKS_INPUT_TURB, "On");
	    OUTPUT_FLOAT_ARRAY("", "", DAYS_IN_SEASON, FLOAT_DEFAULT,
		           dam->turbidity, OCT_AVERAGE, STEPS_PER_DAY, NULL);
    }
    /*else 
	    OUTPUT_STRING("\t\t%s %s\n", TKS_INPUT_TURB, "Off");*/

    return(0);
}

static int output_flow_headwater (outfile, hdwtr)
    FILE *outfile;
    struct headwater *hdwtr;
{
    output_float(outfile, 2, FLOAT_DEFAULT, TRUE, TKS_HW_FLOW_MEAN,
		 hdwtr->flow_mean, &headwater_defaults.flow_mean);
    
    /* output flow */
    output_float(outfile, 2, FLOAT_DEFAULT, TRUE, TKS_HW_FLOW_MAX,
		 hdwtr->flow_max, &headwater_defaults.flow_max);
/*    output_float(outfile, 2, FLOAT_DEFAULT, TRUE, TKS_HW_FLOW_COEF,
      hdwtr->flow_coefficient); */
    OUTPUT_FLOAT_ARRAY("\t\t", TKS_HW_FLOW, DAYS_IN_SEASON, FLOAT_DEFAULT,
		       hdwtr->flow, OCT_NO_CONV, 1, &headwater_defaults.flow[0]);

    if (hdwtr->initial_gas) {
	    OUTPUT_STRING("\t\t%s %s ", TKS_OUTPUT_GAS, "On");
	    OUTPUT_FLOAT_ARRAY("", "", DAM_SLICES_IN_SEASON, FLOAT_DEFAULT,
		           hdwtr->initial_gas, OCT_NO_CONV, 1, NULL);
    }
    /*else
	    OUTPUT_STRING("\t\t%s %s\n", TKS_OUTPUT_GAS, "Off");*/

    return(0);
}

static int output_headwater (outfile, hdwtr)
    FILE *outfile;
    struct headwater *hdwtr;
{
    OUTPUT_FLOAT_ARRAY("\t", TKS_HW_WTEMP, DAYS_IN_SEASON, FLOAT_DEFAULT,
		       hdwtr->water_temp, OCT_AVERAGE, STEPS_PER_DAY, &headwater_defaults.water_temp[0]);

	/* input turbidity */
	if (hdwtr->turbidity_flag) {
	    OUTPUT_STRING("\t\t%s %s ", TKS_INPUT_TURB, "On");
	    OUTPUT_FLOAT_ARRAY("", "", DAYS_IN_SEASON, FLOAT_DEFAULT,
		           hdwtr->turbidity, OCT_AVERAGE, STEPS_PER_DAY, &headwater_defaults.turbidity[0]);
    }
    /*else 
	    OUTPUT_STRING("\t\t%s %s\n", TKS_INPUT_TURB, "Off");*/

    return(0);
}

static int output_reach (outfile, reach)
    FILE *outfile;
    struct reach *reach;
{
    int i;
    char scratch[NAME_LEN]; 

    /* output species specific info */
    for (i=0; i < num_specs; i++)
    {
	    output_float_w_extra(outfile, 2, FLOAT_SCIENTIFIC, TRUE, TKS_RCH_PRED_MEAN,
				string_to_word(specs[i].name, scratch),
		         reach->species[i].pred_pop_mean, &reach_species_defaults.pred_pop_mean);
    }
    output_float(outfile, 2, FLOAT_DEFAULT, TRUE, TKS_RCH_PRED_DIST,
		 reach->pred_dist_coef, &reach_defaults.pred_dist_coef);
    output_float(outfile, 2, FLOAT_SCIENTIFIC, TRUE, TKS_RCH_GAS_THETA,
		 reach->gas_theta, &reach_defaults.gas_theta);


    return(0);
}

static int output_spill (outfile, dam)
    FILE *outfile;
    struct dam *dam;
{
    int error_code=0;
    
    /* output spill */
/*    OUTPUT_FLOAT_ARRAY("\t\t", TKS_DAM_SPILL,
 *	       DAYS_IN_SEASON*DAM_TIME_SLC, dam->spill, OCT_NO_CONV, 1);
 */

    output_float(outfile, 2, FLOAT_FIXED, TRUE, TKS_SPILL_CAP, dam->spill_cap, NULL);
    if (dam->planned_spill_day)
    {
	OUTPUT_STRING("\t\t%s%s", TKS_PLAN_SPILL_DAY, 0);
	if ((error_code=output_period_value_list(outfile,
						 dam->planned_spill_day)))
	    return(error_code);
	OUTPUT_NEWLINE;
    }
    if (dam->planned_spill_night)
    {
	OUTPUT_STRING("\t\t%s%s", TKS_PLAN_SPILL_NIGHT, 0);
	if ((error_code=output_period_value_list(outfile,
						 dam->planned_spill_night)))
	    return(error_code);
	OUTPUT_NEWLINE;
    }
    return(0);
}

static int output_powerhouse(outfile, phouse)
    FILE *outfile;
    struct river_segment *phouse;
{
    struct dam *dam = &phouse->current->dam;
    int spec_count=0; /* misc. counters */
    char scratch[NAME_LEN]; 
    int error_code=0;   /* used to check for errors in subprocesses */
	const int indent_params = 3;

    OUTPUT_STRING("\t\t%s %s\n", TKS_ADDL_PHOUSE,
		  string_to_word(phouse->seg_name, scratch));
    
    OUTPUT_ITEM("\t\t\t%s %.2f\n", TKS_PWRHS_CAP, dam->powerhouse_capacity);
    OUTPUT_ITEM("\t\t\t%s %d\n", TKS_PWRHS_PRIORITY, dam->phouse_priority);
    output_float(outfile, 3, FLOAT_DEFAULT, TRUE, TKS_DAM_FLOW_MIN,
		 dam->flow_min_project, NULL); /* This is always written out since it's important */

	output_float_or_dist(outfile, indent_params, FLOAT_SCIENTIFIC, TRUE, TKS_RSW_SPILL_CAP, &dam->rsw_spill_cap, NULL);

    if (dam->phouse_schedule)
    {
	OUTPUT_STRING("\t\t\t%s%s", TKS_PWRHS_SCHED, 0);
	if ((error_code=output_period_list(outfile, dam->phouse_schedule)))
	    return(error_code);
	OUTPUT_NEWLINE;
    }
    
    /* output powerhouse dependent species specific info */
    for (spec_count=0; spec_count < num_specs; spec_count++)
    {

	output_float_or_dist_w_extra(outfile, indent_params, FLOAT_SCIENTIFIC, TRUE, TKS_SLUICEWAY_PROP,
			string_to_word(specs[spec_count].name, scratch),
			 &dam->species[spec_count].sluiceway_prop, NULL);

	output_float_or_dist_w_extra(outfile, indent_params, FLOAT_SCIENTIFIC, TRUE, TKS_SLUICEWAY_MRT,
		string_to_word(specs[spec_count].name, scratch),
			 &dam->species[spec_count].sluiceway_mort, NULL);

	output_float_or_dist_w_extra(outfile, indent_params, FLOAT_SCIENTIFIC, TRUE, TKS_BY_MRT,
		string_to_word(specs[spec_count].name, scratch),
			 &dam->species[spec_count].bypass_mort, NULL);
	
	output_float_or_dist_w_extra(outfile, indent_params, FLOAT_SCIENTIFIC, TRUE, TKS_TURB_MRT,
		string_to_word(specs[spec_count].name, scratch),
			 &dam->species[spec_count].turbine_mort, NULL);
	
	/*output_float_or_dist_w_extra(outfile, indent_params, FLOAT_SCIENTIFIC, TRUE, TKS_DAY_FGE,
		string_to_word(specs[spec_count].name, scratch),
			 &dam->species[spec_count].day_fge, NULL);

	output_float_or_dist_w_extra(outfile, indent_params, FLOAT_SCIENTIFIC, TRUE, TKS_NT_FGE,
		string_to_word(specs[spec_count].name, scratch),
			 &dam->species[spec_count].night_fge, NULL);*/

	output_float_w_extra(outfile, indent_params, FLOAT_SCIENTIFIC, TRUE, TKS_SPILL_DELAY,
		string_to_word(specs[spec_count].name, scratch),
		     dam->species[spec_count].spill_delay, &dam_species_defaults.spill_delay);
	output_float_w_extra(outfile, indent_params, FLOAT_SCIENTIFIC, TRUE, TKS_BYPASS_DELAY,
		string_to_word(specs[spec_count].name, scratch),
		     dam->species[spec_count].bypass_delay, &dam_species_defaults.bypass_delay);
	output_float_w_extra(outfile, indent_params, FLOAT_SCIENTIFIC, TRUE, TKS_SLUICEWAY_DELAY,
		string_to_word(specs[spec_count].name, scratch),
		     dam->species[spec_count].sluiceway_delay, &dam_species_defaults.sluiceway_delay);
	output_float_w_extra(outfile, indent_params, FLOAT_SCIENTIFIC, TRUE, TKS_TURBINE_DELAY,
		string_to_word(specs[spec_count].name, scratch),
		     dam->species[spec_count].turbine_delay, &dam_species_defaults.turbine_delay);
	output_float_w_extra(outfile, indent_params, FLOAT_SCIENTIFIC, TRUE, TKS_RSW_DELAY,
		string_to_word(specs[spec_count].name, scratch),
		     dam->species[spec_count].rsw_delay, &dam_species_defaults.rsw_delay);

	output_float_w_extra(outfile, indent_params, FLOAT_SCIENTIFIC, TRUE, TKS_SPILL_DAY,
		string_to_word(specs[spec_count].name, scratch),
		     dam->species[spec_count].spill_day, &dam_species_defaults.spill_day);
	output_float_w_extra(outfile, indent_params, FLOAT_SCIENTIFIC, TRUE, TKS_BYPASS_DAY,
		string_to_word(specs[spec_count].name, scratch),
		     dam->species[spec_count].bypass_day, &dam_species_defaults.bypass_day);
	output_float_w_extra(outfile, indent_params, FLOAT_SCIENTIFIC, TRUE, TKS_SLUICEWAY_DAY,
		string_to_word(specs[spec_count].name, scratch),
		     dam->species[spec_count].sluiceway_day, &dam_species_defaults.sluiceway_day);
	output_float_w_extra(outfile, indent_params, FLOAT_SCIENTIFIC, TRUE, TKS_TURBINE_DAY,
		string_to_word(specs[spec_count].name, scratch),
		     dam->species[spec_count].turbine_day, &dam_species_defaults.turbine_day);
	output_float_w_extra(outfile, indent_params, FLOAT_SCIENTIFIC, TRUE, TKS_RSW_DAY,
		string_to_word(specs[spec_count].name, scratch),
		     dam->species[spec_count].rsw_day, &dam_species_defaults.rsw_day);

	if ((error_code=output_equation(outfile, indent_params, TKS_FGE_EQN,
					string_to_word(specs[spec_count].name, scratch),
					&dam->species[spec_count].fge_eqn, &dam_species_defaults.fge_eqn)))
	    return(error_code);

    } /* species for loop */

    OUTPUT_END("\t\t", TKS_ADDL_PHOUSE,
	       string_to_word(phouse->seg_name, scratch));
    return(0);
}

static int output_dam(outfile, dam)
    FILE *outfile;
    struct dam *dam;
{
    int spec_count=0; /* misc. counters */
    char scratch[NAME_LEN]; 
    int error_code=0;   /* used to check for errors in subprocesses */
	const int indent_params = 2;

    output_float(outfile, 2, FLOAT_FIXED, TRUE, TKS_TR_LENGTH,
		 dam->tailrace_length, &dam_defaults.tailrace_length);
    output_float(outfile, 2, FLOAT_DEFAULT, TRUE, TKS_DAM_FLOW_MIN,
		 dam->flow_min_project, NULL); /* This is always written out since it's important */
    if ((error_code=output_equation(outfile, indent_params, TKS_NSAT_DAY_EQUATION, NULL, &dam->nsat_eqn, &dam_defaults.nsat_eqn)))
	return(error_code);
    if ((error_code=output_equation(outfile, indent_params, TKS_NSAT_NIGHT_EQUATION, NULL, &dam->nsat_night_eqn, &dam_defaults.nsat_night_eqn)))
	return(error_code);
    if ((error_code=output_equation(outfile, indent_params, TKS_NSAT_BACKUP_EQUATION, NULL, &dam->nsat_backup_eqn, &dam_defaults.nsat_backup_eqn)))
	return(error_code);
    output_float(outfile, 2, FLOAT_SCIENTIFIC, TRUE, TKS_DAM_GAS_THETA,
		 dam->gas_theta, &dam_defaults.gas_theta);
    output_float(outfile, 2, FLOAT_SCIENTIFIC, TRUE, TKS_K_ENTRAIN,
		 dam->k_entrain, &dam_defaults.k_entrain);
   
    /* output species specific info */
    for (spec_count=0; spec_count < num_specs; spec_count++)
    {
	output_float_or_dist_w_extra(outfile, indent_params, FLOAT_SCIENTIFIC, TRUE, TKS_RSW_MRT,
		string_to_word(specs[spec_count].name, scratch),
			 &dam->species[spec_count].rsw_mort, NULL);

	output_float_or_dist_w_extra(outfile, indent_params, FLOAT_SCIENTIFIC, TRUE, TKS_SPL_MRT,
		string_to_word(specs[spec_count].name, scratch),
			 &dam->species[spec_count].spill_mort, NULL);
	
	output_float_or_dist_w_extra(outfile, indent_params, FLOAT_SCIENTIFIC, TRUE, TKS_SLUICEWAY_PROP,
			string_to_word(specs[spec_count].name, scratch),
			 &dam->species[spec_count].sluiceway_prop, NULL);

	output_float_or_dist_w_extra(outfile, indent_params, FLOAT_SCIENTIFIC, TRUE, TKS_SLUICEWAY_MRT,
		string_to_word(specs[spec_count].name, scratch),
			 &dam->species[spec_count].sluiceway_mort, NULL);

	output_float_or_dist_w_extra(outfile, indent_params, FLOAT_SCIENTIFIC, TRUE, TKS_BY_MRT,
		string_to_word(specs[spec_count].name, scratch),
			 &dam->species[spec_count].bypass_mort, NULL);
	
	output_float_or_dist_w_extra(outfile, indent_params, FLOAT_SCIENTIFIC, TRUE, TKS_TURB_MRT,
		string_to_word(specs[spec_count].name, scratch),
			 &dam->species[spec_count].turbine_mort, NULL);
	
	/*output_float_or_dist_w_extra(outfile, indent_params, FLOAT_SCIENTIFIC, TRUE, TKS_DAY_FGE,
		string_to_word(specs[spec_count].name, scratch),
			 &dam->species[spec_count].day_fge, NULL);

	output_float_or_dist_w_extra(outfile, indent_params, FLOAT_SCIENTIFIC, TRUE, TKS_NT_FGE,
		string_to_word(specs[spec_count].name, scratch),
			 &dam->species[spec_count].night_fge, NULL);*/
	
	output_float_or_dist_w_extra(outfile, indent_params, FLOAT_SCIENTIFIC, TRUE, TKS_TRNS_MRT,
		string_to_word(specs[spec_count].name, scratch),
			 &dam->species[spec_count].transport_mort, NULL);
	
	output_float_w_extra(outfile, indent_params, FLOAT_SCIENTIFIC, TRUE, TKS_DAM_PRED_DENS_FB,
		string_to_word(specs[spec_count].name, scratch),
		     dam->species[spec_count].pred_pop_mean, &dam_species_defaults.pred_pop_mean);
	
	output_float_w_extra(outfile, indent_params, FLOAT_SCIENTIFIC, TRUE, TKS_DAM_PRED_DENS_TR,
		string_to_word(specs[spec_count].name, scratch),
		     dam->species[spec_count].pred_pop_tailrace, &dam_species_defaults.pred_pop_tailrace);
	
	output_float_w_extra(outfile, indent_params, FLOAT_SCIENTIFIC, TRUE, TKS_MEAN_FOREBAY_TRANSIT_TIME,
		string_to_word(specs[spec_count].name, scratch),
		     dam->species[spec_count].mean_forebay_transit_time, &dam_species_defaults.mean_forebay_transit_time);
	
	output_float_w_extra(outfile, indent_params, FLOAT_SCIENTIFIC, TRUE, TKS_SEP_PROB,
		string_to_word(specs[spec_count].name, scratch),
		     dam->species[spec_count].separation_prob, NULL);
	
	OUTPUT_FLOAT_ARRAY("\t\t" TKS_DAM_PRED_PROB " ", string_to_word(specs[spec_count].name, scratch), DAM_TIME_SLC, FLOAT_SCIENTIFIC,
			   dam->species[spec_count].pred, OCT_NO_CONV, 1, &dam_species_defaults.pred[0]);

	if ((error_code=output_equation(outfile, indent_params, TKS_SPILL_EQUATION,
		      string_to_word(specs[spec_count].name, scratch),
					&dam->species[spec_count].spill_eqn, NULL)))
	    return(error_code);

	if ((error_code=output_equation(outfile, indent_params, TKS_FGE_EQN,
		      string_to_word(specs[spec_count].name, scratch),
					&dam->species[spec_count].fge_eqn, &dam_species_defaults.fge_eqn)))
	    return(error_code);

	if ((error_code=output_equation(outfile, indent_params, TKS_TRANS_EQN,
		      string_to_word(specs[spec_count].name, scratch),
					&dam->species[spec_count].trans_eqn, &dam_species_defaults.trans_eqn)))
	    return(error_code);

	if ((error_code=output_equation(outfile, indent_params, TKS_DELAY_EQUATION,
		      string_to_word(specs[spec_count].name, scratch),
					&dam->species[spec_count].delay_eqn, &dam_species_defaults.delay_eqn)))
	    return(error_code);

	if ((error_code=output_equation(outfile, indent_params, TKS_RSW_EQUATION,
		      string_to_word(specs[spec_count].name, scratch),
					&dam->species[spec_count].rsw_eqn, NULL)))
	    return(error_code);
	
	output_float_w_extra(outfile, indent_params, FLOAT_SCIENTIFIC, TRUE, TKS_SPILL_DELAY,
		string_to_word(specs[spec_count].name, scratch),
		     dam->species[spec_count].spill_delay, &dam_species_defaults.spill_delay);
	output_float_w_extra(outfile, indent_params, FLOAT_SCIENTIFIC, TRUE, TKS_BYPASS_DELAY,
		string_to_word(specs[spec_count].name, scratch),
		     dam->species[spec_count].bypass_delay, &dam_species_defaults.bypass_delay);
	output_float_w_extra(outfile, indent_params, FLOAT_SCIENTIFIC, TRUE, TKS_SLUICEWAY_DELAY,
		string_to_word(specs[spec_count].name, scratch),
		     dam->species[spec_count].sluiceway_delay, &dam_species_defaults.sluiceway_delay);
	output_float_w_extra(outfile, indent_params, FLOAT_SCIENTIFIC, TRUE, TKS_TURBINE_DELAY,
		string_to_word(specs[spec_count].name, scratch),
		     dam->species[spec_count].turbine_delay, &dam_species_defaults.turbine_delay);
	output_float_w_extra(outfile, indent_params, FLOAT_SCIENTIFIC, TRUE, TKS_RSW_DELAY,
		string_to_word(specs[spec_count].name, scratch),
		     dam->species[spec_count].rsw_delay, &dam_species_defaults.rsw_delay);

	output_float_w_extra(outfile, indent_params, FLOAT_SCIENTIFIC, TRUE, TKS_SPILL_DAY,
		string_to_word(specs[spec_count].name, scratch),
		     dam->species[spec_count].spill_day, &dam_species_defaults.spill_day);
	output_float_w_extra(outfile, indent_params, FLOAT_SCIENTIFIC, TRUE, TKS_BYPASS_DAY,
		string_to_word(specs[spec_count].name, scratch),
		     dam->species[spec_count].bypass_day, &dam_species_defaults.bypass_day);
	output_float_w_extra(outfile, indent_params, FLOAT_SCIENTIFIC, TRUE, TKS_SLUICEWAY_DAY,
		string_to_word(specs[spec_count].name, scratch),
		     dam->species[spec_count].sluiceway_day, &dam_species_defaults.sluiceway_day);
	output_float_w_extra(outfile, indent_params, FLOAT_SCIENTIFIC, TRUE, TKS_TURBINE_DAY,
		string_to_word(specs[spec_count].name, scratch),
		     dam->species[spec_count].turbine_day, &dam_species_defaults.turbine_day);
	output_float_w_extra(outfile, indent_params, FLOAT_SCIENTIFIC, TRUE, TKS_RSW_DAY,
		string_to_word(specs[spec_count].name, scratch),
		     dam->species[spec_count].rsw_day, &dam_species_defaults.rsw_day);

    } /* species for loop */

    output_float(outfile, 2, FLOAT_FIXED, TRUE, TKS_PWRHS_CAP,
		 dam->powerhouse_capacity, NULL);
    if (dam->phouse_schedule)
    {
	OUTPUT_STRING("\t\t%s%s", TKS_PWRHS_SCHED, 0);
	if ((error_code=output_period_list(outfile, dam->phouse_schedule)))
	    return(error_code);
	OUTPUT_NEWLINE;
    }

	output_float_or_dist(outfile, indent_params, FLOAT_SCIENTIFIC, TRUE, TKS_RSW_SPILL_CAP, &dam->rsw_spill_cap, NULL);
    
    /* output additional powerhouses if present */
    if (dam->additional_pwrhs)
    {
	struct river_segment *phouse;

	/* these two items for main phouse, but only if other phouses present */
	OUTPUT_ITEM("\t\t%s %d\n", TKS_PWRHS_PRIORITY, dam->phouse_priority);

	/* then the other powerhouses */
	for (phouse = dam->additional_pwrhs; phouse; 
	     phouse = phouse->current->dam.additional_pwrhs)
	{
	    output_powerhouse(outfile, phouse);
	}
    }
    return(0);
}

static int output_transport(outfile, dam)
    FILE *outfile;
    struct dam *dam;
{
    char scratch[NAME_LEN]; 
    struct transport *trns = dam->transport;

    OUTPUT_STRING("\t\t%s %s\n", TKS_TRANSPORT, "");

    /* output transport info */
    OUTPUT_ITEM("\t\t\t%s %d\n", TKS_START_BY_DATE, trns->start_by_date);
    OUTPUT_ITEM("\t\t\t%s %d\n", TKS_START_DATE, trns->start_date);
    OUTPUT_ITEM("\t\t\t%s %d\n", TKS_START_COUNT, trns->start_count);
    OUTPUT_ITEM("\t\t\t%s %d\n", TKS_MAX_RESTARTS, trns->max_restarts);
    OUTPUT_ITEM("\t\t\t%s %d\n", TKS_RESTART_BY_DATE, trns->restart_by_date);
    OUTPUT_ITEM("\t\t\t%s %d\n", TKS_RESTART_DATE, trns->restart_date);
    OUTPUT_ITEM("\t\t\t%s %d\n", TKS_RESTART_COUNT, trns->restart_count);
    OUTPUT_ITEM("\t\t\t%s %d\n", TKS_END_DATE, trns->end_date);
    OUTPUT_ITEM("\t\t\t%s %d\n", TKS_END_COUNT, trns->end_count);
    OUTPUT_ITEM("\t\t\t%s %d\n", TKS_NUM_LOW_DAYS, trns->num_low_days);
    output_float(outfile, 3, FLOAT_DEFAULT, TRUE, TKS_HFL_PASSED,
		 trns->hfl_passed_perc, NULL);
    output_float(outfile, 3, FLOAT_DEFAULT, TRUE, TKS_HI_FLOW,
		 trns->hi_flow, NULL);
    if (trns->hfl_spec) {OUTPUT_ITEM("\t\t\t%s %s\n", TKS_HFL_SPEC, string_to_word(trns->hfl_spec->name, scratch));}
	else log_msg(L_WARNING, "There was an error writing transport hfl_spec\n");
    output_float(outfile, 3, FLOAT_DEFAULT, TRUE, TKS_RATE, trns->rate, NULL);
    if (trns->target_seg) {OUTPUT_ITEM("\t\t\t%s %s\n", TKS_RLS_PT, string_to_word(trns->target_seg->seg_name, scratch));}
	else log_msg(L_WARNING, "There was an error writing transport target_seg->seg_name\n");

    if (trns->source_seg) {OUTPUT_END("\t\t", TKS_TRANSPORT, string_to_word(trns->source_seg->seg_name, scratch));}
	else log_msg(L_WARNING, "There was an error writing transport source_seg->seg_name\n");
    return(0);
}

static int output_release(FILE *outfile, struct release *rel)
{
    char scratch[NAME_LEN]; 

    /* format: "release species site date" */
    OUTPUT_STRING("\t%s %s ", TKS_RLS, string_to_word(rel->species->name, scratch));
    OUTPUT_ITEM("%s %d\n", string_to_word(rel->site->name, scratch), rel->start_date);
    
    OUTPUT_STRING("\t\t%s %s\n", TKS_STOCK, string_to_word(rel->stock->name, scratch));
    if (rel->smolt_onset != release_defaults.smolt_onset) OUTPUT_ITEM ("\t\t%s %d\n", TKS_RLS_SMOLT_ON, rel->smolt_onset);
    if (rel->smolt_finish != release_defaults.smolt_finish) OUTPUT_ITEM ("\t\t%s %d\n", TKS_RLS_SMOLT_OFF, rel->smolt_finish);
    output_float(outfile, 2, FLOAT_DEFAULT, TRUE, TKS_RLS_VIT,
		 rel->vitality_at_release, &release_defaults.vitality_at_release);
    output_float(outfile, 2, FLOAT_DEFAULT, TRUE, TKS_RLS_VIT_CHANGE,
		 rel->vitality_change_rate, &release_defaults.vitality_change_rate);
    output_float(outfile, 2, FLOAT_DEFAULT, TRUE, TKS_INITIAL_SPILL_EXPERIENCE,
		 rel->initial_spill_experience, &release_defaults.initial_spill_experience);
    output_float(outfile, 2, FLOAT_DEFAULT, TRUE, TKS_RLS_LENGTH,
		 rel->length, &release_defaults.length);
    OUTPUT_FLOAT_ARRAY ("\t\t", TKS_RLS_NUM, DAYS_IN_RUN, FLOAT_INTEGER,
			rel->number, OCT_SUM, STEPS_PER_DAY, NULL);

#ifdef REALTIME
    if (rel->rt != NULL) {
    	OUTPUT_STRING("\t\t%s %s\n", TKS_RT_STOCK, string_to_word(rel->rt->name, scratch));
    	OUTPUT_ITEM("\t\t%s %d\n", TKS_RT_DATA_DATE, rel->rt->data_date);
    }
#endif
    
    OUTPUT_END ("\t", TKS_RLS, string_to_word(rel->species->name, scratch));

    return(0);
}

static int output_river_parameters(outfile, run_count)
    FILE *outfile;
    int run_count;
{
    char scratch[NAME_LEN];

    output_float(outfile, 1, FLOAT_DEFAULT, TRUE, TKS_GAS_DIS_EXP,
		 run[run_count].gas_dissp_exp, NULL);
    output_float(outfile, 1, FLOAT_DEFAULT, TRUE, TKS_FLOW_PROP,
		 run[run_count].flow_proportion, NULL);
    output_float(outfile, 1, FLOAT_SCIENTIFIC, TRUE, TKS_UFREE,
		 run[run_count].Ufree, NULL);
    output_float(outfile, 1, FLOAT_SCIENTIFIC, TRUE, TKS_PREY_ENERGY_DENSITY,
		 run[run_count].prey_energy_density, NULL);
    output_float(outfile, 1, FLOAT_SCIENTIFIC, TRUE, TKS_LENGTH_WEIGHT_B0,
		 run[run_count].length_weight_b0, NULL);
    output_float(outfile, 1, FLOAT_SCIENTIFIC, TRUE, TKS_LENGTH_WEIGHT_B1,
		 run[run_count].length_weight_b1, NULL);
    output_float(outfile, 1, FLOAT_SCIENTIFIC, TRUE, TKS_FORK_THRESHOLD,
		 run[run_count].fork_threshold, NULL);
    OUTPUT_ITEM("\t%s %s\n", TKS_WATER_TT_UPPER,
		string_to_word(run[run_count].water_travel.upper->seg_name,
			       scratch));
    OUTPUT_ITEM("\t%s %s\n", TKS_WATER_TT_LOWER,
		string_to_word(run[run_count].water_travel.lower->seg_name,
			       scratch));
    OUTPUT_ITEM("\t%s %d\n", TKS_WATER_TT_FIRST_DAY,
		run[run_count].water_travel.first_day);
    OUTPUT_ITEM("\t%s %d\n", TKS_WATER_TT_LAST_DAY,
		run[run_count].water_travel.last_day);

    return (0);
}

/* 
 * output post-Bonneville species parameters
 */
static int output_post_bonneville_spec(outfile, spec)
    FILE *outfile;
    struct species *spec;
{
	int error_code = 0;

	output_float_or_dist(outfile, 1, FLOAT_SCIENTIFIC, TRUE, TKS_INRIVER_LATENT_MORT, &spec->inriver_latent_mort, NULL);
	output_float_or_dist(outfile, 1, FLOAT_SCIENTIFIC, TRUE, TKS_TRANSPORT_LATENT_MORT, &spec->transport_latent_mort, NULL);

	output_float_or_dist(outfile, 1, FLOAT_SCIENTIFIC, TRUE, TKS_DIFFERENTIAL_RETURN, &spec->differential_return, NULL);

    if ((error_code=output_equation(outfile, 1, TKS_INRIVER_LATENT_MORT_EQUATION, NULL, &spec->inriver_latent_mort_eqn, NULL)))
	return(error_code);

	return(0);
}
static int output_post_bonneville(outfile, run_count)
    FILE *outfile;
	int run_count;
{
    extern struct species *specs;

    int count;
    int error_code=0;   /* used to check for errors in subprocesses */
    char scratch[NAME_LEN];

	/* Hypothesis */
    OUTPUT_ITEM("%s %s\n\n", TKS_POST_BONNEVILLE_HYPOTHESIS,
		post_bonneville_hypothesis_names[run[run_count].post_bonneville_hypothesis]);

    for (count=0; count < num_specs; count++)
    {
	OUTPUT_STRING("%s %s\n", TKS_SPEC,
		      string_to_word(specs[count].name, scratch));
	if ( (error_code=output_post_bonneville_spec(outfile, &specs[count])) )
	    return (error_code);
	OUTPUT_END("", TKS_SPEC, string_to_word(specs[count].name, scratch));
    } /* species loop */

    return (0);
}


static int output_run_flags(outfile, run_count)
    FILE *outfile;
    int run_count;
{
    char scratch[NAME_LEN];

    OUTPUT_ITEM("\t%s %d\n", TKS_RUN_FLAGS,
		(run[run_count].flags & ALL_RUN_FLAGS));
    OUTPUT_ITEM("\t%s %s\n", TKS_MORTALITY_CLASS,
		(mortality_class_names[run[run_count].mortality_class]));
    if (run[run_count].flow_seg)
	OUTPUT_ITEM("\t%s %s\n", TKS_FLOW_SEG,
		    string_to_word(run[run_count].flow_seg->seg_name,
				   scratch));
    OUTPUT_NEWLINE;

    return (0);
}

/* this one is a little odd, because it needs to be called for both
 * reaches and dams.  every one.
 */
static int output_monte_flags(outfile, seg)
    FILE *outfile;
    struct river_segment *seg;
{
    OUTPUT_ITEM("\t\t%s %d\n", TKS_OUTPUT_SETTINGS, seg->output_settings);
    return(0);
}

/*---------------------------------------------------------------------------*
 *									     *
 *  Multi-File Output Section						     *
 *									     *
 *  The following are the routines for outputting the data in the multi-file *
 *  format.                                                                  *
 *									     *
 *  To assure that all the data is output by this group of routines, each of *
 *  the routines in the Low-Level Data section (immediately above) should be *
 *  called exactly once.						     *
 * 									     *
 *---------------------------------------------------------------------------*/

int output_all_reaches (outfile)
    FILE *outfile;
{
    char scratch[NAME_LEN];
    int error_code=0;   /* used to check for errors in subprocesses */

    struct river_segment *seg;
    int run_count=0;

    /* start with a run, for that run, traverse
       the river */
    
#if 0
    for (run_count=0; /* run_count < NRUNS */ run_count < 1; run_count++)
    {
	OUTPUT_ITEM("%s %d\n", TKS_RUN, run_count); /* use the name later */
#else
	run_count = 0;
#endif

	for (seg=river_mouth; seg; seg=seg->next)
	    if (seg->seg_type == REACH_SEG)
	    {
		OUTPUT_STRING("\t%s %s\n", TKS_REACH, string_to_word(seg->seg_name, scratch));
		if ( (error_code=output_reach(outfile, &(seg->run[run_count].reach))) )
		    return (error_code);
		OUTPUT_END("\t", TKS_REACH, string_to_word(seg->seg_name, scratch));
	    }

#if 0
	OUTPUT_ITEM("end %s (%d)\n", TKS_RUN, run_count); /* use the name later, and use output_end */
    }
#endif
    
    return (error_code);
}

int output_all_spill (outfile)
    FILE *outfile;
{
    char scratch[NAME_LEN];
    int error_code=0;   /* used to check for errors in subprocesses */

    struct river_segment *seg;
    int run_count=0;

    /* start with a run, for that run, traverse
       the river */
#if 0
    for (run_count=0; /* run_count < NRUNS */ run_count < 1; run_count++)
    {
	OUTPUT_ITEM("%s %d\n", TKS_RUN, run_count); /* use the name later */
#else
	run_count = 0;
#endif
	for (seg=river_mouth; seg; seg=seg->next)
	    if (seg->seg_type == DAM_SEG)
	    {
		OUTPUT_STRING("\t%s %s\n", TKS_DAM, string_to_word(seg->seg_name, scratch));
		if ( (error_code=output_spill(outfile, &(seg->run[run_count].dam))) )
		    return (error_code);
		OUTPUT_END("\t", TKS_DAM, string_to_word(seg->seg_name, scratch));
	    }

#if 0
	OUTPUT_ITEM("end %s (%d)\n", TKS_RUN, run_count); /* use the name later, and use output_end */
    }
#endif

    return (error_code);
}

int output_all_flow (outfile)
    FILE *outfile;
{
    char scratch[NAME_LEN];
    int error_code=0;   /* used to check for errors in subprocesses */

    struct river_segment *seg;
    int run_count=0;

#if 0
    /* start with a run, for that run, traverse
       the river */
    for (run_count=0; /* run_count < NRUNS */ run_count < 1; run_count++)
    {
	OUTPUT_ITEM("%s %d\n", TKS_RUN, run_count); /* use the name later */
#else
	run_count = 0;
#endif

	for (seg=river_mouth; seg; seg=seg->next) {
	    switch (seg->seg_type)
	    {
	    case DAM_SEG:
		    OUTPUT_STRING("\t%s %s\n", TKS_DAM,
				  string_to_word(seg->seg_name, scratch));
		    if ( (error_code=output_flow_dam (outfile,
				&(seg->run[run_count].dam))) )
			return (error_code);
		    OUTPUT_END("\t", TKS_DAM,
			       string_to_word(seg->seg_name, scratch));
		break; 
	    case REACH_SEG:
		OUTPUT_STRING("\t%s %s\n", TKS_REACH,
			      string_to_word(seg->seg_name, scratch));
		if ( (error_code =
		      output_flow_reach(outfile, &(seg->run[run_count].reach))))
		    return (error_code);
		OUTPUT_END("\t", TKS_REACH,
			   string_to_word(seg->seg_name, scratch));
		break;
	    case HEADWATER_SEG:
		OUTPUT_STRING("\t%s %s\n", TKS_HDWTR,
			      string_to_word(seg->seg_name, scratch));
		if ( (error_code =
		      output_flow_headwater(outfile,
				    &(seg->run[run_count].headwater))) )
		    return (error_code);
		OUTPUT_END("\t", TKS_HDWTR,
			   string_to_word(seg->seg_name, scratch));
		break;
	    case POWERHOUSE_SEG:
		break;
	    default:
		log_msg(L_ERROR,
			"ERROR: unknown segment type in output_crisp_data\n");
		break;
	    } /* end of switch */
	}

#if 0
	OUTPUT_ITEM("end %s (%d)\n", TKS_RUN, run_count); /* use the name later, and use output_end */
    }
#endif

    return (error_code);
}

int output_all_dam (outfile)
    FILE *outfile;
{
    char scratch[NAME_LEN];
    int error_code=0;   /* used to check for errors in subprocesses */

    struct river_segment *seg;
    int run_count=0;

#if 0
    /* start with a run, for that run, traverse
       the river */
    for (run_count=0; /* run_count < NRUNS */ run_count < 1; run_count++)
    {
	OUTPUT_ITEM("%s %d\n", TKS_RUN, run_count); /* use the name later */
#else
	run_count = 0;
#endif

	for (seg=river_mouth; seg; seg=seg->next)
	    if (seg->seg_type == DAM_SEG)
	    {
		struct dam *damptr = &(seg->run[run_count].dam);

		OUTPUT_STRING("\t%s %s\n", TKS_DAM,
			      string_to_word(seg->seg_name, scratch));

		/* output dam file info */
		if ( (error_code=output_dam(outfile, damptr)))
		    return (error_code);

		/* output transport if present */
		if (damptr->transport)
		    if ( (error_code=output_transport(outfile, damptr)))
			return (error_code);

		OUTPUT_END("\t", TKS_DAM, string_to_word(seg->seg_name,
							 scratch));
	    }

#if 0
	/* use the name later, and use output_end */
	OUTPUT_ITEM("end %s (%d)\n", TKS_RUN, run_count);

    }
#endif

    return (error_code);
}

int output_river (outfile)
    FILE *outfile;
{
    int run_count=0;	/* a misc. counter */
    char scratch[NAME_LEN];
    int error_code=0;   /* used to check for errors in subprocesses */

    struct river_segment *seg;
    
#if 0
    /* start with a run, for that run, output all river data,
     * including headwater info
     */
    for (run_count=0; /* run_count < NRUNS */ run_count < 1; run_count++)
    {
	OUTPUT_ITEM("%s %d\n", TKS_RUN, run_count); /* use the name later */
#else
	run_count = 0;
#endif

	if ( (error_code=output_river_parameters(outfile, run_count)) )
	    return(error_code);

	for (seg=river_mouth; seg; seg=seg->next)
	    if (seg->seg_type == HEADWATER_SEG)
	    {
		OUTPUT_STRING("\t%s %s\n", TKS_HDWTR,
			      string_to_word(seg->seg_name, scratch));
    
		/* output headwater info */
		if ((error_code =
		     output_headwater(outfile,
				      &(seg->run[run_count].headwater))) )
		    return (error_code);

		OUTPUT_END("\t", TKS_HDWTR,
			   string_to_word(seg->seg_name, scratch));
	    }

#if 0
	OUTPUT_ITEM("end %s (%d)\n", TKS_RUN, run_count); /* use the name later, and use output_end */
    }
#endif

    return (error_code);
}

int output_reach_classes (outfile)
	FILE *outfile;
{
    int run_count=0;	/* a misc. counter */
    char scratch[NAME_LEN];
    int error_code=0;   /* used to check for errors in subprocesses */

    struct river_segment *seg;

#if 0
    for (run_count=0; /* run_count < NRUNS */ run_count < 1; run_count++)
    {
	OUTPUT_ITEM("%s %d\n", TKS_RUN, run_count); /* use the name later */
#else
	run_count = 0;
#endif

	if ( (error_code=output_reach_class_names(outfile)) )
	return(error_code);
	for (seg=river_mouth; seg; seg=seg->next)
	{
	    if (seg->seg_type == REACH_SEG)
	    {
		OUTPUT_STRING("\t%s %s\n", TKS_REACH,
			      string_to_word(seg->seg_name, scratch));
		OUTPUT_STRING("\t\t%s %s\n", TKS_REACH_CLASS,
			      reach_class_names[seg->current->reach.rclass] );
    
		if ((error_code = output_monte_flags(outfile, seg)))
		    return (error_code);

		OUTPUT_END("\t", TKS_REACH,
			   string_to_word(seg->seg_name, scratch));
	    }
	}
#if 0
	OUTPUT_ITEM("end %s (%d)\n", TKS_RUN, run_count); 
    }
#endif

    return (error_code);
}

int output_config (outfile)
    FILE *outfile;
{
    int run_count=0;	/* a misc. counter */
    char scratch[NAME_LEN];
    int error_code=0;   /* used to check for errors in subprocesses */

    struct river_segment *seg;
    
    /* output the overall run flags, then the monte flags in each seg.
     *  Also output reach class definitions and class of each reach.
     */

#if 0
    for (run_count=0; /* run_count < NRUNS */ run_count < 1; run_count++)
    {
	OUTPUT_ITEM("%s %d\n", TKS_RUN, run_count); /* use the name later */
#else
	run_count = 0;
#endif

	for (seg=river_mouth; seg; seg=seg->next)
	{
	    if (seg->seg_type == REACH_SEG)
	    {
		OUTPUT_STRING("\t%s %s\n", TKS_REACH,
			      string_to_word(seg->seg_name, scratch));
		OUTPUT_STRING("\t\t%s %s\n", TKS_REACH_CLASS,
			      reach_class_names[seg->current->reach.rclass] );
    
		if ((error_code = output_monte_flags(outfile, seg)))
		    return (error_code);

		OUTPUT_END("\t", TKS_REACH,
			   string_to_word(seg->seg_name, scratch));
	    }
	    else if (seg->seg_type == DAM_SEG)
	    {
		OUTPUT_STRING("\t%s %s\n", TKS_DAM,
			      string_to_word(seg->seg_name, scratch));
    
		if ((error_code = output_monte_flags(outfile, seg)))
		    return (error_code);

		OUTPUT_END("\t", TKS_DAM,
			   string_to_word(seg->seg_name, scratch));
	    }
	    else if (seg->seg_type == HEADWATER_SEG)
	    {
		OUTPUT_STRING("\t%s %s\n", TKS_HDWTR,
			      string_to_word(seg->seg_name, scratch));
    
		if ((error_code = output_monte_flags(outfile, seg)))
		    return (error_code);

		OUTPUT_END("\t", TKS_HDWTR,
			   string_to_word(seg->seg_name, scratch));
	    }
	}
#if 0
	OUTPUT_ITEM("end %s (%d)\n", TKS_RUN, run_count); 
    }
#endif

    return (error_code);
}

int output_all_rls (outfile)
    FILE *outfile;
{
    int error_code=0;   /* used to check for errors in subprocesses */

    struct release *rel;
    int run_count=0;

#if 0
    /* start with a run, for that run, output
       all releases */
    for (run_count=0; /* run_count < NRUNS */ run_count < 1; run_count++)
    {
	OUTPUT_ITEM("%s %d\n", TKS_RUN, run_count); /* use the name later */
#else
	run_count = 0;
#endif

    	for (rel=run[run_count].release_list; rel; rel=rel->next)
	{
	    if ( (error_code=output_release (outfile, rel)) ) return (error_code);
	}

#if 0
	OUTPUT_ITEM("end %s (%d)\n", TKS_RUN, run_count); /* use the name later, and use output_end */
    }
#endif

    return (error_code);
}
/*---------------------------------------------------------------------------*
 *									     *
 *  Single-File Output Section						     *
 *									     *
 *  The following are the routines for outputting the data in the format     *
 *  that keeps all the data for a section of the river in one section of the *
 *  file, and only writes out one huge file.				     *
 * 									     *
 *  To assure that all the data is output by this group of routines, each of *
 *  the routines in the Low-Level Data section (above) should be 	     *
 *  called exactly once.						     *
 *									     *
 *---------------------------------------------------------------------------*/

static int output_entire_reach(outfile, seg, run_count)
    FILE *outfile;
    struct river_segment *seg;
    int run_count;
{
    int error_code=0;		/* used to check for errors in subprocesses */
    char scratch[NAME_LEN];
    
    OUTPUT_STRING("\t%s %s\n", TKS_REACH,
		  string_to_word(seg->seg_name, scratch));
    
	OUTPUT_STRING("\t\t%s %s\n", TKS_REACH_CLASS,
			  reach_class_names[seg->current->reach.rclass] );

    if ( (error_code=output_monte_flags(outfile, seg)))
	return (error_code);

    if ( (error_code=output_reach(outfile, &(seg->run[run_count].reach))) )
	return (error_code);

    if ( (error_code=output_flow_reach(outfile, &(seg->run[run_count].reach))) )
 	return (error_code);
 
    
    OUTPUT_END("\t", TKS_REACH, string_to_word(seg->seg_name, scratch));

    return(0);
}	


static int output_entire_headwater(outfile, seg, run_count)
    FILE *outfile;
    struct river_segment *seg;
    int run_count;
{
    int error_code=0;   /* used to check for errors in subprocesses */
    char scratch[NAME_LEN];

    OUTPUT_STRING("\t%s %s\n", TKS_HDWTR,
		  string_to_word(seg->seg_name, scratch));

    /* monte flags first */
    if ( (error_code=output_monte_flags(outfile, seg)))
	return (error_code);
    
    if ( (error_code=output_flow_headwater(outfile,
					   &(seg->run[run_count].headwater))) )
	return (error_code);
    if ( (error_code=output_headwater(outfile,
				      &(seg->run[run_count].headwater))) )
	return (error_code);

    OUTPUT_END("\t", TKS_HDWTR, string_to_word(seg->seg_name, scratch));

    return(0);
}


static int output_entire_dam(outfile, seg, run_count)
    FILE *outfile;
    struct river_segment *seg;
    int run_count;
{
    int error_code=0;   /* used to check for errors in subprocesses */
    char scratch[NAME_LEN];

    OUTPUT_STRING("\t%s %s\n", TKS_DAM, string_to_word(seg->seg_name, scratch));

    /* monte flags first */
    if ( (error_code=output_monte_flags(outfile, seg)))
	return (error_code);
    
    /* output dam file info */
    if ( (error_code=output_dam(outfile, &(seg->run[run_count].dam))) )
	return (error_code);

    /* output transport if present */
    if (seg->run[run_count].dam.transport)
	if ( (error_code=output_transport(outfile, &(seg->run[run_count].dam))))
	    return (error_code);

    /* output spill */
    if ( (error_code=output_spill(outfile, &(seg->run[run_count].dam))) )
	return (error_code);

    /* output flow */
    if ( (error_code=output_flow_dam(outfile, &(seg->run[run_count].dam))) )
 	return (error_code);
    
    OUTPUT_END("\t", TKS_DAM, string_to_word(seg->seg_name, scratch));

    return(0);
} /* end of Output_entire_Dam; */

static int output_entire_river(outfile, run_count)
    FILE *outfile;
    int run_count;
{
    int error_code=0;   /* used to check for errors in subprocesses */
    struct river_segment *seg;

    /* start at river mouth and traverse the structure */
    for (seg=river_mouth; seg; seg=seg->next)
    {
	switch (seg->seg_type)
	{
	case DAM_SEG:
	    if ( (error_code=output_entire_dam (outfile, seg, run_count)) ) return (error_code);
	    break; 
	case REACH_SEG:
	    if ( (error_code=output_entire_reach (outfile, seg, run_count)) ) return (error_code);
	    break;
	case HEADWATER_SEG:
	    if ( (error_code=output_entire_headwater (outfile, seg, run_count)) ) return (error_code);
	    break;
	case POWERHOUSE_SEG:
	    /* handled in output_dam() */
	    break;
	default:
	    log_msg(L_ERROR,
		    "ERROR: unknown segment type in output_crisp_data\n");
	    break;
	} /* end of switch */
    } /* end of traversal of the river_segment data structure */
    return (error_code);
}    

static int output_entire_release_list(outfile, run_count)
    FILE *outfile;
    int run_count;
{
    int error_code=0;   /* used to check for errors in subprocesses */
    struct release *rel;
    
    for (rel=run[run_count].release_list; rel; rel=rel->next)
    {
	if ( (error_code=output_release (outfile, rel)) ) return (error_code);
    }
    return (0);
}

/*
 * int output_all_runs(FILE *outfile)
 */
int output_all_runs(outfile)
    FILE *outfile;
{
    int run_count=0; 	/* misc. counters */
    int error_code=0;   /* used to check for errors in subprocesses */

#if 0
    /* start with a run, for that run, traverse
       the river and the run structures.   */
    for (run_count=0; /* run_count < NRUNS */ run_count < 1; run_count++)
    {
	OUTPUT_ITEM("%s %d\n", TKS_RUN, run_count); /* use the name later */
#else
	run_count = 0;
#endif

	if ( (error_code=output_entire_river(outfile, run_count)) )
	    return (error_code);

	if ( (error_code=output_river_parameters(outfile, run_count)) )
	    return(error_code);
	
	if ( (error_code=output_run_flags(outfile, run_count)) )
	    return(error_code);

	if ( (error_code=output_entire_release_list(outfile, run_count)) )
	    return (error_code);
	
#if 0
	OUTPUT_ITEM("end %s (%d)\n", TKS_RUN, run_count);
    } /* run loop */
#endif

    return (0);
}

int output_file_data (outfile)
    FILE *outfile;
{
    int error_code=0;   /* used to check for errors in subprocesses */

	/* If we don't have a file creation date, then make one, otherwise update modification date */
    if (!file_creation_date) {
		time_t tt = time(NULL);
		struct tm *t = localtime(&tt);
		file_creation_date = (char*) malloc(sizeof(char) * MAX_TOKEN_LEN);
		strftime(file_creation_date, MAX_TOKEN_LEN, "%Y-%m-%d", t);
	}
	else {
		/* Update modification date */
		if (file_modification_date) free(file_modification_date);

		{
			time_t tt = time(NULL);
			struct tm *t = localtime(&tt);
			file_modification_date = (char*) malloc(sizeof(char) * MAX_TOKEN_LEN);
			strftime(file_modification_date, MAX_TOKEN_LEN, "%Y-%m-%d", t);
		}
	}
	OUTPUT_ITEM("%s %s\n", TKS_FILE_CREATION_DATE, file_creation_date);
    if (file_creator) OUTPUT_ITEM_ESCAPED("%s \"%s\"\n", TKS_FILE_CREATOR, file_creator);

	if (file_modification_date) OUTPUT_ITEM("%s %s\n", TKS_FILE_MODIFICATION_DATE, file_modification_date);
    if (file_modifier) OUTPUT_ITEM_ESCAPED("%s \"%s\"\n", TKS_FILE_MODIFIER, file_modifier);

	/* Output escaped notes section */
    if (input_notes) OUTPUT_ITEM_ESCAPED("%s \"%s\"\n", TKS_NOTES, input_notes);

	return error_code;
}

/*
 * int output_crisp_data(FILE *outfile) -- Routine which outputs crisp data
 * 	in a single file.
 *	Returns 0 if successfull, errno otherwise
 */
int output_crisp_data (outfile)
    FILE *outfile;
{
    int error_code=0;   /* used to check for errors in subprocesses */

    /* Output file data. */
    if ( (error_code=output_file_data(outfile)) ) return (error_code);

    /* Output reach class names. */
    if ( (error_code=output_reach_class_names(outfile)) ) return (error_code);

    /* output species data */
    if ( (error_code=output_all_species(outfile, TRUE)) ) return (error_code);

    /* output stock data */
    if ( (error_code=output_all_stocks(outfile, TRUE)) ) return (error_code);

	/* output post-Bonneville hypothesis and related parameters */
	if ( (error_code=output_post_bonneville(outfile, 0)) ) return (error_code);

    /* output run data */
    if ( (error_code=output_all_runs(outfile)) ) return (error_code);

    return (error_code);
} /* output fcn */

int	output_version(FILE *outfile)
{
	int error_code=0;

    OUTPUT_ITEM("%s %d\n", TKS_VERSION, CURRENT_OUTFILE_VERSION);

	return error_code;
}


/* These are the main file output routines for writing out the
 * separate post-Bonneville, calibration, dam operations, river environment,
 * and etc output files. */
int write_river_environment_file( char *fname ) {
	int err;
	FILE *fp = fopen( fname, "w" );
	if (fp == NULL)
		return -1;
	write_header( fp, "COMPASS River Environment File", fname );
	err = output_version(fp);
	err |= output_all_flow( fp );
	err |= output_river( fp );
	fclose( fp );
	return err;
}
int write_operations_file( char *fname ) {
	int err;
	FILE *fp = fopen( fname, "w" );
	if (fp == NULL)
		return -1;
	write_header( fp, "COMPASS Operations File", fname );
	err = output_version(fp);
	err |= output_all_spill( fp );
	err |= output_all_dam( fp );
	fclose( fp );
	return err;
}
int write_calibration_file( char *fname ) {
	int err;
	FILE *fp = fopen( fname, "w" );
	if (fp == NULL)
		return -1;
	write_header( fp, "COMPASS Calibration File", fname );
	err = output_version(fp);
	err |= output_reach_classes( fp );
	err |= output_all_specs_and_stocks( fp );
	err |= output_all_reaches( fp );
	err |= output_all_rls( fp );
	err |= output_run_flags( fp, 0 );
	fclose( fp );
	return err;
}
int write_post_bonneville_file(char *fname) {
	int err;
	FILE *fp = fopen( fname, "w" );
	if (fp == NULL)
		return -1;
	write_header( fp, "COMPASS Post-Bonneville File", fname );
	err = output_version(fp);

	err |= output_post_bonneville(fp, 0);

	fclose( fp );
	return err;
}
int write_etc_file( char *fname ) {
	int err;
	FILE *fp = fopen( fname, "w" );
	if (fp == NULL)
		return -1;
	write_header( fp, "COMPASS Configuration File", fname );
	err = output_version(fp);
	err |= output_config( fp );
	fclose( fp );
	return err;
}

int write_data_file( char *fname ) {
	int err;
	FILE *fp = fopen( fname, "w" );
	if (fp == NULL)
		return -1;
	write_header( fp, "COMPASS Data File", fname );
	err = output_version(fp);
	err |= output_crisp_data( fp );
	fclose( fp );
	return err;
}
