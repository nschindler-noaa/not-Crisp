/** \file
 * \brief  This file provides platform-independant string functions which,
 *   for whatever reason, are not available in the C libraries on
 *   both the SUN and the PC.
 */

#ifndef _STR_TOOLS_
#define _STR_TOOLS_

#ifdef SOLARIS2
#include <strings.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef WIN32
/* zero out the first "num" bytes of "*str" */
extern void bzero(void *str, int num);
/* copy the first "num" bytes of "*b1" to "*b2". Handle overlapped
 *  moves correctly. */
extern void bcopy(void *b1, void *b2, int num);

/* case independant strcmp */
extern int strcasecmp( const char *s1, const char *s2 );
#endif

#ifdef __cplusplus
}
#endif

#endif

