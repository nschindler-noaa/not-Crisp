/** \file
 * \brief  This file provides platform-independant string functions which,
 *   for whatever reason, are not available in the C libraries on
 *   both the SUN and the PC.
 *
 * I think this file should probably be removed if at all possible because it
 * relies on ASCII encoding of characters and there is no good reason that I
 * see to override the native methods that have been tested much more
 * thoroughly.
 */

#include "strtools.h"

#ifdef WIN32
/** zero out the first "num" bytes of "*str" */
extern void bzero(void *v_str, int num){
	char *str = (char *)v_str;

	int i;
	for(i=0;i<num;i++){
		*str=0;
		str++;
	}

}

/** copy the first "num" bytes of "*b1" to "*b2". Handle overlapped
 *  copies correctly. */
extern void bcopy(void *v_b1,void *v_b2, int num){
	char *b1 = (char *)v_b1;
	char *b2 = (char *)v_b2;

	int i;
	if (b1 > b2) {
		for(i=0;i<num;i++){
			*b2=*b1;
			b1++;
			b2++;
		}
	} else {
		b1 += num;
		b2 += num;
		for(i=0;i<num;i++){
			b1--;
			b2--;
			*b2=*b1;
		}
	}

}

/** case independant strcmp */
int strcasecmp( const char *s1, const char *s2 )
{
	int c1, c2;
	do {
		c1 = *(s1++);
		c2 = *(s2++);
		if (c1 >= 'a' && c1 <= 'z')
			c1 = (c1 - 'a') + 'A';
		if (c2 >= 'a' && c2 <= 'z')
			c2 = (c2 - 'a') + 'A';
		if (c1 != c2)
			return c1 - c2;  // return on first miscompare.
	} while (c1 || c2);
	return 0;  // strings ended without any difference detected.
}

#endif

