/** \file
 * \brief Event handlers for a few specific events to update the GUI from C.
 *
 * These event handlers are usually called from the main model code in C and
 * they update the GUI with status information. These functions have null
 * implementations for batch mode in the extra_batch.c file.*/
#ifndef	__GUI_H__
#define	__GUI_H__

#ifdef __cplusplus
extern "C" {
#endif

/** Announce a fatal error and terminate */
extern void AnnounceFatalError();

/** Set the Monte Carlo run status gauges. This function does nothing in batch mode. */
extern void SetMonteGauges(int game, int gameprog, int year, int yearprog, int alt, int altprog);

/** Set the GUI status line with the appropriate text (i.e. the segment that
 * is being calculated). This function does nothing in batch mode. */
extern void SetStatusLine(char *text);

/** Add a message to the log window. This function does nothing in batch mode. */
extern void addToLogWindow(char *msg, int id);

#ifdef __cplusplus
}
#endif

#endif

