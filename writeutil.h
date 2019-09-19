/** \file
 * \brief Utility functions for writing out information.
 *
 * Note that this module makes heavy use of macros to save typing (hopefully). */

#ifndef CRISP_WRITEUTIL_H
#define CRISP_WRITEUTIL_H

#include <stdio.h>
#include "log.h"
#include "token.h"
#include <stdlib.h>

typedef enum output_conversion_type { OCT_NO_CONV, OCT_SUM, OCT_AVERAGE, OCT_DAM_DAY, OCT_DAM_NIGHT }
oconv;

enum IndentType { INDENT_CONTINUATION = -1 };

enum FloatType {
	FLOAT_DEFAULT,
	FLOAT_INTEGER,
	FLOAT_FIXED,
	FLOAT_PRECISE, /* This indicates a double-precision floating point number, so any default value MUST be a double! */
	FLOAT_SCIENTIFIC /* Single-precision floating point number */
};

#ifdef __cplusplus
extern "C" {
#endif

extern int output_all; /* Whether or not everything should be written out (even extraneous values) */

int write_output_summary(int game, int year, int alt);
    
extern int output_float_array (FILE *outfile, const char *prefix, const char *name,
								 int size, float *array, 
								 oconv conversion, unsigned mult, 
								 enum FloatType output_type, float *default_value);
extern int write_header(FILE *outfile, char *title, char *filename);
extern void log_msg_float_array (int size, float *array);
extern void log_raw_float_array (int size, float *array);
extern void log_float_array (int logtype, int size, float *array,
								 oconv conversion, unsigned mult);
extern void perror_msg( char *txt );

/* To prevent quotation marks from appearing, use these functions for
 * escaping " into &quot; and \n into &nl; and vice versa.
 * These functions return newly allocated strings. */
extern char* strdup_escape(const char *str);
extern char* strdup_unescape(const char *str);

#ifdef __cplusplus
}
#endif
    

/*===========================================================================*/
/*---------------------------------------------------------------------------*
 *									     *
 * The following definitions make outputting data and changing the format of *
 * the output easier.                                                        *
 *									     *
 * NOTE:								     *
 * The definitions assume that there is a variable called "outfile" defined, *
 * and that that variable is a valid open file stream.                       *
 *---------------------------------------------------------------------------*/

/** OUTPUT ERROR is called by the other routines whenever there is an error
 * outputting information.  It uses "perror" to print the last error
 * encountered, and then calls "return" with the error number as a parameter.
 */

#define OUTPUT_ERROR { perror_msg ("output_crisp_data"); return (errno); }

/** OUTPUT ITEM prints a generic item and checks for any errors.  format is of
 * the type used by printf.  name is a null terminated string.  item is a
 * number, character, or string.  An example of use is:
 * OUTPUT_ITEM(" %s %f", "variable is: ", float_var); where float_var=1.0
 * the output from the example would be " variable is: 1.000000"
 */

#define OUTPUT_ITEM(format, name, item) \
{if (outfile){if (fprintf(outfile, format, name, item) == EOF) OUTPUT_ERROR;}}

/* This should only be used for outputting strings, but it should generate
 * compiler errors any other way. */
#define OUTPUT_ITEM_ESCAPED(format, name, item) \
{if (outfile){char *esc = strdup_escape(item); if (esc) {if (fprintf(outfile, format, name, esc) == EOF) OUTPUT_ERROR; free(esc);}}}

/** OUTPUT_STRING is very similar to OUTPUT ITEM, except that it checks
 * "string" for a null value and substitutes a " " if the test is true.
 * format is of the type used by printf, name and string are both null
 * terminated strings.
 */

#define OUTPUT_STRING(format, name, string) \
{if (outfile){if (fprintf(outfile, format, name, (string==0 ? " " : string)) == EOF) OUTPUT_ERROR;}}

/** OUTPUT END is defined because we are not currently certain what the
 * format of the "end" statement will be.  Currently it is of the form
 * "     end category (name)" followed by a blank line.  This
 * definition makes it easy to change.  prefix is the amount of space to
 * put before the "end"
 */

#define OUTPUT_END(prefix, name, string) \
{if (outfile){if (fprintf(outfile, "%send %s (%s)\n\n", prefix, name, \
	     (string==0 ? " " : string)) == EOF) OUTPUT_ERROR;}}

/** OUTPUT NEWLINE is a generic way to add blank lines to the output file.
 * An example of it's use is: OUTPUT_NEWLINE;  which would add one blank
 * line to the file
 */

#define OUTPUT_NEWLINE \
{if (outfile){if (fprintf(outfile, "\n") == EOF) OUTPUT_ERROR;}}

/** OUTPUT FLOAT ARRAY outputs a floating point array 4 numbers to a line.
 * prefix is the amount of space to put before the 'name' of the variable
 * (the first line).  Currently each line except the first is indented
 * three tab stops. name is a null terminated character string.
 * size is an integer denoting the number of elements in the array, and
 * array is the floating point array.
 */

#define OUTPUT_FLOAT_ARRAY(prefix, name, size, float_type, array, conversion, multiplier, default_value)\
{if (outfile){\
    output_float_array(outfile, prefix, name, size, array, conversion, multiplier, float_type, default_value);\
}}\

#define CURRENT_OUTFILE_VERSION	9
    
#endif /* WRITEUTIL_H */

