#include <stdlib.h>
#include <string.h>
#ifdef WIN32
#include <stdio.h>
#else
#include <unistd.h>
#include <strings.h>
#include <stdio.h>
#include <sys/param.h>
#endif

#include "crisp.h"
#include "parseutil.h"
#include "tokens_alt.h"
#include "monte.h"

extern void set_flow_archive( struct altern_data *altern, 
							   char *ptr );

struct altern_data *parse_single_alternative(buffer, alternative)
    PARSER_FILE *buffer;
    struct altern_data *alternative;
{
    extern void free_multi_data();

    char token[MAX_COMMAND_LEN+1], scratch[MAX_COMMAND_LEN+1];
    static COMMAND_LIST alt_cmd_list = 0;
    int count;

    /* set up the command menu */
    if (!alt_cmd_list)
	alt_cmd_list = com_create_list(alt_cmd_list, single_alt_tokens);

    /* parse alternative name */
    if (pop_token(buffer->fp, token, "Alternative name -> ") != EOF)
    {

	/* Automatically free the multi data */
	if (alternative->multi)
	    free_multi_data(alternative->multi);
	if (alternative->load_directory)
	    free(alternative->load_directory);
	if (alternative->alternative_name)
	    free(alternative->alternative_name);

	alternative->alternative_name = strdup(token);

	alternative->load_directory = strdup(current_dir());

	if (!(alternative->alternative_name && alternative->load_directory))
	{
	    log_msg(L_FATAL, "No memory available for alternative name.\n");
	    return((struct altern_data *)0);
	}
    }
    else
    {
	/* error message - return */
	p_warning(buffer->fp,
	  "Unexpected EOF at begining of alternative file.  File not read.\n");
	return((struct altern_data *)0);
    }
    
    /* parse alternative  insides */
    while (1)
    {
	if (pop_token(buffer->fp, token, "alternative command -> ") == EOF)
	{
	    if (can_return_from(buffer))
	    {
		buffer = return_after_parse(buffer);
		continue;
	    }
	    else
	    {
		p_error(buffer->fp,
			"EOF detected while parsing alternative.\n");
		return(alternative);
	    }
	}

	/* token  based case stmt */
	switch (com_reference(alt_cmd_list, lowercase(token, scratch)))
	{
	  case TK_INCLUDE:
	    buffer = include_to_parse(buffer);
	    break;

	  case TK_END:
	    reset_input(buffer->fp);
	    return(alternative);

	  case TK_USE_FLOW_FROM:
	    if (buffer->version >= 2)
	    {
		if (pop_token(buffer->fp, token, "Take flow from -> ")
		    == EOF)
		{
		    /* error message -- return */
		    p_warning(buffer->fp,
		        "Unexpected EOF while looking for flow selection.\n");
		    return(alternative);
		}
		if (!strcmp(token, "archive"))
		    alternative->use_flow_from = ARCHIVE_FLOW;
		else
		    alternative->use_flow_from = 0;
		break;
	    }
	    else
	    {
		/* version < 2 uses this token as a spill token.
		 * drop through to next case.  NO BREAK!!!
		 */
	    }
	    /* NO BREAK HERE!! (see above comment) */
	    
	  case TK_USE_SPILL_FROM:
	    if (pop_token(buffer->fp, token, "Take spill from -> ")
		== EOF)
	    {
		/* error message -- return */
		p_warning(buffer->fp,
			  "Unexpected EOF while looking for spill source.\n");
		return(alternative);
	    }
	    if (!strcmp(token, "archive"))
		alternative->use_spill_from = ARCHIVE_SPILL;
	    else
		alternative->use_spill_from = 0;
	    break;
	  case TK_USE_ELEV_FROM:
	    if (pop_token(buffer->fp, token, "Take elevation from -> ")
		== EOF)
	    {
		/* error message -- return */
		p_warning(buffer->fp,
			  "Unexpected EOF while looking for elev source.\n");
		return(alternative);
	    }
	    if (!strcmp(token, "archive"))
		alternative->use_elev_from = ARCHIVE_ELEV;
	    else
		alternative->use_elev_from = 0;
	    break;
	  case TK_FLOW_ARCHIVE_NAME:
	    if (pop_token(buffer->fp, token, "Flow archive file name -> ")
		== EOF)
	    {
		/* error message -- return */
		p_warning(buffer->fp,
			"Unexpected EOF while looking for flow archive name.\n");
		return(alternative);
	    }
	    set_flow_archive(alternative, token);
	    if (!alternative->flow_archive)
	    {
		log_msg(L_FATAL, "No memory available for flow archive name.\n");
		return((struct altern_data *)0);
	    }
	    break;

	  case TK_NGAMES:
	    /* read in the number of games */
	    if (!read_int(buffer->fp,
			  &alternative->ngames,
			  "Enter number of games -> "))
	    {
		/* error message -- return */
		p_error(buffer->fp,
		      "Error while looking for the number of games.\n");
		return(alternative);
	    }
	    break;
	    
	  case TK_INPUT_FILES:
	    /* read in the number of years */
	    if (!read_int(buffer->fp,
			  &alternative->nyears,
			  "Enter number of years (and datafiles) -> "))
	    {
		/* error message -- return */
		p_error(buffer->fp,
		      "Error while looking for the number of years.\n");
		return(alternative);
	    }

	    /* allocate space for the input files */
	    if (alternative->input_files)
		free(alternative->input_files);
	    alternative->input_files = (struct input_file *)malloc(sizeof(struct input_file) * alternative->nyears);
	    if (!alternative->input_files)
	    {
		log_msg(L_FATAL, "No memory available for alternatives input files.\n");
		return((struct altern_data *)0);
	    }
	    bzero(alternative->input_files, sizeof(struct input_file) * alternative->nyears);

	    /* read in the input file names */
	    for (count=0; count < alternative->nyears; count++)
	    {
		if (pop_token(buffer->fp, token, "Input file name -> ") == EOF)
		{
		    /* error message -- return */
		    p_error(buffer->fp,
			    "Unexpected EOF while looking for input file name %d.\n",
			    count+1);
		    return(alternative);
		}
		alternative->input_files[count].file_name = malloc(sizeof(char) * (strlen(token)+1));
		if (!alternative->input_files[count].file_name)
		{
		    log_msg(L_FATAL, "No memory available for input file name.\n");
		    return((struct altern_data *)0);
		}
		(void) strcpy(alternative->input_files[count].file_name, token);
	    }
	    break;

	  case TK_MULTI:
	    {
		int ngames, nyears;

		/* read in the number of games of multi data */
		if (!read_int(buffer->fp, &ngames, "Number of multi games -> ")) {
		    p_error(buffer->fp,
			  "Error looking for number of games.\n");
		    return(alternative);
		}
		if (!read_int(buffer->fp, &nyears,
			      "Number of multi years -> ")) {
		    p_error(buffer->fp,
			    "Error looking for number of years.\n");
		    return(alternative);
		}

		if (ngames < 1) {
		    log_msg(L_ERROR, "%s (%d) read from alternative %s.\n",
			    "Invalid number of games", ngames,
			    alternative->alternative_name);
		    return alternative;
		}

		if (nyears < 1) {
		    log_msg(L_ERROR, "%s (%d) read from alternative %s.\n",
			    "Invalid number of years", nyears,
			    alternative->alternative_name);
		    return alternative;
		}

		/* Games and years check out, allocate memory -- freed in
		   free_alternative_object (free_multi_data) */
		alternative->multi = (struct multi_data *)
		    malloc(sizeof(struct multi_data));
		if (!alternative->multi) {
		    p_error(buffer->fp,
			"No memory available for alternative multi data.\n");
		    return (struct altern_data *) 0;
		}
		bzero(alternative->multi, sizeof(struct multi_data));

		/* Set read values */
		alternative->multi->ngames = ngames;
		alternative->multi->nyears = nyears;

		/* Allocate memory for the nruns we know are coming. */
		alternative->multi->nruns = (int *) malloc(ngames*sizeof(int));
		if (!alternative->multi->nruns) {
		    log_msg(L_ERROR,
			    "No memory available for multi runs.\n");
		    return (struct altern_data *) 0;
		}
		bzero(alternative->multi->nruns, ngames*sizeof(int));
		for (count = 0 ; count < ngames ; count++) {
		    if (!read_int(buffer->fp, alternative->multi->nruns+count,
				  "multi run -> ") ||
			alternative->multi->nruns[count] < 0) {
			p_error(buffer->fp,
				"Error looking for multi data.\n");
			return alternative;
		    }
		    alternative->multi->gamesum +=
			alternative->multi->nruns[count];
		}
/*		alternative->ngames = alternative->multi->gamesum; */
	    }
	    break;

	default:
	    /* default - output some error message */
	    UNKNOWN_COMMAND_ERROR(buffer->fp, token);
	}
    }
}

unsigned char open_and_parse_alternative(filename, alternative)
    char *filename;
    struct altern_data *alternative;
{
    extern void free_archive_header();
    extern struct archive_header *parse_archive_header_in_alternative();

    PARSER_FILE *buffer;
    char token[MAX_COMMAND_LEN+1], scratch[MAX_COMMAND_LEN+1];
    static COMMAND_LIST alt_main_list = 0;

    if (!(buffer = open_to_parse(filename)))
	return(1);

    /* set up the command menu */
    if (!alt_main_list)
	alt_main_list = com_create_list(alt_main_list, alt_tokens);

    /* parse alternative and include */
    while (1)
    {
	if (pop_token(buffer->fp, token, "alternative command -> ") == EOF)
	{
	    if (can_return_from(buffer))
	    {
		buffer = return_after_parse(buffer);
		continue;
	    }
	    else
	    {
		close_after_parse(buffer);
		return(0);
	    }
	}
	
	/* token  based case stmt */
	switch (com_reference(alt_main_list, lowercase(token, scratch)))
	{
	case TK_INCLUDE:
	    buffer = include_to_parse(buffer);
	    break;

	case TK_ALTERNATIVE:
	    (void) parse_single_alternative(buffer, alternative);
	    if (alternative->use_flow_from == ARCHIVE_FLOW)
	    {
		if (alternative->multi) {
		    alternative->ngames = alternative->multi->gamesum;
		} else {
		    struct archive_header *head;

		    head = parse_archive_header_in_alternative(alternative);
		    if (head) {
			alternative->ngames = head->ngames;
			free_archive_header(head);
		    } else {
			log_msg(L_ERROR, "Can't parse flow archive\n");
		    }
		}
	    }
	    break;
	case TK_VERSION:
	    if (!read_int(buffer->fp, &buffer->version, "data version ->"))
		P_ERROR("Parsing version %s\n"," ");
	    break;
	default:
	    /* default - output some error message */
	    UNKNOWN_COMMAND_ERROR(buffer->fp, token);
	}
    }
}
