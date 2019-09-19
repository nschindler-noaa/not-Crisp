/** \file
 * \brief Functions for finding the COMPASS files and the actual executable. */

#ifndef FILE_H
#define FILE_H

#ifdef __cplusplus
extern "C" {
#endif
    
#define FS_EXISTS       0x1
#define FS_EXECABLE     0x2

#ifndef MAXPATHLEN
#define MAXPATHLEN (1024)
#endif

extern int file_status(/* char *filename */);

extern void locate_image(const char * const argzero);
extern char *find_compass_file(char *fname, char *full_name);

extern const char *get_image_location();

#ifdef __cplusplus
}
#endif

#endif
