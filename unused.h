/** \file
 * \brief This file contains a macro for marking parameters as unused (e.g. for callbacks).
 *
 * It prevents the code from actually using the parameter and suppresses warnings
 * from GCC. Note that this only works under GCC, but since the code needs to be
 * compiled using GCC, any errors should show up eventually. */

#ifndef	_UNUSED_H
#define	_UNUSED_H

#ifdef UNUSED
#elif defined(__GNUC__)
#define UNUSED(a) __UNUSED_ ## a __attribute__((unused)) 
#else
#define UNUSED(a) a
#endif

#endif

