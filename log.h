/** \file
 * \brief Message logging support.
 *
 * \section log Logging in COMPASS
 *
 * The logging facility in COMPASS works in a similar manner to printf() except
 * that it has the ability to screen messages for output based on their
 * "level" (e.g. warning, debug, etc.). The log messages go to the log window
 * in the GUI and to standard output in batch mode.
 *
 * The user may select which kinds of messages he/she wants to receive and the
 * others are silently ignored.
 */ 

#ifndef _CBR_LOG_H_
#define _CBR_LOG_H_

#include <stdarg.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/** In batch mode, create a log file at program start. */
extern void create_log_file( void );
/** Set the file to write log messages to */
extern void set_log_file(FILE *new_log_file);
/** Delete the log file on program exit so that too many files are not created */
extern void delete_log_file( void );
    
/** Whether warnings should be logged. */
extern int log_warning;
/** Whether messages should be logged. */
extern int log_message;
/** Whether raw output should be logged. */
extern int log_raw;
/** Whether debug message should be logged. */
extern int log_debug;




/** These are the valid log levels for error messages */
enum log_types {L_ERROR, L_WARNING, L_MSG, L_DUMP, L_DBGMSG, L_FATAL,
		    L_FORCE, END_OF_LOG_LIST};

/** Log a message using the given log level. This function works like fprintf()
 * except instead of a FILE* for the first parameter a logging level is given.
 */
extern void log_msg(int level, const char *text, ...);
    
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _CBR_LOG_H_ */

