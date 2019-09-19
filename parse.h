/** \file
 * \brief Input file parsing definitions. See parse.c for more information. */

#ifndef PARSE_H
#define PARSE_H

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

extern int parse_datafile(
const char *filename,
    int replace,			/* bool; delete data first if true */
int refresh			/* bool; refresh wins after if true */
);

extern void write_saved_comments(FILE *fp, char *filename);
    
#ifdef __cplusplus
}
#endif

#endif /* PARSE_H */
