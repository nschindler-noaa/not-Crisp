/** \file
 * \brief Utility functions and macros for parsing input files.
 */

#ifndef CRISP_PARSEUTIL_H
#define CRISP_PARSEUTIL_H

#include "command.h"
#include "log.h"
#include "include.h"
#include "dir.h"
#include "parser_file.h"
#include "monte.h"

#ifndef TRUE
#define FALSE	0
#define TRUE	1
#endif

/* UNKNOWN_COMMAND_ERROR prints an appropriat message using P_ERROR.  It also
 * checks to see if the token is a number, and skips any following numbers if
 * it is. */
#define UCE_STR1 "unknown command {%s}, which is a number.\n\tSkipping all numbers until a non-numeric token is reached.\n"
#define UCE_STR2 "unknown command {%s}: line discarded\n"

#define UNKNOWN_COMMAND_ERROR(buffer, token)\
{\
    if (is_float(token))\
    {\
	p_error(buffer, \
		UCE_STR1, \
		token);\
	(void)skip_all_numbers(buffer);\
    }\
    else\
    {\
	p_error (buffer, UCE_STR2, token);\
	reset_input(buffer);\
    }\
}

typedef enum array_conversion_type { ACT_NO_CONV, ACT_DUP, ACT_SPLIT, ACT_HALF } aconv;

extern char * lowercase(/* char *s, char *scratch */);

extern char * unders_to_spaces (/* char *string */);

extern unsigned char is_float (/* char *string */);

extern unsigned char is_int (/* char *string */);


extern void p_error (FILE *buffer, char *format, ... );



extern void p_warning (FILE *buffer, char *format, ... );


extern unsigned char read_int (/* FILE *buffer, int *number, char *prompt */);

extern unsigned char read_uns (/* FILE *buffer,uns int *number,char *prompt */);

extern unsigned char read_float (/* FILE *buffer, float *number, char *prompt */);

extern unsigned char read_float_or_na (/* FILE *buffer, float *number, char *prompt , int *was_na*/);

extern unsigned char read_double (/* FILE *buffer, double *number, char *prompt */);

extern unsigned char read_float_array (/* FILE *buffer, float array[],
					  int max_elem, aconv conversion,
					  unsigned mult, char *prompt */);

extern unsigned char skip_all_numbers (/* FILE *buffer */);

extern unsigned char skip_numbers (/* FILE *buffer, int max */);

extern unsigned char skip_num_array (/* FILE *buffer, int max */);

extern struct period_list_struct *parse_periods (/* FILE *buffer */);

extern PARSER_FILE * open_to_parse(/* char * filename */);

extern void close_after_parse(/* PARSER_FILE *buffer */);

extern FILE * include(/* FILE *buffer */);

extern PARSER_FILE *include_to_parse(/* PARSER_FILE *buffer */);

#define can_return_from(buffer) can_pop_buf()

extern PARSER_FILE *return_after_parse(/* PARSER_FILE *buffer */);

extern int write_separator(FILE *outfile);

extern void free_archive_headers (struct archive_header **archive_data, int nheaders);
extern void read_flow_and_spill_from_archive (int game, int year, struct archive_header *archive_data,
	struct archive_spill **game_spills, FILE *fp, long **tells, int archive_elevation_flag);
extern void free_archive_header(struct archive_header *header);


#endif /* CRISP_PARSEUTIL_H */

