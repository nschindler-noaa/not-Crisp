/** \file
 * \brief Utility functions for writing out data. */

#include <stdlib.h>
#include <string.h>
#ifdef WIN32
#include <memory.h>
#include <errno.h>
#include <stddef.h>
#include <time.h>
#include <sys/types.h>
#include <io.h>
#else
#include <unistd.h>
#include <strings.h>
#include <memory.h>
#include <errno.h>
#include <netdb.h>
#include <stddef.h>
#include <time.h>
#include <sys/types.h>
#include <pwd.h>
#endif

#include "writeutil.h"
#include "crisp.h"	/* for the program version */
#include "parse.h"
#include "parseutil.h"
#include "dataio.h"
#include "crisp_time.h"

/************************************************************************
 *									*
 *  Output routines							*
 *									*
 ***********************************************************************/

#ifndef F_OK
#define F_OK  4   /* needed by PC */
#endif

int output_all = 0;

/** quick and dirty routine to open a file */

int open_outfile(fpp, fn)
    FILE **fpp;
    char *fn;
{
    /* open file for writing; log message if overwriting */
    if (! access(fn, F_OK))
	log_msg(L_WARNING, "file %s already exists; overwriting...\n", fn);
    
    if ( ! (*fpp = fopen(fn, "w")))
    {
	log_msg(L_ERROR, "cannot write to output file %s\n", fn);
	return 1;
    }
    else return 0;
}

static float float_array_value(array, index, conversion, mult)
	float array[];
	int index;
	oconv conversion;
	unsigned mult;
{
    int i;
    float value;

    switch (conversion) {
      case OCT_NO_CONV:
	return array[index];
	break;
      case OCT_SUM:
	value = 0.0;
	for (i = index++ * mult ; i < (int)(index*mult) ; ++i)
	    value += array[i];
	return value;
	break;
      case OCT_AVERAGE:
	value = 0.0;
	for (i = index++ * mult ; i < (int)(index*mult) ; ++i)
	    value += array[i];
	value /= mult;
	return value;
	break;
      case OCT_DAM_DAY:
	value = 0.0;
	for (i = index++ * mult ; i < (int)(index*mult) ; ++i)
	    if (!is_night(i%DAM_TIME_SLC)) value += array[i];
	value /= (mult/2); /* half of the 24-hour period was summed */
	return value;
	break;
      case OCT_DAM_NIGHT:
	value = 0.0;
	for (i = index++ * mult ; i < (int)(index*mult) ; ++i)
	    if (is_night(i%DAM_TIME_SLC)) value += array[i];
	value /= (mult/2); /* half of the 24-hour period was summed */
	return value;
	break;
      default:
	log_msg(L_ERROR, "Programmer error in float_array_value!\n");
	return 0.0;
	break;
    }
}

/** OUTPUT FLOAT ARRAY outputs a floating point array 4 numbers to a line.
 * Currently each line except the first is indented
 * three tab stops. 
 * Size is an integer denoting the number of elements in the array, and
 * array is the floating point array.
 */
int output_float_array (FILE *outfile, const char *prefix, const char *name,
	int size, float array[],
	oconv conversion, unsigned int mult, enum FloatType output_type,
	float *default_value)
{
    int cur_elem = 0;
    int seek_elem;
    int num_on_line=0;

	/* Check to see if the array is empty */
	if (!output_all && default_value) {
		int i;
		int blank = 1;
		for (i=0; i < size; i++) {
			if (array[i] != *default_value) {
				blank = 0;
				break;
			}
		}

		/* Don't print if the array is full of the default value */
		if (blank) return 1;
	}

    OUTPUT_STRING("%s%s ", prefix, name);

    while (cur_elem < size)
    {
	if (cur_elem + 2 < size &&
	    float_array_value(array, cur_elem, conversion, mult) ==
	    float_array_value(array, cur_elem+1, conversion, mult)
	    && float_array_value(array, cur_elem, conversion, mult) ==
	    float_array_value(array, cur_elem+2, conversion, mult) )
	{
	    seek_elem = cur_elem+3;
	    while (seek_elem < size &&
		   float_array_value(array, cur_elem, conversion, mult) ==
		   float_array_value(array, seek_elem, conversion, mult))
		seek_elem++;

	    if (cur_elem == 0 && seek_elem == size)
	    {
		output_float(outfile, 0, output_type, FALSE, "[*]",
			float_array_value(array, cur_elem, conversion, mult), NULL);
		OUTPUT_NEWLINE;
		return 1;
	    }

	    if (num_on_line > 2)
	    {
		fprintf (outfile, "\n\t\t\t");
		num_on_line = 0;
	    }

	    fprintf (outfile, "[%d:%d] ", cur_elem, seek_elem - 1);
	    output_float(outfile, 0, output_type, FALSE, (char *) 0,
		     float_array_value(array, cur_elem, conversion, mult), NULL);
	    putc(' ', outfile);
	    cur_elem = seek_elem;
	    num_on_line += 2;
	}
	else
	{
	    if (num_on_line >= 4)
	    {
		fprintf (outfile, "\n\t\t\t");
		num_on_line = 0;
	    }
	    output_float(outfile, 0, output_type, FALSE, (char *) 0,
		     float_array_value(array, cur_elem, conversion, mult), NULL);
	    putc(' ', outfile);
	    cur_elem++; num_on_line++;
	}
    }

    OUTPUT_NEWLINE;

	return 0;
}

/**
 * A copy of the above, but uses log_msg(whatever), and it doesn't
 * need to muck with conversions.
 */
void log_msg_float_array (size, array)
    int size;
    float array[];
{
	log_float_array( L_MSG, size, array, OCT_NO_CONV, 1 );
}

void log_raw_float_array (size, array)
    int size;
    float array[];
{
	log_float_array( L_DUMP, size, array, OCT_NO_CONV, 1 );
}

void log_float_array (logtype, size, array, conversion, mult)
	int logtype;
    int size;
    float array[];
    oconv		conversion;
    unsigned		mult;
{
    int cur_elem = 0;
    int seek_elem;
    int num_on_line=0;

    while (cur_elem < size)
    {
	if (cur_elem + 2 < size &&
	    float_array_value(array, cur_elem, conversion, mult) ==
	    float_array_value(array, cur_elem+1, conversion, mult)
	    && float_array_value(array, cur_elem, conversion, mult) ==
	    float_array_value(array, cur_elem+2, conversion, mult) )
	{
	    seek_elem = cur_elem+3;
	    while (seek_elem < size &&
		   float_array_value(array, cur_elem, conversion, mult) ==
		   float_array_value(array, seek_elem, conversion, mult))
		seek_elem++;

	    if (cur_elem == 0 && seek_elem == size)
	    {
		log_msg(logtype, "[*] %f ", 
				float_array_value(array, cur_elem, conversion, mult));
		return;
	    }
	    
	    if (num_on_line > 2)
	    {
		log_msg(logtype, "\n\t\t\t");
		num_on_line = 0;
	    }
	    
	    log_msg(logtype, "[%d:%d] %f ", cur_elem, seek_elem -1,
		     float_array_value(array, cur_elem, conversion, mult));
	    cur_elem = seek_elem;
	    num_on_line += 2;
	}
	else
	{
	    if (num_on_line >= 4)
	    {
		log_msg(logtype, "\n\t\t\t");
		num_on_line = 0;
	    }
	    log_msg(logtype, "%f ", 
			float_array_value(array, cur_elem, conversion, mult));
	    cur_elem++; num_on_line++;
	}
    }
}

int write_header(outfile, title, filename)
    FILE *outfile;
    char *title;
	char *filename;
{
#ifndef WIN32
    struct passwd *tmp_passwd;
    struct hostent *host = 0;
#endif
    time_t lt;
    struct tm *tmp_time;
    char name[100];
    
    write_separator(outfile);

#ifdef WIN32
	name[0] = '\0';
#else
    /* get the host name */

    if (gethostname(name,100))
    {
	log_msg(L_WARNING,
		"Error in gethostname.  Called from write_header.\n");
	name[0] = '\0';
    }
#ifdef SYSV    
    if ( !(host = gethostbyname(name)) )
	log_msg(L_WARNING,
		"Error getting full host name. Unknown_host_error.\n%s%s\n",
		"Host was: ",
		name);
#endif /* SYSV */
#endif /* WIN32 */

    /* get the time and date */
    lt = time(NULL);
    tmp_time = localtime(&lt);

#ifndef WIN32
    /* get the users id and name. */
    tmp_passwd=getpwuid((uid_t)getuid());
#endif

    OUTPUT_STRING("# %-50s %s\n#\n", title, get_program_version());

#ifdef WIN32
    OUTPUT_STRING("%s%s", "#Written on ", asctime(tmp_time));
#else
    OUTPUT_STRING("#Written by user %s (%s)",
		  tmp_passwd ? tmp_passwd->pw_name : "UNKNOWN_ID",
		  tmp_passwd ? tmp_passwd->pw_gecos : "");

    OUTPUT_STRING(" on %s#\tfrom host %s.\n",
		  asctime(tmp_time),
		  host ? host->h_name : "unknown host");
#endif

    if (strcmp(buf_path(outfile), buf_path(stdin)))
	OUTPUT_STRING("#%s%s\n#\n", "Original filename: ", buf_path(outfile));

    write_separator(outfile);

    write_saved_comments(outfile, filename);

    return 0;
}


/** String escaping for placing text into/extracting text from an input file.
 * These really only need to be used where quotation marks and such might
 * appear (e.g. in free-form text entry boxes). */
char* strdup_escape(const char *str) {
	int i;
	int quote_count;
	int nl_count;

	if (!str) return NULL;

	for (i=0, quote_count=0, nl_count=0; str[i] != '\0'; i++) {
		if (str[i] == '"') quote_count++;
		else if (str[i] == '\n') nl_count++;
	}

	if (quote_count <= 0 && nl_count <= 0) return strdup(str);
	else {
		const int escaped_length = i + quote_count*5 + nl_count*3;
		char *escaped_str = (char*) malloc(sizeof(char) * escaped_length + 1);

		if (!escaped_str) {
			log_msg(L_ERROR, "Could not allocate %d bytes for escaped string\n", escaped_length);
			return NULL;
		}
		else {
			int j;
			for (i=0, j=0; str[i] != '\0'; i++, j++) {
				if (str[i] == '"') {
					escaped_str[j++] = '&';
					escaped_str[j++] = 'q';
					escaped_str[j++] = 'u';
					escaped_str[j++] = 'o';
					escaped_str[j++] = 't';
					escaped_str[j] = ';';
				}
				else if (str[i] == '\n') {
					escaped_str[j++] = '&';
					escaped_str[j++] = 'n';
					escaped_str[j++] = 'l';
					escaped_str[j] = ';';
				}
				else escaped_str[j] = str[i];
			}
			escaped_str[j] = '\0';
			return escaped_str;
		}
	}
}
char* strdup_unescape(const char *str) {
	int i;
	int quote_count;
	int nl_count;

	if (!str) return NULL;

	for (i=0, quote_count=0, nl_count=0; str[i] != '\0'; i++) {
		if (str[i] == '&' && str[i+1] == 'q' && str[i+2] == 'u' && str[i+3] == 'o'
			&& str[i+4] == 't' && str[i+5] == ';') quote_count++;
		else if (str[i] == '&' && str[i+1] == 'n' && str[i+2] == 'l'
			&& str[i+3] == ';') nl_count++;
	}

	if (quote_count <= 0 && nl_count <= 0) return strdup(str);
	else {
		const int unescaped_length = i - quote_count*5 - nl_count*3;
		char *unescaped_str = (char*) malloc(sizeof(char) * unescaped_length + 1);

		if (!unescaped_str) {
			log_msg(L_ERROR, "Could not allocate %d bytes for unescaped string\n", unescaped_length);
			return NULL;
		}
		else {
			int j;
			for (i=0, j=0; str[i] != '\0'; i++, j++) {
				if (str[i] == '&' && str[i+1] == 'q' && str[i+2] == 'u' && str[i+3] == 'o'
					&& str[i+4] == 't' && str[i+5] == ';') {
					unescaped_str[j] = '"';
					i += 5;
				}
				else if (str[i] == '&' && str[i+1] == 'n' && str[i+2] == 'l'
					&& str[i+3] == ';') {
					unescaped_str[j] = '\n';
					i += 3;
				}
				else unescaped_str[j] = str[i];
			}
			unescaped_str[j] = '\0';
			return unescaped_str;
		}
	}
}

