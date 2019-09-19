/** \file
 * \brief  basic definitions for model code (time steps per day, season length, etc.) */

#ifndef CRISP_DEFS_H
#define CRISP_DEFS_H


/** The length of char strings used for names */
#define NAME_LEN	64 	

/** Model steps per day (this should be a power of 2 and at most DAM_TIME_SLC) */
#define	STEPS_PER_DAY	(2)	

/** Julian day offset */
#define SEASON_START_DAY	(0)	
/** Number of the model step corresponding to the start of the season */
#define SEASON_START_STEP	(SEASON_START_DAY * STEPS_PER_DAY)

/** Number of days in a season */
#define DAYS_IN_SEASON	(366)	
/** Number of model steps in a season */
#define STEPS_IN_SEASON	(DAYS_IN_SEASON * STEPS_PER_DAY)

#ifdef REALTIME
/** Maximum run length */
#define DAYS_IN_RUN	DAYS_IN_SEASON	
#else
# ifdef ADULT
/* Maximum run length */
# define DAYS_IN_RUN	DAYS_IN_SEASON	
# else
/* Trim run length to 153 days for speed for the juvenile model */
# define DAYS_IN_RUN	(153)	
# endif
#endif

/** Number of model steps in the run */
#define STEPS_IN_RUN	(DAYS_IN_RUN * STEPS_PER_DAY)

/** Number of dam time slices (this should be a power of 2, at least STEPS_PER_DAY, and at most 16) */
#define DAM_TIME_SLC	(2)	
/** Number of dam time slices in a season */
#define DAM_SLICES_IN_SEASON (DAYS_IN_SEASON * DAM_TIME_SLC)
#define SLICES_IN_STEP (DAM_TIME_SLC / STEPS_PER_DAY)  /* helps if this
							* is integer result
							*/
/** Number of runs */
#define	NRUNS		1

/* math.h on the pc is missing "pi" */
#ifdef WIN32
#define	M_PI	3.14159265358979323846
#endif

/* Annoying difference between Unix and Win32:
 *      Unix requires '/' as the separator between directories in pathnames.
 *      Win32 accepts either '/' or '\' (which must be given as '\\' for
 *      other reasons), but when Win32 provides the app with a pathname
 *      it will always have '\' separators. The solution is to convert
 *      Win32-provided pathnames to the Unix convention as soon as practical.
 */
#ifdef WIN32
#define LINETERM "\r\n"
#define PATHSEPERATOR '\\'
#else
#define LINETERM "\n"
#define PATHSEPERATOR '/'
#endif

#endif

