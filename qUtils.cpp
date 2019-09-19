/* ZasUtils.h
 *  Application-specific utilities for COMPASS. These routines will not
 *  be members of any class.
 */

#include <string.h>
#include <stdio.h>
#include <qlistbox.h>
#include "qUtils.h"
#include "run_mode.h"
#include <qstring.h>
#include <qcombobox.h>
#include <qmenubar.h>
#include "Qmain.h"
#include "Qcrisp.h"

/* Given a segment name, return a pointer to the river_segment structure,
 *  or NULL if that name was not found. */
river_segment *getRiverSegmentHandle(
				const char *sourceName, const int segType) {
	river_segment *cursor = river_mouth;
	int found = 0;
	
	do{	
		while(cursor && cursor->next && cursor->seg_type != segType){
			cursor = cursor->next;
		}
		if(cursor && (cursor->seg_type == segType) && !strcmp(sourceName, cursor->seg_name))
			found = 1;
		else if(cursor && cursor->next)
			 cursor = cursor->next;
		else break;

	}while(!found);
	if(found)
		return cursor;

	return NULL;
}

/* Routine to format a date string from a Julian (integer) date.
 */
void formatDate( int day, QString *pstr ) {
	char str[10];

	if (day < 0)
		str[0] = '\0';
	else if (day < 31)
		sprintf( str, "Jan %d", day+1 );
	else if (day < 60)
		sprintf( str, "Feb %d", day-30 );
	else if (day < 91)
		sprintf( str, "March %d", day-59 );
	else if (day < 121)
		sprintf( str, "April %d", day-90 );
	else if (day < 152)
		sprintf( str, "May %d", day-120 );
	else if (day < 182)
		sprintf( str, "June %d", day-151 );
	else if (day < 213)
		sprintf( str, "July %d", day-181 );
	else if (day < 244)
		sprintf( str, "Aug %d", day-212 );
	else if (day < 274)
		sprintf( str, "Sept %d", day-243 );
	else if (day < 305)
		sprintf( str, "Oct %d", day-273 );
	else if (day < 335)
		sprintf( str, "Nov %d", day-304 );
	else if (day < 366)
		sprintf( str, "Dec %d", day-334 );
	else
		str[0] = '\0';

	*pstr = str;
}

/* Routine to set the selection in a combobox, given a string to select.
 *  This is necessary because qComboBox::selection will accept an exact
 *  match *or* a prefix string match, which is often not what we want.
 *  Return value is zero-based index if selection successful, -1 elsewise.
 */
 
int setExactSelection( 	QComboBox *cbb, const char *text ) {
	
	QListBox * lb;
	QListBoxItem * lbi;

	lb = cbb->listBox();
	lbi = lb->findItem(text, Qt::ExactMatch);
	if(lbi > 0)
		cbb->setCurrentItem(lb->index(lbi));
	else{
		cbb->setCurrentItem(0);
		return -1;
		//cbb->clear ();
		//lb->clearSelection();
		//cbb->setListBox ( lb ) ;
	}
	
	//lb->setSelected (lbi, true);
	return 0;
	
}

/* Is a simulation running right now?
 */
int isRunning( void ) {
	return run_mode.running != 0;
}

/* Used by C1Main and by qReleaseTool
 */
QPopupMenu *CreateReleaseSiteMenu( QCrisp* mainWindow, int *pidStart )
{
	
	struct release_site  *site = release_site;
	QPopupMenu *menu = new QPopupMenu( (QWidget*)mainWindow, "Dynamic Menu" );

	while (site) {
		menu->insertItem ( site->name, mainWindow, SLOT(DecodeCommand(int)),0, (*pidStart)++ );
		site = site->next;
	}
	
	return menu;
	
}

