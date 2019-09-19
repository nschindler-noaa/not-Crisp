/** \file
 * \breif Header file for the nrutil_short.c file.  Taken from Numerical Recipes in C.
 */
#ifndef	_NRUTIL_SHORT_H
#define _NRUTIL_SHORT_H
 
double *dvector(long nl,long nh);
void free_dvector(double *v,long nl);
void nrerror(char error_text[]);

#endif

