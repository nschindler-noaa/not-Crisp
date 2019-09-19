/* parse_monte_index.c
 *
 * 
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "parseutil.h"
#include "monte_index.h"
#include "tokens_mindex.h"
#include "strtools.h"

void release_to_word(string)
	char *string;
{
    while (*string) {
	if (*string == '+') {
	    if (*(string + 1) == '~')
		*string = ')';
	    else *string = '(';
	} else if (*string == '~') {
	    if (*(string + 1) == '\0')
		*string = ']';
	    else *string = '[';
	}
	++string;
    }
}

/*
 * struct monte_index *parse_monte_index(char *filename) -- 
 */
struct monte_index *parse_monte_index(filename)
	char *filename;
{
    void monte_index_free();

    static COMMAND_LIST index_list = 0;

    char token[MAX_COMMAND_LEN + 1], scratch[MAX_COMMAND_LEN + 1];
    FILE *index_infile;
    struct monte_index *index_data;
    int segment_count = 0;
    int release_count = 0;

    index_data = (struct monte_index *) malloc(sizeof(struct monte_index));
    if (!index_data) {
	log_msg(L_ERROR, "No memory to create monte index!\n");
	return (struct monte_index *) 0;
    }
    bzero(index_data, sizeof(struct monte_index));
    if (!(index_infile = buf_open(filename, "r"))) {
	log_msg(L_MSG, "can't open file {%s}\n", filename);
	free(index_data);
	return (struct monte_index *) 0;
    }
    if (!index_list)
	index_list = com_create_list(index_list, index_tokens);

    while (pop_token(index_infile, token, "->") != EOF) {
	switch(com_reference(index_list, lowercase(token, scratch))) {
	  case TK_MI_YEARS:
	    if (!read_int(index_infile, &index_data->years,
			  "number of years ->")) {
		p_error(index_infile,
			"Error parsing # of years in index file {%s}.  %s\n",
			filename, "Aborting");
		monte_index_free(index_data);
		buf_close(index_infile);
		return (struct monte_index *) 0;
	    }
	    break;
	  case TK_MI_NRELEASES:
	    if (!read_int(index_infile, &index_data->release_count,
			  "number of releases ->")) {
		p_error(index_infile,
			"Error parsing # of years in index file {%s}.  %s\n",
			filename, "Aborting");
		monte_index_free(index_data);
		buf_close(index_infile);
		return (struct monte_index *) 0;
	    }
	    index_data->releases = (char **) malloc(index_data->release_count
		* sizeof(char *));
	    if (!index_data->releases) {
		log_msg(L_ERROR, "No memory for index releases!\n");
		monte_index_free(index_data);
		buf_close(index_infile);
		return (struct monte_index *) 0;
	    }
	    bzero(index_data->releases, index_data->release_count
		  * sizeof(char *));
	    break;
	  case TK_MI_NSEGMENTS:
	    if (!read_int(index_infile, &index_data->segment_count,
			  "number of segments ->")) {
		p_error(index_infile,
			"Error parsing # of years in index file {%s}.  %s\n",
			filename, "Aborting");
		monte_index_free(index_data);
		buf_close(index_infile);
		return (struct monte_index *) 0;
	    }
	    index_data->segments = (char **) malloc(index_data->segment_count
		* sizeof(char *));
	    if (!index_data->segments) {
		log_msg(L_ERROR, "No memory for index segments!\n");
		monte_index_free(index_data);
		buf_close(index_infile);
		return (struct monte_index *) 0;
	    }
	    bzero(index_data->segments, index_data->segment_count
		  * sizeof(char *));
	    break;
	  case TK_MI_RELEASE:
	    if (!index_data->releases) {
		log_msg(L_ERROR,
		    "release token read before # of releases token.  %s\n",
		    "Aborting");
		monte_index_free(index_data);
		buf_close(index_infile);
		return (struct monte_index *) 0;
	    }
	    if (pop_token(index_infile, token, "release -> ") == EOF) {
		p_error(index_infile,
			"Unexpected EOF reading monte index {%s}  %s\n",
			filename, "Aborting");
		monte_index_free(index_data);
		buf_close(index_infile);
		return (struct monte_index *) 0;
	    }
	    unders_to_spaces(token);
	    release_to_word(token);
	    if (release_count >= index_data->release_count) {
		log_msg(L_WARNING,
			"Read segment number %d, index claims only %d!\n",
			++release_count, index_data->release_count);
	    } else {
		index_data->releases[release_count] = (char *)
		    malloc(strlen(token) + 1);
		if (!index_data->releases[release_count]) {
		    log_msg(L_ERROR, "No memory for index release data!  %s\n",
			    "Aborting");
		    monte_index_free(index_data);
		    buf_close(index_infile);
		    return (struct monte_index *) 0;
		}
		strcpy(index_data->releases[release_count], token);
		++release_count;
	    }
	    break;
	  case TK_MI_SEGMENT:
	    if (!index_data->segments) {
		log_msg(L_ERROR,
		    "segment token read before # of segments token.  %s\n",
		    "Aborting");
		monte_index_free(index_data);
		buf_close(index_infile);
		return (struct monte_index *) 0;
	    }
	    if (pop_token(index_infile, token, "segment -> ") == EOF) {
		p_error(index_infile,
			"Unexpected EOF reading monte index {%s}  %s\n",
			filename, "Aborting");
		monte_index_free(index_data);
		buf_close(index_infile);
		return (struct monte_index *) 0;
	    }
	    unders_to_spaces(token);
	    if (segment_count >= index_data->segment_count) {
		log_msg(L_WARNING,
			"Read segment number %d, index claims only %d!\n",
			++segment_count, index_data->segment_count);
	    } else {
		index_data->segments[segment_count] = (char *)
		    malloc(strlen(token) + 1);
		if (!index_data->segments[segment_count]) {
		    log_msg(L_ERROR, "No memory for index segment data!  %s\n",
			    "Aborting");
		    monte_index_free(index_data);
		    buf_close(index_infile);
		    return (struct monte_index *) 0;
		}
		strcpy(index_data->segments[segment_count], token);
		++segment_count;
	    }
	    break;
	  case TK_MI_ALTERN:
	    if (pop_token(index_infile, token, "alternative -> ") == EOF) {
		monte_index_free(index_data);
		buf_close(index_infile);
		return (struct monte_index *) 0;
	    }
	    index_data->name = (char *) malloc(strlen(token) + 1);
	    if (!index_data->name) {
		log_msg(L_ERROR, "No memory for index segment data!  %s\n",
			"Aborting");
		monte_index_free(index_data);
		buf_close(index_infile);
		return (struct monte_index *) 0;
	    }
	    strcpy(index_data->name, token);
	    break;
	  case TK_MI_END:
	    if (release_count != index_data->release_count) {
		log_msg(L_WARNING,
			"Index file %s contains only %d releases, claims %d\n",
			release_count, index_data->release_count);
	    }
	    if (segment_count != index_data->segment_count) {
		log_msg(L_WARNING,
			"Index file %s contains only %d segments, claims %d\n",
			segment_count, index_data->segment_count);
	    }
	    buf_close(index_infile);
	    return index_data;
	    break;
	  default:
	    UNKNOWN_COMMAND_ERROR(index_infile, token);
	    break;
	}
    }
    log_msg(L_WARNING, "No end of file marker in monte index file %s\n",
	    filename);
    buf_close(index_infile);
    return index_data;
}
