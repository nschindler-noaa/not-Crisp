/* \file
 * \brief Julian day and calendrical calculations. */

/* =========================================================*/
/* get_julian(month, day) returns the julian date corresponding *
 * to the month and day in the year 2001. */
/* January is month 1. */
/* =========================================================*/
extern int get_julian(/* int month, int day */);

/* =========================================================*/
/* get_month_day(julian, month, day) sets the integers pointed
 * at by month and day to the month and day corresponding to the
 * julian date in the year 2001. Note that month and day are
 * pointers to int. */
/* January is month 1. */
/* =========================================================*/
extern void get_month_day(/* int julian, int * month, int * day */);

/* =========================================================*/
/* get_date_string(julian, string) sets the string to be the
 * month name and day of the month that corresponds to the
 * julian date in the year 2001. */
/* =========================================================*/
extern char * get_date_string(/* int julian, char * string */);

/* =========================================================*/
/* This procedure pops up the date tool.
 *
 * If the julian_day is non-zero, the tool will be initialized
 * to that julian_day.
 *
 * If title is non-zero, the title of the date tool is set to
 * be this string, otherwise it is set to be "Date Tool".
 *
 * If proc is non-zero, an "Apply" button wil be on the tool
 * which will call the procedure with the current julian day.
 * This is usefull for setting panel items to be a particular
 * julian day.
 * The proc is of the form "void (*proc) ( int julian );" where
 * julian is a julian day.
 *
 */
/* =========================================================*/
extern void date_tool( /* int julian_day,
			  char *title,
			  void (*proc)() */ );


/* =========================================================*/
/* This procedure is in a format that can be put in xview menus */
/* It only calls the date tool with null parameters. */
/* =========================================================*/
extern void date_tool_proc();
