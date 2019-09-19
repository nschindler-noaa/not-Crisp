/*
 * A window to log messages in.
 * init_messages() must be called before any messages are used.
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#ifdef WIN32
#include <io.h>
#endif

#define MESSAGE_SIZE	256

#include "log.h"
#include "gui.h"

static FILE *msg_file = 0;
static char last_message[MESSAGE_SIZE] = { 0 };
static  int message_count = 0;
static int last_level = 0;
char message_filename[30];

static char *error_prefixes[] = {  /* Text to be prepended to messages. */
    "ERROR: ",
    "Warning: ",
    "  ",
    "  ",
    "  ",
    "FATAL: ",
    "",
    (char *) 0
};
	
/* logging options. */
int log_warning = 1;
int log_message = 0;
int log_raw = 0;
int log_debug = 0;

/* Used in batch mode: log file is created on program start, logs everything,
 *  is destroyed on normal program exit. Hopefully survives program crashes
 *  to leave some evidence behind.
 */
void create_log_file( void ) {

#ifndef WIN32
	int fd;
	strcpy( message_filename, "/tmp/COMPASSXXXXXX" );

	if ((fd = mkstemp( message_filename )) != -1) {
		msg_file = fdopen( fd, "w+" );
	}
#else
	strcpy( message_filename, "COMPASSXXXXXX" );

	if (mktemp( message_filename )) {
		strcat( message_filename, ".logfile" );
		msg_file = fopen( message_filename, "w+" );
	}
#endif
}

/** This sets the log file to be the specified file.
 * This is useful in cases like testing where errors should just be written to stdout.
 * Note that this does not close the old (if any) message file. */
void set_log_file(FILE *new_msg_file) {
	msg_file = new_msg_file;
}

void delete_log_file( void ) {
	if (msg_file != NULL)
		fclose( msg_file );
	remove( message_filename );
}

/**
 * \brief Send a message to the message file (and window if it's
 *	active).
 *
 *	Note that sending the information to the window and to the file
 *	requires that textsw_reset be called if window is destroyed and
 *	recreated.
 *
 *	Parameters are an integer for the log level, and then a printf
 *	style argument list.
 *
 */


void log_msg(int level, const char *text, ...)
{
    va_list args;
    char message[MESSAGE_SIZE];
    char c_str[MESSAGE_SIZE];
    va_start(args, text);

    sprintf(c_str, "%s%s", error_prefixes[level], text);
    vsprintf(message, c_str, args);
    if (level == L_FATAL) {
		fputs(message, stderr);
		fflush(stderr);
    }

	/* Logging in batch mode.
	 */
    if (msg_file) {
		int log_me = 1;
		switch (level) {
		case  L_WARNING:
			if (!log_warning)
				log_me = 0;
			break;
		case L_MSG:
			if (!log_message)
				log_me = 0;
			break;
		case L_DUMP:
			if (!log_raw)
                log_me = 0;
			break;
		case L_DBGMSG:
			if (!log_debug)
				log_me = 0;
		}

		if (log_me) {
            /* only batch repeated error and warning messages */
            if (!strcmp(message, last_message)
                && (level == L_ERROR || level == L_WARNING)) {
                if (++message_count > 1) goto interactive; /* yuck! */
                last_level = level;
            } else {
                int times = message_count - 1;
                strcpy(last_message, message);
 
                message_count = 1;
                if (times > 0) {
                    sprintf(c_str, "%sLast message repeated %d time%s.\n",
                                error_prefixes[last_level],
                                    times, ((times > 1) ? "s" : ""));
			        fputs(c_str, msg_file);
			        fflush(msg_file);
			        fputs( c_str, stdout );
			        fflush( stdout );
                }
            }

			fputs(message, msg_file);
			fflush(msg_file);
			fputs( message, stdout );
			fflush( stdout );
		}
    }

	if (level == L_FATAL) {
		AnnounceFatalError();
    }

	
	/* Logging in interactive mode.
	 */
interactive:
		va_end(args);
		addToLogWindow(message,level);
	
    return;
}

/**
 * \brief  Behaves like perror(3), except prints to the message
 *	window
 *
 *	Note that presently perror_msg always logs the message as type
 *	L_ERROR.
 *
 *	Parameter is a pointer to char.
 */

void
perror_msg(text)
	char *text;
{
    //extern int sys_nerr;
    //extern char *sys_errlist[];

    log_msg(L_ERROR, "%s:%s\n",
	    (text) ? text : "",
		strerror(errno) );
	    //(errno > 0 && errno < sys_nerr) ? sys_errlist[errno] : "");
}

