/** \file
 * \brief Directory traversal and searching functions */

#ifndef CRISP_DIR_H
#define CRISP_DIR_H

#ifndef MAXPATHLEN
#define MAXPATHLEN (1024)
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern int pushd ( /* char * newdir */ );
   /* returns chdir codes */

extern unsigned char can_popd();
   /* returns 1 if you can pop a directory, 0 otherwise */

extern int popd ();
   /* returns chdir codes */

extern char* base_name (char *path, char *name);
   /* returns a pointer to the basename */

extern char * dirname ( /* char * path, char * dir */ );
   /* returns a pointer to the directory */

extern char * real_filename (/* char * path, char * newpath */);
   /* returns a pointer to newpath */

extern void remember_current_directory();

char *current_dir();
char *tilde(char *file, char *scratch);


/* Unix routines that are also usefull
       int access(path, mode)
	     char *path;
	     int mode;
		      
       char *realpath(path, resolved_path)
	    char *path;
	    char resolved_path[MAXPATHLEN];    

*/

#ifdef __cplusplus
}
#endif

#endif

