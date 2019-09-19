#include <stdlib.h>
#include <string.h>
#ifdef WIN32
#include <stdio.h>
#include <memory.h>
#include <time.h>
#else
#include <unistd.h>
#include <strings.h>
#include <memory.h>
#include <sys/time.h>
#include <sys/resource.h>
#endif

#include <errno.h>
    
#include "token.h"
#include "log.h"

extern void perror_msg();
	

/**==============================================================*/
/* These are the tests to see if a character is part of a token,*/
/* a token devider, or the begining of a comment.               */
/* A token devider is counted as the last character of the      */
/* current token.  All token deviders should be included in     */
/* the test for token components.  Comment tokens should NOT be */
/* included as token components.   A token may have from 1 to   */
/* MAX_TOKEN_LEN characters (defined in input.h).               */
/* When a comment token is found, the rest of the line is       */
/* discarded.                                                   */
/*==============================================================*/
/* make sure these are surrounded by parenthesis. */
#define TOKEN_QUOTE_TEST(c) (c == '"')
#define TOKEN_DIVIDER_TEST(c) ((c < '.' || c > '9') && (c < 'A' || c > 'Z') &&\
			       (c < 'a' || c > 'z') && (c != '_') && \
			       (c != '+') && (c != '-')  && (c != '~') && !TOKEN_QUOTE_TEST(c) )
#define TOKEN_COMPONENT_TEST(c) (c >= '!' && c <= '~' && c != '#') 


/**==============================================================*/
/* code for keeping track of line #'s in multiple buffers and   */
/* path names.  Uses a bit of memory.                           */
/*==============================================================*/
    
/** set up a record for the buffer info */
struct buf_info 
{
    unsigned int line; /* current line in the file */
    char input[LINE_LENGTH+1], *current; 
    unsigned char last_length; /* used to make push x dummy proof */
    char *path; /* path name for the file */
};

static struct buf_info **buf=0;

static int max_buffers = -1;

#define LEGAL_BUFFER(buffer) ((buffer && (fileno(buffer) >= 0) && (fileno(buffer) < max_buffers)) ? 1 : 0)

#define INPUT (buf[buf_no]->input)
#define CURRENT (buf[buf_no]->current)
#define LAST_LENGTH (buf[buf_no]->last_length)
#define PATH (buf[buf_no]->path)
#define LINE (buf[buf_no]->line)

static struct buf_info * new_buf_info (path)
    char *path;
{
    struct buf_info *elem = (struct buf_info *) calloc (1, sizeof(struct buf_info));

    (elem->path) = malloc ((unsigned)(sizeof(char) * (strlen(path) + 1)));
    (void) strcpy ((elem->path), path);

    elem->current = elem->input;
    
    return (elem);
}

/** init_buffers should initilize the array the buffer info is stored in, and
 * make a record for stdin if it is open */
static void init_buffers ()
{
#ifndef WIN32
/* TODO figure out what this means and what the equivalent PC-ish thing is */
    struct rlimit *rlp = (struct rlimit *)malloc(sizeof(struct rlimit));
#endif

    unsigned old_max = max_buffers; 
    /* use a temporary variable in case there is already data  in buf */
    struct buf_info **temp;
    	
#ifdef WIN32        
/* TODO figure out what this means and what the equivalent PC-ish thing is */
    max_buffers = 16;
#else
    if (getrlimit (RLIMIT_NOFILE, rlp))
    {
	perror_msg ("Input Module, getting resource information:");
	exit (0);
    }
    max_buffers = rlp->rlim_max;
    free ((char *)rlp);
#endif //WIN32

    temp = (struct buf_info **)
	calloc (max_buffers, (unsigned) sizeof (void *));
    if (buf)
    {
	temp = (struct buf_info **)
	    memcpy ((char *)temp, (char *)buf, (int)(old_max+1));
	free ((char *)buf);
    }
    else if (fileno(stdin) >= 0) {
		temp[fileno(stdin)] = new_buf_info("Standard Input");
	}
	
    buf = temp;
}

static void alloc_info (buffer, path)
    FILE *buffer;
    char *path;
{
    /* make sure that buf is big enough (and exists)
       and then call new_buf_info */
    if (!buf || !LEGAL_BUFFER(buffer))
    {
	init_buffers();
	if (!LEGAL_BUFFER(buffer))
	{
	    log_msg(L_FATAL,
		    "FATAL: file handle is greater than max # of buffers or no such buffer.\n");
	}
    }
    buf[fileno(buffer)] = new_buf_info (path);
}


static int get_buf_no(buffer)
    FILE *buffer;
{
    if (!buf) init_buffers();

    if (
	LEGAL_BUFFER(buffer) &&
	buf[fileno(buffer)])
	return (fileno(buffer));
    else
	return (fileno(stdin));
}

/*========================================================*/

FILE * buf_open (path, access_mode)
    char *path, *access_mode;
{
    FILE *buffer;

	/* In the interests of portability, all files are opened in binary
	 *  mode and the line termination problem is addressed explicitly
	 *  in Read_Line(), below.
	 */
	if (!strcmp( access_mode, "r" ))
		buffer = fopen (path, "rb");
	else
		buffer = fopen (path, access_mode);
    
    log_msg(L_DBGMSG, "opening file %s\n", path);
    if (buffer) alloc_info(buffer, path);
    
    return (buffer);
}

int buf_close (   FILE *buffer)
{
    if (buffer)
    {
	int buf_no = get_buf_no(buffer);

	log_msg(L_DBGMSG, "closing file %s\n", buf_path(buffer));
	if (buf_no != fileno(stdin)) {
	    free((*(buf+buf_no))->path);
	    free(*(buf+buf_no));
	}
	buf[buf_no] = 0;
    
	return (fclose(buffer));
    }
    else return 0;
}

char * buf_path(buffer)
    FILE *buffer;
{
    int buf_no = get_buf_no(buffer);
    return (PATH);
}

int buf_line(FILE *buffer)
{
    int buf_no = get_buf_no(buffer);
    return (LINE);
}

/**==========================================================================*/
/* Read_Line reads one entire line from the buffer into the input array */
/* NOTE that, in the interests of portability, this routine has been */
/*  re-written to accept a single '\r', or a single '\n', or a "\r\n" pair */
/*  of characters as */
/*  representing a single line termination. This is necessary because */
/*  ascii files are often transferred between Unix and PC formats without */
/*  properly translating the line terminations. */
/*==========================================================================*/
static char *Read_Line (buffer)
    FILE *buffer;
{
#ifndef WIN32
    FILE *term;
#endif

    int buf_no = get_buf_no(buffer);
    int count=0;
	int ch;

    CURRENT = INPUT;
    *CURRENT = '\n';
    
#ifndef WIN32
/* TODO do we really need this code anymore? */
    if (isatty(fileno(buffer)))
    {
		char *ret;
		if (!(term = fopen(ttyname(fileno(buffer)),"r"))) 
		{
			perror_msg ("io.c: Read_Line: Can't open your tty ");
			exit (1);
		}
		ret = fgets (CURRENT, LINE_LENGTH, term);
		(void) fclose (term);
		LINE++;
    
		while (INPUT[count] != '\n' && count < LINE_LENGTH) count++; 
		INPUT[count] = '\n';
    
		return (ret);
    }
    else
#endif // WIN32

	LINE++;
	while (1) {
		/* Run through the file one character at a time (slow, but sure). */
		ch = fgetc( buffer );

		/* If we have hit the end of the file, return the last partial
		    line, or an EOF indication. */
		if (ch == EOF) {
			if (count == 0)
				return NULL;
			else
				break;
		}

		/* If we have hit the end of a line, try to get resynched on the
		 *  start of the next line. */
		if (ch == '\r' || ch == '\n') {
			if (ch == '\r') {
				ch = fgetc( buffer );
				if (ch != '\n') {
					if (ch != EOF)
						/* Put back the first character of the next line. */
						fseek( buffer, -1, SEEK_CUR );
				}
			}
			break;
		}

		/* If we have a normal (in-line) character, put it in the buffer.
		 *  Check for buffer overflow. */
		INPUT[count++] = ch;
		if (count == LINE_LENGTH-1) {
			break;
		}
	}

	/* Valid input is always terminated with a \n and a NULL */
	INPUT[count++] = '\n';
    INPUT[count] = '\000';
    
    return (INPUT);
} 
 
void incr_buf_line(buffer)
    FILE *buffer;
{
    int buf_no = get_buf_no(buffer);
    LINE++;
}
 
/**==========================================================================*/
/* Reset_Input resets current so the next time an input request occurs a line
   will be read from the input device */
/*==========================================================================*/
void reset_input (buffer)
    FILE *buffer;
{
    int buf_no = get_buf_no(buffer);

    *CURRENT = '\n';
    return;
}

/**==========================================================================*/
/* at_end_of_line returns non-zero if at the end of an input line in the file
    This is useful for optional tokens that may appear at the end of a line
    but may also be omitted.						   */
/*=========================================================================*/
int at_end_of_line(FILE *buffer) {
    int buf_no = get_buf_no(buffer);

    /* skip leading blanks and non-printable characters */
    /* if current input is at end of line/file (or is commented out), return non-zero */
    while (!TOKEN_COMPONENT_TEST(*CURRENT))
    {
	if (*CURRENT == '\n' || *CURRENT == '\0' || COMMENT_TOKEN_TEST(*CURRENT))
		return 1;
	else
	    CURRENT++;
    }
    return 0;
}
 
/**==========================================================================*/
/* Pop_Token extracts the next token from the input array.  If there is
    not a token available, it calls Read_Line until it can extract one 
    or until it gets an eof                                                */
/*=========================================================================*/
int pop_sized_token (buffer, string, prompt, max_token_length) 
    FILE *buffer;
    char *string, *prompt;
	int max_token_length;
{
    int buf_no = get_buf_no(buffer);
	int in_string = 0; /* if we're inside of a string literal, i.e. inside of quote marks */

    /* strip leading blanks and non-printable characters */
    /* if current input is at end of line, prompt, get another line */
    while (!TOKEN_COMPONENT_TEST(*CURRENT))
    {
	if (*CURRENT == '\n' || *CURRENT == '\0' || COMMENT_TOKEN_TEST(*CURRENT))
	{
	    if (feof(buffer) != 0) return (EOF);
#ifndef WIN32
/* TODO do we really need this code anymore? */
	    if (isatty(fileno(buffer)))
	    {
		(void) printf ("%s ", prompt);
		(void) fflush(stdout);
	    }
#endif
	    if (Read_Line (buffer)== NULL) return (EOF);
	}
	else
	    CURRENT++;
    }
    
    /* find and copy next string */

	if (TOKEN_QUOTE_TEST(*CURRENT))
	{
	CURRENT++;
	in_string = 1;
	}
    *string = *CURRENT; LAST_LENGTH = 1;
    if (TOKEN_DIVIDER_TEST(*CURRENT))
    {
	*(++string) = '\0'; CURRENT++;
    }
    else
    {
	++string; ++CURRENT; 
	while ((in_string || !(TOKEN_DIVIDER_TEST(*CURRENT))) && LAST_LENGTH++ < max_token_length) {
		if (TOKEN_QUOTE_TEST(*CURRENT)) {
			CURRENT++;
			in_string = 0;
		}
	    else *(string++) = *(CURRENT++);
	}
	*string = '\0';
    }
    
    return (0);
}
int pop_token(FILE *buffer, char *string, char *prompt) {
	return pop_sized_token(buffer, string, prompt, MAX_TOKEN_LEN);
}

/**==========================================================================*/
/* Push_Last_Token pushes the last token back onto the string.  It uses a 
 * static global variable to be dummy proof.   */
/*=========================================================================*/
void push_last_token (buffer)
    FILE *buffer;
{
    int buf_no = get_buf_no(buffer);

    CURRENT -= LAST_LENGTH;
    LAST_LENGTH = 0;
    return ;
}

/**==========================================================================*/
/* Push_Last_Chars pushes the last x characters back onto the string.      */
/*=========================================================================*/
void push_last_chars (buffer, number)
    FILE *buffer;
    int number; /* the number of characters to push */
{
    int buf_no = get_buf_no(buffer);

    if ( (CURRENT - INPUT + number) < LINE_LENGTH)
	CURRENT -= number;
    else
	CURRENT = INPUT;
    LAST_LENGTH = 0;
}

/**=======================================================================*/
/* Pop_Line  gets the rest of the line from the input buffer.  */
/*=======================================================================*/
int pop_line (buffer, line, prompt)
    FILE *buffer;
    char *line, *prompt;
{
    int buf_no = get_buf_no(buffer);

    /* strip leading blanks and non-printable characters */
    /* if current input is at end of line, prompt, get another line */
    while (*CURRENT <= ' ' || *CURRENT > '~')
    {
	if (*CURRENT == '\n')
	{
	    if (feof(buffer) != 0) return (EOF);
#ifndef WIN32
/* TODO Do we really need this? */
	    if (isatty(fileno(buffer)))
	    {
		(void) printf ("%s ", prompt);
		(void) fflush(stdout);
	    }
#endif
	    if (Read_Line (buffer)== NULL) return (EOF);
	}
	else
	    CURRENT++;
    }
    
    LAST_LENGTH=0;
    /* copy rest of line */
    while ( *CURRENT != '\n' )
    {
	*(line++) = *(CURRENT++);
	LAST_LENGTH++;
    }
    *line = '\0';
    
    return (0);
}


/** fcn to convert a string (which may have spaces) to a single word. *
 * Currently done by converting spaces to underlines.                */
char *string_to_word (string, scratch)
    char *string, *scratch;
{
    char *current=scratch;
    
    while ( *string != '\0')
	if (*string == ' ')
	{
	    *(current++) = '_';
	    string++;
	}
	else	
	    *(current++) = *(string++);
    *current = '\0';

    return (scratch);
}


