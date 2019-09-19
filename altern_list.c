/* altern_list.c
 *
 * This package is a list, built around a TREE, which tracks Monte
 * Carlo alternatives in the current directory, allowing code which
 * uses this list to iterate over the existing alternatives,
 * performing whatever operations on them are desired.
 */


#ifndef WIN32
#include <unistd.h>
#endif
#include <errno.h>
#include <sys/types.h>
#ifdef WIN32
#include <time.h>
#include <io.h>
#include <stdlib.h>
#include <windows.h>
#else
#include <sys/time.h>
#endif
#include <string.h>
#include <sys/stat.h>
#ifndef WIN32
#include <dirent.h>
#endif
#include <stdlib.h>
#include "tree.h"
#include "strtools.h"
#include "log.h"
#include "altern_list.h"

extern char *setup_file;
extern int tree_element_exists();

static int altern_tree_add(struct altern_tree_node *name);

/* The extension in the alternative subdirectory which represents an
 * alternative output file.
 */
#define OUT_EXTENSION	".out"
#define ALT_EXTENSION   ".alt"

/* The maximum filename size.
 */
#define MAX_NAME_LEN 256

#ifdef WIN32
/* we need this constant and don't know where to get it.
 *  TODO - get it from a header file somewhere.
 */
#define R_OK  04
#endif

static TREE *altern_tree = 0;

/**
 * \brief Check to see if the
 *	current directory has changed at all since the time pointed to
 *	by the dirtime pointer.
 *
 *	For general use, this function should be called by a module which
 *	stores a static time_t variable recording the last time it called
 *	this function, so that it can perform any operations it needs to
 *	independent of other uses of this module.  If the calling function
 *	has no desire to maintain its own pointer, then it can pass a null
 *	pointer and the internal static will be used.
 *
 * NOTE we actually track the date on ".compass-alts" in the current directory,
 *  because the DOS file system doesn't maintain a "last modified date" for
 *  directories, only for files.
 */
int working_directory_modified(	time_t *dirtime)
{
    struct stat s_buf;
    static time_t time_save = 0;

    if (!dirtime)
	dirtime = &time_save;

    if (stat(setup_file, &s_buf) < 0) {
        log_msg(L_ERROR, 
            "Can't access .compass-alts in the current working directory!\n");
	return -1;
    }

    if (*dirtime != s_buf.st_mtime) {
	*dirtime = s_buf.st_mtime;
	return 1;
    }
    return 0;
}

static time_t check_save = 0;

/**
 * \brief  Updates the current time for
 *	the altern_files_modified function, treating all monte carlo
 *	files as having been checked as of the current time.
 */

void set_altern_updated_flag(time_t *last_checked){
    if (!last_checked) 
		last_checked = &check_save;

    time(last_checked);
}

/**
 * \brief  Returns the number of Monte Carlo
 *	alternatives which have been modified since the last call to
 *	this function, and sets the altern_list flag for each alternative
 *	which has been modified, or -1 if one or more alternatives are
 *	no longer accessible.
 */
int altern_files_modified(time_t *last_checked){

    extern void reset_altern_list();
    extern char *get_current_list_altern();
    extern void inc_altern_list();

    extern int errno;
	
    char *altern_name;
    char *monte_filename;
    struct stat s_buf;
    int return_flag = 0;

    if (!last_checked)
	last_checked = &check_save;

    for (reset_altern_list() ; (altern_name = get_current_list_altern())
	 ; inc_altern_list()) {
	monte_filename = monte_output_name(altern_name);
	if (access(monte_filename, R_OK) < 0) {
	    log_msg(L_MSG, "Alternative %s no longer accessible, %s\n",
		    altern_name, "removing from list");
	    altern_list_remove(altern_name);
	    return_flag = -1;
	}
	if (stat(monte_filename, &s_buf) < 0) {
	    log_msg(L_WARNING, "Can't access alternative %s, Error %s\n",
		    altern_name,
		    strerror(errno) );
			//(errno <= sys_nerr) ? sys_errlist[errno] : "");
	    continue;
	}
	if (s_buf.st_mtime > *last_checked) {
	    if (return_flag != -1)
		return_flag++;
	    set_current_altern_list_flag(1);
	} else
	    set_current_altern_list_flag(0);
    }
    time(last_checked);

    return return_flag;
}

/**
 * \brief  Returns a pointer to a
 *	static filename buffer which contains the name for the monte
 *	carlo output file for the given directory.
 */
char *monte_output_name(char *dir_name){

    static char fname[MAX_NAME_LEN];
    int i;

	i = strlen( dir_name );

    if (i*2 + 5 >= MAX_NAME_LEN) {
		if (i >= MAX_NAME_LEN)
			log_msg(L_FATAL, "Buffer overrun determining output name\n");
		else
			log_msg(L_ERROR, "alternative name too large for buffer\n");
		return 0;
    }

	strcpy( fname, dir_name );
	strcat( fname, "/" );
	strcat( fname, dir_name );
    strcat( fname, OUT_EXTENSION );
    return fname;
}

/**
 * \brief Returns a pointer to a
 *	static filename buffer which contains the name for the monte
 *	carlo alternative file for the given directory.
 */
char *monte_altern_name(const char *dir_name)
{
    static char fname[MAX_NAME_LEN];
    int i;

	i = strlen( dir_name );

    if (i*2 + 5 >= MAX_NAME_LEN) {
		if (i >= MAX_NAME_LEN)
			log_msg(L_FATAL, "Buffer overrun determining alternative name\n");
		else
			log_msg(L_ERROR, "alternative name too large for buffer\n");
		return 0;
    }

	strcpy( fname, dir_name );
	strcat( fname, "/" );
	strcat( fname, dir_name );
    strcat( fname, ALT_EXTENSION );
    return fname;
}

/**
 * \brief Reads the current directory from
 *	scratch, adding all Monte Carlo alternatives to the list.
 */
int altern_update_directory()
{
    void empty_altern_list();

#ifdef WIN32
	WIN32_FIND_DATA fd;
	HANDLE sh;
#else
    DIR *cur_dir;
    struct dirent *entry;
    struct stat s_buf;
#endif
    char *fname;

    empty_altern_list();
#ifdef WIN32
	sh = FindFirstFile( "*", &fd );
	if (sh == INVALID_HANDLE_VALUE) {
		log_msg(L_ERROR, "Unable to open current directory.\n");
		return 0;
	}
	do {
		if (!strcmp( fd.cFileName, "."))
			continue;
		if (!strcmp( fd.cFileName, ".."))
			continue;
		if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) 
			continue;
		fname = monte_altern_name( fd.cFileName );
		if (access(fname, R_OK) == 0)
			if (!altern_list_add(fd.cFileName)) return 0;
	} while (FindNextFile( sh, &fd ));
	FindClose( sh );
#else
    if ((cur_dir = opendir(".")) == 0) {
	log_msg(L_ERROR, "Unable to open current directory.\n");
	return 0;
    }
    while ((entry = readdir(cur_dir))) {

	if (stat(entry->d_name, &s_buf) < 0) {
	    log_msg(L_MSG, "Can't stat file %s\n", entry->d_name);
	    continue;
	}

	if (!S_ISDIR(s_buf.st_mode)) continue;
	fname = monte_altern_name(entry->d_name);
	if (access(fname, R_OK) == 0)
	    if (!altern_list_add(entry->d_name)) return 0;
    }
    closedir(cur_dir);
#endif
    return 1;
}

/**
 * \brief Remove all list items which no longer
 *	have readable output files.
 */
void prune_altern_list()
{
    extern int errno;

    char *altern_name;
    char *monte_file;

    extern void reset_altern_list();
    extern char *get_current_list_altern();
    extern void inc_altern_list();

    for (reset_altern_list() ; get_current_list_altern() ; inc_altern_list()) {
	altern_name = get_current_list_altern();
	monte_file = monte_output_name(altern_name);

	if (access(monte_file, R_OK) < 0) {
	    if (errno == ENOENT || errno == EACCES || errno == ENOTDIR)
		altern_list_remove(altern_name);
	    else
		log_msg(L_WARNING,
			"File system error accessing %s alternative\n",
			altern_name);
	}
    }
}

/**
 * \brief Returns the current node pointer to the
 *	front of the alternative list.
 */
void reset_altern_list()
{
    extern void tree_reset(TREE *);

    if (altern_tree)
	tree_reset(altern_tree);
}

/**
 * \brief Get the name of the current
 *	list item.
 */
char *get_current_list_altern()
{
    extern data_type get_current_tree_datum(TREE *);
    struct altern_tree_node *node;

    if (altern_tree) {
	node = (struct altern_tree_node *) get_current_tree_datum(altern_tree);
	return (node) ? node->name : 0;
    } else return (char *) 0;
}

/**
 * \brief Returns true if "altern" found
 *  in tree, false otherwise. If found, current altern pointer is left pointing
 *  at "altern"; if not found, is left pointing off end of list. NOTE search
 *  is case independant on PC, case dependant on Unix.
 */
int search_altern_list( const char *altern ) {
    for (reset_altern_list() ; get_current_list_altern() ; inc_altern_list()) {
#ifdef WIN32
		if (!strcasecmp( altern, get_current_list_altern() ))
#else
		if (!strcmp( altern, get_current_list_altern() ))
#endif
			return 1;
	}
	return 0;
}

/**
 * \brief Returns true if there is an element
 *	in the alternative tree whose name matches altern, false otherwise.
 */
int altern_exists(char *altern)
{
    
	static struct altern_tree_node node;
    

    node.name = altern;
    return (altern_tree) ? tree_element_exists(altern_tree, &node) : 0;
}

/**
 * \brief Returns the number of alternatives
 *	in the altern list.
 */
int get_altern_list_size()
{
    return get_tree_size(altern_tree);
}

/**
 * \brief Returns the number of
 *	the current list item, with 0 representing the first
 *	alternative in alphabetical order, and list size - 1
 *	representing the last alternative in alphabetical order.
 */
int get_current_list_item_number()
{
    return tree_current_item_number(altern_tree);
}

/**
 * \brief Increments the altern_tree's current
 *	item pointer.
 */
void inc_altern_list()
{
    extern void tree_increment();

    if (altern_tree)
	tree_increment(altern_tree);
}

/**
 * \brief Toggles the flag for
 *	the current altern list item.
 */
void set_current_altern_list_flag(int flag)
{
    extern data_type get_current_tree_datum();
    struct altern_tree_node *node;

    if (altern_tree) {
	node = (struct altern_tree_node *) get_current_tree_datum(altern_tree);
	if (!node) return;
	node->flag = flag;
    }
}

/**
 * \brief Steps through all altern_tree items,
 *	setting all of its flags to zero.
 */
void clear_all_altern_flags()
{
    for (reset_altern_list(); get_current_list_altern() ; inc_altern_list())
	set_current_altern_list_flag(0);
}

/**
 * \brief Returns the flag value of the
 *	current tree element, which is expected to be 1 or 0.  This function
 *	returns -1 if there is an error condition.  (No list, or current
 *	pointer non-existent).
 */
int get_current_altern_list_flag()
{
    extern data_type get_current_tree_datum();
    struct altern_tree_node *node;

    if (altern_tree) {
	node = (struct altern_tree_node *) get_current_tree_datum(altern_tree);
	if (!node) return -1;
	return node->flag;
    } else return -1;
}

/**
 * \brief Add the named alternative to the tree.
 */
int altern_list_add(const char *name)
{
    struct altern_tree_node *ptr;

    if (!(name && *name)) {
	log_msg(L_ERROR, "Illegal alternative name flagged in altern_list\n");
	return 0;
    }
    if (!(ptr = (struct altern_tree_node *)
	  malloc(sizeof(struct altern_tree_node)))) {
	log_msg(L_ERROR, "Out of virtual memory in altern_list_add!\n");
	return 0;
    }
    if (!(ptr->name = strdup(name))) {
	free(ptr);
	log_msg(L_ERROR, "Out of virtual memory in altern_list_add!\n");
	return 0;
    }
    ptr->flag = 0;
    return altern_tree_add(ptr);
}

/**
 * \brief Removes an alternative matching
 *	name from the tree.
 */
int altern_list_remove(const char *name)
{
    struct altern_tree_node n;
    int i;

    n.name = strdup( name );
    i = tree_remove(altern_tree, &n);
    free( n.name );
    return i;
}

/**
 * \brief comparison function for TREE.
 */
static int altern_tree_compare(struct altern_tree_node *name1,	struct altern_tree_node *name2)
{
    return -strcmp(name1->name, name2->name);
}

/**
 * \brief Frees a struct
 *	altern_tree_node pointer.
 */
static void altern_tree_free(struct altern_tree_node *ptr)
{
    free(ptr->name);
    free(ptr);
}

/**
 * \brief Adds the specified
 *	node to the alternative tree.
 */
static int altern_tree_add(struct altern_tree_node *name)
{
    if (!altern_tree) {
	altern_tree = tree_init(altern_tree_compare, altern_tree_free,
		1, sizeof(struct altern_tree_node));
    } else
	if (tree_element_exists(altern_tree, name)) {
	    log_msg(L_WARNING, "Trying to add existing list element %s!\n",
		    name);
	    return -1;
	}
    return tree_insert(altern_tree, name) != -1;
}

/**
 * \brief Frees the tree.
 */
void empty_altern_list()
{
    extern void tree_free();

    if (altern_tree) {
	tree_free(altern_tree);
	altern_tree = 0;
    }
}

