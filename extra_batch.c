/** \file
 * \brief Function implementations for batch mode only.
 *
 * This file contains function implementations that are for use in batch mode
 * only. These are generally callbacks from the C code that would have an
 * influence on the GUI. For example, the status bar in the GUI is realized
 * using a C function from the main model. Note that the main entry point
 * (i.e. the main() function) is defined in compassb.c instead; these are
 * merely support functions.
 */

#include "gui.h"
#include <stdio.h>
#include <stdlib.h>

void AnnounceFatalError() {
    fprintf( stderr, "compassb has encountered a fatal error\n" );
    exit(1);
}

void SetMonteGauges( int game, int gameprog, int year, int yearprog, int alt, int altprog ) {
}
void SetStatusLine( char *text ) {
}
void addToLogWindow( char *msg, int id ) {
}

