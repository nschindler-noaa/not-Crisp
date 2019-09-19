#ifndef	__CREATE_FLOW_ARCHIVE_DAYNIGHT_H
#define	__CREATE_FLOW_ARCHIVE_DAYNIGHT_H

/* These should be the possible combinations of values specified (that actually make sense):
 *
 *	Inputs Specified			Day Value	Night Value
 * -------------------------------------------------------------------------
 * 1) Day and night				Day		Night
 * 2) Whole day					Whole day	Whole day
 * 3) Whole day and night			Whole day	Night
 * 4) Whole day and day				Day		Whole day
 * 5) Whole day, day, and night (all 3)		Day		Night
 */

/* Here is the encoding of the above table of which inputs should be specified */
typedef struct {
	int whole_day;
	int day;
	int night;
} spec;
spec specified_table[5] = {
	{0, 1, 1},
	{1, 0, 0},
	{1, 0, 1},
	{1, 1, 0},
	{1, 1, 1}
};

/* The expected spill is simply calculated from the day to ensure that the correct day is being read */
#define SPILL_EXPECTED(day)	(((double)day)/365.0)

/* These will divide the expected value to ensure that the correct value is being read */
#define WHOLE_DAY_DIVIDER	(123.0)
#define DAY_DIVIDER		(321.0)
#define NIGHT_DIVIDER		(444.0)

#endif

