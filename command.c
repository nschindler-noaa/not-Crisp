/** \file
 * \brief Implementation of support functions for creating parsers. */

#include <string.h>
#ifdef WIN32
#include <stdlib.h>
#include <stdio.h>
#else
#include <stdio.h>
#include <strings.h>
#endif
#include <errno.h>
#include <stdlib.h>
#include "command.h"
#include "log.h"
    
struct command_list
{
    int index;			/* number returned to calling fcn */
    unsigned int unique_char;   /* the first unique character in the command */
    char command[MAX_COMMAND_LEN]; /* the actual command 		  */
    struct command_list *next;     /* next in the linked list */
};


static unsigned int find_unique_char (s1, s2)
    char *s1, *s2;
{
    if ( (*s1 != *s2) || (*s1 == '\0') )
	return (1);
    else
	return (1 + find_unique_char ( (s1+1), (s2+1) ));
}

static void set_unique_char_list (list, new)
    struct command_list *list, *new;
{
    unsigned int uc;
    
    if (list)
    {
	if ( (uc = find_unique_char (list->command, new->command)) > list->unique_char)
	    list->unique_char = uc;
	if ( uc > new->unique_char )
	    new->unique_char = uc;
	set_unique_char_list (list->next, new);
    }
    return ;
}

static struct command_list *new_command(index, command)
    int index;
    char *command;
{
    struct command_list *new;
    if ((new=(struct command_list *)malloc(sizeof(struct command_list))))
    {
	new->index = index;
	(void) strncpy(new->command, command, MAX_COMMAND_LEN);
	new->unique_char = 0;
	new->next = 0;
    }
    return(new);
}

static void com_add_to_list_r (clist, new)
    struct command_list *clist, *new;
{
    if (!clist->next)
	clist->next = new;
    
    else if (strcmp(new->command, clist->next->command) > 0)
	com_add_to_list_r (clist->next, new);
    
    else if (strcmp(new->command, clist->next->command) < 0)
    {
	new->next = clist->next;
	clist->next = new;
    }
    
    else if (strcmp(new->command, clist->next->command) == 0)
    {
	log_msg(L_WARNING, "Command %s with index %d duplicated.  Index is now %d.\n",
			new->command, clist->next->index, new->index);
	clist->next->index = new->index;
	(void) free ((char *) new);
    }
    
    else
	log_msg(L_MSG, "Pure CHAOS in com_add_to_list_r. new command was (%s).\n",
			new->command);
    return;
}

/** \brief Add a command to the list of commands */
COMMAND_LIST com_add_to_list (clist, index, command)
    struct command_list *clist;
    int index;
    char *command;
{
    struct command_list *new;
    if (!(new=new_command(index,command)))
    {
	perror ("com_add_to_list: error allocating memory:");
	return((COMMAND_LIST) clist);
    }
    set_unique_char_list (clist, new);
    
    if (!clist)
	clist = new;
    else if (strcmp(new->command, clist->command) < 0)
    {
	new->next = clist;
	clist = new;
    }	
    else if (strcmp(new->command, clist->command) == 0)
    {  /* if the commands are identical, have the new one replace the old one */
	log_msg(L_WARNING, "Command %s with index %d duplicated.  Index is now %d.\n",
			clist->command, clist->index, new->index);
	clist->index = new->index;
	(void) free ((char *)new);
    }
    else
	com_add_to_list_r (clist, new);
    
    return((COMMAND_LIST) clist);
}

/** \brief Add an array of commands to the list */
COMMAND_LIST com_add_array_to_list (clist, com_array)
    struct command_struct *com_array;
    struct command_list *clist;
{
    if (com_array && com_array->index)
    {
	clist=(struct command_list *)com_add_to_list(clist, com_array->index, com_array->command);
	clist=(struct command_list *)com_add_array_to_list(clist, (com_array+1));
    }
    return ((COMMAND_LIST)clist);
}


/** \brief Create a command list */
COMMAND_LIST com_create_list (clist, com_array)
    struct command_struct *com_array;
    struct command_list *clist;
{
    clist=0; /* list might already exists, but we don't care */
    if (com_array && com_array->index)
	clist=(struct command_list *)com_add_array_to_list(clist, com_array);
    return((COMMAND_LIST)clist);
}

/** \brief Print out the list of commands */
void com_print_list(buffer, clist)
    FILE *buffer;
    struct command_list *clist;
{
    if (clist)
    {
	(void) fprintf (buffer, "command (%s) is index %d, unique char %d\n",
			clist->command, clist->index, clist->unique_char);
	com_print_list(buffer, clist->next);
    }
}
    
/** \brief Destroy a command list */
void com_destroy_list (clist)
    struct command_list *clist;
{
    if (clist)
    {
	com_destroy_list (clist->next);
	(void) free ( (char *) clist);
    }
}

/* NOTE that c1 must be the command entered and c2 must be the
 * command from the list 
 *      Change this so I can delete the bloody note
 */
static unsigned char command_matches (c1, c2, minlen)
    char *c1, *c2;
    unsigned int minlen;
{
    unsigned int cur=0;
    while ( (*(c1+cur) == *(c2+cur)) && (*(c1+cur) != '\0') )
	cur++;

    if (( *(c1+cur) ==  *(c2+cur) || *(c1+cur) == '\0' ) &&
	(cur+1) >= minlen)
	return (1);
    else
	return (0);
}

/**
 * \brief locate a command in a list.  abbreviation may or may not be allowed,
 *	as specified by the third param.
 *
 * 	\return 0 if not found, or index of matching command
 */

static int com_ref (clist, command, abbrev_ok)
    struct command_list *clist;		/* list of commands */
    char *command;			/* command string to compare */
    int abbrev_ok;			/* bool: abbreviation ok? */
{
    if (clist==0)
	return (0);
    else if (command_matches (command, clist->command,
			      abbrev_ok ? clist->unique_char
			      		: strlen(clist->command)))
	return (clist->index);
    else
	return (com_ref(clist->next, command, abbrev_ok));
}

/**
 * \brief interface to com_ref(), with abbreviations
 */
int com_reference (clist, command)	
    struct command_list *clist;		
    char *command;			
{
    return (com_ref(clist, command, 1));	/* abbreviations ok */ 
}

/** \brief Interface to com_ret() without abbreviations */
int com_reference_full (clist, command)	
    struct command_list *clist;		
    char *command;			
{
    return (com_ref(clist, command, 0));	/* no abbreviations */ 
}

