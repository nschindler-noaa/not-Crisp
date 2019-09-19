/** \file
 * \brief Files in the parser.
 */

#define DEFAULT_VERSION -1

/*
 * A structure which implements all the data we need to keep in mind about
 *	each file for the parser.
 */
typedef struct parser_file {
    FILE *fp;
    int version;
} PARSER_FILE;

/* P_ERROR and P_WARNING print the requested error message to the message
 * window along with information about the location of the error */

#define P_ERROR(format, string) p_error (buffer->fp, format, string)

#define P_WARNING(format, string) p_warning (buffer->fp, format, string)
