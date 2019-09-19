#include <math.h>
#include <errno.h>
#include <stdio.h>


#include <stdlib.h>
#include <string.h>


#include "crisp.h"
#include "log.h"
#include "parseutil.h"
#include "writeutil.h"
#include "period.h"

#include <stdarg.h>

static void swapint (i1, i2)
    int *i1, *i2 ;
{
    int temp;

    temp = *i1;
    *i1 = *i2;
    *i2 = temp;
}

/** \brief Turn a token to lowercase */
char * lowercase(s, scratch)
    char *s, *scratch;
{
    *scratch = (*s >= 'A' && *s <= 'Z') ? *s + ('a' - 'A') : *s;
    if (*s != '\0')
	(void) lowercase (s+1, scratch+1);

    return (scratch);
}

/** \brief Convert underscores to spaces */
char * unders_to_spaces (string)
    char *string;
{
    *string = (*string == '_') ? ' ' : *string ;
    if (*string != '\0')
	(void) unders_to_spaces (string+1);
    return (string);
}

/** Check to see if the string is probably a floating point number */
unsigned char is_float (string)
    char *string;
{
    /* this first silly if statement is for a leading + or - */
    if ((*string == '-') ||
	(*string == '+') )
    {
	string++;
	if (*string == '\0')
	    return (0);
    }

    /* this doesn't check to see if there is more than one "." in the number ^..^*/
    while (*string != '\0')
    {
	if ( !( (*string >= '0' && *string <= '9') ||
	        (*string == '.') ) )
	    return (0);
	string++;
    }
    return (1);
}

/** Check to see if the string is probably an integer */
unsigned char is_int (string)
    char *string;
{
    /* this first silly if statement is for a leading + or - */
    if ((*string == '-') ||
	(*string == '+') )
    {
	string++;
	if (*string == '\0')
	    return (0);
    }

    while (*string != '\0')
    {
	if ( !( *string >= '0' && *string <= '9' ) )
	    return (0);
	string++;
    }
    return (1);
}


/** p_error prints the requested error message to the message window along with information
 * about the location of the error */


void p_error (FILE *buffer, char *format, ...)

{
    va_list args;

    char message[256];

    va_start(args, format);

    (void) vsprintf(message, format, args);

    log_msg(L_ERROR, message);
    log_msg(L_ERROR,
	    "\tLine: %d, File: {%s}\n",
	    buf_line(buffer),
	    (char *)buf_path(buffer));
}


/** p_warning prints a warning level error message to the message window
 * along with information about the location of the error */


void p_warning (FILE *buffer, char *format, ...)

{
    va_list args;
    char message[256];

    va_start(args, format);

    (void) vsprintf(message, format, args);

    log_msg(L_WARNING, message);
    log_msg(L_WARNING,
	    "\tLine: %d, File: {%s}\n",
	    buf_line(buffer),
	    (char *)buf_path(buffer));
}

/*======================================================================*/
/*======================================================================*/

/** \brief Pop a token */
#define POP_TOKEN_NO_EOF(buffer, token, prompt)  if (pop_token(buffer, token, prompt) == EOF) { p_warning(buffer, "unexpected end of file\n", ""); return (0); }
#define ERROR_INV_INPUT_DISCARD(string) {p_error(buffer, string, token); reset_input(buffer); return (0);}

/** \brief Read an integer */
unsigned char read_int ( buffer, number, prompt )
    FILE *buffer;
    int *number;
    char *prompt;
{
    char token[MAX_COMMAND_LEN+1];

    POP_TOKEN_NO_EOF(buffer, token, prompt);

    if (is_int(token))
    {
	*number = atoi(token);
	return (1);
    }
    else
    {
	p_error (buffer,
		 "looking for an integer number, found {%s}: line discarded\n",
		 token);
	reset_input(buffer);
    }

    return(0);
}

/** \brief Read an unsigned int.  this is bogus because it just type casts an int read*/
unsigned char read_uns ( buffer, number, prompt )
    FILE *buffer;
    unsigned int *number;
    char *prompt;
{
    int num;
    unsigned char erc = read_int(buffer, &num, prompt);
    *number = (unsigned int) num;
    return erc;
}

/** \brief Read a floating point numbers */
unsigned char read_float ( buffer, number, prompt )
    FILE *buffer;
    float *number;
    char *prompt;
{
    char token[MAX_COMMAND_LEN+1];

    POP_TOKEN_NO_EOF(buffer, token, prompt);

    if (is_float(token))
    {
	*number = atof(token);
	return (1);
    }
    else
    {
	p_error (buffer,
		 "looking for a floating point number, found {%s}: line discarded\n",
		 token);
	reset_input(buffer);
    }

    return(0);
}

#ifdef WIN32
/* Windows has strnicmp() instead of strncasecmp() */
#define strncasecmp(a, b, c)	strnicmp(a, b, c)
#endif

/** \brief Read a floating point number or an "N/A" (any case is allowed) */
unsigned char read_float_or_na(FILE *buffer, float *number, char *prompt, int *was_na) {
    char token[MAX_COMMAND_LEN+1];

    POP_TOKEN_NO_EOF(buffer, token, prompt);

    if (strncasecmp("n/a", token, 4) == 0) {
	    *number = 0.0f;
	    *was_na = TRUE;
	    return 1;
    }
    else {
	    *was_na = FALSE;
	    push_last_token(buffer);
	    return read_float(buffer, number, prompt);
    }
}

/** \brief Read a double */
unsigned char read_double ( buffer, number, prompt )
    FILE *buffer;
    double *number;
    char *prompt;
{
    char token[MAX_COMMAND_LEN+1];

    POP_TOKEN_NO_EOF(buffer, token, prompt);

    if (is_float(token))
    {
	*number = strtod(token, NULL);
	return (1);
    }
    else
    {
	p_error (buffer,
		 "looking for a floating point number, found {%s}: line discarded\n",
		 token);
	reset_input(buffer);
    }

    return(0);
}

static void assign_float_array(array, index, value, conversion, mult)
	float array[];
	int index;
	float value;
	aconv conversion;
	unsigned mult;
{
    int i;

    switch(conversion) {
      case ACT_NO_CONV:
	array[index] = value;
	break;
      case ACT_DUP:
	for (i = index++ * mult ; i < (int)(index * mult) ; ++i)
	    array[i] = value;
	break;
      case ACT_SPLIT:
	array[index * mult] = value;
	for (i = index++ * mult + 1 ; i < (int)(index * mult) ; ++i)
	    array[i] = 0.0;
	break;
	  case ACT_HALF:
	for (i = index++ * mult; i < (int)(index * mult) ; ++i)
		array[i] = value/2;
	break;
      default:
	log_msg(L_ERROR, "Programmer error in assign_float_array!\n");
	break;
    }
}

/** \brief Read an array of floating point numbers */
unsigned char read_float_array (buffer, array, max_elem, conversion, mult,
				prompt)
    FILE *buffer;	/* a pointer to the file to read from */
    float array[];	/* a pointer to the array to put data in */
    int max_elem;	/* the number of elems in the array */
    aconv conversion;	/* converstion method to use to store data */
    unsigned mult;	/* array element multiplier for conversion */
    char *prompt;	/* the prompt to use to get more elements */
{
    extern double atof();

    char token[MAX_COMMAND_LEN+1];
    int cur_elem=0;
    int count=0;
    float number=0;
    
    while (pop_token(buffer, token, prompt) != EOF)
    {
	if (strcmp(token, "[") == 0)
	{
	    POP_TOKEN_NO_EOF(buffer, token, prompt);

	    /* array_name [*] */
	    if (strcmp(token, "*") == 0)
	    {
		POP_TOKEN_NO_EOF(buffer, token, prompt);
		if (strcmp (token, "]") != 0)
		{
		    ERROR_INV_INPUT_DISCARD(
			"expected a ], got a {%s}: line discarded\n");
		}
		
		POP_TOKEN_NO_EOF(buffer, token, prompt);

		if ( !is_float (token) )
		{
		    ERROR_INV_INPUT_DISCARD(
	    "expected a floating point number, got a {%s}: line discarded\n");
		}
		else
		    number = atof(token);
	    
		for (cur_elem=0; cur_elem < max_elem; cur_elem++)
		    assign_float_array(array, cur_elem, number, conversion,
				       mult);
	    }

	    /* array_name [#:#] or array_name [#] */
	    else if (is_int(token))
	    {
		cur_elem = atoi(token);
		POP_TOKEN_NO_EOF(buffer, token, prompt);

		/* array_name[#] */
		if (strcmp(token, "]") == 0)
		{
		    POP_TOKEN_NO_EOF(buffer, token, prompt);
		    if (is_float(token))
		    {
			if (cur_elem < max_elem)
			    assign_float_array(array, cur_elem++, atof(token),
					       conversion, mult);
			else
			{
			    p_warning(buffer,
				      "array index exceeds array bounds.\n");
			    log_msg(L_DBGMSG, "Array index [%d], max %d\n",
				    cur_elem, max_elem);
			}
		    }
		    else
		    {
			p_warning(buffer,
				  "non numeric token after an array index.\n");
			push_last_token (buffer);
			return (1);
		    }
		}
		/* array_name[#:#] */
		else if (strcmp(token, ":") == 0)
		{
		    POP_TOKEN_NO_EOF(buffer, token, prompt);
		    if (!is_int(token))
		    {
			ERROR_INV_INPUT_DISCARD(
				"expected an int, got {%s}: line discarded\n");
		    }
		    count = atoi(token);

		    if (count < cur_elem)
			swapint( &cur_elem, &count );

		    POP_TOKEN_NO_EOF(buffer, token, prompt);
		    if (strcmp(token, "]"))
		    {
			ERROR_INV_INPUT_DISCARD(
				"expected a {]}, got {%s}: line discarded\n");
		    }

		    POP_TOKEN_NO_EOF(buffer, token, prompt);
		    if (!is_float(token))
		    {
			ERROR_INV_INPUT_DISCARD(
			"expected a float, got {%s}: line discarded\n");
		    }
		    else
			number = (float) atof (token);

		    if (count >= max_elem)
		    {
			p_warning(buffer,
				  "array index exceeds array bounds\n");
			log_msg(L_WARNING, "[%d:%d] index with max elem %d\n",
				cur_elem, count, max_elem);
			count = max_elem-1;
		    }

		    for (; cur_elem <= count ; ++cur_elem)
			assign_float_array(array, cur_elem, number, conversion,
					   mult);
		}
		else
		{
		    ERROR_INV_INPUT_DISCARD(
			"expected a : or ], got {%s}: line discarded\n");
		}
	    }
	    else
	    {
		ERROR_INV_INPUT_DISCARD(
			"expected a * or integer, got {%s}: line discarded\n");
	    }
	}
	else if (is_float(token))
	    if (cur_elem < max_elem)
		assign_float_array(array, cur_elem++, atof(token), conversion,
				   mult);
	    else
	    {
		push_last_token(buffer);
		p_warning(buffer,
		     "exceeded the maximum number of elements of an array\n");
		return(0);
	    }
	else
	{
	    push_last_token(buffer);
	    return(1);
	}
    }
    /* Hit an EOF while parsing an array of floats.  */
    token[0] = 0;
    ERROR_INV_INPUT_DISCARD(
			"%send of file encountered: line discarded\n");
    return(0);
}



/** \brief Skip numbers (after an error, most likely) */
unsigned char skip_all_numbers (buffer)
    FILE *buffer;
{
    char token[MAX_COMMAND_LEN+1];

    while (pop_token(buffer, token, "bogus prompt -> ") != EOF)
	if (!is_float(token))
	{	
	    push_last_token(buffer);
	    return(1);
	}
    return(1);
}

/** \brief Skip numbers (after an error, most likely) */
unsigned char skip_numbers (buffer, max)
    FILE *buffer;
    int max;
{
    char token[MAX_COMMAND_LEN+1];
    int count = 0;

    while (count <= max &&
	   pop_token(buffer, token, "bogus prompt -> ") != EOF)
	if (!is_float(token))
	{	
	    push_last_token(buffer);
	    return(1);
	}
    
    return(1);
}
    
/** \brief Skip numbers (after an error, most likely) */
unsigned char skip_num_array (buffer, max)
    FILE *buffer;
    int max;
{
    char token[MAX_COMMAND_LEN+1];

    POP_TOKEN_NO_EOF(buffer, token, "bogus prompt ->");
    
    if (strcmp(token, "[") == 0)
    {
	while ((pop_token(buffer, token, "bogus prompt -> ") != EOF) &&
	       (strcmp(token, "]") != 0))
	    if ((strcmp(token, "*") != 0) &&
		(strcmp(token, ":") != 0) &&
		(!is_float(token))          )
		ERROR_INV_INPUT_DISCARD("invalid array format.  token = {%s}: line discarded\n");
    }    
    else if (!is_float(token))
    {
	ERROR_INV_INPUT_DISCARD("invalid array format.  token = {%s}: line discarded\n");
    }

    return (skip_num_array(buffer, max-1)) ;
}


command_struct period_elem_tokens[] =
{
    { ':' , ":"},
    { '(' , "("},
    { ')' , ")"},
    { ',' , ","},
    { '=' , "="},
    { 0, "\0" }	  };
    
/** \brief Parse periods from input.
 *
 * form:								
 *   single_period := # {(subperiod_list)} | #:# {(subperiod_list)};    
 *   subperiod_list := period_list := single_period {[,| ] period_list};
 * It returns a pointer to the period if successfull, 0                 
 * if there is a serious error.                                         
 */
struct period_list_struct *parse_periods (buffer)
	FILE *buffer;
{
    struct period_list_struct *period;

    /* declared static for efficiency */
    static char token[MAX_COMMAND_LEN+1], scratch[MAX_COMMAND_LEN+1];
    static COMMAND_LIST period_elem_menu=0;
    static int level=0; /* the current subperiod level */

    if (!period_elem_menu)
	period_elem_menu = com_create_list (period_elem_menu,
					    period_elem_tokens);

    if (pop_token(buffer, token, "Beginning of period ->") == EOF)
	return((struct period_list_struct *)0);
    if (!is_float(token))
    {
	push_last_token(buffer);
	return((struct period_list_struct *)0);
    }
    if (!(period=new_period_struct()))
	return((struct period_list_struct *)0);

    period->start = ((int) atof(token));
    period->end = period->start;
    
    while (pop_token(buffer, token, "period input ->") != EOF)
    {
	switch (com_reference (period_elem_menu, lowercase (token, scratch)))
	{
	case ':':
	    if (!read_int(buffer, &period->end, "End of period ->"))
	    {
		p_error(buffer,
			"Error parsing the end of a period. Remainder of line discarded.\n");
		reset_input(buffer);
		level=0;
		return((struct period_list_struct *)0);
	    }
	    break;
	case '=':
	    if (!read_float(buffer, &period->value, "Period value ->"))
	    {
		p_error(buffer,
			"Error parsing period value. Remainder of line discarded.\n");
		reset_input(buffer);
		level=0;
		return((struct period_list_struct *)0);
	    }
	    break;
	case '(':
	    level++;
	    period->sub_periods = parse_periods(buffer);
	    break;
	case ')':
	    level--;
	    if(level < 0)
	    {
		p_error(buffer, "Extra close parenthesis in period.\n");
		level=0;
	    }
	    return(period);
       
	case ',':
	    /* Currently, commas are discarded.  Obviously this means that you
	     * can have an extra comma at the end of a period, and the parser
	     * won't complain.  Oh well.
	     */
	    break;
	default:
	    push_last_token(buffer);
	    if (is_float(token))
	    {
		period->next=parse_periods(buffer);
		if (period->next)
			period->next->prev = period;
	    }
	    else
		if (level != 0)
		{
		    p_error(buffer,
			"Missing %d close parenthesis in period.\n", level);
		    level=0;
		}
	    return(period);
	}
    }
    if (level != 0)
    {
	p_error(buffer,
		"Detected EOF while missing %d close parenthesis in period.\n",
		level);
	level=0;
    }
    return(period);
}


/** \brief Include another file */
FILE *include(buffer)
    FILE *buffer;
{
    FILE *newbuffer;
    char token[MAX_COMMAND_LEN+1];
    char newdir[MAXPATHLEN], pathname[MAXPATHLEN];
    
    /* parse the file name */
    if (pop_token(buffer, token, "Include file -> ") == EOF)
    {
	p_error(buffer,
		"encountered EOF after include statement.\n");
	return(buffer);
    }
    
    /* push the current buffer */
    push_buf(buffer);
    
    /* get the path and dir of the new file */
    (void) real_filename(token, pathname);
    (void) dirname(pathname, newdir);
    
    /* pushd to the new dir */
    if (pushd(newdir))
    {
	perror_msg ("Include:");
	return(buffer);
    }
    
    /* open the file */
    if ( (newbuffer = buf_open(pathname, "r")) )
	/* return the new buffer */
	return(newbuffer);
    else
    {
	p_error(buffer, "could not open include file {%s}\n",
		pathname);
	(void)pop_buf();
	if (popd())
	{
	    perror_msg("Include:");
	}
	return(buffer);
    }
}

/** \brief Open a file to parse. */
PARSER_FILE *open_to_parse (filename)
    char * filename;
{
    PARSER_FILE * buffer;
    char newdir[MAXPATHLEN], pathname[MAXPATHLEN], nameonly[MAXPATHLEN];

    if (!(filename && *filename))
    {
	log_msg(L_ERROR, "null filename!\n");
	return (PARSER_FILE *) 0;
    }

    /* get the path and dir of the new file */
    (void) real_filename(filename, pathname);
    (void) dirname(pathname, newdir);
	(void) base_name( pathname, nameonly );
    
    /* pushd to the new dir */
    if (*newdir != '\0' &&
	pushd(newdir))
    {
	perror_msg ("Open:");
	return (PARSER_FILE *) 0;
    }

    buffer = (PARSER_FILE *) malloc(sizeof(PARSER_FILE));
    if (!buffer)
    {
	log_msg(L_ERROR, "No memory to open file!\n");
	(void) popd();
	return (PARSER_FILE *) 0;
    }

    buffer->version = DEFAULT_VERSION;

    /* open the file */
    if ( (buffer->fp = buf_open(nameonly, "r")) )
	/* return the new buffer */
	return(buffer);
    else
    {
	log_msg(L_ERROR, "could not open file {%s}\n", pathname);

	if (popd())
	{
	    perror_msg("Open:");
	}

	free(buffer);
	return (PARSER_FILE *) 0;
    }
}

/** \brief Close a file after parsing. */
void close_after_parse (buffer)
    PARSER_FILE * buffer;
{
    if (can_popd() && popd())
	perror_msg("Cleanup after parse:");
    buf_close(buffer->fp);
    free(buffer);
    return;
}

/** \brief Include a file to parse (like open_to_parse()) */
PARSER_FILE *include_to_parse(buffer)
	PARSER_FILE *buffer;
{
    PARSER_FILE *file_buf;
    char token[MAX_COMMAND_LEN + 1];

    if (pop_token(buffer->fp, token, "Include file -> ") == EOF)
    {
	p_error(buffer->fp, "encountered EOF after include statement.\n");
	return buffer;
    }

    /* Push the current buffer on our handy stack. */
    push_buf(buffer);

    if ((file_buf = open_to_parse(token)))
	return file_buf;
    else
    {
	(void) pop_buf();
	return buffer;
    }
}

/** \brief Finish an include like close_after_parse() */
PARSER_FILE *return_after_parse(buffer)
	PARSER_FILE *buffer;
{
/*	No longer needed here.
 *  if (popd())
 *	perror_msg("Return from:");
 */

    if (can_pop_buf())
    {
	close_after_parse(buffer);
	return (PARSER_FILE *) pop_buf();
    }
    else
    {
	p_error(buffer->fp,
		"return_from(buffer) called with no stack!\n");
	return buffer;
    }
}

/** \brief Write out a separator in the file */
int write_separator(outfile)
	FILE	*outfile;
{
    OUTPUT_STRING("%s%s\n",
		  "#======================================",
		  "======================================#");
    return(0);
}

