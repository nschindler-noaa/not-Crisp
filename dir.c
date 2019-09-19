#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef WIN32
#include <direct.h>
#else
#include <sys/param.h>
#include <unistd.h>
#endif

#include <errno.h>

#include "dir.h"

/* These are the characters that the users can use in file names */
#define FILENAME_CHARS "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890!@$%^&*()=+\\|'`\"~;:[]{},.<>?#_-\r\n\t "

/* Define the getwd() and realpath() functions which exist on Unix but 
 *   not on the PC. 
 */
#ifdef WIN32

/** \return Current working directory. */
char * getwd( char *buf )
{
	char *pRet, *p;

	/* NOTE we assume "buf" was preallocated to MAXPATHLEN */
	pRet = getcwd( buf, MAXPATHLEN );

	/* Convert path separator characters ('\') to Unix convention ('/').
	 *  This simplifies multiplatform issues in the legacy C code.
	 */
	if (pRet != NULL) {
		for (p = buf; *p != '\0'; p++)
			if (*p == '\\')
				*p = '/';
	}

	return pRet;

}

/** Get the real path of a file */
char *realpath( char *file_name, char *resolved_name )
{
  return strcpy( resolved_name, file_name );
}

#endif

/* define the stack type */
struct dir_stack
{
    char path[MAXPATHLEN];
    struct dir_stack *next;
};

static struct dir_stack *stack = 0;
static char *current_directory = 0;

/** Push onto the stack */
static void push ( dirname )
    char * dirname;
{
    struct dir_stack *new = (struct dir_stack *)malloc(sizeof(struct dir_stack));

    new->next = stack;
    (void) strncpy ( new->path, dirname, MAXPATHLEN );
    stack = new;
}

/** Pop a directory off the stack */
static void pop ( )
{
    struct dir_stack *old = stack;

    if (stack != 0)
    {
	stack = stack->next;
	free (old);
    }
}

extern void AnnounceFatalError( void );

/** Push a directory on the stack and enter it */
int pushd ( newdir  )
    char * newdir;
{
    int ret=0;
    char cwd[MAXPATHLEN];
    
    /* put the old dir on top of the stack */
    if (getcwd(cwd, MAXPATHLEN) == NULL)
    {
	perror ("pushd");
	AnnounceFatalError();
    }
    push(cwd);
    
    /* chdir there */
    /* if an error during the chdir, pop the top of the stack */
    if ( (ret = chdir(newdir)) ) pop();
    
    /* return the chdir code */
    return (ret);
}

/** \return if the stack can be popped */
unsigned char can_popd()
{
    return (stack?1:0);
}

/** Pop a directory off of the stack */
int popd ()
{
    int ret=0;

    /* if stack null, get out */
    if (!stack)
    {
	fprintf (stderr, "WARNING: popd called with directory stack empty.\n");
	return (ret);
    }
	    
    /* chdir to top of stack */
    ret = chdir(stack->path);
    
    pop();

    return (ret);
}


/** Get the base name */
char * base_name (path,name)
    char * path;
    char * name;
{
    char *temp;

    if ( (temp=strrchr(path, '/')) )
	(void) strcpy(name, ++temp);
    else
	(void) strcpy(name,path);

    return(name);
}

/** Get the directory name */
char * dirname ( path, dir )
    char *path, *dir;
{
    char *temp;

    if (dir) {
	(void) strcpy(dir,path);
    
	if ( (temp = strrchr(dir, '/')) )
	    *temp='\0';
	else
	    *dir = '\0';
    }
    return (dir);
}

#ifdef WIN32

/** We need a "tilde()" routine on WIN32, because it does get called in
 *  various places. But Windows has no concept of a ~username path, so
 *  this routine will just take the default action every time.
 */
char *tilde(file, scratch)
    char *file;
    char *scratch;
{
  return(strcpy(scratch, file));
}


#else

#ifndef VMS
#include <pwd.h>
#endif

/**
 *
 * \brief Evaluate ~ filename expressions, including ~ and ~username
 * It returns 0 if the original expression does not begin with ~.
 * (Is there any use for having ~'s other than as the first character?)
 *
 * Jon "~root/setuid" Sari, 1990
 *
 * Ken Case, 1991	modified to return original string on failure,
 *			rather than NULL, and to get the passwd entry
 *			itself.
 * Pete Pulliam, 1991   small modification to use old style c instead
 * 			of the totally cool ansi c that Jon uses. Also,
 *			made it take two parameters so the path doesn't
 *			get smashed, and to return a pointer to the secon
 * 			parameter.  Also, to evaluate ~ as if it was ~/ and
 *			~name as if it was ~name/.  Also to use strcpy instead
 *			of strncpy in case the user passes a short second var.
 *			whee.
 */

char *tilde(file, scratch)
    char *file;
    char *scratch;
{
  static struct passwd *user = (struct passwd *) 0;
  char *ptr, *indx = file;
  static char buf[MAXPATHLEN];

  if (*indx != '~') return(strcpy(scratch, file));

  /* Evaluate ~ expressions. */
  if (*++indx == '/' || *indx == '\0')		/* Increment past '~' */
    (void)strcpy(scratch, (char *) getenv("HOME"));
  else {
    int i;

    ptr = indx;
    if (!(indx = (char *) strchr(file, '/'))) indx = (char *) strchr(file, '\0');
#ifdef VMS
    for(buf[0]='[', i=1; ptr < indx; buf[i++] = *ptr++);
    buf[i++] = ']';
    buf[i] = '\0';
    indx++;
#else 
    for(i=0; ptr < indx; buf[i++] = *ptr++);
    buf[i] = '\0';
    if(user && strcmp(buf, user->pw_name)==0)
      strcpy(scratch, user->pw_dir);
    else {
      /* Find out who it might be */
      if (!(user = getpwnam(buf))) {
	return (strcpy(scratch, file));
      } else {
	strcpy(scratch, user->pw_dir);
      }
    }
#endif
  }
  strcat(scratch, indx);
  return (scratch);
}

#endif

/** Get the real filename after applying the tilde */
char * real_filename(path, newpath)
    char * path;
    char * newpath;
{
    char scratch[MAXPATHLEN];

    tilde(path, scratch);

    realpath(scratch, newpath);

    return(newpath);
}
       
/**
 * \brief Stores the current directory
 *	in our static directory pointer.
 */
void remember_current_directory()
{
	char dir[MAXPATHLEN];

    if (getcwd(dir, MAXPATHLEN) == NULL) {
	strcpy(dir, "/");
    }
	if (current_directory != NULL)
		free( current_directory );
    current_directory = strdup(dir);
}

/**
 * \brief Returns our internal notion of the current
 *	directory, which may differ from our actual current directory
 *	if we are in a subdirectory reading files.
 */
char *current_dir()
{
    return current_directory;
}

