/** \file
 * \brief Tokens for parsing. */

#ifndef CRISP_IO_H
#define CRISP_IO_H

#include <stdio.h>	

#define BUFFERS             /* Use the multiple io file version of this util */

#define LINE_LENGTH 1024    /* the maximum number of characters to be read on a line */

#define MAX_TOKEN_LEN 128  /* initialize token arrays to be one larger than this
			     * to make room for a '\0' */

#ifdef __cplusplus
extern "C" {
#endif

extern int at_end_of_line(FILE *buffer);
extern int pop_sized_token( FILE *buffer, char *string, char *prompt, int max_token_length );
extern int pop_token( FILE *buffer, char *string, char *prompt );
extern int pop_line ( FILE *buffer, char *string, char *prompt );
extern void push_last_token( FILE *buffer );
extern void push_last_chars( FILE *buffer, int number );
extern void reset_input( FILE *buffer );

extern FILE *buf_open( char *path, char *access_mode );
extern int buf_close( FILE *buffer );
extern char * buf_path( FILE *buffer );
extern int buf_line( FILE *buffer );
extern void incr_buf_line( FILE *buffer );

extern char *string_to_word ( char *string, char *scratch );

#ifdef __cplusplus
}
#endif

#define COMMENT_TOKEN_TEST(c) (c == '#')

#endif /* CRISP_IO_H */


