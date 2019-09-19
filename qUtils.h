/* qUtils.h
 *  Application-specific utilities for COMPASS. These routines will not
 *  be members of any class.
 */

#ifndef __qUTILS_H__
#define __qUTILS_H__

#include "crisp.h"
#include "Qcrisp.h"
#include <qstring.h>

class QPopupMenu;
class QComboBox;
/* Routine to find a pointer to a river segment given name and type
 */
extern river_segment *getRiverSegmentHandle( const char *sourceName, 
												const int segType );

/* Routine to format a date string from a Julian (integer) date.
 */
extern void formatDate( int day, QString *pstr );

/* Routine to set the selection in a combobox, given a string to select.
 *  This is necessary because zComboBoxBase::selection will accept an exact
 *  match *or* a prefix string match, which is often not what we want.
 *  Return value is zero-based index if selection successful, -1 elsewise.
 */
extern int setExactSelection( QComboBox *cbb, const char *text );

/* Is a simulation running right now?
 */
extern int isRunning( void );

/* Create a menu of all the release sites in the system.
 */
extern QPopupMenu *CreateReleaseSiteMenu( QCrisp* mainWindow, int *pidStart );

#endif
