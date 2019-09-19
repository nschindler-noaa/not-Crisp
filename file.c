#include <errno.h>
#include <fcntl.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifdef WIN32
#include <time.h>
#include <io.h>
#else
#include <strings.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/resource.h>
#endif
#include <sys/stat.h>
#include <sys/types.h>

#include "log.h"
#include "dir.h"

/* location of compass image */

char *image_loc = 0;

#ifndef MAP_FILE
#define MAP_FILE 0
#endif

#define u_mode_bits(x) ((unsigned)((x) & 0000700) >> 6)
#define g_mode_bits(x) ((unsigned)((x) & 0000070) >> 3)
#define o_mode_bits(x) ((unsigned)((x) & 0000007) >> 0)

#define X_BIT(x) (x & 1)
#define FS_EXISTS       0x1
#define FS_EXECABLE     0x2

#define savestring(x) (char *)strcpy (malloc (1 + strlen (x)), (x))

/**
 * \return returns TRUE if gid matches one of the
 *	user's groups, FALSE if it does not.  Does not take effective gid
 *	into account, since compass does not run setgid.
 */
#ifndef WIN32
int group_member (gid)
     gid_t gid;
{
    static gid_t *groups;
    static int num_groups;
    int i;

    if (!groups) {
        /* initialize */
        num_groups = getgroups(0, NULL);
        groups = (gid_t *) malloc((num_groups) * sizeof(gid_t));
        if (!groups) return 0;
        if (num_groups && ((num_groups = getgroups(num_groups, groups))
                           < 0)) {
            free(groups);
            groups = 0;
            return 0;
        }
        /* check if the default group is included in groups */
        for (i = 0 ; i < num_groups ; ++i)
            if (groups[i] == getgid())
                break;
        if (i == num_groups) {
            groups = (gid_t *) realloc(groups, (num_groups + 1) * sizeof(gid_t));
            groups[++num_groups] = getgid();
        }
    }
    for (i = 0 ; i < num_groups ; ++i)
        if (gid == groups[i]) return 1;
    return 0;
}
#endif

/** \return some flags based on information about this file.
   The EXISTS bit is non-zero if the file is found.
   The EXECABLE bit is non-zero the file is executble.
   Zero is returned if the file is not found. */
int
file_status (name)
     char *name;
{
  struct stat finfo;
#ifndef WIN32
  static int user_id = -1;
#endif

  /* Determine whether this file exists or not. */
  if (stat (name, &finfo) < 0)
    return (0);

#ifdef WIN32
  return( FS_EXISTS );
#else
  /* If the file is a directory, then it is not "executable" in the
     sense of the shell. */
  if (S_ISDIR (finfo.st_mode))
    return (FS_EXISTS);

  /* Find out if the file is actually executable.  By definition, the
     only other criteria is that the file has an execute bit set that
     we can use. */
  if (user_id == -1)
    user_id = geteuid ();

  /* Root only requires execute permission for any of owner, group or
     others to be able to exec a file. */
  if (user_id == 0)
      {
      int bits;

      bits = (u_mode_bits (finfo.st_mode) |
              g_mode_bits (finfo.st_mode) |
              o_mode_bits (finfo.st_mode));

      if (X_BIT (bits))
        return (FS_EXISTS | FS_EXECABLE);
  }

  /* If we are the owner of the file, the owner execute bit applies. */
  if (user_id == ((int) finfo.st_uid) && X_BIT (u_mode_bits (finfo.st_mode)))
    return (FS_EXISTS | FS_EXECABLE);

  /* If we are in the owning group, the group permissions apply. */
  if (group_member (finfo.st_gid) && X_BIT (g_mode_bits (finfo.st_mode)))
    return (FS_EXISTS | FS_EXECABLE);

  /* If `others' have execute permission to the file, then so do we,
     since we are also `others'. */
  if (X_BIT (o_mode_bits (finfo.st_mode)))
    return (FS_EXISTS | FS_EXECABLE);
  else
    return (FS_EXISTS);
#endif /* WIN32 */
}


/** \brief Find the COMPASS executable */
void locate_image(argzero)
	char *argzero;
{
    extern int file_status();
    extern char *tilde();
    char fbuf[2048], *ptr, *path;
    int status;

    /* figure out where the binary we're running is, and look there */
    strcpy(fbuf, "");
    
    if ((*argzero == '/') || (*argzero == '.'))
	strcpy(fbuf, argzero);
    else if (*argzero == '~')
	tilde(argzero, fbuf);
    else if ((ptr = (char *) getenv("PATH"))) {
	strcpy(path = (char *) malloc(strlen(ptr) + 1), ptr);
	ptr = (char *) strtok(path, ":");
	do {
	    strcpy(fbuf, ptr);
	    strcat(fbuf, "/");
	    strcat(fbuf, argzero);
	    status = file_status(fbuf);
	    if (!(status & 0x2)) /* If file is EXECABLE. */
		strcpy(fbuf, "");
	} while (!strcmp(fbuf, "") && (ptr = (char *) strtok(NULL, ":")));
	free(path);
    }
    if (image_loc)
	free(image_loc);
    strcpy(image_loc = (char *) malloc(strlen(fbuf) + 1), fbuf);
}


/**
 * 
 * 	\brief find a compass file.
 *
 * first looks for fname either as an absolute path,
 * 	or relative to the current working directory.  If not found, it looks
 * 	for fname in the directory where the compass executable resides.
 *	full_name is provided by the caller for the resulting path string.
 *	No permissions are checked.
 *
 *	returns:	NULL if no file found
 *			char *full_name, with complete path
 */

char *find_compass_file(fname, full_name)
    char *fname;
    char *full_name;
{
    if (file_status(real_filename(fname, full_name)) & FS_EXISTS)
	return full_name;

    dirname(image_loc, full_name);
    strcat(full_name, "/");
    strcat(full_name, fname);
    if (file_status(full_name) & FS_EXISTS)
	return full_name;
    else
	return NULL;
}

/** This function is a just a convenience function for getting the directory that the
 * COMPASS executable is in. It should not replace find_compass_file() use unless there
 * is no choice (as in the case of finding directories). */
const char *get_image_location() {
	return image_loc;
}

